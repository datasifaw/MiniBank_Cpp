
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

#include "Banque.h"
#include "EtatBanque.h"
#include "Persistance.h"

// ==========================================
// NOUVEAU : TESTS DE PUBLICATION WINDOWS
// Etape 3.9.3.3
//
// Chaque scenario utilise un sous-dossier
// distinct d'un dossier de test neuf.
//
// Aucun fichier bancaire reel n'est modifie.
// Aucun nettoyage automatique n'est effectue.
// ==========================================

namespace {

namespace fs = std::filesystem;

int nombreTests = 0;
int nombreReussites = 0;

// ==========================================
// NOUVEAU : VERIFIER UNE CONDITION
// ==========================================

void verifier(
    bool condition,
    const std::string& description
) {
    ++nombreTests;

    if (condition) {
        ++nombreReussites;
        std::cout << "[OK]    "
                  << description << '\n';
    } else {
        std::cout << "[ECHEC] "
                  << description << '\n';
    }
}

// ==========================================
// NOUVEAU : CREER UN DOSSIER DE SCENARIO
// ==========================================

fs::path creerDossierScenario(
    const fs::path& racine,
    const std::string& nom
) {
    const fs::path dossier = racine / nom;

    // NOUVEAU : refuser toute reutilisation.
    // Les anciens fichiers doivent rester intacts.
    if (!fs::create_directory(dossier)) {
        throw std::runtime_error(
            "Le dossier du scenario existe deja : "
            + dossier.u8string()
        );
    }

    return dossier;
}

// ==========================================
// NOUVEAU : LIRE UN FICHIER BINAIRE
// ==========================================

bool lireFichier(
    const fs::path& chemin,
    std::string& contenu
) {
    contenu.clear();

    std::ifstream entree(
        chemin,
        std::ios::in | std::ios::binary
    );

    if (!entree.is_open()) {
        return false;
    }

    const std::string texte(
        (std::istreambuf_iterator<char>(entree)),
        std::istreambuf_iterator<char>()
    );

    if (entree.bad()) {
        return false;
    }

    // NOUVEAU : effacer les indicateurs
    // avant de controler la fermeture.
    entree.clear();
    entree.close();

    if (entree.fail()) {
        return false;
    }

    contenu = texte;
    return true;
}

// ==========================================
// NOUVEAU : COMPARER UN FICHIER AU TEXTE
// ==========================================

bool fichierIdentique(
    const fs::path& chemin,
    const std::string& attendu
) {
    std::string contenu;

    return lireFichier(chemin, contenu)
        && contenu == attendu;
}

// ==========================================
// NOUVEAU : PREPARER UN FICHIER DE TEST
// ==========================================

bool ecrireTexteTest(
    const fs::path& chemin,
    const std::string& texte
) {
    // NOUVEAU : cet utilitaire doit servir
    // uniquement a des fichiers du dossier
    // de test, jamais aux donnees reelles.
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
        texte.data(),
        static_cast<std::streamsize>(texte.size())
    );

    const bool ecritureOK = sortie.good();

    sortie.close();

    return ecritureOK && !sortie.fail();
}

// ==========================================
// NOUVEAU : DETECTER LES TEMPORAIRES
// ==========================================

std::size_t compterTemporaires(
    const fs::path& dossier
) {
    std::size_t total = 0;

    for (const fs::directory_entry& entree :
         fs::directory_iterator(dossier)) {

        if (entree.path().extension() == ".tmp") {
            ++total;
        }
    }

    return total;
}

// ==========================================
// NOUVEAU : PREMIERE SAUVEGARDE,
// REMPLACEMENT ET PROTECTION DU .BAK
// ==========================================

