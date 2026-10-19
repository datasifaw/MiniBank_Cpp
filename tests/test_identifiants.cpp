
#include <iostream>
#include <string>
#include "Banque.h"

int main() {

    // ==========================================
    // NOUVEAU : INITIALISATION DES TESTS
    // ==========================================

    Banque banque;

    int testsReussis = 0;
    int testsEchoues = 0;

    // NOUVEAU : fonction pour verifier les resultats
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

    std::cout << "=== TESTS DES IDENTIFIANTS ===\n\n";

    // ==========================================
    // NOUVEAU : TEST 1 - PREMIER IDENTIFIANT
    // ==========================================

    std::string idAlice =
        banque.creerCompte("Alice Martin", 50000);

    verifier(idAlice == "MB-0001",
             "Premier identifiant MB-0001");

    // ==========================================
    // NOUVEAU : TEST 2 - DEUXIEME IDENTIFIANT
    // ==========================================

    std::string idBob =
        banque.creerCompte("Bob Dupont", 120000);

    verifier(idBob == "MB-0002",
             "Deuxieme identifiant MB-0002");

    // ==========================================
    // NOUVEAU : TEST 3 - TROISIEME IDENTIFIANT
    // ==========================================

    std::string idCharlie =
        banque.creerCompte("Charlie Bernard", 75000);

    verifier(idCharlie == "MB-0003",
             "Troisieme identifiant MB-0003");

    // ==========================================
    // NOUVEAU : TEST 4 - IDENTIFIANTS UNIQUES
    // ==========================================

    verifier(
        idAlice != idBob &&
        idAlice != idCharlie &&
        idBob != idCharlie,
        "Les trois identifiants sont uniques"
    );

    // ==========================================
    // NOUVEAU : TEST 5 - COMPTES EXISTANTS
    // ==========================================

    verifier(
        banque.rechercherCompte(idAlice) != nullptr &&
        banque.rechercherCompte(idBob) != nullptr &&
        banque.rechercherCompte(idCharlie) != nullptr,
        "Les trois comptes sont retrouvables"
    );

    // ==========================================
    // NOUVEAU : TEST 6 - NOMBRE DE COMPTES
    // ==========================================

    verifier(banque.nombreComptes() == 3,
             "La banque contient trois comptes");

    // ==========================================
    // NOUVEAU : TEST 7 - TITULAIRE VIDE
    // ==========================================

    std::string idInvalide =
        banque.creerCompte("", 10000);

    verifier(idInvalide.empty(),
             "Creation avec titulaire vide refusee");

    // ==========================================
    // NOUVEAU : TEST 8 - SOLDE NEGATIF
    // ==========================================

    std::string idNegatif =
        banque.creerCompte("Client Invalide", -100);

    verifier(idNegatif.empty(),
             "Creation avec solde negatif refusee");

    // ==========================================
    // NOUVEAU : TEST 9 - COMPTE NON AJOUTE
    // ==========================================

    verifier(banque.nombreComptes() == 3,
             "Les creations invalides sont ignorees");

    // ==========================================
    // NOUVEAU : TEST 10 - NUMEROTATION CONTINUE
    // ==========================================

    std::string idDavid =
        banque.creerCompte("David Martin", 0);

    verifier(idDavid == "MB-0004",
             "Numerotation continue apres les refus");

    // ==========================================
    // NOUVEAU : TEST 11 - EVITER UN DOUBLON
    // ==========================================

    // Simuler un identifiant deja attribue
    // manuellement dans une autre banque.
    Banque autreBanque;

    bool ajoutManuel = autreBanque.ajouterCompte(
        "MB-0001", "Client Manuel", 10000
    );

    std::string idAutomatique =
        autreBanque.creerCompte("Client Auto", 20000);

    verifier(
        ajoutManuel &&
        idAutomatique == "MB-0002",
        "Identifiant deja utilise saute"
    );

    // ==========================================
    // NOUVEAU : TEST 12 - NOMBRE FINAL
    // ==========================================

    verifier(
        banque.nombreComptes() == 4 &&
        autreBanque.nombreComptes() == 2,
        "Nombre final de comptes correct"
    );

    // ==========================================
    // BILAN DES TESTS
    // ==========================================

    std::cout << "\n=== BILAN ===\n";

    std::cout << "Tests reussis : "
              << testsReussis << '\n';

    std::cout << "Tests echoues : "
              << testsEchoues << '\n';

    return testsEchoues == 0 ? 0 : 1;
}
