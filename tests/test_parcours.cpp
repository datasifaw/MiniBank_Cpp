
#include <iostream>
#include <string>
#include <cstdint>
#include <limits>

// ==========================================
// NOUVEAU : MODULES A TESTER ENSEMBLE
// ==========================================

#include "Banque.h"
#include "Compte.h"
#include "Montant.h"

int main() {

    // ==========================================
    // NOUVEAU : PREPARATION DES TESTS
    // ==========================================

    int testsReussis = 0;
    int testsEchoues = 0;

    // NOUVEAU : fonction locale de verification.
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

    std::cout << "=== TESTS DU PARCOURS BANCAIRE ===\n\n";

    // ==========================================
    // NOUVEAU : INITIALISER UNE BANQUE VIDE
    // ==========================================

    Banque banque;

    verifier(
        banque.nombreComptes() == 0,
        "01 - La banque est initialement vide"
    );

    // ==========================================
    // NOUVEAU : CREER ALICE
    // ==========================================

    std::int64_t soldeAlice = 0;

    bool conversionAlice =
        Montant::convertirEnCentimes(
            "500,50",
            soldeAlice
        );

    verifier(
        conversionAlice && soldeAlice == 50050,
        "02 - Conversion du solde initial d'Alice"
    );

    // NOUVEAU : creer le compte seulement
    // lorsque la conversion a reussi.
    std::string idAlice;

    if (conversionAlice) {
        idAlice = banque.creerCompte(
            "Alice Martin",
            soldeAlice
        );
    }

    verifier(
        idAlice == "MB-0001",
        "03 - Creation d'Alice : MB-0001"
    );

    // ==========================================
    // NOUVEAU : CREER BOB
    // ==========================================

    std::int64_t soldeBob = 0;

    bool conversionBob =
        Montant::convertirEnCentimes(
            "1200",
            soldeBob
        );

    verifier(
        conversionBob && soldeBob == 120000,
        "04 - Conversion du solde initial de Bob"
    );

    std::string idBob;

    if (conversionBob) {
        idBob = banque.creerCompte(
            "Bob Dupont",
            soldeBob
        );
    }

    verifier(
        idBob == "MB-0002",
        "05 - Creation de Bob : MB-0002"
    );

    // ==========================================
    // NOUVEAU : VERIFIER LES DEUX COMPTES
    // ==========================================

    verifier(
        banque.nombreComptes() == 2,
        "06 - La banque contient deux comptes"
    );

    Compte* alice = banque.rechercherCompte(idAlice);
    Compte* bob = banque.rechercherCompte(idBob);

    verifier(
        alice != nullptr && bob != nullptr &&
        idAlice != idBob,
        "07 - Les deux comptes sont distincts"
    );

    // NOUVEAU : ne pas utiliser un pointeur nul.
    // En cas d'echec de creation, arreter le test
    // plutot que de provoquer un plantage.
    if (alice == nullptr || bob == nullptr) {

        std::cout
            << "\nImpossible de poursuivre : "
            << "un compte est introuvable.\n";

        std::cout << "Tests reussis : "
                  << testsReussis << '\n';

        std::cout << "Tests echoues : "
                  << testsEchoues << '\n';

        return 1;
    }

    verifier(
        alice->getTitulaire() == "Alice Martin" &&
        bob->getTitulaire() == "Bob Dupont",
        "08 - Les titulaires sont corrects"
    );

    verifier(
        alice->getSoldeCentimes() == 50050 &&
        bob->getSoldeCentimes() == 120000,
        "09 - Les soldes initiaux sont corrects"
    );

    // ==========================================
    // NOUVEAU : DEPOT DE 100,50 EUR SUR ALICE
    // ==========================================

    std::int64_t depotCentimes = 0;

    bool conversionDepot =
        Montant::convertirEnCentimes(
            "100,50",
            depotCentimes
        );

    verifier(
        conversionDepot && depotCentimes == 10050,
        "10 - Conversion du depot de 100,50 EUR"
    );

    bool depotReussi = false;

    if (conversionDepot) {
        depotReussi = alice->deposer(depotCentimes);
    }

    verifier(
        depotReussi,
        "11 - Depot effectue sur Alice"
    );

    verifier(
        alice->getSoldeCentimes() == 60100,
        "12 - Solde d'Alice apres depot : 601 EUR"
    );

    verifier(
        Montant::formater(
            alice->getSoldeCentimes()
        ) == "601,00 EUR",
        "13 - Affichage du solde apres depot"
    );

    // ==========================================
    // NOUVEAU : RETRAIT DE 50 EUR SUR ALICE
    // ==========================================

    std::int64_t retraitCentimes = 0;

    bool conversionRetrait =
        Montant::convertirEnCentimes(
            "50",
            retraitCentimes
        );

    verifier(
        conversionRetrait && retraitCentimes == 5000,
        "14 - Conversion du retrait de 50 EUR"
    );

    bool retraitReussi = false;

    if (conversionRetrait) {
        retraitReussi = alice->retirer(
            retraitCentimes
        );
    }

    verifier(
        retraitReussi,
        "15 - Retrait effectue sur Alice"
    );

    verifier(
        alice->getSoldeCentimes() == 55100,
        "16 - Solde d'Alice apres retrait : 551 EUR"
    );

    // ==========================================
    // NOUVEAU : VERIFIER LE SOLDE DE BOB
    // ==========================================

    verifier(
        bob->getSoldeCentimes() == 120000,
        "17 - Les operations d'Alice n'affectent pas Bob"
    );

    // ==========================================
    // NOUVEAU : REFUSER UN RETRAIT EXCESSIF
    // ==========================================

    bool retraitExcessif = alice->retirer(70000);

    verifier(
        !retraitExcessif,
        "18 - Retrait de 700 EUR refuse"
    );

    verifier(
        alice->getSoldeCentimes() == 55100,
        "19 - Solde inchange apres retrait refuse"
    );

    // ==========================================
    // NOUVEAU : REFUSER LES MONTANTS NULS
    // ==========================================

    bool depotNul = alice->deposer(0);
    bool retraitNul = alice->retirer(0);

    verifier(
        !depotNul && !retraitNul,
        "20 - Depot et retrait nuls refuses"
    );

    // ==========================================
    // NOUVEAU : REFUSER LES MONTANTS NEGATIFS
    // ==========================================

    bool depotNegatif = alice->deposer(-100);
    bool retraitNegatif = alice->retirer(-100);

    verifier(
        !depotNegatif && !retraitNegatif,
        "21 - Operations negatives refusees"
    );

    verifier(
        alice->getSoldeCentimes() == 55100,
        "22 - Solde conserve apres operations invalides"
    );

    // ==========================================
    // NOUVEAU : VERIFIER LES SAISIES INVALIDES
    // ==========================================

    std::int64_t resultat = 12345;

    bool texteValide =
        Montant::convertirEnCentimes(
            "abc",
            resultat
        );

    verifier(
        !texteValide && resultat == 12345,
        "23 - Texte invalide refuse sans modification"
    );

    bool decimalesValides =
        Montant::convertirEnCentimes(
            "10.999",
            resultat
        );

    verifier(
        !decimalesValides && resultat == 12345,
        "24 - Trop de decimales refuse"
    );

    // ==========================================
    // NOUVEAU : RECHERCHE D'UN COMPTE INEXISTANT
    // ==========================================

    verifier(
        banque.rechercherCompte("MB-9999") == nullptr,
        "25 - Compte inexistant introuvable"
    );

    // ==========================================
    // NOUVEAU : VERIFIER LA LECTURE SEULE
    // ==========================================

    const Banque& banqueLecture = banque;

    const Compte* aliceLecture =
        banqueLecture.rechercherCompte("MB-0001");

    verifier(
        aliceLecture != nullptr &&
        aliceLecture->getSoldeCentimes() == 55100,
        "26 - Consultation en lecture seule correcte"
    );

    // ==========================================
    // NOUVEAU : VERIFIER LE FORMATAGE FINAL
    // ==========================================

    verifier(
        Montant::formater(
            alice->getSoldeCentimes()
        ) == "551,00 EUR",
        "27 - Affichage final d'Alice : 551 EUR"
    );

    verifier(
        Montant::formater(
            bob->getSoldeCentimes()
        ) == "1200,00 EUR",
        "28 - Affichage final de Bob : 1200 EUR"
    );

    // ==========================================
    // NOUVEAU : VERIFIER LES LIMITES NUMERIQUES
    // ==========================================

    // Utiliser une banque separee pour verifier
    // le depassement sans toucher a Alice et Bob.
    Banque banqueLimite;

    std::string idLimite =
        banqueLimite.creerCompte(
            "Compte Limite",
            std::numeric_limits<std::int64_t>::max()
        );

    Compte* compteLimite =
        banqueLimite.rechercherCompte(idLimite);

    verifier(
        compteLimite != nullptr,
        "29 - Creation d'un compte au solde maximal"
    );

    if (compteLimite != nullptr) {

        bool depotExcessif =
            compteLimite->deposer(1);

        verifier(
            !depotExcessif,
            "30 - Depot provoquant un depassement refuse"
        );

        verifier(
            compteLimite->getSoldeCentimes() ==
            std::numeric_limits<std::int64_t>::max(),
            "31 - Solde maximal conserve apres refus"
        );

    } else {

        // NOUVEAU : comptabiliser les deux
        // controles impossibles a effectuer.
        verifier(false,
                 "30 - Depot provoquant un depassement refuse");

        verifier(false,
                 "31 - Solde maximal conserve apres refus");
    }

    // ==========================================
    // NOUVEAU : BILAN FINAL DES DEUX COMPTES
    // ==========================================

    verifier(
        banque.nombreComptes() == 2,
        "32 - Le nombre de comptes reste correct"
    );

    verifier(
        alice->getSoldeCentimes() == 55100 &&
        bob->getSoldeCentimes() == 120000,
        "33 - Soldes finaux d'Alice et Bob corrects"
    );

    // ==========================================
    // NOUVEAU : AFFICHAGE DU BILAN
    // ==========================================

    std::cout << "\n=== SOLDES FINAUX ===\n";

    std::cout << "Alice : "
              << Montant::formater(
                     alice->getSoldeCentimes()
                 )
              << '\n';

    std::cout << "Bob   : "
              << Montant::formater(
                     bob->getSoldeCentimes()
                 )
              << '\n';

    std::cout << "\n=== BILAN DES TESTS ===\n";

    std::cout << "Tests reussis : "
              << testsReussis << '\n';

    std::cout << "Tests echoues : "
              << testsEchoues << '\n';

    // NOUVEAU : signaler un echec au systeme
    // si au moins un test a echoue.
    return testsEchoues == 0 ? 0 : 1;
}
