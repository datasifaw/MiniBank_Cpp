
#include <iostream>
// Créer et tester notre premier objet Compte
#include "Compte.h"

int main() {


    // Créer et tester notre premier objet Compte

    
    // Creation du premier compte fictif
    Compte compteAlice("MB-0001", "Said Tech", 50000);

    // Affichage des informations du compte
    std::cout << "\n=== PREMIER COMPTE BANCAIRE ===" << std::endl;
    std::cout << "Identifiant : " << compteAlice.getIdentifiant() << std::endl;
    std::cout << "Titulaire   : " << compteAlice.getTitulaire() << std::endl;

    std::int64_t solde = compteAlice.getSoldeCentimes();

    std::cout << "Solde       : "
              << solde / 100 << "."
              << (solde % 100 < 10 ? "0" : "")
              << solde % 100 << " EUR"
              << std::endl;



    int choix;

    do {
        std::cout << "\n========================================" << std::endl;
        std::cout << "              MINIBANK C++              " << std::endl;
        std::cout << "========================================" << std::endl;

        std::cout << "\n1. Creer un compte " << std::endl;
        std::cout << "2. Consulter un compte" << std::endl;
        std::cout << "3. Deposer de l'argent" << std::endl;
        std::cout << "4. Retirer de l'argent" << std::endl;
        std::cout << "5. Effectuer un virement" << std::endl;
        std::cout << "6. Consulter l'historique" << std::endl;
        std::cout << "7. Afficher tous les comptes" << std::endl;
        std::cout << "8. Sauvegarder les donnees" << std::endl;
        std::cout << "0. Quitter" << std::endl;

        std::cout << "\nVotre choix : ";
        if (!(std::cin >> choix)) {
            if (std::cin.eof()) {
                std::cout << "\nFin de la saisie." << std::endl;
                break;
            }

            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Erreur : entrez un nombre valide." << std::endl;
            continue;
        }

        switch (choix) {
            case 0:
                std::cout << "Au revoir !" << std::endl;
                break;

            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
                std::cout << "Fonctionnalite a venir !" << std::endl;
                break;

            default:
                std::cout << "Choix invalide !" << std::endl;
        }

    } while (choix != 0);

    return 0;
}