void testerPublicationComplete(
    const fs::path& racine
) {
    std::cout
        << "\n=== PUBLICATION COMPLETE ===\n";

    const fs::path dossier =
        creerDossierScenario(
            racine,
            "publication_complete"
        );

    const fs::path principal =
        dossier / "scenario.txt";

    const fs::path precedent =
        dossier / "scenario.bak";

    Banque banque;

    const std::string alice =
        banque.creerCompte("Alice", 50000);

    const std::string bob =
        banque.creerCompte("Bob", 120000);

    verifier(
        alice == "MB-0001"
            && bob == "MB-0002",
        "Comptes de test crees"
    );

    // ======================================
    // NOUVEAU : PREMIERE SAUVEGARDE
    // ======================================

    const EtatBanque etatA =
        banque.exporterEtat();

    const ResultatSerialisation texteA =
        Persistance::serialiser(etatA);

    verifier(
        texteA.estUnSucces(),
        "Premier etat serialise"
    );

    const ResultatSauvegarde premier =
        Persistance::sauvegarder(
            etatA,
            principal.u8string()
        );

    verifier(
        premier.estUnSucces(),
        "Premiere sauvegarde : OK"
    );

    verifier(
        premier.statut ==
            StatutSauvegarde::Succes,
        "Premiere sauvegarde : statut Succes"
    );

    verifier(
        fs::is_regular_file(principal),
        "Premiere sauvegarde : .txt cree"
    );

    verifier(
        texteA.estUnSucces()
            && fichierIdentique(
                principal,
                texteA.texte
            ),
        "Premiere sauvegarde : octets exacts"
    );

    verifier(
        !fs::exists(precedent),
        "Premiere sauvegarde : aucun .bak"
    );

    verifier(
        compterTemporaires(dossier) == 0,
        "Premiere sauvegarde : aucun .tmp residuel"
    );

    // ======================================
    // NOUVEAU : MODIFIER LA BANQUE
    // ======================================

    const bool depotOK =
        banque.deposerSurCompte(
            alice,
            10000
        );

    verifier(
        depotOK,
        "Depot supplementaire execute"
    );

    const EtatBanque etatB =
        banque.exporterEtat();

    const ResultatSerialisation texteB =
        Persistance::serialiser(etatB);

    verifier(
        texteB.estUnSucces(),
        "Nouvel etat serialise"
    );

    verifier(
        texteA.estUnSucces()
            && texteB.estUnSucces()
            && texteA.texte != texteB.texte,
        "Ancien et nouvel etat differents"
    );

    // ======================================
    // NOUVEAU : REMPLACER LE PRINCIPAL
    // ======================================

    const ResultatSauvegarde deuxieme =
        Persistance::sauvegarder(
            etatB,
            principal.u8string()
        );

    verifier(
        deuxieme.estUnSucces(),
        "Remplacement et backup : OK"
    );

    verifier(
        deuxieme.statut ==
            StatutSauvegarde::Succes,
        "Remplacement : statut Succes"
    );

    verifier(
        texteB.estUnSucces()
            && fichierIdentique(
                principal,
                texteB.texte
            ),
        "Remplacement : nouveau contenu .txt"
    );

    verifier(
        fs::is_regular_file(precedent),
        "Remplacement : fichier .bak cree"
    );

    verifier(
        texteA.estUnSucces()
            && fichierIdentique(
                precedent,
                texteA.texte
            ),
        "Remplacement : ancien contenu dans .bak"
    );

    verifier(
        compterTemporaires(dossier) == 0,
        "Remplacement : aucun .tmp residuel"
    );

    // ======================================
    // NOUVEAU : TROISIEME SAUVEGARDE
    // ======================================

    const ResultatSauvegarde troisieme =
        Persistance::sauvegarder(
            etatB,
            principal.u8string()
        );

    verifier(
        !troisieme.estUnSucces(),
        "Troisieme sauvegarde : refus"
    );

    verifier(
        troisieme.statut ==
            StatutSauvegarde::
                SauvegardePrecedenteExistante,
        "Protection du .bak : bon statut"
    );

    verifier(
        !troisieme.message.empty(),
        "Protection du .bak : message present"
    );

    verifier(
        texteB.estUnSucces()
            && fichierIdentique(
                principal,
                texteB.texte
            ),
        "Troisieme essai : principal inchange"
    );

    verifier(
        texteA.estUnSucces()
            && fichierIdentique(
                precedent,
                texteA.texte
            ),
        "Troisieme essai : backup inchange"
    );

    verifier(
        compterTemporaires(dossier) == 0,
        "Troisieme essai : aucun .tmp nouveau"
    );

    verifier(
        banque.nombreComptes() == 2
            && banque.nombreOperations() == 1,
        "Publication : banque en memoire preservee"
    );

    // NOUVEAU : afficher les diagnostics
    // si une operation a ete refusee.
    if (!premier.estUnSucces()) {
        std::cout
            << "Diagnostic premiere sauvegarde : "
            << premier.message << '\n';
    }

    if (!deuxieme.estUnSucces()) {
        std::cout
            << "Diagnostic remplacement : "
            << deuxieme.message << '\n';
    }

    if (troisieme.statut !=
        StatutSauvegarde::
            SauvegardePrecedenteExistante) {

        std::cout
            << "Diagnostic protection .bak : "
            << troisieme.message << '\n';
    }
}

