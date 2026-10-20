
#include "Banque.h"

#include <iomanip>
#include <sstream>
#include <limits>
#include <exception>
#include <utility>

// ==========================================
// GENERER UN IDENTIFIANT UNIQUE
// ==========================================

std::string Banque::genererIdentifiant() {

    while (true) {

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

    if (titulaire.empty() ||
        soldeInitialCentimes < 0) {
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
// CONSULTER TOUS LES COMPTES
// ==========================================

std::vector<Compte> Banque::consulterTousLesComptes() const {

    // Retourner une copie des comptes.
    // Leur ordre d'insertion est conserve.
    return comptes;
}

// ==========================================
// NOUVEAU : EXPORTER L'ETAT COMPLET
// ==========================================

EtatBanque Banque::exporterEtat() const {

    // NOUVEAU : creer un instantane local.
    //
    // Cet objet est independant des
    // collections internes de Banque.
    EtatBanque etat;

    // NOUVEAU : copier tous les comptes,
    // dans leur ordre de creation.
    etat.comptes = comptes;

    // NOUVEAU : copier l'historique GLOBAL.
    //
    // Contrairement a consulterHistorique(),
    // aucun filtrage par compte n'est effectue.
    etat.operations = historique;

    // NOUVEAU : conserver le compteur exact
    // qui servira au prochain compte.
    etat.prochainNumeroCompte = prochainNumero;

    // NOUVEAU : conserver le compteur exact
    // qui servira a la prochaine operation.
    etat.prochainNumeroOperation =
        prochainNumeroOperation;

    // NOUVEAU : retourner l'instantane.
    //
    // Aucun compte, solde ou historique
    // de la banque n'a ete modifie.
    //
    // Aucun fichier n'est cree.
    return etat;
}

// ==========================================
// PREPARER L'HISTORIQUE
// ==========================================

bool Banque::preparerHistorique(
    std::size_t nombreOperations
) {

    const std::uint64_t maximumNumero =
        std::numeric_limits<std::uint64_t>::max();

    if (nombreOperations == 0 ||
        nombreOperations > maximumNumero) {
        return false;
    }

    if (prochainNumeroOperation >
        maximumNumero - nombreOperations) {
        return false;
    }

    if (nombreOperations >
        historique.max_size() - historique.size()) {
        return false;
    }

    try {

        historique.reserve(
            historique.size() + nombreOperations
        );

    } catch (const std::exception&) {
        return false;
    }

    return true;
}

// ==========================================
// DEPOT AVEC HISTORIQUE
// ==========================================

bool Banque::deposerSurCompte(
    const std::string& idCompte,
    std::int64_t montantCentimes
) {

    Compte* compte = rechercherCompte(idCompte);

    if (compte == nullptr ||
        montantCentimes <= 0) {
        return false;
    }

    const std::int64_t solde =
        compte->getSoldeCentimes();

    const std::int64_t maximum =
        std::numeric_limits<std::int64_t>::max();

    if (montantCentimes > maximum - solde) {
        return false;
    }

    Operation operation;

    try {

        operation.numero = prochainNumeroOperation;
        operation.type = TypeOperation::Depot;
        operation.identifiantCompte = idCompte;
        operation.montantCentimes = montantCentimes;
        operation.identifiantContrepartie = "";

    } catch (const std::exception&) {
        return false;
    }

    if (!preparerHistorique(1)) {
        return false;
    }

    if (!compte->deposer(montantCentimes)) {
        return false;
    }

    historique.push_back(std::move(operation));

    ++prochainNumeroOperation;

    return true;
}

// ==========================================
// RETRAIT AVEC HISTORIQUE
// ==========================================

bool Banque::retirerDuCompte(
    const std::string& idCompte,
    std::int64_t montantCentimes
) {

    Compte* compte = rechercherCompte(idCompte);

    if (compte == nullptr ||
        montantCentimes <= 0) {
        return false;
    }

    if (montantCentimes >
        compte->getSoldeCentimes()) {
        return false;
    }

    Operation operation;

    try {

        operation.numero = prochainNumeroOperation;
        operation.type = TypeOperation::Retrait;
        operation.identifiantCompte = idCompte;
        operation.montantCentimes = montantCentimes;
        operation.identifiantContrepartie = "";

    } catch (const std::exception&) {
        return false;
    }

    if (!preparerHistorique(1)) {
        return false;
    }

    if (!compte->retirer(montantCentimes)) {
        return false;
    }

    historique.push_back(std::move(operation));

    ++prochainNumeroOperation;

    return true;
}

// ==========================================
// VIREMENT ENTRE DEUX COMPTES
// ==========================================

bool Banque::virer(
    const std::string& idSource,
    const std::string& idDestination,
    std::int64_t montantCentimes
) {

    if (idSource.empty() ||
        idDestination.empty() ||
        idSource == idDestination) {
        return false;
    }

    if (montantCentimes <= 0) {
        return false;
    }

    Compte* source = rechercherCompte(idSource);

    Compte* destination =
        rechercherCompte(idDestination);

    if (source == nullptr ||
        destination == nullptr) {
        return false;
    }

    const std::int64_t soldeSource =
        source->getSoldeCentimes();

    const std::int64_t soldeDestination =
        destination->getSoldeCentimes();

    if (montantCentimes > soldeSource) {
        return false;
    }

    const std::int64_t soldeMaximum =
        std::numeric_limits<std::int64_t>::max();

    if (montantCentimes >
        soldeMaximum - soldeDestination) {
        return false;
    }

    // Deux lignes pour un seul virement :
    // une sortante et une entrante.
    Operation operationSortante;
    Operation operationEntrante;

    try {

        operationSortante.numero =
            prochainNumeroOperation;

        operationSortante.type =
            TypeOperation::VirementSortant;

        operationSortante.identifiantCompte =
            idSource;

        operationSortante.montantCentimes =
            montantCentimes;

        operationSortante.identifiantContrepartie =
            idDestination;

        operationEntrante.type =
            TypeOperation::VirementEntrant;

        operationEntrante.identifiantCompte =
            idDestination;

        operationEntrante.montantCentimes =
            montantCentimes;

        operationEntrante.identifiantContrepartie =
            idSource;

    } catch (const std::exception&) {
        return false;
    }

    if (!preparerHistorique(2)) {
        return false;
    }

    operationEntrante.numero =
        prochainNumeroOperation + 1;

    if (!source->retirer(montantCentimes)) {
        return false;
    }

    if (!destination->deposer(montantCentimes)) {

        // Protection de secours pour le virement.
        source->deposer(montantCentimes);

        return false;
    }

    historique.push_back(
        std::move(operationSortante)
    );

    historique.push_back(
        std::move(operationEntrante)
    );

    prochainNumeroOperation += 2;

    return true;
}

// ==========================================
// CONSULTER L'HISTORIQUE D'UN COMPTE
// ==========================================

std::vector<Operation> Banque::consulterHistorique(
    const std::string& idCompte
) const {

    std::vector<Operation> resultat;

    for (const Operation& operation : historique) {

        if (operation.identifiantCompte == idCompte) {
            resultat.push_back(operation);
        }
    }

    return resultat;
}

// ==========================================
// COMPTER LES LIGNES D'HISTORIQUE
// ==========================================

std::size_t Banque::nombreOperations() const {
    return historique.size();
}
