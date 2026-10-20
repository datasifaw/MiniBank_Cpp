
#ifndef BANQUE_H
#define BANQUE_H

#include <vector>
#include <string>
#include <cstddef>
#include <cstdint>

#include "Compte.h"
#include "Operation.h"

class Banque {

private:

    // Comptes conserves en memoire.
    std::vector<Compte> comptes;

    // Prochain numero pour les identifiants MB-xxxx.
    std::uint64_t prochainNumero = 1;

    // Generer un identifiant disponible.
    std::string genererIdentifiant();

    // ==========================================
    // HISTORIQUE DES OPERATIONS BANCAIRES
    // ==========================================

    // Lignes d'historique conservees en memoire.
    std::vector<Operation> historique;

    // Numero de la prochaine ligne.
    std::uint64_t prochainNumeroOperation = 1;

    // Preparer la place et la numerotation
    // avant de modifier les soldes.
    bool preparerHistorique(
        std::size_t nombreOperations
    );

public:

    // ==========================================
    // GESTION DES COMPTES
    // ==========================================

    bool ajouterCompte(
        const std::string& id,
        const std::string& titulaire,
        std::int64_t soldeInitialCentimes
    );

    std::string creerCompte(
        const std::string& titulaire,
        std::int64_t soldeInitialCentimes
    );

    Compte* rechercherCompte(
        const std::string& id
    );

    const Compte* rechercherCompte(
        const std::string& id
    ) const;

    std::size_t nombreComptes() const;

    // ==========================================
    // NOUVEAU : CONSULTER TOUS LES COMPTES
    // ==========================================

    // NOUVEAU : retourner une copie des comptes.
    //
    // L'ordre de creation est conserve.
    // Si la banque est vide, le vector retourne
    // est egalement vide.
    //
    // La methode ne modifie pas la banque.
    std::vector<Compte> consulterTousLesComptes() const;

    // ==========================================
    // DEPOT AVEC HISTORIQUE
    // ==========================================

    bool deposerSurCompte(
        const std::string& idCompte,
        std::int64_t montantCentimes
    );

    // ==========================================
    // RETRAIT AVEC HISTORIQUE
    // ==========================================

    bool retirerDuCompte(
        const std::string& idCompte,
        std::int64_t montantCentimes
    );

    // ==========================================
    // VIREMENT ENTRE DEUX COMPTES
    // ==========================================

    bool virer(
        const std::string& idSource,
        const std::string& idDestination,
        std::int64_t montantCentimes
    );

    // ==========================================
    // CONSULTATION DE L'HISTORIQUE
    // ==========================================

    std::vector<Operation> consulterHistorique(
        const std::string& idCompte
    ) const;

    std::size_t nombreOperations() const;
};

#endif
