
#ifndef BANQUE_H
#define BANQUE_H

#include <vector>
#include <string>
#include <cstddef>

#include "Compte.h"

class Banque {

private:
    // Collection des comptes bancaires
    std::vector<Compte> comptes;

public:
    // Ajouter un nouveau compte
    bool ajouterCompte(
        const std::string& id,
        const std::string& titulaire,
        std::int64_t soldeInitialCentimes
    );

    // Rechercher un compte par identifiant
    Compte* rechercherCompte(const std::string& id);

    // Rechercher un compte sans autoriser sa modification
    const Compte* rechercherCompte(const std::string& id) const;

    // Obtenir le nombre de comptes
    std::size_t nombreComptes() const;
};

#endif
