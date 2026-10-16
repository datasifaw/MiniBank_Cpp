
#include "Compte.h"
//Ajouter les méthodes de dépôt et de retrait
#include <limits>
#include <stdexcept>


// Constructeur de la classe Compte

Compte::Compte(const std::string& id,
               const std::string& nom,
               std::int64_t soldeInitialCentimes)
    : identifiant(id),
      titulaire(nom),
      soldeCentimes(soldeInitialCentimes)
{
    if (id.empty() || nom.empty()) {
        throw std::invalid_argument(
            "Identifiant ou titulaire invalide."
        );
    }

    if (soldeInitialCentimes < 0) {
        throw std::invalid_argument(
            "Le solde initial ne peut pas etre negatif."
        );
    }
}



// Retourne l'identifiant du compte
std::string Compte::getIdentifiant() const {
    return identifiant;
}

// Retourne le nom du titulaire
std::string Compte::getTitulaire() const {
    return titulaire;
}

// Retourne le solde en centimes
std::int64_t Compte::getSoldeCentimes() const {
    return soldeCentimes;
}




// Deposer de l'argent
bool Compte::deposer(std::int64_t montantCentimes) {
    if (montantCentimes <= 0) {
        return false;
    }

    if (montantCentimes >
        std::numeric_limits<std::int64_t>::max() - soldeCentimes) {
        return false;
    }

    soldeCentimes += montantCentimes;
    return true;
}

// Retirer de l'argent
bool Compte::retirer(std::int64_t montantCentimes) {
    if (montantCentimes <= 0) {
        return false;
    }

    if (montantCentimes > soldeCentimes) {
        return false;
    }

    soldeCentimes -= montantCentimes;
    return true;
}
