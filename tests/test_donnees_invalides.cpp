
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <string>

#include "Banque.h"
#include "EtatBanque.h"
#include "Operation.h"
#include "Persistance.h"

// ==========================================
// NOUVEAU : TESTS DES DONNEES INVALIDES
// Etape 3.9.3.4 - MiniBank C++
//
// Tous les fichiers eventuels sont limites
// au dossier de test fourni en argument.
//
// Aucun fichier existant n'est supprime.
// ==========================================

namespace {

namespace fs = std::filesystem;

int nombreTests = 0;
int nombreReussites = 0;
int nombreScenarios = 0;

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
        std::cout
            << "[OK]    " << description << '\n';
    } else {
        std::cout
            << "[ECHEC] " << description << '\n';
    }
}

// ==========================================
// NOUVEAU : CREER UNE OPERATION DE REFERENCE
// ==========================================

Operation creerDepotValide(
    const std::string& identifiant
) {
    Operation operation;

    operation.numero = 1;
    operation.type = TypeOperation::Depot;
    operation.identifiantCompte = identifiant;
    operation.montantCentimes = 100;
    operation.identifiantContrepartie = "";

    return operation;
}

// ==========================================
// NOUVEAU : AJOUTER UNE OPERATION
// ==========================================

EtatBanque avecOperation(
    const EtatBanque& reference,
    const Operation& operation
) {
    EtatBanque etat = reference;

    etat.operations.push_back(operation);
    etat.prochainNumeroOperation = 2;

    return etat;
}

// ==========================================
// NOUVEAU : TESTER UN ETAT INVALIDE
// ==========================================

void testerEtatInvalide(
    const fs::path& dossier,
    const std::string& nomFichier,
    const std::string& description,
    const EtatBanque& etat
) {
    ++nombreScenarios;

    std::cout
        << "\n=== " << description << " ===\n";

    // NOUVEAU : noms distincts pour les
    // scenarios, dans le dossier isole.
    const fs::path principal =
        dossier / (nomFichier + ".txt");

    const fs::path precedent =
        dossier / (nomFichier + ".bak");

    // ======================================
    // NOUVEAU : TEST DE SERIALISATION
    // ======================================

    const ResultatSerialisation serialisation =
        Persistance::serialiser(etat);

    verifier(
        serialisation.statut ==
            StatutSerialisation::DonneesInvalides
            && !serialisation.estUnSucces(),
        description + " : serialisation refusee"
    );

    verifier(
        serialisation.texte.empty(),
        description + " : aucun texte produit"
    );

    verifier(
        !serialisation.message.empty(),
        description + " : diagnostic serialisation"
    );

    // ======================================
    // NOUVEAU : TEST DE SAUVEGARDE
    // ======================================

    const ResultatSauvegarde sauvegarde =
        Persistance::sauvegarder(
            etat,
            principal.u8string()
        );

    verifier(
        sauvegarde.statut ==
            StatutSauvegarde::ErreurSerialisation
            && !sauvegarde.estUnSucces(),
        description + " : sauvegarde refusee"
    );

    verifier(
        !sauvegarde.message.empty(),
        description + " : diagnostic sauvegarde"
    );

    verifier(
        !fs::exists(principal),
        description + " : aucun .txt"
    );

    verifier(
        !fs::exists(precedent),
        description + " : aucun .bak"
    );

    // NOUVEAU : si la validation a refuse
    // chaque etat AVANT toute ecriture,
    // le dossier entier reste vide.
    verifier(
        fs::is_empty(dossier),
        description + " : aucun fichier residuel"
    );

    // NOUVEAU : aider au diagnostic
    // si le refus attendu n'est pas obtenu.
    if (serialisation.statut !=
        StatutSerialisation::DonneesInvalides) {

        std::cout
            << "Diagnostic serialisation : "
            << serialisation.message << '\n';
    }

    if (sauvegarde.statut !=
        StatutSauvegarde::ErreurSerialisation) {

        std::cout
            << "Diagnostic sauvegarde : "
            << sauvegarde.message << '\n';
    }
}

// ==========================================
// NOUVEAU : TESTER LES COMPTEURS
// ==========================================

void testerCompteurs(
    const fs::path& dossier,
    const EtatBanque& valide,
    const std::string& alice
) {
    EtatBanque compteNul = valide;
    compteNul.prochainNumeroCompte = 0;

    testerEtatInvalide(
        dossier,
        "compteur_compte_nul",
        "Compteur de compte nul",
        compteNul
    );

    EtatBanque operationNulle = valide;
    operationNulle.prochainNumeroOperation = 0;

    testerEtatInvalide(
        dossier,
        "compteur_operation_nul",
        "Compteur d'operation nul",
        operationNulle
    );

    // NOUVEAU : operation numero 1
    // avec un compteur suivant egal a 1.
    EtatBanque compteurIncoherent = valide;
    compteurIncoherent.operations.push_back(
        creerDepotValide(alice)
    );

    compteurIncoherent.prochainNumeroOperation = 1;

    testerEtatInvalide(
        dossier,
        "compteur_historique_incoherent",
        "Compteur d'historique incoherent",
        compteurIncoherent
    );
}

