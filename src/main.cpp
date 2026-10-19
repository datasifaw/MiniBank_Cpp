
#include <iostream>
#include <limits>
#include <string>
#include <cstdint>

// ==========================================
// NOUVEAU : INCLUSION DES MODULES MINIBANK
// ==========================================

#include "Banque.h"
#include "Montant.h"

int main() {

    // ==========================================
    // 1. INITIALISATION DE LA BANQUE
    // ==========================================

    // La banque contient tous les comptes
    // crees pendant l'execution du programme.
    Banque banque;

    std::cout << "Initialisation de MiniBank..."
              << std::endl;

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
        // 3. VALIDATION DE LA SAISIE DU MENU
        // ==========================================

        if (!(std::cin >> choix)) {

            if (std::cin.eof()) {
                std::cout << "\nFin de la saisie."
                          << std::endl;
                break;
            }

            std::cin.clear();

            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n'
            );

            std::cout << "Erreur : entrez un nombre valide."
                      << std::endl;

            continue;
        }

        // NOUVEAU : supprimer le retour a la ligne
        // restant apres la lecture du choix.
        // Cela permet d'utiliser getline() ensuite.
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        // ==========================================
        // 4. TRAITEMENT DES CHOIX
        // ==========================================

        switch (choix) {

            case 0:
                std::cout << "Au revoir !"
                          << std::endl;
                break;

            // ==========================================
            // NOUVEAU : OPTION 1 - CREER UN COMPTE
            // ==========================================

            case 1: {

                std::cout << "\n========================================"
                          << std::endl;

                std::cout << "       CREATION D'UN COMPTE"
                          << std::endl;

                std::cout << "========================================"
                          << std::endl;

                // NOUVEAU : variables de saisie.
                std::string titulaire;
                std::string montantTexte;

                std::int64_t soldeInitialCentimes = 0;

                // ======================================
                // NOUVEAU : SAISIE DU TITULAIRE
                // ======================================

                std::cout << "\nNom du titulaire : ";

                if (!std::getline(std::cin, titulaire)) {
                    std::cout << "\nSaisie interrompue."
                              << std::endl;
                    return 0;
                }

                // NOUVEAU : verifier que le nom ne
                // contient pas uniquement des espaces.
                if (titulaire.find_first_not_of(
                        " \t\r\n"
                    ) == std::string::npos) {

                    std::cout << "Erreur : le nom du titulaire"
                              << " ne peut pas etre vide."
                              << std::endl;

                    break;
                }

                // ======================================
                // NOUVEAU : SAISIE DU SOLDE INITIAL
                // ======================================

                std::cout << "Solde initial (EUR) : ";

                if (!std::getline(std::cin, montantTexte)) {
                    std::cout << "\nSaisie interrompue."
                              << std::endl;
                    return 0;
                }

                // ======================================
                // NOUVEAU : CONVERSION DU MONTANT
                // ======================================

                bool montantValide =
                    Montant::convertirEnCentimes(
                        montantTexte,
                        soldeInitialCentimes
                    );

                if (!montantValide) {

                    std::cout << "Erreur : montant invalide."
                              << std::endl;

                    std::cout << "Exemples acceptes : "
                              << "100, 100.50 ou 100,50"
                              << std::endl;

                    break;
                }

                // ======================================
                // NOUVEAU : CREATION DANS LA BANQUE
                // ======================================

                // La classe Banque genere elle-meme
                // un identifiant unique.
                std::string identifiant =
                    banque.creerCompte(
                        titulaire,
                        soldeInitialCentimes
                    );

                if (identifiant.empty()) {

                    std::cout << "Erreur : impossible de creer"
                              << " le compte."
                              << std::endl;

                    break;
                }

                // ======================================
                // NOUVEAU : AFFICHAGE DU RESULTAT
                // ======================================

                std::cout << "\nCompte cree avec succes !"
                          << std::endl;

                std::cout << "\nIdentifiant : "
                          << identifiant
                          << std::endl;

                std::cout << "Titulaire   : "
                          << titulaire
                          << std::endl;

                std::cout << "Solde       : "
                          << Montant::formater(
                                 soldeInitialCentimes
                             )
                          << std::endl;

                // NOUVEAU : afficher le nombre
                // de comptes actuellement crees.
                std::cout << "\nNombre total de comptes : "
                          << banque.nombreComptes()
                          << std::endl;

                break;
            }

            // ==========================================
            // OPTIONS NON ENCORE IMPLEMENTEES
            // ==========================================

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