// ==========================================
// NOUVEAU : TESTER LES CHEMINS INVALIDES
// ==========================================

void testerCheminsInvalides(
    const fs::path& racine
) {
    std::cout
        << "\n=== CHEMINS INVALIDES ===\n";

    const fs::path dossier =
        creerDossierScenario(
            racine,
            "chemins_invalides"
        );

    const Banque banque;
    const EtatBanque etat =
        banque.exporterEtat();

    // ======================================
    // NOUVEAU : CHEMIN VIDE
    // ======================================

    const ResultatSauvegarde vide =
        Persistance::sauvegarder(
            etat,
            ""
        );

    verifier(
        vide.statut ==
            StatutSauvegarde::CheminInvalide,
        "Refus : chemin vide"
    );

    // ======================================
    // NOUVEAU : EXTENSION INCORRECTE
    // ======================================

    const fs::path mauvais =
        dossier / "interdit.tmp";

    const ResultatSauvegarde extension =
        Persistance::sauvegarder(
            etat,
            mauvais.u8string()
        );

    verifier(
        extension.statut ==
            StatutSauvegarde::CheminInvalide,
        "Refus : extension .tmp"
    );

    verifier(
        !fs::exists(mauvais),
        "Extension .tmp : aucun fichier cree"
    );

    const fs::path sansExtension =
        dossier / "sans_extension";

    const ResultatSauvegarde sansExt =
        Persistance::sauvegarder(
            etat,
            sansExtension.u8string()
        );

    verifier(
        sansExt.statut ==
            StatutSauvegarde::CheminInvalide,
        "Refus : fichier sans extension"
    );

    verifier(
        !fs::exists(sansExtension),
        "Sans extension : aucun fichier cree"
    );

    // ======================================
    // NOUVEAU : DOSSIER INEXISTANT
    // ======================================

    const fs::path dossierManquant =
        dossier / "inexistant";

    const fs::path cheminManquant =
        dossierManquant / "scenario.txt";

    const ResultatSauvegarde manquant =
        Persistance::sauvegarder(
            etat,
            cheminManquant.u8string()
        );

    verifier(
        manquant.statut ==
            StatutSauvegarde::CheminInvalide,
        "Refus : dossier parent inexistant"
    );

    verifier(
        !fs::exists(dossierManquant),
        "Dossier parent non cree"
    );

    // ======================================
    // NOUVEAU : PRINCIPAL EST UN DOSSIER
    // ======================================

    const fs::path fauxPrincipal =
        dossier / "dossier.txt";

    fs::create_directory(fauxPrincipal);

    const ResultatSauvegarde dossierAuLieuFichier =
        Persistance::sauvegarder(
            etat,
            fauxPrincipal.u8string()
        );

    verifier(
        dossierAuLieuFichier.statut ==
            StatutSauvegarde::
                FichierPrincipalInvalide,
        "Refus : dossier au lieu d'un .txt"
    );

    verifier(
        fs::is_directory(fauxPrincipal),
        "Dossier existant preserve"
    );

    verifier(
        compterTemporaires(dossier) == 0,
        "Chemins invalides : aucun .tmp"
    );
}