// ==========================================
// NOUVEAU : TESTER LES COMPTES
// ==========================================

void testerComptes(
    const fs::path& dossier,
    const EtatBanque& valide
) {
    EtatBanque doublon = valide;

    // NOUVEAU : copie d'un Compte valide.
    // On fabrique le doublon sans appeler
    // un constructeur avec des valeurs vides.
    doublon.comptes.push_back(
        doublon.comptes.front()
    );

    testerEtatInvalide(
        dossier,
        "identifiant_duplique",
        "Identifiant de compte duplique",
        doublon
    );
}

// ==========================================
// NOUVEAU : TESTER LES NUMEROS
// ==========================================

void testerNumerotation(
    const fs::path& dossier,
    const EtatBanque& valide,
    const std::string& alice
) {
    const Operation depot =
        creerDepotValide(alice);

    EtatBanque numeroNul =
        avecOperation(valide, depot);

    numeroNul.operations[0].numero = 0;

    testerEtatInvalide(
        dossier,
        "operation_numero_nul",
        "Numero d'operation nul",
        numeroNul
    );

    EtatBanque doublon =
        avecOperation(valide, depot);

    doublon.operations.push_back(depot);
    doublon.prochainNumeroOperation = 3;

    testerEtatInvalide(
        dossier,
        "operations_numeros_dupliques",
        "Numeros d'operations dupliques",
        doublon
    );

    // NOUVEAU : 2 suivi de 1.
    // Ce cas est distinct de deux numeros
    // strictement identiques.
    EtatBanque decroissant =
        avecOperation(valide, depot);

    decroissant.operations[0].numero = 2;
    decroissant.operations.push_back(depot);
    decroissant.prochainNumeroOperation = 3;

    testerEtatInvalide(
        dossier,
        "operations_numeros_decroissants",
        "Numeros d'operations decroissants",
        decroissant
    );
}

// ==========================================
// NOUVEAU : TESTER LES MONTANTS
// ==========================================

void testerMontants(
    const fs::path& dossier,
    const EtatBanque& valide,
    const std::string& alice
) {
    const Operation depot =
        creerDepotValide(alice);

    EtatBanque montantNul =
        avecOperation(valide, depot);

    montantNul.operations[0].montantCentimes = 0;

    testerEtatInvalide(
        dossier,
        "montant_nul",
        "Montant d'operation nul",
        montantNul
    );

    EtatBanque montantNegatif =
        avecOperation(valide, depot);

    montantNegatif.operations[0].montantCentimes = -1;

    testerEtatInvalide(
        dossier,
        "montant_negatif",
        "Montant d'operation negatif",
        montantNegatif
    );
}

// ==========================================
// NOUVEAU : TESTER LES REFERENCES DE COMPTES
// ==========================================

void testerReferencesComptes(
    const fs::path& dossier,
    const EtatBanque& valide,
    const std::string& alice,
    const std::string& bob
) {
    const Operation depot =
        creerDepotValide(alice);

    EtatBanque compteAbsent =
        avecOperation(valide, depot);

    compteAbsent.operations[0].identifiantCompte =
        "COMPTE-INEXISTANT";

    testerEtatInvalide(
        dossier,
        "compte_operation_inexistant",
        "Compte d'operation inexistant",
        compteAbsent
    );

    EtatBanque depotAvecContrepartie =
        avecOperation(valide, depot);

    depotAvecContrepartie.operations[0]
        .identifiantContrepartie = bob;

    testerEtatInvalide(
        dossier,
        "depot_avec_contrepartie",
        "Depot avec contrepartie",
        depotAvecContrepartie
    );

    EtatBanque retraitAvecContrepartie =
        avecOperation(valide, depot);

    retraitAvecContrepartie.operations[0].type =
        TypeOperation::Retrait;

    retraitAvecContrepartie.operations[0]
        .identifiantContrepartie = bob;

    testerEtatInvalide(
        dossier,
        "retrait_avec_contrepartie",
        "Retrait avec contrepartie",
        retraitAvecContrepartie
    );
}

// ==========================================
// NOUVEAU : TESTER LES VIREMENTS INVALIDES
// ==========================================

