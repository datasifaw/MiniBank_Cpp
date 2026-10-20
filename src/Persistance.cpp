
#include "Persistance.h"

// ==========================================
// MODULE DE PERSISTANCE
// Etape 3.9.2.5
// ==========================================

#include <atomic>       // NOUVEAU
#include <chrono>       // NOUVEAU
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <new>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>

// NOUVEAU : API Win32 pour la publication.
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

namespace {

namespace fs = std::filesystem;

// ==========================================
// ENCODER EN HEXADECIMAL
// ==========================================

std::string encoderHexadecimal(
    const std::string& texte
) {
    const char chiffres[] = "0123456789ABCDEF";

    std::string resultat;

    if (texte.size() > resultat.max_size() / 2) {
        throw std::length_error(
            "Texte trop long pour l'encodage."
        );
    }

    resultat.reserve(texte.size() * 2);

    for (unsigned char octet : texte) {
        resultat.push_back(
            chiffres[(octet >> 4) & 0x0F]
        );

        resultat.push_back(
            chiffres[octet & 0x0F]
        );
    }

    return resultat;
}

// ==========================================
// CONVERTIR UN TYPE D'OPERATION
// ==========================================

bool convertirTypeOperation(
    TypeOperation type,
    std::string& resultat
) {
    switch (type) {
        case TypeOperation::Depot:
            resultat = "DEPOT";
            return true;

        case TypeOperation::Retrait:
            resultat = "RETRAIT";
            return true;

        case TypeOperation::VirementSortant:
            resultat = "VIREMENT_SORTANT";
            return true;

        case TypeOperation::VirementEntrant:
            resultat = "VIREMENT_ENTRANT";
            return true;
    }

    return false;
}

// ==========================================
// VALIDER UN ETAT BANCAIRE
// ==========================================

bool verifierEtat(
    const EtatBanque& etat,
    std::string& erreur
) {
    if (etat.prochainNumeroCompte == 0 ||
        etat.prochainNumeroOperation == 0) {

        erreur = "Compteur bancaire invalide.";
        return false;
    }

    std::set<std::string> identifiants;

    for (const Compte& compte : etat.comptes) {
        const std::string& id =
            compte.getIdentifiant();

        if (id.empty() ||
            compte.getTitulaire().empty()) {

            erreur =
                "Identifiant ou titulaire vide.";
            return false;
        }

        if (compte.getSoldeCentimes() < 0) {
            erreur = "Solde negatif detecte.";
            return false;
        }

        if (!identifiants.insert(id).second) {
            erreur =
                "Identifiant de compte en double.";
            return false;
        }
    }

    std::uint64_t dernierNumero = 0;

    for (const Operation& operation :
         etat.operations) {

        if (operation.numero == 0 ||
            operation.numero <= dernierNumero) {

            erreur =
                "Numerotation historique invalide.";
            return false;
        }

        if (operation.numero >=
            etat.prochainNumeroOperation) {

            erreur =
                "Compteur d'operations incoherent.";
            return false;
        }

        dernierNumero = operation.numero;

        if (operation.montantCentimes <= 0) {
            erreur =
                "Montant d'operation invalide.";
            return false;
        }

        if (identifiants.count(
                operation.identifiantCompte
            ) == 0) {

            erreur =
                "Operation associee a un compte "
                "inexistant.";
            return false;
        }

        switch (operation.type) {
            case TypeOperation::Depot:
            case TypeOperation::Retrait:

                if (!operation
                         .identifiantContrepartie
                         .empty()) {

                    erreur =
                        "Contrepartie inattendue.";
                    return false;
                }

                break;

            case TypeOperation::VirementSortant:
            case TypeOperation::VirementEntrant:

                if (operation
                        .identifiantContrepartie
                        .empty() ||

                    operation
                        .identifiantContrepartie ==
                        operation.identifiantCompte ||

                    identifiants.count(
                        operation
                            .identifiantContrepartie
                    ) == 0) {

                    erreur =
                        "Contrepartie de virement "
                        "invalide.";
                    return false;
                }

                break;

            default:
                erreur = "Type d'operation inconnu.";
                return false;
        }
    }

    return true;
}

// ==========================================
// RELIRE ET COMPARER LE FICHIER TEMPORAIRE
// ==========================================

StatutEcritureTemporaire verifierFichierTemporaire(
    const fs::path& chemin,
    const std::string& texte
) {
    std::ifstream entree(
        chemin,
        std::ios::in | std::ios::binary
    );

    if (!entree.is_open()) {
        return StatutEcritureTemporaire::
            ErreurLecture;
    }

    std::string contenuRelu(
        (std::istreambuf_iterator<char>(entree)),
        std::istreambuf_iterator<char>()
    );

    if (entree.bad()) {
        return StatutEcritureTemporaire::
            ErreurLecture;
    }

    entree.close();

    if (entree.fail()) {
        return StatutEcritureTemporaire::
            ErreurLecture;
    }

    if (contenuRelu != texte) {
        return StatutEcritureTemporaire::
            ContenuDifferent;
    }

    return StatutEcritureTemporaire::Succes;
}

// ==========================================
// NOUVEAU : VERIFIER L'EXISTENCE D'UN CHEMIN
// ==========================================

// NOUVEAU : distingue trois situations :
// - Absent
// - Fichier regulier
// - Autre entree ou erreur
//
// Cette verification ne suit pas un lien
// symbolique et refuse un dossier a la place
// du fichier attendu.
enum class EtatChemin {
    Absent,
    FichierRegulier,
    Invalide
};

EtatChemin examinerChemin(
    const fs::path& chemin
) {
    std::error_code erreur;

    const fs::file_status statut =
        fs::symlink_status(chemin, erreur);

    if (erreur ==
        std::errc::no_such_file_or_directory) {
        return EtatChemin::Absent;
    }

    if (erreur) {
        return EtatChemin::Invalide;
    }

    if (statut.type() == fs::file_type::not_found) {
        return EtatChemin::Absent;
    }

    if (statut.type() == fs::file_type::regular) {
        return EtatChemin::FichierRegulier;
    }

    return EtatChemin::Invalide;
}

// ==========================================
// NOUVEAU : GENERER UN NOM TEMPORAIRE
// ==========================================

// NOUVEAU : noms distincts pour les appels
// successifs de ce programme.
//
// Cette methode reduit les collisions, mais
// ne garantit pas une creation atomiquement
// exclusive face a un second processus.
// Nous restons dans un dossier de test local.
fs::path genererCheminTemporaire(
    const fs::path& principal
) {
    static std::atomic<unsigned long long>
        sequence{0};

    const auto maintenant =
        std::chrono::steady_clock::now()
            .time_since_epoch().count();

    const unsigned long long numero =
        ++sequence;

    const std::string suffixe =
        std::to_string(maintenant)
        + "_"
        + std::to_string(
            GetCurrentProcessId()
        )
        + "_"
        + std::to_string(numero);

    // NOUVEAU : le fichier temporaire reste
    // dans le MEME DOSSIER que le principal.
    return fs::path(
        principal.string()
        + "."
        + suffixe
        + ".tmp"
    );
}

// ==========================================
// NOUVEAU : CONTROLER UN FICHIER EXISTANT
// ==========================================

// NOUVEAU : avant de remplacer une sauvegarde,
// verifier sa structure MINIBANK|1 de base.
//
// Il s'agit d'un controle conservateur, mais
// pas encore du deserializeur complet de 3.9.4.
bool verifierPrincipalExistant(
    const fs::path& chemin
) {
    std::ifstream entree(
        chemin,
        std::ios::in | std::ios::binary
    );

    if (!entree.is_open()) {
        return false;
    }

    std::string premiereLigne;
    std::string derniereLigne;
    std::string ligne;

    if (!std::getline(entree, premiereLigne)) {
        return false;
    }

    if (premiereLigne != "MINIBANK|1") {
        return false;
    }

    // NOUVEAU : rechercher une fin explicite.
    // La validation complete des sections
    // sera ajoutee avec la restauration.

    // NOUVEAU : lire toutes les lignes
    // jusqu'a la fin normale du fichier.
    while (std::getline(entree, ligne)) {
        derniereLigne = ligne;
    }

    // NOUVEAU : badbit signale une erreur
    // reelle de lecture.
    if (entree.bad()) {
        return false;
    }

    // NOUVEAU : eofbit est attendu ici.
    // En revanche, un echec sans fin de fichier
    // indique un probleme de lecture.
    if (!entree.eof()) {
        return false;
    }

    // NOUVEAU : reinitialiser eofbit et failbit
    // avant de fermer le fichier.
    entree.clear();

    // NOUVEAU : fermer puis verifier
    // uniquement le resultat de fermeture.
    entree.close();

    if (entree.fail()) {
        return false;
    }

    // NOUVEAU : verifier le marqueur de fin.
    return derniereLigne == "END";
}

// ==========================================
// NOUVEAU : DECRIRE UNE ERREUR WINDOWS
// ==========================================

std::string messageErreurWindows(
    const std::string& operation,
    DWORD code
) {
    return operation
        + " a echoue (code Windows "
        + std::to_string(code)
        + "). Inspecter les fichiers avant "
          "toute nouvelle tentative.";
}

} // namespace

