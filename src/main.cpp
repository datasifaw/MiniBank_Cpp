
#include <iostream>
#include <limits>
#include <string>
#include <cstdint>
#include <vector>

// ==========================================
// MODULES MINIBANK
// ==========================================

#include "Banque.h"
#include "Montant.h"
#include "Operation.h"

int main() {

    // ==========================================
    // 1. INITIALISATION DE LA BANQUE
    // ==========================================

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
        std::cout << "              MINIBANK C++"
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

        std::cin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            '\n'
        );

        // ==========================================
        // 4. TRAITEMENT DES OPTIONS
        // ==========================================

        switch (choix) {

            // ======================================
            // OPTION 0 : QUITTER
            // ======================================

            case 0: {

                std::cout << "Au revoir !"
                          << std::endl;
                break;
            }

            // ======================================
            // OPTION 1 : CREER UN COMPTE
            // ======================================

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
                          << identifiant << std::endl;

                std::cout << "Titulaire   : "
                          << titulaire << std::endl;

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

            // ======================================
            // OPTION 2 : CONSULTER UN COMPTE
            // ======================================

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

            // ======================================
            // OPTION 3 : DEPOT D'ARGENT
            // ======================================

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

                const Banque& banqueLecture = banque;

                const Compte* compte =
                    banqueLecture.rechercherCompte(
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
                    banque.deposerSurCompte(
                        identifiantDepot,
                        montantCentimes
                    );

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

            // ======================================
            // OPTION 4 : RETRAIT D'ARGENT
            // ======================================

            case 4: {

                std::cout << "\n========================================"
                          << std::endl;
                std::cout << "            RETRAIT D'ARGENT"
                          << std::endl;
                std::cout << "========================================"
                          << std::endl;

                std::string identifiantRetrait;
                std::string montantTexte;
                std::int64_t montantCentimes = 0;

                std::cout << "\nIdentifiant du compte : ";

                if (!std::getline(
                        std::cin,
                        identifiantRetrait
                    )) {
                    std::cout << "\nSaisie interrompue."
                              << std::endl;
                    return 0;
                }

                if (identifiantRetrait.find_first_not_of(
                        " \t\r\n"
                    ) == std::string::npos) {

                    std::cout
                        << "Erreur : l'identifiant ne peut"
                        << " pas etre vide."
                        << std::endl;
                    break;
                }

                const Banque& banqueLecture = banque;

                const Compte* compte =
                    banqueLecture.rechercherCompte(
                        identifiantRetrait
                    );

                if (compte == nullptr) {
                    std::cout
                        << "Erreur : aucun compte trouve"
                        << " avec cet identifiant."
                        << std::endl;
                    break;
                }

                std::cout << "Montant du retrait (EUR) : ";

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
                        << "Erreur : le retrait doit etre"
                        << " superieur a zero."
                        << std::endl;
                    break;
                }

                std::int64_t ancienSolde =
                    compte->getSoldeCentimes();

                bool retraitReussi =
                    banque.retirerDuCompte(
                        identifiantRetrait,
                        montantCentimes
                    );

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

            // ======================================
            // OPTION 5 : EFFECTUER UN VIREMENT
            // ======================================

            case 5: {

                std::cout << "\n========================================"
                          << std::endl;
                std::cout << "           VIREMENT BANCAIRE"
                          << std::endl;
                std::cout << "========================================"
                          << std::endl;

                std::string idSource;
                std::string idDestination;
                std::string montantTexte;
                std::int64_t montantCentimes = 0;

                std::cout << "\nCompte source       : ";

                if (!std::getline(std::cin, idSource)) {
                    std::cout << "\nSaisie interrompue."
                              << std::endl;
                    return 0;
                }

                if (idSource.find_first_not_of(
                        " \t\r\n"
                    ) == std::string::npos) {

                    std::cout
                        << "Erreur : l'identifiant source"
                        << " ne peut pas etre vide."
                        << std::endl;
                    break;
                }

                std::cout << "Compte destinataire : ";

                if (!std::getline(
                        std::cin,
                        idDestination
                    )) {
                    std::cout << "\nSaisie interrompue."
                              << std::endl;
                    return 0;
                }

                if (idDestination.find_first_not_of(
                        " \t\r\n"
                    ) == std::string::npos) {

                    std::cout
                        << "Erreur : l'identifiant"
                        << " destinataire ne peut pas etre vide."
                        << std::endl;
                    break;
                }

                if (idSource == idDestination) {
                    std::cout
                        << "Erreur : le compte source et"
                        << " le destinataire doivent etre"
                        << " differents."
                        << std::endl;
                    break;
                }

                const Banque& banqueLecture = banque;

                const Compte* source =
                    banqueLecture.rechercherCompte(idSource);

                const Compte* destination =
                    banqueLecture.rechercherCompte(
                        idDestination
                    );

                if (source == nullptr) {
                    std::cout
                        << "Erreur : compte source introuvable."
                        << std::endl;
                    break;
                }

                if (destination == nullptr) {
                    std::cout
                        << "Erreur : compte destinataire introuvable."
                        << std::endl;
                    break;
                }

                std::cout << "Montant (EUR)       : ";

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
                        << "Erreur : le virement doit etre"
                        << " superieur a zero."
                        << std::endl;
                    break;
                }

                const std::int64_t ancienSoldeSource =
                    source->getSoldeCentimes();

                const std::int64_t ancienSoldeDestination =
                    destination->getSoldeCentimes();

                bool virementReussi =
                    banque.virer(
                        idSource,
                        idDestination,
                        montantCentimes
                    );

                if (!virementReussi) {
                    std::cout
                        << "\nErreur : virement refuse."
                        << std::endl;
                    std::cout
                        << "Verifiez les soldes et les"
                        << " limites des comptes."
                        << std::endl;
                    std::cout
                        << "Aucun montant n'a ete transfere."
                        << std::endl;
                    break;
                }

                std::cout
                    << "\nVirement effectue avec succes !"
                    << std::endl;

                std::cout << "\nSource       : "
                          << source->getTitulaire()
                          << " (" << idSource << ")"
                          << std::endl;

                std::cout << "Destinataire : "
                          << destination->getTitulaire()
                          << " (" << idDestination << ")"
                          << std::endl;

                std::cout << "Montant      : "
                          << Montant::formater(montantCentimes)
                          << std::endl;

                std::cout << "\nSolde source avant       : "
                          << Montant::formater(
                                 ancienSoldeSource
                             )
                          << std::endl;

                std::cout << "Solde source apres       : "
                          << Montant::formater(
                                 source->getSoldeCentimes()
                             )
                          << std::endl;

                std::cout << "Solde destinataire avant : "
                          << Montant::formater(
                                 ancienSoldeDestination
                             )
                          << std::endl;

                std::cout << "Solde destinataire apres : "
                          << Montant::formater(
                                 destination->getSoldeCentimes()
                             )
                          << std::endl;

                break;
            }

            // ======================================
            // OPTION 6 : CONSULTER L'HISTORIQUE
            // ======================================

            case 6: {

                std::cout << "\n========================================"
                          << std::endl;
                std::cout << "       HISTORIQUE DES OPERATIONS"
                          << std::endl;
                std::cout << "========================================"
                          << std::endl;

                std::string identifiantHistorique;

                std::cout << "\nIdentifiant du compte : ";

                if (!std::getline(
                        std::cin,
                        identifiantHistorique
                    )) {
                    std::cout << "\nSaisie interrompue."
                              << std::endl;
                    return 0;
                }

                if (identifiantHistorique.find_first_not_of(
                        " \t\r\n"
                    ) == std::string::npos) {

                    std::cout
                        << "Erreur : l'identifiant ne peut"
                        << " pas etre vide."
                        << std::endl;
                    break;
                }

                const Banque& banqueLecture = banque;

                const Compte* compte =
                    banqueLecture.rechercherCompte(
                        identifiantHistorique
                    );

                if (compte == nullptr) {
                    std::cout
                        << "Erreur : aucun compte trouve"
                        << " avec cet identifiant."
                        << std::endl;
                    break;
                }

                std::vector<Operation> operations =
                    banqueLecture.consulterHistorique(
                        identifiantHistorique
                    );

                std::cout << "\nTitulaire : "
                          << compte->getTitulaire()
                          << std::endl;

                std::cout << "Compte    : "
                          << compte->getIdentifiant()
                          << std::endl;

                if (operations.empty()) {

                    std::cout
                        << "\nAucune operation enregistree."
                        << std::endl;

                    std::cout
                        << "Nombre d'operations : 0"
                        << std::endl;

                    break;
                }

                for (const Operation& operation : operations) {

                    std::cout << "\n----------------------------"
                              << std::endl;

                    std::cout << "Operation n. "
                              << operation.numero
                              << std::endl;

                    std::cout << "Type     : "
                              << typeOperationEnTexte(
                                     operation.type
                                 )
                              << std::endl;

                    bool estEntree =
                        operation.type == TypeOperation::Depot ||
                        operation.type ==
                            TypeOperation::VirementEntrant;

                    if (operation.type ==
                        TypeOperation::VirementSortant) {

                        std::cout << "Vers     : "
                                  << operation.identifiantContrepartie
                                  << std::endl;

                    } else if (operation.type ==
                               TypeOperation::VirementEntrant) {

                        std::cout << "Depuis   : "
                                  << operation.identifiantContrepartie
                                  << std::endl;
                    }

                    std::cout << "Montant  : "
                              << (estEntree ? "+" : "-")
                              << Montant::formater(
                                     operation.montantCentimes
                                 )
                              << std::endl;
                }

                std::cout << "\n----------------------------"
                          << std::endl;

                std::cout << "Nombre d'operations : "
                          << operations.size()
                          << std::endl;

                break;
            }

            // ======================================
            // NOUVEAU : OPTION 7 - TOUS LES COMPTES
            // ======================================

            case 7: {

                // NOUVEAU : afficher la liste des
                // comptes enregistres dans la banque.
                std::cout << "\n========================================"
                          << std::endl;

                std::cout << "          LISTE DES COMPTES"
                          << std::endl;

                std::cout << "========================================"
                          << std::endl;

                // ==================================
                // NOUVEAU : CONSULTATION EN LECTURE SEULE
                // ==================================

                // L'appel est effectue sur une
                // reference constante a la banque.
                const Banque& banqueLecture = banque;

                // NOUVEAU : la methode retourne
                // une COPIE des comptes internes.
                //
                // Aucun solde et aucune operation
                // d'historique ne sont modifies.
                const std::vector<Compte> listeComptes =
                    banqueLecture.consulterTousLesComptes();

                // ==================================
                // NOUVEAU : GERER UNE BANQUE VIDE
                // ==================================

                if (listeComptes.empty()) {

                    std::cout
                        << "\nAucun compte bancaire enregistre."
                        << std::endl;

                    std::cout
                        << "\nNombre total de comptes : 0"
                        << std::endl;

                    break;
                }

                // ==================================
                // NOUVEAU : AFFICHER CHAQUE COMPTE
                // ==================================

                // Le vector preserve l'ordre de
                // creation des comptes bancaires.
                std::size_t numeroAffichage = 1;

                for (const Compte& compte : listeComptes) {

                    std::cout
                        << "\nCompte n. "
                        << numeroAffichage
                        << std::endl;

                    std::cout << "Identifiant : "
                              << compte.getIdentifiant()
                              << std::endl;

                    std::cout << "Titulaire   : "
                              << compte.getTitulaire()
                              << std::endl;

                    // NOUVEAU : Montant::formater()
                    // convertit les centimes en texte
                    // lisible avec deux decimales.
                    std::cout << "Solde       : "
                              << Montant::formater(
                                     compte.getSoldeCentimes()
                                 )
                              << std::endl;

                    // NOUVEAU : separer les comptes
                    // pour faciliter la lecture.
                    std::cout
                        << "\n----------------------------------------"
                        << std::endl;

                    ++numeroAffichage;
                }

                // ==================================
                // NOUVEAU : AFFICHER LE TOTAL
                // ==================================

                // Le total provient de la liste
                // obtenue lors de la consultation.
                std::cout << "\nNombre total de comptes : "
                          << listeComptes.size()
                          << std::endl;

                break;
            }

            // ======================================
            // OPTION 8 : A DEVELOPPER
            // ======================================

            case 8: {

                std::cout << "Fonctionnalite a venir !"
                          << std::endl;
                break;
            }

            default: {

                std::cout << "Choix invalide !"
                          << std::endl;
                break;
            }
        }

    } while (choix != 0);

    return 0;
}
