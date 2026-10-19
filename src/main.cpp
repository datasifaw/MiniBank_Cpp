
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

    // La banque conserve les comptes fictifs
    // pendant l'execution du programme.
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

        // Nettoyer la fin de ligne avant getline().
        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        // ==========================================
        // 4. TRAITEMENT DES OPTIONS
        // ==========================================

        switch (choix) {

            case 0: {

                std::cout << "Au revoir !"
                          << std::endl;

                break;
            }

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

                std::cout << "\nNom du titulaire : ";

                if (!std::getline(std::cin, titulaire)) {

                    std::cout << "\nSaisie interrompue."
                              << std::endl;

                    return 0;
                }

                // Refuser les titulaires vides
                // ou composes uniquement d'espaces.
                if (titulaire.find_first_not_of(
                        " \t\r\n"
                    ) == std::string::npos) {

                    std::cout
                        << "Erreur : le nom du titulaire"
                        << " ne peut pas etre vide."
                        << std::endl;

                    break;
                }

                std::cout << "Solde initial (EUR) : ";

                if (!std::getline(std::cin, montantTexte)) {

                    std::cout << "\nSaisie interrompue."
                              << std::endl;

                    return 0;
                }

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

                std::string identifiant =
                    banque.creerCompte(
                        titulaire,
                        soldeInitialCentimes
                    );

                if (identifiant.empty()) {

                    std::cout
                        << "Erreur : impossible de creer"
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
            // OPTION 2 : CONSULTER UN COMPTE
            // ==========================================

            case 2: {

                std::cout << "\n========================================"
                          << std::endl;

                std::cout << "       CONSULTATION D'UN COMPTE"
                          << std::endl;

                std::cout << "========================================"
                          << std::endl;

                std::string identifiantRecherche;

                std::cout << "\nIdentifiant du compte : ";

                if (!std::getline(
                        std::cin,
                        identifiantRecherche
                    )) {

                    std::cout << "\nSaisie interrompue."
                              << std::endl;

                    return 0;
                }

                if (identifiantRecherche.find_first_not_of(
                        " \t\r\n"
                    ) == std::string::npos) {

                    std::cout
                        << "Erreur : l'identifiant ne peut"
                        << " pas etre vide."
                        << std::endl;

                    break;
                }

                // Consultation en lecture seule.
                const Banque& banqueLecture = banque;

                const Compte* compte =
                    banqueLecture.rechercherCompte(
                        identifiantRecherche
                    );

                if (compte == nullptr) {

                    std::cout
                        << "\nErreur : aucun compte trouve"
                        << " avec cet identifiant."
                        << std::endl;

                    break;
                }

                std::cout << "\nCompte trouve !"
                          << std::endl;

                std::cout << "\nIdentifiant : "
                          << compte->getIdentifiant()
                          << std::endl;

                std::cout << "Titulaire   : "
                          << compte->getTitulaire()
                          << std::endl;

                std::cout << "Solde       : "
                          << Montant::formater(
                                 compte->getSoldeCentimes()
                             )
                          << std::endl;

                break;
            }

            // ==========================================
            // OPTION 3 : DEPOT D'ARGENT
            // ==========================================

            case 3: {

                std::cout << "\n========================================"
                          << std::endl;

                std::cout << "             DEPOT D'ARGENT"
                          << std::endl;

                std::cout << "========================================"
                          << std::endl;

                std::string identifiantDepot;
                std::string montantTexte;

                std::int64_t montantCentimes = 0;

                std::cout << "\nIdentifiant du compte : ";

                if (!std::getline(
                        std::cin,
                        identifiantDepot
                    )) {

                    std::cout << "\nSaisie interrompue."
                              << std::endl;

                    return 0;
                }

                if (identifiantDepot.find_first_not_of(
                        " \t\r\n"
                    ) == std::string::npos) {

                    std::cout
                        << "Erreur : l'identifiant ne peut"
                        << " pas etre vide."
                        << std::endl;

                    break;
                }

                // Pointeur modifiable pour le depot.
                Compte* compte =
                    banque.rechercherCompte(
                        identifiantDepot
                    );

                if (compte == nullptr) {

                    std::cout
                        << "Erreur : aucun compte trouve"
                        << " avec cet identifiant."
                        << std::endl;

                    break;
                }

                std::cout << "Montant du depot (EUR) : ";

                if (!std::getline(
                        std::cin,
                        montantTexte
                    )) {

                    std::cout << "\nSaisie interrompue."
                              << std::endl;

                    return 0;
                }

                bool montantValide =
                    Montant::convertirEnCentimes(
                        montantTexte,
                        montantCentimes
                    );

                if (!montantValide) {

                    std::cout
                        << "Erreur : montant invalide."
                        << std::endl;

                    break;
                }

                if (montantCentimes <= 0) {

                    std::cout
                        << "Erreur : le depot doit etre"
                        << " superieur a zero."
                        << std::endl;

                    break;
                }

                std::int64_t ancienSolde =
                    compte->getSoldeCentimes();

                bool depotReussi =
                    compte->deposer(montantCentimes);

                if (!depotReussi) {

                    std::cout
                        << "Erreur : depot refuse."
                        << std::endl;

                    std::cout
                        << "Le solde du compte reste"
                        << " inchange."
                        << std::endl;

                    break;
                }

                std::cout
                    << "\nDepot effectue avec succes !"
                    << std::endl;

                std::cout << "\nIdentifiant    : "
                          << compte->getIdentifiant()
                          << std::endl;

                std::cout << "Titulaire      : "
                          << compte->getTitulaire()
                          << std::endl;

                std::cout << "Ancien solde   : "
                          << Montant::formater(ancienSolde)
                          << std::endl;

                std::cout << "Montant depose : "
                          << Montant::formater(montantCentimes)
                          << std::endl;

                std::cout << "Nouveau solde  : "
                          << Montant::formater(
                                 compte->getSoldeCentimes()
                             )
                          << std::endl;

                break;
            }

            // ==========================================
            // NOUVEAU : OPTION 4 - RETRAIT D'ARGENT
            // ==========================================

            case 4: {

                // NOUVEAU : titre de l'operation.
                std::cout << "\n========================================"
                          << std::endl;

                std::cout << "            RETRAIT D'ARGENT"
                          << std::endl;

                std::cout << "========================================"
                          << std::endl;

                // NOUVEAU : variables propres au retrait.
                std::string identifiantRetrait;
                std::string montantTexte;

                std::int64_t montantCentimes = 0;

                // ======================================
                // NOUVEAU : LECTURE DE L'IDENTIFIANT
                // ======================================

                std::cout << "\nIdentifiant du compte : ";

                if (!std::getline(
                        std::cin,
                        identifiantRetrait
                    )) {

                    std::cout << "\nSaisie interrompue."
                              << std::endl;

                    return 0;
                }

                // NOUVEAU : refuser un identifiant vide
                // ou compose uniquement d'espaces.
                if (identifiantRetrait.find_first_not_of(
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

                // NOUVEAU : un pointeur Compte* permet
                // de modifier le solde du compte.
                Compte* compte =
                    banque.rechercherCompte(
                        identifiantRetrait
                    );

                // NOUVEAU : verifier que le compte existe
                // avant de manipuler ses informations.
                if (compte == nullptr) {

                    std::cout
                        << "Erreur : aucun compte trouve"
                        << " avec cet identifiant."
                        << std::endl;

                    break;
                }

                // ======================================
                // NOUVEAU : LECTURE DU MONTANT
                // ======================================

                std::cout << "Montant du retrait (EUR) : ";

                if (!std::getline(
                        std::cin,
                        montantTexte
                    )) {

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
                        montantCentimes
                    );

                // NOUVEAU : refuser une saisie incorrecte.
                if (!montantValide) {

                    std::cout
                        << "Erreur : montant invalide."
                        << std::endl;

                    break;
                }

                // NOUVEAU : un retrait doit etre
                // strictement superieur a zero.
                if (montantCentimes <= 0) {

                    std::cout
                        << "Erreur : le retrait doit etre"
                        << " superieur a zero."
                        << std::endl;

                    break;
                }

                // ======================================
                // NOUVEAU : MEMORISER L'ANCIEN SOLDE
                // ======================================

                std::int64_t ancienSolde =
                    compte->getSoldeCentimes();

                // ======================================
                // NOUVEAU : EXECUTER LE RETRAIT
                // ======================================

                // NOUVEAU : la methode retirer() verifie
                // que le solde est suffisant.
                bool retraitReussi =
                    compte->retirer(montantCentimes);

                // NOUVEAU : si le retrait est refuse,
                // le solde ne doit pas changer.
                if (!retraitReussi) {

                    std::cout
                        << "Erreur : retrait refuse."
                        << std::endl;

                    std::cout
                        << "Solde insuffisant ou operation"
                        << " non autorisee."
                        << std::endl;

                    std::cout
                        << "Le solde du compte reste"
                        << " inchange."
                        << std::endl;

                    break;
                }

                // ======================================
                // NOUVEAU : AFFICHAGE DU RESULTAT
                // ======================================

                std::cout
                    << "\nRetrait effectue avec succes !"
                    << std::endl;

                std::cout << "\nIdentifiant    : "
                          << compte->getIdentifiant()
                          << std::endl;

                std::cout << "Titulaire      : "
                          << compte->getTitulaire()
                          << std::endl;

                std::cout << "Ancien solde   : "
                          << Montant::formater(ancienSolde)
                          << std::endl;

                std::cout << "Montant retire : "
                          << Montant::formater(montantCentimes)
                          << std::endl;

                std::cout << "Nouveau solde  : "
                          << Montant::formater(
                                 compte->getSoldeCentimes()
                             )
                          << std::endl;

                break;
            }

            // ==========================================
            // OPTIONS NON ENCORE IMPLEMENTEES
            // ==========================================

            case 5:
            case 6:
            case 7:
            case 8:

                std::cout
                    << "Fonctionnalite a venir !"
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
