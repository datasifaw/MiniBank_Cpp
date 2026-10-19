
#include "Banque.h"

#include <iomanip>
#include <sstream>
#include <limits>

// ==========================================
// GENERER UN IDENTIFIANT UNIQUE
// ==========================================

std::string Banque::genererIdentifiant() {

    while (true) {

        // Eviter le depassement du compteur.
        if (prochainNumero ==
            std::numeric_limits<std::uint64_t>::max()) {
            return "";
        }

        std::ostringstream flux;

        flux << "MB-"
             << std::setw(4)
             << std::setfill('0')
             << prochainNumero;

        ++prochainNumero;

        std::string id = flux.str();

        // Ne pas reutiliser un identifiant existant.
        if (rechercherCompte(id) == nullptr) {
            return id;
        }
    }
}

// ==========================================
// CREER UN COMPTE AUTOMATIQUEMENT
// ==========================================

std::string Banque::creerCompte(
    const std::string& titulaire,
    std::int64_t soldeInitialCentimes
) {

    if (titulaire.empty() || soldeInitialCentimes < 0) {
        return "";
    }

    std::string id = genererIdentifiant();

    if (id.empty()) {
        return "";
    }

    if (!ajouterCompte(
            id,
            titulaire,
            soldeInitialCentimes
        )) {
        return "";
    }

    return id;
}

// ==========================================
// AJOUTER UN COMPTE
// ==========================================

bool Banque::ajouterCompte(
    const std::string& id,
    const std::string& titulaire,
    std::int64_t soldeInitialCentimes
) {

    if (rechercherCompte(id) != nullptr) {
        return false;
    }

    if (id.empty() ||
        titulaire.empty() ||
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
// RECHERCHER UN COMPTE MODIFIABLE
// ==========================================

Compte* Banque::rechercherCompte(
    const std::string& id
) {

    for (Compte& compte : comptes) {

        if (compte.getIdentifiant() == id) {
            return &compte;
        }
    }

    return nullptr;
}

// ==========================================
// RECHERCHER UN COMPTE EN LECTURE SEULE
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
// OBTENIR LE NOMBRE DE COMPTES
// ==========================================

std::size_t Banque::nombreComptes() const {
    return comptes.size();
}

// ==========================================
// NOUVEAU : VIREMENT ATOMIQUE ENTRE COMPTES
// ==========================================

bool Banque::virer(
    const std::string& idSource,
    const std::string& idDestination,
    std::int64_t montantCentimes
) {

    // ======================================
    // NOUVEAU : VALIDATION DES IDENTIFIANTS
    // ======================================

    // Un virement doit concerner deux comptes
    // distincts, avec des identifiants non vides.
    if (idSource.empty() ||
        idDestination.empty() ||
        idSource == idDestination) {
        return false;
    }

    // ======================================
    // NOUVEAU : VALIDATION DU MONTANT
    // ======================================

    // Le montant doit etre strictement positif.
    if (montantCentimes <= 0) {
        return false;
    }

    // ======================================
    // NOUVEAU : RECHERCHE DES DEUX COMPTES
    // ======================================

    Compte* source = rechercherCompte(idSource);
    Compte* destination = rechercherCompte(idDestination);

    // Si l'un des comptes est absent,
    // aucun transfert ne doit etre effectue.
    if (source == nullptr || destination == nullptr) {
        return false;
    }

    // ======================================
    // NOUVEAU : RECUPERER LES SOLDES
    // ======================================

    std::int64_t soldeSource =
        source->getSoldeCentimes();

    std::int64_t soldeDestination =
        destination->getSoldeCentimes();

    // ======================================
    // NOUVEAU : VERIFIER LE SOLDE SOURCE
    // ======================================

    // Verifier avant de retirer quoi que ce soit.
    if (montantCentimes > soldeSource) {
        return false;
    }

    // ======================================
    // NOUVEAU : EVITER LE DEPASSEMENT NUMERIQUE
    // ======================================

    // Le destinataire doit pouvoir recevoir
    // le montant sans depasser int64_t.
    const std::int64_t soldeMaximum =
        std::numeric_limits<std::int64_t>::max();

    if (montantCentimes >
        soldeMaximum - soldeDestination) {
        return false;
    }

    // ======================================
    // NOUVEAU : EFFECTUER LE TRANSFERT
    // ======================================

    // Toutes les conditions ont ete verifiees.
    // Le retrait doit maintenant reussir.
    if (!source->retirer(montantCentimes)) {
        return false;
    }

    // Crediter le destinataire.
    if (!destination->deposer(montantCentimes)) {

        // NOUVEAU : protection supplementaire.
        // Si un credit echoue malgre les controles,
        // essayer de restituer les fonds a la source.
        source->deposer(montantCentimes);

        return false;
    }

    // Le debit et le credit ont reussi.
    return true;
}
