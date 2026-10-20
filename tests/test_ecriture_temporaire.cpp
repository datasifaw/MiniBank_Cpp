
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <cstdint>

#include "Banque.h"
#include "EtatBanque.h"
#include "Persistance.h"

// ==========================================
// NOUVEAU : TESTS DES FICHIERS TEMPORAIRES
// Etape 3.9.3.2
//
// Les tests utilisent exclusivement
// un dossier isole fourni par PowerShell.
// ==========================================

namespace {

namespace fs = std::filesystem;

int nombreTests = 0;
int nombreReussites = 0;

// ==========================================
// NOUVEAU : VERIFICATION D'UN TEST
// ==========================================

void verifier(
    bool condition,
    const std::string& description
) {
    ++nombreTests;

    if (condition) {
        ++nombreReussites;
        std::cout << "[OK]    " << description << '\n';
    } else {
        std::cout << "[ECHEC] " << description << '\n';
    }
}

// ==========================================
// NOUVEAU : LECTURE BINAIRE DE TEST
// ==========================================

std::string lireFichier(const fs::path& chemin) {
    std::ifstream entree(
        chemin,
        std::ios::in | std::ios::binary
    );

    if (!entree.is_open()) {
        return "";
    }

    const std::string contenu(
        (std::istreambuf_iterator<char>(entree)),
        std::istreambuf_iterator<char>()
    );

    if (entree.bad()) {
        return "";
    }

    return contenu;
}

// ==========================================
// NOUVEAU : CONSTRUIRE UN CHEMIN ISOLE
// ==========================================

fs::path cheminDans(
    const fs::path& dossier,
    const std::string& nom
) {
    return dossier / nom;
}

// ==========================================
// NOUVEAU : TESTS D'UNE BANQUE VIDE
// ==========================================

void testerBanqueVide(const fs::path& dossier) {
    std::cout << "\n=== BANQUE VIDE ===\n";

    const Banque banque;
    const EtatBanque etat = banque.exporterEtat();

    const fs::path chemin =
        cheminDans(dossier, "banque_vide.tmp");

    const ResultatEcritureTemporaire resultat =
        Persistance::ecrireFichierTemporaire(
            etat, chemin.u8string()
        );

    const std::string attendu =
        "MINIBANK|1\n"
        "COUNTERS|1|1\n"
        "ACCOUNTS|0\n"
        "OPERATIONS|0\n"
        "END\n";

    verifier(
        resultat.estUnSucces(),
        "Banque vide : ecriture reussie"
    );

    verifier(
        resultat.statut ==
            StatutEcritureTemporaire::Succes,
        "Banque vide : statut Succes"
    );

    verifier(
        fs::is_regular_file(chemin),
        "Banque vide : fichier cree"
    );

    verifier(
        lireFichier(chemin) == attendu,
        "Banque vide : contenu exact"
    );

    verifier(
        etat.comptes.empty()
            && etat.operations.empty(),
        "Banque vide : instantane inchange"
    );
}

// ==========================================
// NOUVEAU : TESTS DE COMPTES ET HISTORIQUE
// ==========================================

void testerComptesEtHistorique(
    const fs::path& dossier
) {
    std::cout << "\n=== COMPTES ET HISTORIQUE ===\n";

    Banque banque;

    const std::string alice =
        banque.creerCompte("Alice", 50000);

    const std::string bob =
        banque.creerCompte("Bob", 120000);

    const bool operationsOK =
        !alice.empty()
        && !bob.empty()
        && banque.deposerSurCompte(alice, 10000)
        && banque.retirerDuCompte(alice, 5000)
        && banque.virer(alice, bob, 2500);

    verifier(
        operationsOK,
        "Depot, retrait et virement executes"
    );

    const EtatBanque etat = banque.exporterEtat();

    const ResultatSerialisation serialisation =
        Persistance::serialiser(etat);

    const fs::path chemin =
        cheminDans(dossier, "comptes_historique.tmp");

    const ResultatEcritureTemporaire resultat =
        Persistance::ecrireFichierTemporaire(
            etat, chemin.u8string()
        );

    verifier(
        resultat.estUnSucces(),
        "Comptes : ecriture temporaire reussie"
    );

    verifier(
        serialisation.estUnSucces(),
        "Comptes : serialisation reussie"
    );

    const std::string texte = lireFichier(chemin);

    verifier(
        serialisation.estUnSucces()
            && texte == serialisation.texte,
        "Comptes : octets identiques"
    );

    verifier(
        texte.find("COUNTERS|3|5\n") !=
            std::string::npos,
        "Comptes : compteurs conserves"
    );

    verifier(
        texte.find("ACCOUNTS|2\n") !=
            std::string::npos,
        "Comptes : deux comptes conserves"
    );

    verifier(
        texte.find("OPERATIONS|4\n") !=
            std::string::npos,
        "Comptes : quatre mouvements conserves"
    );

    verifier(
        texte.find(
            "ACCOUNT|4D422D30303031|"
            "416C696365|52500\n"
        ) != std::string::npos,
        "Comptes : solde final Alice conserve"
    );

    verifier(
        texte.find(
            "ACCOUNT|4D422D30303032|"
            "426F62|122500\n"
        ) != std::string::npos,
        "Comptes : solde final Bob conserve"
    );

    verifier(
        banque.nombreComptes() == 2
            && banque.nombreOperations() == 4,
        "Comptes : banque en memoire inchangee"
    );
}

// ==========================================
// NOUVEAU : CHEMINS INVALIDES
// ==========================================

void testerCheminsInvalides(
    const fs::path& dossier
) {
    std::cout << "\n=== CHEMINS INVALIDES ===\n";

    const Banque banque;
    const EtatBanque etat = banque.exporterEtat();

    const ResultatEcritureTemporaire vide =
        Persistance::ecrireFichierTemporaire(
            etat, ""
        );

    verifier(
        vide.statut ==
            StatutEcritureTemporaire::CheminInvalide,
        "Refus : chemin vide"
    );

    const fs::path extensionTxt =
        cheminDans(dossier, "interdit.txt");

    const ResultatEcritureTemporaire txt =
        Persistance::ecrireFichierTemporaire(
            etat, extensionTxt.u8string()
        );

    verifier(
        txt.statut ==
            StatutEcritureTemporaire::CheminInvalide,
        "Refus : extension .txt"
    );

    verifier(
        !fs::exists(extensionTxt),
        "Chemin .txt : aucun fichier cree"
    );

    const fs::path sansExtension =
        cheminDans(dossier, "sans_extension");

    const ResultatEcritureTemporaire sansExt =
        Persistance::ecrireFichierTemporaire(
            etat, sansExtension.u8string()
        );

    verifier(
        sansExt.statut ==
            StatutEcritureTemporaire::CheminInvalide,
        "Refus : absence d'extension .tmp"
    );

    const fs::path sousDossier =
        cheminDans(dossier, "dossier_inexistant");

    const fs::path fichierImpossible =
        sousDossier / "interdit.tmp";

    const ResultatEcritureTemporaire introuvable =
        Persistance::ecrireFichierTemporaire(
            etat, fichierImpossible.u8string()
        );

    verifier(
        introuvable.statut ==
            StatutEcritureTemporaire::CheminInvalide,
        "Refus : dossier inexistant"
    );

    verifier(
        !fs::exists(sousDossier),
        "Dossier manquant non cree"
    );
}

// ==========================================
// NOUVEAU : REFUS D'ECRASEMENT
// ==========================================

void testerRefusEcrasement(
    const fs::path& dossier
) {
    std::cout << "\n=== REFUS D'ECRASEMENT ===\n";

    const Banque banque;
    const EtatBanque etat = banque.exporterEtat();

    const fs::path chemin =
        cheminDans(dossier, "deja_present.tmp");

    const ResultatEcritureTemporaire premier =
        Persistance::ecrireFichierTemporaire(
            etat, chemin.u8string()
        );

    const std::string avant = lireFichier(chemin);

    const ResultatEcritureTemporaire deuxieme =
        Persistance::ecrireFichierTemporaire(
            etat, chemin.u8string()
        );

    const std::string apres = lireFichier(chemin);

    verifier(
        premier.estUnSucces(),
        "Premier fichier temporaire cree"
    );

    verifier(
        deuxieme.statut ==
            StatutEcritureTemporaire::
                FichierDejaExistant,
        "Deuxieme ecriture refusee"
    );

    verifier(
        avant == apres && !avant.empty(),
        "Fichier existant non modifie"
    );
}

// ==========================================
// NOUVEAU : DONNEES INVALIDES
// ==========================================

void testerDonneesInvalides(
    const fs::path& dossier
) {
    std::cout << "\n=== DONNEES INVALIDES ===\n";

    Banque banque;
    banque.creerCompte("Alice", 50000);

    EtatBanque etat = banque.exporterEtat();

    // NOUVEAU : rendre une copie incoherente
    // sans modifier la classe Banque.
    etat.prochainNumeroCompte = 0;

    const fs::path chemin =
        cheminDans(dossier, "donnees_invalides.tmp");

    const ResultatEcritureTemporaire resultat =
        Persistance::ecrireFichierTemporaire(
            etat, chemin.u8string()
        );

    verifier(
        resultat.statut ==
            StatutEcritureTemporaire::
                ErreurSerialisation,
        "Etat invalide : serialisation refusee"
    );

    verifier(
        !fs::exists(chemin),
        "Etat invalide : aucun fichier cree"
    );
}

// ==========================================
// NOUVEAU : PROTECTION DU FICHIER PRINCIPAL
// ==========================================

void testerProtectionPrincipal(
    const fs::path& dossier
) {
    std::cout << "\n=== PROTECTION PRINCIPAL ===\n";

    const Banque banque;
    const EtatBanque etat = banque.exporterEtat();

    // NOUVEAU : le module d'ecriture temporaire
    // ne doit pas accepter un fichier principal
    // se terminant par .txt.
    const fs::path principal =
        cheminDans(dossier, "minibank.txt");

    const ResultatEcritureTemporaire resultat =
        Persistance::ecrireFichierTemporaire(
            etat, principal.u8string()
        );

    verifier(
        resultat.statut ==
            StatutEcritureTemporaire::CheminInvalide,
        "Fichier principal .txt refuse"
    );

    verifier(
        !fs::exists(principal),
        "Fichier principal non cree"
    );
}

} // namespace