// ==========================================
// SERIALISATION EN MEMOIRE
// ==========================================

ResultatSerialisation Persistance::serialiser(
    const EtatBanque& etat
) {
    ResultatSerialisation resultat;

    try {
        std::string erreur;

        if (!verifierEtat(etat, erreur)) {
            resultat.statut =
                StatutSerialisation::DonneesInvalides;

            resultat.message = erreur;
            return resultat;
        }

        std::ostringstream flux;

        flux << "MINIBANK|1\n";

        flux << "COUNTERS|"
             << etat.prochainNumeroCompte
             << '|'
             << etat.prochainNumeroOperation
             << '\n';

        flux << "ACCOUNTS|"
             << etat.comptes.size()
             << '\n';

        for (const Compte& compte : etat.comptes) {
            flux << "ACCOUNT|"
                 << encoderHexadecimal(
                        compte.getIdentifiant()
                    )
                 << '|'
                 << encoderHexadecimal(
                        compte.getTitulaire()
                    )
                 << '|'
                 << compte.getSoldeCentimes()
                 << '\n';
        }

        flux << "OPERATIONS|"
             << etat.operations.size()
             << '\n';

        for (const Operation& operation :
             etat.operations) {

            std::string type;

            if (!convertirTypeOperation(
                    operation.type, type
                )) {
                resultat.statut =
                    StatutSerialisation::
                        DonneesInvalides;

                resultat.message =
                    "Type d'operation inconnu.";

                return resultat;
            }

            std::string contrepartie = "-";

            if (!operation
                     .identifiantContrepartie
                     .empty()) {

                contrepartie =
                    encoderHexadecimal(
                        operation
                            .identifiantContrepartie
                    );
            }

            flux << "OP|"
                 << operation.numero
                 << '|'
                 << type
                 << '|'
                 << encoderHexadecimal(
                        operation.identifiantCompte
                    )
                 << '|'
                 << operation.montantCentimes
                 << '|'
                 << contrepartie
                 << '\n';
        }

        flux << "END\n";

        if (!flux) {
            resultat.statut =
                StatutSerialisation::ErreurMemoire;

            resultat.message =
                "Impossible de produire le texte.";

            return resultat;
        }

        resultat.texte = flux.str();

        resultat.statut =
            StatutSerialisation::Succes;

        resultat.message =
            "Serialisation en memoire reussie.";

        return resultat;

    } catch (const std::bad_alloc&) {
        resultat.texte.clear();

        resultat.statut =
            StatutSerialisation::ErreurMemoire;

        resultat.message =
            "Memoire insuffisante pour "
            "serialiser la banque.";

        return resultat;

    } catch (const std::length_error&) {
        resultat.texte.clear();

        resultat.statut =
            StatutSerialisation::ErreurMemoire;

        resultat.message =
            "Taille du texte trop importante.";

        return resultat;
    }
}