// ==========================================
// NOUVEAU : PRINCIPAL INVALIDE
// ==========================================

void testerPrincipalInvalide(
    const fs::path& racine
) {
    std::cout
        << "\n=== PRINCIPAL INVALIDE ===\n";

    const fs::path dossier =
        creerDossierScenario(
            racine,
            "principal_invalide"
        );

    const fs::path principal =
        dossier / "scenario.txt";

    const fs::path precedent =
        dossier / "scenario.bak";

    // NOUVEAU : fichier volontairement
    // mal forme, dans le dossier isole.
    const std::string ancienTexte =
        "FORMAT_INCONNU\n"
        "DONNEES_INCORRECTES\n"
        "FIN_INCONNUE\n";

    verifier(
        ecrireTexteTest(
            principal,
            ancienTexte
        ),
        "Principal invalide : fichier prepare"
    );

    Banque banque;
    banque.creerCompte("Alice", 50000);

    const ResultatSauvegarde resultat =
        Persistance::sauvegarder(
            banque.exporterEtat(),
            principal.u8string()
        );

    verifier(
        resultat.statut ==
            StatutSauvegarde::
                FichierPrincipalInvalide,
        "Principal invalide : remplacement refuse"
    );

    verifier(
        fichierIdentique(
            principal,
            ancienTexte
        ),
        "Principal invalide : contenu conserve"
    );

    verifier(
        !fs::exists(precedent),
        "Principal invalide : aucun .bak"
    );

    verifier(
        compterTemporaires(dossier) == 0,
        "Principal invalide : aucun .tmp"
    );

    verifier(
        !resultat.message.empty(),
        "Principal invalide : diagnostic present"
    );
}

// ==========================================
// NOUVEAU : DONNEES BANCAIRES INVALIDES
// ==========================================

void testerDonneesInvalides(
    const fs::path& racine
) {
    std::cout
        << "\n=== DONNEES INVALIDES ===\n";

    const fs::path dossier =
        creerDossierScenario(
            racine,
            "donnees_invalides"
        );

    const fs::path principal =
        dossier / "scenario.txt";

    const fs::path precedent =
        dossier / "scenario.bak";

    Banque banque;
    banque.creerCompte("Alice", 50000);

    EtatBanque etat =
        banque.exporterEtat();

    // NOUVEAU : un compteur nul rend
    // volontairement l'instantane invalide.
    // La Banque d'origine reste correcte.
    etat.prochainNumeroCompte = 0;

    const ResultatSauvegarde resultat =
        Persistance::sauvegarder(
            etat,
            principal.u8string()
        );

    verifier(
        resultat.statut ==
            StatutSauvegarde::
                ErreurSerialisation,
        "Etat invalide : sauvegarde refusee"
    );

    verifier(
        !resultat.estUnSucces(),
        "Etat invalide : pas de faux succes"
    );

    verifier(
        !resultat.message.empty(),
        "Etat invalide : diagnostic present"
    );

    verifier(
        !fs::exists(principal),
        "Etat invalide : aucun .txt cree"
    );

    verifier(
        !fs::exists(precedent),
        "Etat invalide : aucun .bak cree"
    );

    verifier(
        compterTemporaires(dossier) == 0,
        "Etat invalide : aucun .tmp cree"
    );

    verifier(
        banque.nombreComptes() == 1
            && banque.nombreOperations() == 0
            && banque.exporterEtat()
                    .prochainNumeroCompte == 2,
        "Etat invalide : Banque d'origine inchangee"
    );
}

// ==========================================
// NOUVEAU : BACKUP PREEXISTANT
// ==========================================

