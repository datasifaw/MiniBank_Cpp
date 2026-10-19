
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

// NOUVEAU : types et textes des operations.
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

                // Depot centralise dans Banque :
                // le mouvement reussi est historise.
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

                // Retrait centralise dans Banque :
                // le mouvement reussi est historise.
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

                // Banque enregistre les deux mouvements
                // d'historique du virement reussi.
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
            // NOUVEAU : OPTION 6 - HISTORIQUE
            // ======================================

            case 6: {

                // NOUVEAU : ecran de consultation.
                std::cout << "\n========================================"
                          << std::endl;
                std::cout << "       HISTORIQUE DES OPERATIONS"
                          << std::endl;
                std::cout << "========================================"
                          << std::endl;

                // NOUVEAU : identifier le compte
                // dont on veut consulter l'historique.
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

                // NOUVEAU : refuser les identifiants
                // vides ou composes uniquement d'espaces.
                if (identifiantHistorique.find_first_not_of(
                        " \t\r\n"
                    ) == std::string::npos) {

                    std::cout
                        << "Erreur : l'identifiant ne peut"
                        << " pas etre vide."
                        << std::endl;
                    break;
                }

                // ==================================
                // NOUVEAU : VERIFIER LE COMPTE
                // ==================================

                // La consultation est en lecture seule.
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

                // ==================================
                // NOUVEAU : RECUPERER L'HISTORIQUE
                // ==================================

                // Cette methode renvoie une copie
                // des operations du compte uniquement.
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

                // ==================================
                // NOUVEAU : HISTORIQUE VIDE
                // ==================================

                if (operations.empty()) {

                    std::cout
                        << "\nAucune operation enregistree."
                        << std::endl;

                    std::cout
                        << "Nombre d'operations : 0"
                        << std::endl;

                    break;
                }

                // ==================================
                // NOUVEAU : AFFICHER LES OPERATIONS
                // ==================================

                // Les lignes ont ete conservees dans
                // l'ordre de leur enregistrement.
                for (const Operation& operation : operations) {

                    std::cout << "\n----------------------------"
                              << std::endl;

                    std::cout << "Operation n. "
                              << operation.numero
                              << std::endl;

                    // NOUVEAU : afficher le type
                    // avec la fonction d'Operation.h.
                    std::cout << "Type     : "
                              << typeOperationEnTexte(
                                     operation.type
                                 )
                              << std::endl;

                    // ==================================
                    // NOUVEAU : SENS DU MOUVEMENT
                    // ==================================

                    // Depot et virement entrant :
                    // argent recu sur le compte.
                    //
                    // Retrait et virement sortant :
                    // argent quitte le compte.
                    bool estEntree =
                        operation.type == TypeOperation::Depot ||
                        operation.type ==
                            TypeOperation::VirementEntrant;

                    // ==================================
                    // NOUVEAU : COMPTE LIE AU VIREMENT
                    // ==================================

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

                    // ==================================
                    // NOUVEAU : AFFICHAGE DU MONTANT
                    // ==================================

                    // Montant::formater() recoit un
                    // montant positif en centimes.
                    // Le signe vient du type d'operation.
                    std::cout << "Montant  : "
                              << (estEntree ? "+" : "-")
                              << Montant::formater(
                                     operation.montantCentimes
                                 )
                              << std::endl;
                }

                std::cout << "\n----------------------------"
                          << std::endl;

                // NOUVEAU : afficher uniquement le
                // nombre d'operations de ce compte.
                std::cout << "Nombre d'operations : "
                          << operations.size()
                          << std::endl;

                break;
            }

            // ======================================
            // OPTIONS A DEVELOPPER
            // ======================================

            case 7:
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
