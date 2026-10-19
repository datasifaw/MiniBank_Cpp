
#ifndef BANQUE_H
#define BANQUE_H

#include <vector>
#include <string>
#include <cstddef>
#include <cstdint>

#include "Compte.h"

class Banque {

private:
    // Collection des comptes bancaires fictifs.
    std::vector<Compte> comptes;

    // Compteur utilise pour generer les identifiants.
    std::uint64_t prochainNumero = 1;

    // Generer un identifiant disponible.
    std::string genererIdentifiant();

public:

    // Ajouter un compte avec un identifiant fourni.
    // Cette methode est conservee pour compatibilite.
    bool ajouterCompte(
        const std::string& id,
        const std::string& titulaire,
        std::int64_t soldeInitialCentimes
    );

    // Creer un compte avec un identifiant automatique.
    // Retourne une chaine vide si la creation echoue.
    std::string creerCompte(
        const std::string& titulaire,
        std::int64_t soldeInitialCentimes
    );

    // Rechercher un compte modifiable.
    Compte* rechercherCompte(const std::string& id);

    // Rechercher un compte en lecture seule.
    const Compte* rechercherCompte(
        const std::string& id
    ) const;

    // Obtenir le nombre total de comptes.
    std::size_t nombreComptes() const;

    // ==========================================
    // NOUVEAU : VIREMENT ENTRE DEUX COMPTES
    // ==========================================

    // Transferer un montant en centimes du compte
    // source vers le compte destinataire.
    //
    // Retourne true si le virement reussit.
    // Retourne false si une condition est refusee.
    //
    // En cas de refus, aucun solde ne doit changer.
    bool virer(
        const std::string& idSource,
        const std::string& idDestination,
        std::int64_t montantCentimes
    );
};

#endif
