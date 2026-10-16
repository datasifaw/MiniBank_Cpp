
#include <iostream>
#include <limits>

// NOUVEAU : inclusion de notre classe Banque
#include "Banque.h"

int main() {

    // ==========================================
    // 1. INITIALISATION DE LA BANQUE
    // ==========================================

    // NOUVEAU : creation d'un objet Banque.
    // Il contiendra les comptes bancaires fictifs.
    // Pour le moment, aucun compte n'est ajoute.
    Banque banque;

    // NOUVEAU : afficher le nombre de comptes.
    std::cout << "Initialisation de MiniBank..." << std::endl;

    std::cout << "Nombre de comptes : "
              << banque.nombreComptes()
              << std::endl;

    // ==========================================
    // 2. MENU PRINCIPAL
    // ==========================================

    int choix = -1;

    do {

        std::cout << "\n========================================"
                  << std::endl;

        std::cout << "              MINIBANK C++              "
                  << std::endl;

        std::cout << "========================================"
                  << std::endl;

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

        // ==========================================
        // 3. VALIDATION DE LA SAISIE
        // ==========================================

        if (!(std::cin >> choix)) {

            // Fin de la saisie (Ctrl+Z puis Entree
            // sous Windows, par exemple).
            if (std::cin.eof()) {
                std::cout << "\nFin de la saisie."
                          << std::endl;
                break;
            }

            // Nettoyer une saisie invalide.
            std::cin.clear();

            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n'
            );

            std::cout << "Erreur : entrez un nombre valide."
                      << std::endl;

            continue;
        }

        // ==========================================
        // 4. TRAITEMENT DES CHOIX
        // ==========================================

        switch (choix) {

            case 0:
                std::cout << "Au revoir !" << std::endl;
                break;

            // Les fonctionnalites bancaires seront
            // integrees progressivement.
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
                std::cout << "Fonctionnalite a venir !"
                          << std::endl;
                break;

            default:
                std::cout << "Choix invalide !"
                          << std::endl;
                break;
        }

    } while (choix != 0);

    return 0;
}