// ==========================================
// ECRITURE CONTROLEE DU FICHIER TEMPORAIRE
// ==========================================

ResultatEcritureTemporaire
Persistance::ecrireFichierTemporaire(
    const EtatBanque& etat,
    const std::string& cheminTemporaire
) {
    ResultatEcritureTemporaire resultat;

    if (cheminTemporaire.empty()) {
        resultat.statut =
            StatutEcritureTemporaire::CheminInvalide;

        resultat.message =
            "Chemin temporaire vide.";

        return resultat;
    }

    try {
        const fs::path chemin =
            fs::u8path(cheminTemporaire);

        if (chemin.filename().empty() ||
            chemin.extension() != ".tmp") {

            resultat.statut =
                StatutEcritureTemporaire::
                    CheminInvalide;

            resultat.message =
                "Le fichier doit se terminer "
                "par .tmp.";

            return resultat;
        }

        fs::path dossier = chemin.parent_path();

        if (dossier.empty()) {
            dossier = ".";
        }

        if (!fs::is_directory(dossier)) {
            resultat.statut =
                StatutEcritureTemporaire::
                    CheminInvalide;

            resultat.message =
                "Dossier temporaire invalide.";

            return resultat;
        }

        // NOUVEAU : conserver le refus
        // d'ecraser un fichier existant.
        if (examinerChemin(chemin) !=
            EtatChemin::Absent) {

            resultat.statut =
                StatutEcritureTemporaire::
                    FichierDejaExistant;

            resultat.message =
                "Chemin temporaire deja utilise "
                "ou inaccessible.";

            return resultat;
        }

        const ResultatSerialisation donnees =
            serialiser(etat);

        if (!donnees.estUnSucces()) {
            resultat.statut =
                StatutEcritureTemporaire::
                    ErreurSerialisation;

            resultat.message =
                "Serialisation impossible : "
                + donnees.message;

            return resultat;
        }

        // NOUVEAU : eviter un depassement
        // lors de la conversion de taille.
        const std::size_t tailleMaximale =
            static_cast<std::size_t>(
                std::numeric_limits<
                    std::streamsize
                >::max()
            );

        if (donnees.texte.size() >
            tailleMaximale) {

            resultat.statut =
                StatutEcritureTemporaire::
                    ErreurEcriture;

            resultat.message =
                "Fichier temporaire trop grand.";

            return resultat;
        }

        std::ofstream sortie(
            chemin,
            std::ios::out | std::ios::binary
        );

        if (!sortie.is_open()) {
            resultat.statut =
                StatutEcritureTemporaire::
                    ErreurOuverture;

            resultat.message =
                "Impossible d'ouvrir le temporaire.";

            return resultat;
        }

        sortie.write(
            donnees.texte.data(),
            static_cast<std::streamsize>(
                donnees.texte.size()
            )
        );

        const bool ecritureCorrecte = sortie.good();

        sortie.close();

        const bool fermetureCorrecte =
            !sortie.fail();

        if (!ecritureCorrecte) {
            resultat.statut =
                StatutEcritureTemporaire::
                    ErreurEcriture;

            resultat.message =
                "Erreur pendant l'ecriture.";

            return resultat;
        }

        if (!fermetureCorrecte) {
            resultat.statut =
                StatutEcritureTemporaire::
                    ErreurFermeture;

            resultat.message =
                "Erreur pendant la fermeture.";

            return resultat;
        }

        resultat.statut =
            verifierFichierTemporaire(
                chemin,
                donnees.texte
            );

        if (resultat.statut ==
            StatutEcritureTemporaire::
                ErreurLecture) {

            resultat.message =
                "Impossible de relire le temporaire.";

            return resultat;
        }

        if (resultat.statut ==
            StatutEcritureTemporaire::
                ContenuDifferent) {

            resultat.message =
                "Le contenu relu est different.";

            return resultat;
        }

        resultat.statut =
            StatutEcritureTemporaire::Succes;

        resultat.message =
            "Fichier temporaire ecrit, "
            "ferme et verifie.";

        return resultat;

    } catch (const std::exception&) {
        resultat.statut =
            StatutEcritureTemporaire::ErreurEcriture;

        resultat.message =
            "Erreur pendant la preparation "
            "du fichier temporaire.";

        return resultat;
    }
}

