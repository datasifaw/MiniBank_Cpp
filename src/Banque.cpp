
#include "Banque.h"

#include <iomanip>
#include <sstream>
#include <limits>

// ==========================================
// NOUVEAU : GENERER UN IDENTIFIANT UNIQUE
// ==========================================

std::string Banque::genererIdentifiant() {

    // Chercher un identifiant qui n'existe pas
    while (true) {

        // Verifier que le compteur peut encore avancer
        if (prochainNumero ==
            std::numeric_limits<std::uint64_t>::max()) {
            return "";
        }

        // Construire l'identifiant au format MB-0001
        std::ostringstream flux;

        flux << "MB-"
             << std::setw(4)
             << std::setfill('0')
             << prochainNumero;

        // Passer au numero suivant
        ++prochainNumero;

        std::string id = flux.str();

        // Verifier que cet identifiant est libre
        if (rechercherCompte(id) == nullptr) {
            return id;
        }
    }
}

// ==========================================
// NOUVEAU : CREER UN COMPTE AUTOMATIQUEMENT
// ==========================================

std::string Banque::creerCompte(
    const std::string& titulaire,
    std::int64_t soldeInitialCentimes
) {

    // Refuser les donnees invalides
    if (titulaire.empty() || soldeInitialCentimes < 0) {
        return "";
    }

    // Generer automatiquement l'identifiant
    std::string id = genererIdentifiant();

    if (id.empty()) {
        return "";
    }

    // Ajouter le compte avec la methode existante
    if (!ajouterCompte(id, titulaire, soldeInitialCentimes)) {
        return "";
    }

    // Retourner l'identifiant du nouveau compte
    return id;
}

// ==========================================
// METHODE EXISTANTE : AJOUTER UN COMPTE
// ==========================================

bool Banque::ajouterCompte(
    const std::string& id,
    const std::string& titulaire,
    std::int64_t soldeInitialCentimes
) {

    if (rechercherCompte(id) != nullptr) {
        return false;
    }

    if (id.empty() || titulaire.empty() ||
        soldeInitialCentimes < 0) {
        return false;
    }

    comptes.emplace_back(
        id,
        titulaire,
        soldeInitialCentimes
    );

    return true;
}

// ==========================================
// METHODE EXISTANTE : RECHERCHE MODIFIABLE
// ==========================================

Compte* Banque::rechercherCompte(const std::string& id) {

    for (Compte& compte : comptes) {

        if (compte.getIdentifiant() == id) {
            return &compte;
        }
    }

    return nullptr;
}

// ==========================================
// METHODE EXISTANTE : RECHERCHE CONST
// ==========================================

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

// ==========================================
// METHODE EXISTANTE : NOMBRE DE COMPTES
// ==========================================

std::size_t Banque::nombreComptes() const {
    return comptes.size();
}
