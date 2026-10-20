
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

#include "Banque.h"
#include "Persistance.h"

// ==========================================
// NOUVEAU : TESTS DE CHARGEMENT
// Etape 3.9.4.4
//
// Tous les fichiers appartiennent au
// dossier de test fourni en argument.
//
// Aucun fichier n'est supprime.
// ==========================================

namespace {

namespace fs = std::filesystem;

int nombreTests = 0;
int nombreReussites = 0;

// ==========================================
// NOUVEAU : ASSERTION
// ==========================================

void verifier(
    bool condition,
    const std::string& description
) {
    ++nombreTests;

    if (condition) {
        ++nombreReussites;
    }

    std::cout
        << (condition ? "[OK]    " : "[ECHEC] ")
        << description << '\n';
}

// ==========================================
// NOUVEAU : ECRIRE UN FICHIER DE TEST
// ==========================================

bool creerFichier(
    const fs::path& chemin,
    const std::string& contenu
) {
    // NOUVEAU : ne jamais ecraser
    // un fichier deja existant.
    if (fs::exists(chemin)) {
        return false;
    }

    std::ofstream sortie(
        chemin,
        std::ios::out | std::ios::binary
    );

    if (!sortie.is_open()) {
        return false;
    }

    sortie.write(
        contenu.data(),
        static_cast<std::streamsize>(
            contenu.size()
        )
    );

    const bool ecritureOK = sortie.good();

    sortie.close();

    return ecritureOK && !sortie.fail();
}

// ==========================================
// NOUVEAU : RELIRE POUR VERIFIER
// QUE CHARGER N'A RIEN MODIFIE
// ==========================================

bool fichierIdentique(
    const fs::path& chemin,
    const std::string& attendu
) {
    std::ifstream entree(
        chemin,
        std::ios::in | std::ios::binary
    );

    if (!entree.is_open()) {
        return false;
    }

    const std::string contenu(
        (std::istreambuf_iterator<char>(entree)),
        std::istreambuf_iterator<char>()
    );

    if (entree.bad()) {
        return false;
    }

    entree.clear();
    entree.close();

    return !entree.fail()
        && contenu == attendu;
}

// ==========================================
// NOUVEAU : ETAT PARTIEL INTERDIT
// ==========================================

bool etatVideApresEchec(
    const ResultatChargement& resultat
) {
    return resultat.etat.comptes.empty()
        && resultat.etat.operations.empty()
        && resultat.etat.prochainNumeroCompte == 1
        && resultat.etat.prochainNumeroOperation == 1;
}

// ==========================================
// NOUVEAU : TESTER UN CHARGEMENT VALIDE
// ==========================================

void testerValide(
    const fs::path& dossier,
    const std::string& nom,
    const Banque& banque
) {
    std::cout
        << "\n=== CHARGEMENT VALIDE : "
        << nom << " ===\n";

    const fs::path chemin =
        dossier / (nom + ".txt");

    const ResultatSerialisation texte =
        Persistance::serialiser(
            banque.exporterEtat()
        );

    verifier(
        texte.estUnSucces(),
        nom + " : serialisation reussie"
    );

    if (!texte.estUnSucces()) {
        return;
    }

    verifier(
        creerFichier(chemin, texte.texte),
        nom + " : fichier de test cree"
    );

    const ResultatChargement resultat =
        Persistance::charger(
            chemin.u8string()
        );

    verifier(
        resultat.estUnSucces()
            && resultat.statut ==
                StatutChargement::Succes,
        nom + " : chargement reussi"
    );

    verifier(
        resultat.statutDeserialisation ==
            StatutDeserialisation::Succes,
        nom + " : deserialisation valide"
    );

    const ResultatSerialisation retour =
        Persistance::serialiser(
            resultat.etat
        );

    verifier(
        retour.estUnSucces()
            && retour.texte == texte.texte,
        nom + " : etat reconstruit exactement"
    );

    verifier(
        fichierIdentique(
            chemin,
            texte.texte
        ),
        nom + " : fichier source inchange"
    );
}

// ==========================================
// NOUVEAU : TESTER UN FICHIER CORROMPU
// ==========================================

void testerCorrompu(
    const fs::path& dossier,
    const std::string& nom,
    const std::string& contenu,
    StatutDeserialisation attendu
) {
    std::cout
        << "\n=== FICHIER CORROMPU : "
        << nom << " ===\n";

    const fs::path chemin =
        dossier / (nom + ".txt");

    verifier(
        creerFichier(chemin, contenu),
        nom + " : fichier prepare"
    );

    const ResultatChargement resultat =
        Persistance::charger(
            chemin.u8string()
        );

    verifier(
        resultat.statut ==
            StatutChargement::
                ErreurDeserialisation
            && !resultat.estUnSucces(),
        nom + " : chargement refuse"
    );

    verifier(
        resultat.statutDeserialisation ==
            attendu,
        nom + " : statut detaille correct"
    );

    verifier(
        etatVideApresEchec(resultat)
            && !resultat.message.empty(),
        nom + " : aucun etat partiel"
    );

    verifier(
        fichierIdentique(chemin, contenu),
        nom + " : fichier preserve"
    );
}

// ==========================================
// NOUVEAU : FICHIERS VALIDES
// ==========================================

void testerFichiersValides(
    const fs::path& dossier
) {
    Banque vide;

    testerValide(
        dossier,
        "banque_vide",
        vide
    );

    Banque comptes;

    comptes.creerCompte("Alice", 50000);
    comptes.creerCompte("Bob", 120000);

    testerValide(
        dossier,
        "deux_comptes",
        comptes
    );

    Banque historique;

    const std::string alice =
        historique.creerCompte(
            "Alice", 50000
        );

    const std::string bob =
        historique.creerCompte(
            "Bob", 120000
        );

    const bool operationsOK =
        historique.deposerSurCompte(
            alice, 10000
        )
        && historique.retirerDuCompte(
            alice, 5000
        )
        && historique.virer(
            alice, bob, 2500
        );

    verifier(
        operationsOK,
        "Historique : operations executees"
    );

    if (operationsOK) {
        testerValide(
            dossier,
            "historique_complet",
            historique
        );
    }

    // NOUVEAU : accents et separateur.
    Banque utf8;

    utf8.creerCompte(
        std::string("El")
            + "\xC3\xA9"
            + "a | Martin",
        12345
    );

    testerValide(
        dossier,
        "nom_utf8",
        utf8
    );
}

// ==========================================
// NOUVEAU : CHEMINS INVALIDES
// ==========================================

void testerChemins(
    const fs::path& dossier
) {
    std::cout
        << "\n=== CHEMINS INVALIDES ===\n";

    const ResultatChargement vide =
        Persistance::charger("");

    verifier(
        vide.statut ==
            StatutChargement::CheminInvalide,
        "Chemin vide refuse"
    );

    verifier(
        etatVideApresEchec(vide),
        "Chemin vide : aucun etat"
    );

    const fs::path mauvais =
        dossier / "interdit.bak";

    const ResultatChargement extension =
        Persistance::charger(
            mauvais.u8string()
        );

    verifier(
        extension.statut ==
            StatutChargement::CheminInvalide,
        "Extension .bak refusee"
    );

    verifier(
        etatVideApresEchec(extension),
        "Extension .bak : aucun etat"
    );

    const fs::path manquant =
        dossier / "introuvable.txt";

    const ResultatChargement absent =
        Persistance::charger(
            manquant.u8string()
        );

    verifier(
        absent.statut ==
            StatutChargement::FichierIntrouvable,
        "Fichier inexistant detecte"
    );

    verifier(
        !fs::exists(manquant),
        "Fichier inexistant non cree"
    );

    // NOUVEAU : un dossier se termine
    // volontairement par .txt.
    const fs::path cheminDossier =
        dossier / "dossier.txt";

    const bool cree =
        fs::create_directory(cheminDossier);

    verifier(
        cree,
        "Dossier de test prepare"
    );

    const ResultatChargement dossierRefuse =
        Persistance::charger(
            cheminDossier.u8string()
        );

    verifier(
        dossierRefuse.statut ==
            StatutChargement::
                FichierNonRegulier,
        "Dossier .txt refuse"
    );

    verifier(
        fs::is_directory(cheminDossier),
        "Dossier source preserve"
    );
}

// ==========================================
// NOUVEAU : FICHIERS CORROMPUS
// ==========================================

void testerCorruptions(
    const fs::path& dossier
) {
    const std::string canonique =
        "MINIBANK|1\n"
        "COUNTERS|1|1\n"
        "ACCOUNTS|0\n"
        "OPERATIONS|0\n"
        "END\n";

    testerCorrompu(
        dossier,
        "fichier_vide",
        "",
        StatutDeserialisation::FormatInvalide
    );

    testerCorrompu(
        dossier,
        "version_inconnue",
        "MINIBANK|2\n"
        "COUNTERS|1|1\n"
        "ACCOUNTS|0\n"
        "OPERATIONS|0\n"
        "END\n",
        StatutDeserialisation::VersionInconnue
    );

    testerCorrompu(
        dossier,
        "fichier_tronque",
        canonique.substr(
            0,
            canonique.size() - 4
        ),
        StatutDeserialisation::FormatInvalide
    );

    testerCorrompu(
        dossier,
        "crlf",
        "MINIBANK|1\r\n"
        "COUNTERS|1|1\r\n"
        "ACCOUNTS|0\r\n"
        "OPERATIONS|0\r\n"
        "END\r\n",
        StatutDeserialisation::FormatInvalide
    );

    testerCorrompu(
        dossier,
        "etat_incoherent",
        "MINIBANK|1\n"
        "COUNTERS|0|1\n"
        "ACCOUNTS|0\n"
        "OPERATIONS|0\n"
        "END\n",
        StatutDeserialisation::EtatIncoherent
    );
}

// ==========================================
// NOUVEAU : FICHIER TROP GRAND
// ==========================================

void testerTaille(
    const fs::path& dossier
) {
    std::cout
        << "\n=== LIMITE DE TAILLE ===\n";

    const fs::path chemin =
        dossier / "trop_grand.txt";

    const std::string contenu(
        8 * 1024 * 1024 + 1,
        'X'
    );

    verifier(
        creerFichier(chemin, contenu),
        "Fichier de plus de 8 Mio prepare"
    );

    const ResultatChargement resultat =
        Persistance::charger(
            chemin.u8string()
        );

    verifier(
        resultat.statut ==
            StatutChargement::FichierTropGrand,
        "Fichier trop grand refuse"
    );

    verifier(
        etatVideApresEchec(resultat),
        "Fichier trop grand : aucun etat"
    );

    verifier(
        fs::file_size(chemin) == contenu.size(),
        "Fichier trop grand non tronque"
    );
}

// ==========================================
// NOUVEAU : BANQUE ACTIVE PRESERVEE
// ==========================================

void testerPreservationBanque(
    const fs::path& dossier
) {
    std::cout
        << "\n=== BANQUE ACTIVE PRESERVEE ===\n";

    Banque banque;

    banque.creerCompte(
        "Alice",
        50000
    );

    const ResultatSerialisation avant =
        Persistance::serialiser(
            banque.exporterEtat()
        );

    const fs::path chemin =
        dossier / "etat_incoherent.txt";

    const ResultatChargement resultat =
        Persistance::charger(
            chemin.u8string()
        );

    const ResultatSerialisation apres =
        Persistance::serialiser(
            banque.exporterEtat()
        );

    verifier(
        !resultat.estUnSucces(),
        "Chargement incoherent refuse"
    );

    verifier(
        avant.estUnSucces()
            && apres.estUnSucces()
            && avant.texte == apres.texte,
        "Banque active inchangee"
    );
}

} // namespace