// ==========================================
// NOUVEAU : SAUVEGARDE DEFINITIVE WINDOWS
// ==========================================

ResultatSauvegarde Persistance::sauvegarder(
    const EtatBanque& etat,
    const std::string& chemin
) {
    ResultatSauvegarde resultat;

    if (chemin.empty()) {
        resultat.statut =
            StatutSauvegarde::CheminInvalide;

        resultat.message =
            "Chemin de sauvegarde vide.";

        return resultat;
    }

    try {
        const fs::path principal =
            fs::u8path(chemin);

        // NOUVEAU : un fichier principal
        // ne doit jamais etre un temporaire
        // ni une sauvegarde precedente.
        if (principal.filename().empty() ||
            principal.extension() != ".txt") {

            resultat.statut =
                StatutSauvegarde::CheminInvalide;

            resultat.message =
                "Le fichier principal doit "
                "se terminer par .txt.";

            return resultat;
        }

        fs::path dossier =
            principal.parent_path();

        if (dossier.empty()) {
            dossier = ".";
        }

        if (!fs::is_directory(dossier)) {
            resultat.statut =
                StatutSauvegarde::CheminInvalide;

            resultat.message =
                "Dossier de sauvegarde invalide.";

            return resultat;
        }

        // ==================================
        // NOUVEAU : CHEMIN .BAK
        // ==================================

        fs::path precedent = principal;
        precedent.replace_extension(".bak");

        // NOUVEAU : NE PAS remplacer
        // un ancien fichier .bak.
        //
        // Cette politique conservatrice
        // limite volontairement les versions
        // de sauvegarde de cette etape.
        if (examinerChemin(precedent) !=
            EtatChemin::Absent) {

            resultat.statut =
                StatutSauvegarde::
                    SauvegardePrecedenteExistante;

            resultat.message =
                "Une sauvegarde .bak existe deja. "
                "Aucune nouvelle sauvegarde "
                "n'a ete effectuee.";

            return resultat;
        }

        const EtatChemin etatPrincipal =
            examinerChemin(principal);

        if (etatPrincipal == EtatChemin::Invalide) {
            resultat.statut =
                StatutSauvegarde::
                    FichierPrincipalInvalide;

            resultat.message =
                "Le chemin principal n'est "
                "pas un fichier regulier.";

            return resultat;
        }

        // NOUVEAU : controle conservateur
        // du principal existant.
        //
        // Ce n'est PAS encore la validation
        // complete de l'etape 3.9.4.
        if (etatPrincipal ==
                EtatChemin::FichierRegulier &&
            !verifierPrincipalExistant(principal)) {

            resultat.statut =
                StatutSauvegarde::
                    FichierPrincipalInvalide;

            resultat.message =
                "Sauvegarde principale illisible "
                "ou structure MINIBANK invalide.";

            return resultat;
        }

        // ==================================
        // NOUVEAU : VALIDATION DES DONNEES
        // ==================================

        // NOUVEAU : ne pas ouvrir de temporaire
        // si l'etat bancaire est invalide.
        const ResultatSerialisation verification =
            serialiser(etat);

        if (!verification.estUnSucces()) {
            resultat.statut =
                StatutSauvegarde::
                    ErreurSerialisation;

            resultat.message =
                "Serialisation refusee : "
                + verification.message;

            return resultat;
        }

        // ==================================
        // NOUVEAU : NOM TEMPORAIRE UNIQUE
        // ==================================

        fs::path temporaire;
        bool nomDisponible = false;

        for (int tentative = 0;
             tentative < 32;
             ++tentative) {

            temporaire =
                genererCheminTemporaire(principal);

            if (examinerChemin(temporaire) ==
                EtatChemin::Absent) {

                nomDisponible = true;
                break;
            }
        }

        if (!nomDisponible) {
            resultat.statut =
                StatutSauvegarde::CheminInvalide;

            resultat.message =
                "Impossible de reserver un "
                "nom de fichier temporaire.";

            return resultat;
        }

        // ==================================
        // NOUVEAU : ECRITURE VERIFIEE
        // ==================================

        const ResultatEcritureTemporaire ecriture =
            ecrireFichierTemporaire(
                etat,
                temporaire.u8string()
            );

        if (!ecriture.estUnSucces()) {
            resultat.statut =
                StatutSauvegarde::ErreurEcriture;

            resultat.message =
                "Preparation temporaire echouee : "
                + ecriture.message
                + " Verifier tout fichier .tmp "
                  "avant de le supprimer.";

            return resultat;
        }

        // ==================================
        // NOUVEAU : PREMIERE SAUVEGARDE
        // ==================================

        if (etatPrincipal == EtatChemin::Absent) {

            // NOUVEAU : aucun indicateur
            // MOVEFILE_REPLACE_EXISTING.
            //
            // La destination doit etre libre.
            const BOOL publie =
                MoveFileExW(
                    temporaire.c_str(),
                    principal.c_str(),
                    MOVEFILE_WRITE_THROUGH
                );

            if (!publie) {
                const DWORD code = GetLastError();

                resultat.statut =
                    StatutSauvegarde::
                        ErreurRemplacement;

                resultat.message =
                    messageErreurWindows(
                        "MoveFileExW",
                        code
                    );

                return resultat;
            }

            resultat.statut =
                StatutSauvegarde::Succes;

            resultat.message =
                "Premiere sauvegarde publiee "
                "avec succes.";

            return resultat;
        }

        // ==================================
        // NOUVEAU : REMPLACEMENT PROTEGE
        // ==================================

        // NOUVEAU : ReplaceFileW cree
        // la sauvegarde precedente en .bak
        // et place le nouveau contenu sous
        // le nom du fichier principal.
        //
        // Ne pas utiliser ici
        // REPLACEFILE_IGNORE_MERGE_ERRORS :
        // nous ne voulons pas ignorer
        // les problemes d'attributs ou d'ACL.
        const BOOL remplace =
            ReplaceFileW(
                principal.c_str(),
                temporaire.c_str(),
                precedent.c_str(),
                0,
                nullptr,
                nullptr
            );

        if (!remplace) {
            const DWORD code = GetLastError();

            // NOUVEAU : NE PAS supprimer
            // automatiquement les fichiers.
            //
            // Certaines erreurs ReplaceFileW
            // peuvent modifier leur disposition.
            resultat.statut =
                StatutSauvegarde::
                    ErreurRemplacement;

            resultat.message =
                messageErreurWindows(
                    "ReplaceFileW",
                    code
                );

            return resultat;
        }

        resultat.statut =
            StatutSauvegarde::Succes;

        resultat.message =
            "Sauvegarde remplacee avec succes. "
            "Version precedente conservee "
            "dans le fichier .bak.";

        return resultat;

    } catch (const fs::filesystem_error&) {
        resultat.statut =
            StatutSauvegarde::CheminInvalide;

        resultat.message =
            "Erreur du systeme de fichiers.";

        return resultat;

    } catch (const std::exception&) {
        resultat.statut =
            StatutSauvegarde::ErreurEcriture;

        resultat.message =
            "Erreur pendant la preparation "
            "ou la publication de la sauvegarde.";

        return resultat;
    }
}