// ==========================================
// NOUVEAU : PROGRAMME PRINCIPAL
// ==========================================

int main(int argc, char* argv[]) {
    std::cout
        << "====================================\n"
        << " TESTS ECRITURE TEMPORAIRE\n"
        << " Etape 3.9.3.2\n"
        << "====================================\n";

    if (argc != 2) {
        std::cerr
            << "Erreur : dossier de test requis.\n";
        return 2;
    }

    const fs::path dossier = fs::u8path(argv[1]);

    // NOUVEAU : le programme doit recevoir
    // un dossier deja cree et isole.
    if (!fs::is_directory(dossier)) {
        std::cerr
            << "Erreur : dossier de test invalide.\n";
        return 2;
    }

    testerBanqueVide(dossier);
    testerComptesEtHistorique(dossier);
    testerCheminsInvalides(dossier);
    testerRefusEcrasement(dossier);
    testerDonneesInvalides(dossier);
    testerProtectionPrincipal(dossier);

    std::cout
        << "\n====================================\n"
        << "Tests executes : " << nombreTests << '\n'
        << "Tests reussis : " << nombreReussites << '\n'
        << "Tests echoues : "
        << nombreTests - nombreReussites << '\n'
        << "====================================\n";

    return nombreTests == nombreReussites ? 0 : 1;
}
