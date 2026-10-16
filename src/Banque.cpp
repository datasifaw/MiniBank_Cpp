
#include "Banque.h"

#include <stdexcept>

// Ajouter un nouveau compte bancaire
bool Banque::ajouterCompte(
    const std::string& id,
    const std::string& titulaire,
    std::int64_t soldeInitialCentimes
) {
    // Verifier si l'identifiant existe deja
    if (rechercherCompte(id) != nullptr) {
        return false;
    }

    // Verifier les informations du compte
    if (id.empty() || titulaire.empty() ||
        soldeInitialCentimes < 0) {
        return false;
    }

    // Creer et ajouter le compte dans le vector
    comptes.emplace_back(
        id,
        titulaire,
        soldeInitialCentimes
    );

    return true;
}

// Rechercher un compte modifiable
Compte* Banque::rechercherCompte(const std::string& id) {
    for (Compte& compte : comptes) {
        if (compte.getIdentifiant() == id) {
            return &compte;
        }
    }

    return nullptr;
}

// Rechercher un compte en lecture seule
const Compte* Banque::rechercherCompte(
    const std::string& id
) const {
    for (const Compte& compte : comptes) {
        if (compte.getIdentifiant() == id) {
            return &compte;
        }
    }

    return nullptr;
}

// Retourner le nombre total de comptes
std::size_t Banque::nombreComptes() const {
    return comptes.size();
}