void testerVirements(
    const fs::path& dossier,
    const EtatBanque& valide,
    const std::string& alice
) {
    const Operation depot =
        creerDepotValide(alice);

    EtatBanque sortantSansContrepartie =
        avecOperation(valide, depot);

    sortantSansContrepartie.operations[0].type =
        TypeOperation::VirementSortant;

    testerEtatInvalide(
        dossier,
        "sortant_sans_contrepartie",
        "Virement sortant sans contrepartie",
        sortantSansContrepartie
    );

    EtatBanque entrantSansContrepartie =
        avecOperation(valide, depot);

    entrantSansContrepartie.operations[0].type =
        TypeOperation::VirementEntrant;

    testerEtatInvalide(
        dossier,
        "entrant_sans_contrepartie",
        "Virement entrant sans contrepartie",
        entrantSansContrepartie
    );

    EtatBanque virementVersSoi =
        avecOperation(valide, depot);

    virementVersSoi.operations[0].type =
        TypeOperation::VirementSortant;

    virementVersSoi.operations[0]
        .identifiantContrepartie = alice;

    testerEtatInvalide(
        dossier,
        "virement_vers_soi",
        "Virement vers le meme compte",
        virementVersSoi
    );

    EtatBanque contrepartieAbsente =
        avecOperation(valide, depot);

    contrepartieAbsente.operations[0].type =
        TypeOperation::VirementEntrant;

    contrepartieAbsente.operations[0]
        .identifiantContrepartie =
            "COMPTE-INEXISTANT";

    testerEtatInvalide(
        dossier,
        "contrepartie_inexistante",
        "Contrepartie inexistante",
        contrepartieAbsente
    );
}

// ==========================================
// NOUVEAU : TESTER UN TYPE INCONNU
// ==========================================

void testerTypeInconnu(
    const fs::path& dossier,
    const EtatBanque& valide,
    const std::string& alice
) {
    EtatBanque etat = avecOperation(
        valide,
        creerDepotValide(alice)
    );

    // NOUVEAU : type volontairement
    // absent de l'enumeration officielle.
    etat.operations[0].type =
        static_cast<TypeOperation>(999);

    testerEtatInvalide(
        dossier,
        "type_operation_inconnu",
        "Type d'operation inconnu",
        etat
    );
}

} // namespace

// ==========================================
// NOUVEAU : PROGRAMME PRINCIPAL
// ==========================================

int main(int argc, char* argv[]) {
    std::cout
        << "====================================\n"
        << " TESTS DONNEES INVALIDES MINIBANK\n"
        << " Etape 3.9.3.4\n"
        << "====================================\n";

    if (argc != 2) {
        std::cerr
            << "Erreur : fournir un dossier de test.\n";
        return 2;
    }

    try {
        const fs::path dossier =
            fs::u8path(argv[1]);

        // NOUVEAU : n'accepter qu'un dossier
        // deja cree et totalement vide.
        if (!fs::is_directory(dossier) ||
            !fs::is_empty(dossier)) {

            std::cerr
                << "Erreur : dossier invalide "
                   "ou non vide.\n";

            return 2;
        }

        verifier(
            fs::is_empty(dossier),
            "Dossier de test initialement vide"
        );

        // ==================================
        // NOUVEAU : ETAT BANCAIRE VALIDE
        // ==================================

        Banque banque;

        const std::string alice =
            banque.creerCompte("Alice", 50000);

        const std::string bob =
            banque.creerCompte("Bob", 120000);

        verifier(
            alice == "MB-0001"
                && bob == "MB-0002",
            "Banque de reference creee"
        );

        const EtatBanque reference =
            banque.exporterEtat();

        const ResultatSerialisation texteAvant =
            Persistance::serialiser(reference);

        verifier(
            texteAvant.estUnSucces(),
            "Etat de reference valide"
        );

        // ==================================
        // NOUVEAU : SCENARIOS INVALIDES
        // ==================================

        testerCompteurs(
            dossier,
            reference,
            alice
        );

        testerComptes(
            dossier,
            reference
        );

        testerNumerotation(
            dossier,
            reference,
            alice
        );

        testerMontants(
            dossier,
            reference,
            alice
        );

        testerReferencesComptes(
            dossier,
            reference,
            alice,
            bob
        );

        testerVirements(
            dossier,
            reference,
            alice
        );

        testerTypeInconnu(
            dossier,
            reference,
            alice
        );

        // ==================================
        // NOUVEAU : NON-REGRESSION
        // ==================================

        const ResultatSerialisation texteApres =
            Persistance::serialiser(
                banque.exporterEtat()
            );

        verifier(
            texteAvant.estUnSucces()
                && texteApres.estUnSucces()
                && texteAvant.texte == texteApres.texte,
            "Banque d'origine totalement inchangee"
        );

        verifier(
            banque.nombreComptes() == 2
                && banque.nombreOperations() == 0,
            "Comptes et historique d'origine preserves"
        );

        verifier(
            fs::is_empty(dossier),
            "Aucun fichier cree dans le dossier"
        );

        // ==================================
        // NOUVEAU : BILAN
        // ==================================

        std::cout
            << "\n====================================\n"
            << "Scenarios invalides : "
            << nombreScenarios << '\n'
            << "Tests executes : "
            << nombreTests << '\n'
            << "Tests reussis : "
            << nombreReussites << '\n'
            << "Tests echoues : "
            << nombreTests - nombreReussites << '\n'
            << "====================================\n";

        // NOUVEAU : 0 seulement si
        // toutes les verifications passent.
        return nombreTests == nombreReussites
            ? 0
            : 1;

    } catch (const std::exception& erreur) {
        std::cerr
            << "Erreur pendant les tests : "
            << erreur.what() << '\n';

        // NOUVEAU : ne jamais supprimer
        // automatiquement des fichiers.
        return 2;
    }
}