void testerBackupPreexistant(
    const fs::path& racine
) {
    std::cout
        << "\n=== BACKUP PREEXISTANT ===\n";

    const fs::path dossier =
        creerDossierScenario(
            racine,
            "backup_preexistant"
        );

    const fs::path principal =
        dossier / "scenario.txt";

    const fs::path precedent =
        dossier / "scenario.bak";

    // NOUVEAU : une ancienne sauvegarde
    // existe deja, meme sans principal.
    const std::string sentinelle =
        "BACKUP_A_PRESERVER\n";

    verifier(
        ecrireTexteTest(
            precedent,
            sentinelle
        ),
        "Backup preexistant : fichier prepare"
    );

    const Banque banque;

    const ResultatSauvegarde resultat =
        Persistance::sauvegarder(
            banque.exporterEtat(),
            principal.u8string()
        );

    verifier(
        resultat.statut ==
            StatutSauvegarde::
                SauvegardePrecedenteExistante,
        "Backup preexistant : publication refusee"
    );

    verifier(
        fichierIdentique(
            precedent,
            sentinelle
        ),
        "Backup preexistant : contenu preserve"
    );

    verifier(
        !fs::exists(principal),
        "Backup preexistant : principal non cree"
    );

    verifier(
        compterTemporaires(dossier) == 0,
        "Backup preexistant : aucun .tmp"
    );
}

// ==========================================
// NOUVEAU : BACKUP OCCUPE PAR UN DOSSIER
// ==========================================

void testerBackupDossier(
    const fs::path& racine
) {
    std::cout
        << "\n=== BACKUP OCCUPE PAR UN DOSSIER ===\n";

    const fs::path dossier =
        creerDossierScenario(
            racine,
            "backup_dossier"
        );

    const fs::path principal =
        dossier / "scenario.txt";

    const fs::path precedent =
        dossier / "scenario.bak";

    // NOUVEAU : un dossier occupe le chemin
    // reserve au fichier .bak.
    fs::create_directory(precedent);

    const Banque banque;

    const ResultatSauvegarde resultat =
        Persistance::sauvegarder(
            banque.exporterEtat(),
            principal.u8string()
        );

    verifier(
        resultat.statut ==
            StatutSauvegarde::
                SauvegardePrecedenteExistante,
        "Chemin .bak occupe : sauvegarde refusee"
    );

    verifier(
        fs::is_directory(precedent),
        "Chemin .bak occupe : dossier preserve"
    );

    verifier(
        !fs::exists(principal),
        "Chemin .bak occupe : aucun principal"
    );

    verifier(
        compterTemporaires(dossier) == 0,
        "Chemin .bak occupe : aucun .tmp"
    );
}

} // namespace

// ==========================================
// NOUVEAU : PROGRAMME PRINCIPAL DE TEST
// ==========================================

int main(int argc, char* argv[]) {
    std::cout
        << "====================================\n"
        << " TESTS PUBLICATION WINDOWS\n"
        << " Etape 3.9.3.3\n"
        << "====================================\n";

    if (argc != 2) {
        std::cerr
            << "Erreur : fournir un dossier de test "
               "neuf en argument.\n";
        return 2;
    }

    try {
        const fs::path racine =
            fs::u8path(argv[1]);

        // NOUVEAU : nous exigeons un dossier
        // prealablement cree et totalement vide.
        if (!fs::is_directory(racine)) {
            std::cerr
                << "Erreur : dossier inexistant "
                   "ou invalide.\n";
            return 2;
        }

        if (fs::directory_iterator(racine) !=
            fs::directory_iterator()) {

            std::cerr
                << "Erreur : le dossier de test "
                   "n'est pas vide.\n";
            return 2;
        }

        testerPublicationComplete(racine);
        testerCheminsInvalides(racine);
        testerPrincipalInvalide(racine);
        testerDonneesInvalides(racine);
        testerBackupPreexistant(racine);
        testerBackupDossier(racine);

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

        // NOUVEAU : code 0 seulement si
        // toutes les verifications reussissent.
        return nombreTests == nombreReussites
            ? 0
            : 1;

    } catch (const std::exception& erreur) {
        std::cerr
            << "Erreur pendant les tests Windows : "
            << erreur.what() << '\n';

        // NOUVEAU : ne jamais supprimer
        // automatiquement des fichiers
        // apres une erreur.
        return 2;
    }
}
