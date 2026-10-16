
#include <iostream>
#include <string>

#include "Banque.h"

int main() {

    Banque banque;
    int testsReussis = 0;
    int testsEchoues = 0;

    // Fonction pour afficher les resultats
    auto verifier = [&](bool condition, const std::string& nomTest) {
        if (condition) {
            std::cout << "[OK] " << nomTest << '\n';
            ++testsReussis;
        } else {
            std::cout << "[ECHEC] " << nomTest << '\n';
            ++testsEchoues;
        }
    };

    std::cout << "=== TESTS DE LA CLASSE BANQUE ===\n\n";

    // Test 1 : Banque initialement vide
    verifier(banque.nombreComptes() == 0,
             "Banque initialement vide");

    // Test 2 : Ajouter Alice
    verifier(banque.ajouterCompte(
                 "MB-0001", "Alice Martin", 50000),
             "Ajouter Alice");

    // Test 3 : Ajouter Bob
    verifier(banque.ajouterCompte(
                 "MB-0002", "Bob Dupont", 120000),
             "Ajouter Bob");

    // Test 4 : Ajouter Charlie
    verifier(banque.ajouterCompte(
                 "MB-0003", "Charlie Bernard", 75000),
             "Ajouter Charlie");

    // Test 5 : Compter les comptes
    verifier(banque.nombreComptes() == 3,
             "La banque contient 3 comptes");

    // Test 6 : Rechercher Bob
    Compte* bob = banque.rechercherCompte("MB-0002");

    verifier(bob != nullptr &&
             bob->getTitulaire() == "Bob Dupont" &&
             bob->getSoldeCentimes() == 120000,
             "Rechercher le compte de Bob");

    // Test 7 : Rechercher un compte inexistant
    verifier(banque.rechercherCompte("MB-9999") == nullptr,
             "Compte inexistant retourne nullptr");

    // Test 8 : Refuser un identifiant en double
    verifier(!banque.ajouterCompte(
                 "MB-0001", "Autre Alice", 30000),
             "Refuser un identifiant en double");

    // Test 9 : Refuser un solde initial negatif
    verifier(!banque.ajouterCompte(
                 "MB-0004", "Client Invalide", -100),
             "Refuser un solde initial negatif");

    // Test 10 : Refuser un titulaire vide
    verifier(!banque.ajouterCompte(
                 "MB-0005", "", 10000),
             "Refuser un titulaire vide");

    // Test 11 : Refuser un identifiant vide
    verifier(!banque.ajouterCompte(
                 "", "Client Test", 10000),
             "Refuser un identifiant vide");

    // Test 12 : Rechercher depuis une banque const
    const Banque& banqueLectureSeule = banque;

    const Compte* alice =
        banqueLectureSeule.rechercherCompte("MB-0001");

    verifier(alice != nullptr &&
             alice->getTitulaire() == "Alice Martin",
             "Recherche sur une banque const");

    // Test 13 : Verifier le nombre final
    verifier(banque.nombreComptes() == 3,
             "Les ajouts invalides ne modifient pas la banque");

    std::cout << "\n=== BILAN ===\n";
    std::cout << "Tests reussis : " << testsReussis << '\n';
    std::cout << "Tests echoues : " << testsEchoues << '\n';

    return testsEchoues == 0 ? 0 : 1;
}
