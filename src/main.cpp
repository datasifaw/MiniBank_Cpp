
#include <iostream>

int main() {

    int choix;

    do {
        std::cout << "\n========================================" << std::endl;
        std::cout << "              MINIBANK C++              " << std::endl;
        std::cout << "========================================" << std::endl;

        std::cout << "\n1. Creer un compte" << std::endl;
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