// ==========================================
// NOUVEAU : PROGRAMME PRINCIPAL
// ==========================================

int main(int argc, char* argv[]) {
    std::cout
        << "====================================\n"
        << " TESTS CHARGEMENT MINIBANK\n"
        << " Etape 3.9.4.4\n"
        << "====================================\n";

    if (argc != 2) {
        std::cerr
            << "Erreur : dossier de test manquant.\n";
        return 2;
    }

    try {
        const fs::path dossier =
            fs::u8path(argv[1]);

        // NOUVEAU : refuser tout dossier
        // non vide pour ne pas reutiliser
        // ou ecraser d'anciennes donnees.
        if (
            !fs::is_directory(dossier) ||
            !fs::is_empty(dossier)
        ) {
            std::cerr
                << "Erreur : fournir un "
                "dossier neuf et vide.\n";

            return 2;
        }

        testerFichiersValides(dossier);
        testerChemins(dossier);
        testerCorruptions(dossier);
        testerTaille(dossier);
        testerPreservationBanque(dossier);

        std::cout
            << "\n====================================\n"
            << "Tests executes : "
            << nombreTests << '\n'
            << "Tests reussis : "
            << nombreReussites << '\n'
            << "Tests echoues : "
            << nombreTests - nombreReussites
            << '\n'
            << "====================================\n";

        return nombreTests == nombreReussites
            ? 0
            : 1;

    } catch (const std::exception& erreur) {
        std::cerr
            << "Erreur pendant les tests : "
            << erreur.what() << '\n';

        // NOUVEAU : aucun nettoyage
        // automatique des fichiers.
        return 2;
    }
}
