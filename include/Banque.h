
#ifndef BANQUE_H
#define BANQUE_H

#include <vector>
#include <string>
#include <cstddef>
#include <cstdint>

#include "Compte.h"

// NOUVEAU : modele des mouvements bancaires.
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
    // NOUVEAU : STOCKAGE DE L'HISTORIQUE
    // ==========================================

    // Chaque element represente une ligne
    // d'historique appartenant a un compte.
    std::vector<Operation> historique;

    // Numero attribue a la prochaine ligne.
    std::uint64_t prochainNumeroOperation = 1;

    // NOUVEAU : verifier la numerotation et
    // reserver la place pour les futures lignes.
    //
    // Cette methode ne modifie aucun solde.
    bool preparerHistorique(
        std::size_t nombreOperations
    );

public:

    // ==========================================
    // GESTION DES COMPTES EXISTANTE
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
    // NOUVEAU : DEPOT AVEC HISTORIQUE
    // ==========================================

    bool deposerSurCompte(
        const std::string& idCompte,
        std::int64_t montantCentimes
    );

    // ==========================================
    // NOUVEAU : RETRAIT AVEC HISTORIQUE
    // ==========================================

    bool retirerDuCompte(
        const std::string& idCompte,
        std::int64_t montantCentimes
    );

    // ==========================================
    // VIREMENT ENTRE DEUX COMPTES
    // ==========================================

    // NOUVEAU : un virement reussi produira
    // deux lignes d'historique.
    bool virer(
        const std::string& idSource,
        const std::string& idDestination,
        std::int64_t montantCentimes
    );

    // ==========================================
    // NOUVEAU : CONSULTATION DE L'HISTORIQUE
    // ==========================================

    // Retourne les operations du compte demande,
    // dans leur ordre d'enregistrement.
    //
    // Une liste vide signifie qu'aucune operation
    // n'est trouvee pour cet identifiant.
    std::vector<Operation> consulterHistorique(
        const std::string& idCompte
    ) const;

    // NOUVEAU : nombre total de lignes
    // d'historique dans la banque.
    std::size_t nombreOperations() const;
};

#endif
