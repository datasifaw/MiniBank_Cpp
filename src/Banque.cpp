
#include "Banque.h"

#include <iomanip>
#include <sstream>
#include <limits>

// NOUVEAU : exceptions liees notamment
// aux allocations et aux chaines.
#include <exception>

// NOUVEAU : std::move pour deplacer les
// operations preparees dans l'historique.
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
// NOUVEAU : PREPARER L'HISTORIQUE
// ==========================================

bool Banque::preparerHistorique(
    std::size_t nombreOperations
) {

    // NOUVEAU : chaque operation doit pouvoir
    // recevoir un numero sans debordement.
    //
    // On conserve toujours un prochain numero
    // representable apres l'enregistrement.
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

    // NOUVEAU : verifier que le vector peut
    // contenir les nouvelles operations.
    if (nombreOperations >
        historique.max_size() - historique.size()) {
        return false;
    }

    // NOUVEAU : reserver l'espace AVANT
    // de modifier les comptes.
    try {

        historique.reserve(
            historique.size() + nombreOperations
        );

    } catch (const std::exception&) {

        // Si la reservation echoue,
        // aucune operation bancaire n'a
        // encore modifie les soldes.
        return false;
    }

    return true;
}

// ==========================================
// NOUVEAU : DEPOT AVEC HISTORIQUE
// ==========================================

bool Banque::deposerSurCompte(
    const std::string& idCompte,
    std::int64_t montantCentimes
) {

    // NOUVEAU : chercher le compte concerne.
    Compte* compte = rechercherCompte(idCompte);

    if (compte == nullptr ||
        montantCentimes <= 0) {
        return false;
    }

    const std::int64_t solde =
        compte->getSoldeCentimes();

    const std::int64_t maximum =
        std::numeric_limits<std::int64_t>::max();

    // NOUVEAU : le depot ne doit pas
    // provoquer de depassement numerique.
    if (montantCentimes > maximum - solde) {
        return false;
    }

    // NOUVEAU : preparer la ligne complete
    // avant de modifier le solde.
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

    // NOUVEAU : reserver une ligne.
    if (!preparerHistorique(1)) {
        return false;
    }

    // NOUVEAU : appliquer le mouvement.
    if (!compte->deposer(montantCentimes)) {
        return false;
    }

    // NOUVEAU : l'operation est deja preparee
    // et la capacite necessaire est reservee.
    historique.push_back(std::move(operation));

    ++prochainNumeroOperation;

    return true;
}

// ==========================================
// NOUVEAU : RETRAIT AVEC HISTORIQUE
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

    // NOUVEAU : verifier les fonds avant
    // de preparer l'historique.
    if (montantCentimes >
        compte->getSoldeCentimes()) {
        return false;
    }

    // NOUVEAU : preparer une ligne Retrait.
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

    // NOUVEAU : retirer les fonds.
    if (!compte->retirer(montantCentimes)) {
        return false;
    }

    // NOUVEAU : enregistrer le retrait reussi.
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

    // ======================================
    // VALIDATION DES IDENTIFIANTS
    // ======================================

    if (idSource.empty() ||
        idDestination.empty() ||
        idSource == idDestination) {
        return false;
    }

    // ======================================
    // VALIDATION DU MONTANT
    // ======================================

    if (montantCentimes <= 0) {
        return false;
    }

    // ======================================
    // RECHERCHE DES COMPTES
    // ======================================

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

    // ======================================
    // VERIFIER LE SOLDE SOURCE
    // ======================================

    if (montantCentimes > soldeSource) {
        return false;
    }

    // ======================================
    // VERIFIER LA LIMITE DU DESTINATAIRE
    // ======================================

    const std::int64_t soldeMaximum =
        std::numeric_limits<std::int64_t>::max();

    if (montantCentimes >
        soldeMaximum - soldeDestination) {
        return false;
    }

    // ======================================
    // NOUVEAU : PREPARER LES DEUX HISTORIQUES
    // ======================================

    // NOUVEAU : un virement reussi donne
    // une ligne pour le compte source
    // et une ligne pour le destinataire.
    Operation operationSortante;
    Operation operationEntrante;

    try {

        // NOUVEAU : ligne du compte source.
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

        // NOUVEAU : ligne du destinataire.
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

    // NOUVEAU : deux places et deux numeros
    // doivent etre disponibles.
    if (!preparerHistorique(2)) {
        return false;
    }

    // NOUVEAU : attribuer le deuxieme numero
    // seulement apres avoir verifie la limite.
    operationEntrante.numero =
        prochainNumeroOperation + 1;

    // ======================================
    // EFFECTUER LE VIREMENT
    // ======================================

    if (!source->retirer(montantCentimes)) {
        return false;
    }

    if (!destination->deposer(montantCentimes)) {

        // Protection de secours conservee
        // depuis l'etape 3.6.1.
        source->deposer(montantCentimes);

        return false;
    }

    // ======================================
    // NOUVEAU : ENREGISTRER LE VIREMENT
    // ======================================

    // NOUVEAU : les deux lignes ont ete
    // preparees et l'espace a ete reserve
    // avant le transfert d'argent.
    historique.push_back(
        std::move(operationSortante)
    );

    historique.push_back(
        std::move(operationEntrante)
    );

    // NOUVEAU : deux numeros ont ete utilises.
    prochainNumeroOperation += 2;

    return true;
}

// ==========================================
// NOUVEAU : CONSULTER L'HISTORIQUE D'UN COMPTE
// ==========================================

std::vector<Operation> Banque::consulterHistorique(
    const std::string& idCompte
) const {

    // NOUVEAU : la copie retournee permet
    // de consulter sans modifier l'historique
    // interne de la banque.
    std::vector<Operation> resultat;

    for (const Operation& operation : historique) {

        if (operation.identifiantCompte == idCompte) {

            resultat.push_back(operation);
        }
    }

    return resultat;
}


// ==========================================
// NOUVEAU : COMPTER LES LIGNES D'HISTORIQUE
// ==========================================

std::size_t Banque::nombreOperations() const {

    return historique.size();
}
