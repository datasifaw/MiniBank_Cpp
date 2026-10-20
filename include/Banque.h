
#ifndef BANQUE_H
#define BANQUE_H

#include <vector>
#include <string>
#include <cstddef>
#include <cstdint>

#include "Compte.h"
#include "Operation.h"

// NOUVEAU : definition de l'instantane
// complet utilise pour la persistance.
#include "EtatBanque.h"

class Banque {

private:

    // Comptes conserves en memoire.
    std::vector<Compte> comptes;

    // Prochain numero des comptes automatiques.
    std::uint64_t prochainNumero = 1;

    // Generer un identifiant disponible.
    std::string genererIdentifiant();

    // ==========================================
    // HISTORIQUE DES OPERATIONS
    // ==========================================

    // Lignes d'historique en memoire.
    std::vector<Operation> historique;

    // Numero de la prochaine operation.
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
    // CONSULTATION DE TOUS LES COMPTES
    // ==========================================

    // Retourner une copie independante
    // des comptes dans leur ordre de creation.
    std::vector<Compte> consulterTousLesComptes() const;

    // ==========================================
    // NOUVEAU : EXPORT DE L'ETAT BANCAIRE
    // ==========================================

    // NOUVEAU : produire un instantane complet
    // contenant les comptes, l'historique
    // global et les deux compteurs.
    //
    // La methode est en lecture seule.
    // Elle ne sauvegarde rien sur disque.
    //
    // La copie retournee est independante
    // des donnees internes de Banque.
    EtatBanque exporterEtat() const;

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
