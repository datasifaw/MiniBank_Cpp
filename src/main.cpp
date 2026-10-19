
#include <iostream>
#include <limits>
#include <string>
#include <cstdint>

// ==========================================
// MODULES MINIBANK
// ==========================================

#include "Banque.h"
#include "Montant.h"

int main() {

    // ==========================================
    // 1. INITIALISATION DE LA BANQUE
    // ==========================================

    // Cet objet conserve les comptes pendant
    // toute l'execution du programme.
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
        // 3. VALIDATION DU CHOIX
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

        // Nettoyer la fin de la ligne apres le choix.
        // Indispensable avant d'utiliser getline().
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        // ==========================================
        // 4. TRAITEMENT DES OPTIONS
        // ==========================================

        switch (choix) {

            case 0:
                std::cout << "Au revoir !"
                          << std::endl;
                break;

            // ==========================================
            // OPTION 1 : CREER UN COMPTE
            // ==========================================

            case 1: {

                std::cout << "\n========================================"
                          << std::endl;

                std::cout << "       CREATION D'UN COMPTE"
                          << std::endl;

                std::cout << "========================================"
                          << std::endl;

                std::string titulaire;
                std::string montantTexte;

                std::int64_t soldeInitialCentimes = 0;

                // Lire le nom complet du titulaire.
                std::cout << "\nNom du titulaire : ";

                if (!std::getline(std::cin, titulaire)) {
                    std::cout << "\nSaisie interrompue."
                              << std::endl;
                    return 0;
                }

                // Refuser un nom vide ou compose
                // uniquement d'espaces.
                if (titulaire.find_first_not_of(
                        " \t\r\n"
                    ) == std::string::npos) {

                    std::cout << "Erreur : le nom du titulaire"
                              << " ne peut pas etre vide."
                              << std::endl;

                    break;
                }

                // Lire le solde initial en euros.
                std::cout << "Solde initial (EUR) : ";

                if (!std::getline(std::cin, montantTexte)) {
                    std::cout << "\nSaisie interrompue."
                              << std::endl;
                    return 0;
                }

                // Convertir les euros en centimes.
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

                // Creer le compte et recuperer
                // son identifiant automatique.
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

                std::cout << "\nNombre total de comptes : "
                          << banque.nombreComptes()
                          << std::endl;

                break;
            }

            // ==========================================
            // NOUVEAU : OPTION 2 - CONSULTER UN COMPTE
            // ==========================================

            case 2: {

                // NOUVEAU : afficher un titre specifique
                // pour l'ecran de consultation.
                std::cout << "\n========================================"
                          << std::endl;

                std::cout << "       CONSULTATION D'UN COMPTE"
                          << std::endl;

                std::cout << "\n========================================"
                          << std::endl;

                // NOUVEAU : stocker l'identifiant saisi.
                std::string identifiantRecherche;

                std::cout << "\nIdentifiant du compte : ";

                // NOUVEAU : lire toute la ligne pour
                // eviter les problemes de saisie.
                if (!std::getline(
                        std::cin,
                        identifiantRecherche
                    )) {

                    std::cout << "\nSaisie interrompue."
                              << std::endl;

                    return 0;
                }

                // ======================================
                // NOUVEAU : VALIDATION DE L'IDENTIFIANT
                // ======================================

                // Refuser une ligne vide ou constituee
                // uniquement d'espaces.
                if (identifiantRecherche.find_first_not_of(
                        " \t\r\n"
                    ) == std::string::npos) {

                    std::cout
                        << "Erreur : l'identifiant ne peut"
                        << " pas etre vide."
                        << std::endl;

                    break;
                }

                // ======================================
                // NOUVEAU : RECHERCHE DU COMPTE
                // ======================================

                // Utiliser une reference constante
                // vers Banque pour obtenir un pointeur
                // const Compte* en lecture seule.
                const Banque& banqueLecture = banque;

                const Compte* compte =
                    banqueLecture.rechercherCompte(
                        identifiantRecherche
                    );

                // ======================================
                // NOUVEAU : COMPTE INTROUVABLE
                // ======================================

                // nullptr signifie qu'aucun compte
                // ne correspond a cet identifiant.
                if (compte == nullptr) {

                    std::cout
                        << "\nErreur : aucun compte trouve"
                        << " avec cet identifiant."
                        << std::endl;

                    break;
                }

                // ======================================
                // NOUVEAU : AFFICHAGE DES INFORMATIONS
                // ======================================

                std::cout << "\nCompte trouve !"
                          << std::endl;

                // NOUVEAU : utiliser -> pour appeler
                // les methodes via le pointeur.
                std::cout << "\nIdentifiant : "
                          << compte->getIdentifiant()
                          << std::endl;

                std::cout << "Titulaire   : "
                          << compte->getTitulaire()
                          << std::endl;

                // NOUVEAU : le solde est stocke
                // en centimes et affiche en euros.
                std::cout << "Solde       : "
                          << Montant::formater(
                                 compte->getSoldeCentimes()
                             )
                          << std::endl;

                break;
            }

            // ==========================================
            // OPTIONS NON ENCORE IMPLEMENTEES
            // ==========================================

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
