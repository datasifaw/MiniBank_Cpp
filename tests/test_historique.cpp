
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>
#include <limits>

// ==========================================
// NOUVEAU : TESTS DU MOTEUR D'HISTORIQUE
// ==========================================

#include "Banque.h"
#include "Operation.h"

int main() {

    // ==========================================
    // NOUVEAU : OUTIL DE VERIFICATION
    // ==========================================

    int testsReussis = 0;
    int testsEchoues = 0;

    auto verifier = [&](bool condition,
                        const std::string& nomTest) {

        if (condition) {
            std::cout << "[OK] " << nomTest << '\n';
            ++testsReussis;
        } else {
            std::cout << "[ECHEC] " << nomTest << '\n';
            ++testsEchoues;
        }
    };

    std::cout << "=== TESTS DE L'HISTORIQUE ===\n\n";

    // ==========================================
    // NOUVEAU : INITIALISER LA BANQUE
    // ==========================================

    Banque banque;

    verifier(
        banque.nombreOperations() == 0,
        "01 - Une nouvelle banque a un historique vide"
    );

    std::string idAlice =
        banque.creerCompte("Alice Martin", 50000);

    std::string idBob =
        banque.creerCompte("Bob Dupont", 120000);

    verifier(
        idAlice == "MB-0001" &&
        idBob == "MB-0002",
        "02 - Identifiants automatiques corrects"
    );

    verifier(
        banque.nombreComptes() == 2,
        "03 - Deux comptes crees"
    );

    verifier(
        banque.nombreOperations() == 0,
        "04 - Creer des comptes ne cree aucun mouvement"
    );

    // NOUVEAU : la consultation initiale doit
    // retourner une collection vide.
    verifier(
        banque.consulterHistorique(idAlice).empty() &&
        banque.consulterHistorique(idBob).empty(),
        "05 - Historiques initiaux vides"
    );

    // NOUVEAU : recuperer les comptes APRES
    // leur creation pour eviter une invalidation
    // des pointeurs par std::vector.
    const Banque& banqueLecture = banque;

    const Compte* alice =
        banqueLecture.rechercherCompte(idAlice);

    const Compte* bob =
        banqueLecture.rechercherCompte(idBob);

    verifier(
        alice != nullptr && bob != nullptr,
        "06 - Alice et Bob retrouvables"
    );

    // NOUVEAU : si les comptes sont absents,
    // les tests suivants ne sont pas possibles.
    if (alice == nullptr || bob == nullptr) {
        std::cout << "\nImpossible de poursuivre.\n";
        return 1;
    }

    // ==========================================
    // NOUVEAU : TESTER UN DEPOT REUSSI
    // ==========================================

    verifier(
        banque.deposerSurCompte(idAlice, 10000),
        "07 - Depot de 100 EUR accepte"
    );

    verifier(
        alice->getSoldeCentimes() == 60000,
        "08 - Solde Alice apres depot : 600 EUR"
    );

    verifier(
        banque.nombreOperations() == 1,
        "09 - Le depot ajoute une seule ligne"
    );

    std::vector<Operation> historiqueAlice =
        banque.consulterHistorique(idAlice);

    verifier(
        historiqueAlice.size() == 1,
        "10 - Alice possede une ligne apres depot"
    );

    verifier(
        historiqueAlice.size() == 1 &&
        historiqueAlice[0].numero == 1 &&
        historiqueAlice[0].type == TypeOperation::Depot &&
        historiqueAlice[0].identifiantCompte == idAlice &&
        historiqueAlice[0].montantCentimes == 10000 &&
        historiqueAlice[0].identifiantContrepartie.empty(),
        "11 - Ligne du depot correctement renseignee"
    );

    // ==========================================
    // NOUVEAU : TESTER UN RETRAIT REUSSI
    // ==========================================

    verifier(
        banque.retirerDuCompte(idAlice, 5000),
        "12 - Retrait de 50 EUR accepte"
    );

    verifier(
        alice->getSoldeCentimes() == 55000,
        "13 - Solde Alice apres retrait : 550 EUR"
    );

    verifier(
        banque.nombreOperations() == 2,
        "14 - Le retrait ajoute une seule ligne"
    );

    historiqueAlice =
        banque.consulterHistorique(idAlice);

    verifier(
        historiqueAlice.size() == 2,
        "15 - Alice possede deux lignes"
    );

    verifier(
        historiqueAlice.size() == 2 &&
        historiqueAlice[1].numero == 2 &&
        historiqueAlice[1].type == TypeOperation::Retrait &&
        historiqueAlice[1].identifiantCompte == idAlice &&
        historiqueAlice[1].montantCentimes == 5000 &&
        historiqueAlice[1].identifiantContrepartie.empty(),
        "16 - Ligne du retrait correctement renseignee"
    );

    // ==========================================
    // NOUVEAU : TESTER UN VIREMENT REUSSI
    // ==========================================

    verifier(
        banque.virer(idAlice, idBob, 2500),
        "17 - Virement de 25 EUR accepte"
    );

    verifier(
        alice->getSoldeCentimes() == 52500 &&
        bob->getSoldeCentimes() == 122500,
        "18 - Soldes Alice et Bob corrects"
    );

    verifier(
        banque.nombreOperations() == 4,
        "19 - Le virement ajoute deux lignes"
    );

    historiqueAlice =
        banque.consulterHistorique(idAlice);

    std::vector<Operation> historiqueBob =
        banque.consulterHistorique(idBob);

    verifier(
        historiqueAlice.size() == 3,
        "20 - Alice possede trois lignes"
    );

    verifier(
        historiqueBob.size() == 1,
        "21 - Bob possede une ligne"
    );

    // NOUVEAU : verifier le virement sortant
    // dans l'historique du compte source.
    verifier(
        historiqueAlice.size() == 3 &&
        historiqueAlice[2].numero == 3 &&
        historiqueAlice[2].type ==
            TypeOperation::VirementSortant &&
        historiqueAlice[2].identifiantCompte == idAlice &&
        historiqueAlice[2].identifiantContrepartie == idBob &&
        historiqueAlice[2].montantCentimes == 2500,
        "22 - Virement sortant correctement enregistre"
    );

    // NOUVEAU : verifier le virement entrant
    // dans l'historique du destinataire.
    verifier(
        historiqueBob.size() == 1 &&
        historiqueBob[0].numero == 4 &&
        historiqueBob[0].type ==
            TypeOperation::VirementEntrant &&
        historiqueBob[0].identifiantCompte == idBob &&
        historiqueBob[0].identifiantContrepartie == idAlice &&
        historiqueBob[0].montantCentimes == 2500,
        "23 - Virement entrant correctement enregistre"
    );

    // NOUVEAU : verifier l'ordre des lignes
    // visibles depuis le compte Alice.
    verifier(
        historiqueAlice.size() == 3 &&
        historiqueAlice[0].numero == 1 &&
        historiqueAlice[1].numero == 2 &&
        historiqueAlice[2].numero == 3,
        "24 - Ordre chronologique d'Alice respecte"
    );

    verifier(
        alice->getSoldeCentimes() +
        bob->getSoldeCentimes() == 175000,
        "25 - Total conserve apres le virement"
    );

    // ==========================================
    // NOUVEAU : TESTER LES DEPOTS REFUSES
    // ==========================================

    verifier(
        !banque.deposerSurCompte(idAlice, 0),
        "26 - Depot nul refuse"
    );

    verifier(
        !banque.deposerSurCompte(idAlice, -100),
        "27 - Depot negatif refuse"
    );

    verifier(
        !banque.deposerSurCompte("MB-9999", 1000),
        "28 - Depot sur compte inexistant refuse"
    );

    verifier(
        banque.nombreOperations() == 4 &&
        alice->getSoldeCentimes() == 52500 &&
        bob->getSoldeCentimes() == 122500,
        "29 - Depots refuses sans effet sur la banque"
    );

    // ==========================================
    // NOUVEAU : TESTER LES RETRAITS REFUSES
    // ==========================================

    verifier(
        !banque.retirerDuCompte(idAlice, 0),
        "30 - Retrait nul refuse"
    );

    verifier(
        !banque.retirerDuCompte(idAlice, -100),
        "31 - Retrait negatif refuse"
    );

    verifier(
        !banque.retirerDuCompte(idAlice, 70000),
        "32 - Retrait avec solde insuffisant refuse"
    );

    verifier(
        !banque.retirerDuCompte("MB-9999", 100),
        "33 - Retrait sur compte inexistant refuse"
    );

    verifier(
        banque.nombreOperations() == 4 &&
        alice->getSoldeCentimes() == 52500 &&
        bob->getSoldeCentimes() == 122500,
        "34 - Retraits refuses sans effet sur la banque"
    );

    // ==========================================
    // NOUVEAU : TESTER LES VIREMENTS REFUSES
    // ==========================================

    verifier(
        !banque.virer(idAlice, idBob, 0),
        "35 - Virement nul refuse"
    );

    verifier(
        !banque.virer(idAlice, idBob, -100),
        "36 - Virement negatif refuse"
    );

    verifier(
        !banque.virer(idAlice, idBob, 70000),
        "37 - Virement avec solde insuffisant refuse"
    );

    verifier(
        !banque.virer(idAlice, idAlice, 100),
        "38 - Auto-virement refuse"
    );

    verifier(
        !banque.virer("MB-9999", idBob, 100),
        "39 - Virement avec source absente refuse"
    );

    verifier(
        !banque.virer(idAlice, "MB-9999", 100),
        "40 - Virement avec destination absente refuse"
    );

    verifier(
        banque.nombreOperations() == 4 &&
        alice->getSoldeCentimes() == 52500 &&
        bob->getSoldeCentimes() == 122500,
        "41 - Virements refuses sans effet"
    );

    // ==========================================
    // NOUVEAU : VERIFIER LES HISTORIQUES INCHANGES
    // ==========================================

    historiqueAlice =
        banque.consulterHistorique(idAlice);

    historiqueBob =
        banque.consulterHistorique(idBob);

    verifier(
        historiqueAlice.size() == 3 &&
        historiqueBob.size() == 1,
        "42 - Les refus ne creent aucune ligne"
    );

    // ==========================================
    // NOUVEAU : CONSULTATION SANS MODIFICATION
    // ==========================================

    const std::size_t nombreAvantConsultation =
        banque.nombreOperations();

    const std::int64_t soldeAliceAvant =
        alice->getSoldeCentimes();

    std::vector<Operation> lecture =
        banque.consulterHistorique(idAlice);

    verifier(
        lecture.size() == 3 &&
        banque.nombreOperations() ==
            nombreAvantConsultation &&
        alice->getSoldeCentimes() ==
            soldeAliceAvant,
        "43 - Consultation sans effet sur l'etat"
    );

    // ==========================================
    // NOUVEAU : COPIE INDEPENDANTE DE L'HISTORIQUE
    // ==========================================

    if (!lecture.empty()) {
        lecture[0].montantCentimes = 999999;
        lecture[0].identifiantCompte = "MODIFIE";
    }

    std::vector<Operation> relecture =
        banque.consulterHistorique(idAlice);

    verifier(
        relecture.size() == 3 &&
        relecture[0].montantCentimes == 10000 &&
        relecture[0].identifiantCompte == idAlice,
        "44 - Modifier une copie ne change pas l'historique"
    );

    // ==========================================
    // NOUVEAU : HISTORIQUE D'UN COMPTE INCONNU
    // ==========================================

    verifier(
        banque.consulterHistorique("MB-9999").empty(),
        "45 - Historique inconnu retourne une liste vide"
    );

    // ==========================================
    // NOUVEAU : CONVERSION DES TYPES EN TEXTE
    // ==========================================

    verifier(
        typeOperationEnTexte(TypeOperation::Depot) ==
            "DEPOT",
        "46 - Texte du type Depot"
    );

    verifier(
        typeOperationEnTexte(TypeOperation::Retrait) ==
            "RETRAIT",
        "47 - Texte du type Retrait"
    );

    verifier(
        typeOperationEnTexte(
            TypeOperation::VirementSortant
        ) == "VIREMENT SORTANT",
        "48 - Texte du virement sortant"
    );

    verifier(
        typeOperationEnTexte(
            TypeOperation::VirementEntrant
        ) == "VIREMENT ENTRANT",
        "49 - Texte du virement entrant"
    );

    // ==========================================
    // NOUVEAU : VERIFIER LE DEPASSEMENT MONETAIRE
    // ==========================================

    // Une banque independante permet de tester
    // les limites sans changer Alice ou Bob.
    Banque banqueLimite;

    const std::int64_t maximum =
        std::numeric_limits<std::int64_t>::max();

    const std::string idMax =
        banqueLimite.creerCompte("Compte Maximum", maximum);

    verifier(
        !idMax.empty(),
        "50 - Compte de limite cree"
    );

    verifier(
        !banqueLimite.deposerSurCompte(idMax, 1),
        "51 - Depot avec depassement refuse"
    );

    verifier(
        banqueLimite.nombreOperations() == 0,
        "52 - Depassement sans ajout d'historique"
    );

    const Compte* compteMax =
        banqueLimite.rechercherCompte(idMax);

    verifier(
        compteMax != nullptr &&
        compteMax->getSoldeCentimes() == maximum,
        "53 - Solde maximum conserve"
    );

    // ==========================================
    // NOUVEAU : VERIFIER LA BANQUE PRINCIPALE
    // ==========================================

    verifier(
        banque.nombreComptes() == 2 &&
        banque.nombreOperations() == 4,
        "54 - Comptes et historique conserves"
    );

    verifier(
        alice->getSoldeCentimes() == 52500 &&
        bob->getSoldeCentimes() == 122500,
        "55 - Soldes finaux corrects"
    );

    // ==========================================
    // NOUVEAU : BILAN DES TESTS
    // ==========================================

    std::cout << "\n=== SOLDES FINAUX ===\n";

    std::cout << "Alice : "
              << alice->getSoldeCentimes()
              << " centimes\n";

    std::cout << "Bob   : "
              << bob->getSoldeCentimes()
              << " centimes\n";

    std::cout << "\n=== HISTORIQUES ===\n";

    std::cout << "Alice : "
              << banque.consulterHistorique(idAlice).size()
              << " operations\n";

    std::cout << "Bob   : "
              << banque.consulterHistorique(idBob).size()
              << " operation\n";

    std::cout << "Total : "
              << banque.nombreOperations()
              << " lignes\n";

    std::cout << "\n=== BILAN DES TESTS ===\n";

    std::cout << "Tests reussis : "
              << testsReussis << '\n';

    std::cout << "Tests echoues : "
              << testsEchoues << '\n';

    return testsEchoues == 0 ? 0 : 1;
}
