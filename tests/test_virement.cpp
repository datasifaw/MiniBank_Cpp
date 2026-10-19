
#include <iostream>
#include <string>
#include <cstdint>
#include <limits>

// ==========================================
// NOUVEAU : TESTS DE LA CLASSE BANQUE
// ==========================================

#include "Banque.h"

int main() {

    // ==========================================
    // NOUVEAU : PREPARATION DES TESTS
    // ==========================================

    int testsReussis = 0;
    int testsEchoues = 0;

    // NOUVEAU : verifier chaque resultat.
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

    std::cout << "=== TESTS DES VIREMENTS ===\n\n";

    // ==========================================
    // NOUVEAU : CREER DEUX COMPTES
    // ==========================================

    Banque banque;

    std::string idAlice =
        banque.creerCompte("Alice Martin", 55100);

    std::string idBob =
        banque.creerCompte("Bob Dupont", 120000);

    verifier(
        idAlice == "MB-0001",
        "01 - Identifiant d'Alice correct"
    );

    verifier(
        idBob == "MB-0002",
        "02 - Identifiant de Bob correct"
    );

    verifier(
        banque.nombreComptes() == 2,
        "03 - Deux comptes sont presents"
    );

    // NOUVEAU : obtenir les pointeurs apres
    // avoir cree les deux comptes.
    //
    // L'ajout de comptes dans un std::vector
    // peut invalider les anciens pointeurs.
    Compte* alice = banque.rechercherCompte(idAlice);
    Compte* bob = banque.rechercherCompte(idBob);

    verifier(
        alice != nullptr && bob != nullptr,
        "04 - Alice et Bob sont retrouvables"
    );

    // NOUVEAU : arreter proprement le test
    // si un des comptes est introuvable.
    if (alice == nullptr || bob == nullptr) {
        std::cout
            << "\nImpossible de poursuivre : "
            << "un compte est introuvable.\n";

        return 1;
    }

    verifier(
        alice->getSoldeCentimes() == 55100 &&
        bob->getSoldeCentimes() == 120000,
        "05 - Soldes initiaux corrects"
    );

    // ==========================================
    // NOUVEAU : VIREMENT REUSSI ALICE -> BOB
    // ==========================================

    bool premierVirement =
        banque.virer(idAlice, idBob, 10000);

    verifier(
        premierVirement,
        "06 - Virement de 100 EUR accepte"
    );

    verifier(
        alice->getSoldeCentimes() == 45100,
        "07 - Alice possede 451 EUR"
    );

    verifier(
        bob->getSoldeCentimes() == 130000,
        "08 - Bob possede 1300 EUR"
    );

    // NOUVEAU : verifier la conservation
    // de la somme totale des deux comptes.
    verifier(
        alice->getSoldeCentimes() +
        bob->getSoldeCentimes() == 175100,
        "09 - Total conserve apres virement"
    );

    // ==========================================
    // NOUVEAU : REFUSER UN MONTANT NUL
    // ==========================================

    bool virementNul =
        banque.virer(idAlice, idBob, 0);

    verifier(
        !virementNul,
        "10 - Virement nul refuse"
    );

    verifier(
        alice->getSoldeCentimes() == 45100 &&
        bob->getSoldeCentimes() == 130000,
        "11 - Soldes inchanges apres montant nul"
    );

    // ==========================================
    // NOUVEAU : REFUSER UN MONTANT NEGATIF
    // ==========================================

    bool virementNegatif =
        banque.virer(idAlice, idBob, -100);

    verifier(
        !virementNegatif,
        "12 - Virement negatif refuse"
    );

    verifier(
        alice->getSoldeCentimes() == 45100 &&
        bob->getSoldeCentimes() == 130000,
        "13 - Soldes inchanges apres montant negatif"
    );

    // ==========================================
    // NOUVEAU : REFUSER UN SOLDE INSUFFISANT
    // ==========================================

    bool soldeInsuffisant =
        banque.virer(idAlice, idBob, 70000);

    verifier(
        !soldeInsuffisant,
        "14 - Virement avec solde insuffisant refuse"
    );

    verifier(
        alice->getSoldeCentimes() == 45100 &&
        bob->getSoldeCentimes() == 130000,
        "15 - Soldes inchanges apres solde insuffisant"
    );

    // ==========================================
    // NOUVEAU : COMPTE SOURCE INEXISTANT
    // ==========================================

    bool sourceAbsente =
        banque.virer("MB-9999", idBob, 1000);

    verifier(
        !sourceAbsente,
        "16 - Source inexistante refusee"
    );

    verifier(
        alice->getSoldeCentimes() == 45100 &&
        bob->getSoldeCentimes() == 130000,
        "17 - Soldes inchanges si source absente"
    );

    // ==========================================
    // NOUVEAU : DESTINATION INEXISTANTE
    // ==========================================

    bool destinationAbsente =
        banque.virer(idAlice, "MB-9999", 1000);

    verifier(
        !destinationAbsente,
        "18 - Destination inexistante refusee"
    );

    verifier(
        alice->getSoldeCentimes() == 45100 &&
        bob->getSoldeCentimes() == 130000,
        "19 - Soldes inchanges si destination absente"
    );

    // ==========================================
    // NOUVEAU : REFUSER UN VIREMENT VERS SOI
    // ==========================================

    bool memeCompte =
        banque.virer(idAlice, idAlice, 1000);

    verifier(
        !memeCompte,
        "20 - Virement vers le meme compte refuse"
    );

    verifier(
        alice->getSoldeCentimes() == 45100 &&
        bob->getSoldeCentimes() == 130000,
        "21 - Soldes inchanges apres auto-virement"
    );

    // ==========================================
    // NOUVEAU : IDENTIFIANTS VIDES
    // ==========================================

    bool sourceVide =
        banque.virer("", idBob, 1000);

    bool destinationVide =
        banque.virer(idAlice, "", 1000);

    verifier(
        !sourceVide && !destinationVide,
        "22 - Identifiants vides refuses"
    );

    verifier(
        alice->getSoldeCentimes() == 45100 &&
        bob->getSoldeCentimes() == 130000,
        "23 - Soldes inchanges apres IDs vides"
    );

    // ==========================================
    // NOUVEAU : DEPASSEMENT CHEZ LE DESTINATAIRE
    // ==========================================

    // NOUVEAU : une banque independante evite
    // de perturber les comptes Alice et Bob.
    Banque banqueLimite;

    const std::int64_t maximum =
        std::numeric_limits<std::int64_t>::max();

    std::string idSourceLimite =
        banqueLimite.creerCompte(
            "Source Limite", 10000
        );

    std::string idDestinationLimite =
        banqueLimite.creerCompte(
            "Destination Limite",
            maximum - 500
        );

    // NOUVEAU : recuperer les pointeurs seulement
    // apres la creation des deux comptes.
    Compte* sourceLimite =
        banqueLimite.rechercherCompte(idSourceLimite);

    Compte* destinationLimite =
        banqueLimite.rechercherCompte(idDestinationLimite);

    verifier(
        sourceLimite != nullptr &&
        destinationLimite != nullptr,
        "24 - Comptes de limite crees"
    );

    // NOUVEAU : proteger les tests suivants.
    if (sourceLimite != nullptr &&
        destinationLimite != nullptr) {

        // Le destinataire ne peut recevoir
        // que 500 centimes supplementaires.
        bool depassement =
            banqueLimite.virer(
                idSourceLimite,
                idDestinationLimite,
                1000
            );

        verifier(
            !depassement,
            "25 - Depassement numerique refuse"
        );

        verifier(
            sourceLimite->getSoldeCentimes() == 10000 &&
            destinationLimite->getSoldeCentimes() ==
                maximum - 500,
            "26 - Aucun solde modifie apres depassement"
        );

    } else {

        // NOUVEAU : enregistrer explicitement
        // les controles impossibles a effectuer.
        verifier(false,
                 "25 - Depassement numerique refuse");

        verifier(false,
                 "26 - Aucun solde modifie apres depassement");
    }

    // ==========================================
    // NOUVEAU : VIREMENT EN SENS INVERSE
    // ==========================================

    // Bob envoie 50 EUR a Alice.
    bool retourBob =
        banque.virer(idBob, idAlice, 5000);

    verifier(
        retourBob,
        "27 - Virement inverse de 50 EUR accepte"
    );

    verifier(
        alice->getSoldeCentimes() == 50100 &&
        bob->getSoldeCentimes() == 125000,
        "28 - Soldes corrects apres virement inverse"
    );

    // ==========================================
    // NOUVEAU : VIREMENT DU SOLDE TOTAL
    // ==========================================

    // Alice envoie la totalite de ses 501 EUR a Bob.
    bool virementTotal =
        banque.virer(idAlice, idBob, 50100);

    verifier(
        virementTotal,
        "29 - Virement de la totalite du solde accepte"
    );

    verifier(
        alice->getSoldeCentimes() == 0,
        "30 - Solde source egal a zero"
    );

    verifier(
        bob->getSoldeCentimes() == 175100,
        "31 - Le destinataire recoit le solde total"
    );

    // ==========================================
    // NOUVEAU : CONSERVER LE TOTAL FINAL
    // ==========================================

    verifier(
        alice->getSoldeCentimes() +
        bob->getSoldeCentimes() == 175100,
        "32 - Conservation du total apres tous les virements"
    );

    verifier(
        banque.nombreComptes() == 2,
        "33 - Aucun compte ajoute ou supprime"
    );

    // ==========================================
    // NOUVEAU : AFFICHAGE DU BILAN
    // ==========================================

    std::cout << "\n=== SOLDES FINAUX ===\n";

    std::cout << "Alice : "
              << alice->getSoldeCentimes()
              << " centimes\n";

    std::cout << "Bob   : "
              << bob->getSoldeCentimes()
              << " centimes\n";

    std::cout << "\n=== BILAN DES TESTS ===\n";

    std::cout << "Tests reussis : "
              << testsReussis << '\n';

    std::cout << "Tests echoues : "
              << testsEchoues << '\n';

    // NOUVEAU : retourner un code d'erreur
    // si au moins une verification echoue.
    return testsEchoues == 0 ? 0 : 1;
}
