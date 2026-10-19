
#ifndef BANQUE_H
#define BANQUE_H

#include <vector>
#include <string>
#include <cstddef>
#include <cstdint>

// NOUVEAU : bibliotheque pour le compteur d'identifiants
#include <cstdint>

#include "Compte.h"

class Banque {

private:
    // Collection des comptes bancaires
    std::vector<Compte> comptes;

    // ==========================================
    // NOUVEAU : GENERATION DES IDENTIFIANTS
    // ==========================================

    // Prochain numero disponible
    std::uint64_t prochainNumero = 1;

    // Generer un identifiant unique
    std::string genererIdentifiant();

public:

    // Methode existante : conserver pour les tests
    bool ajouterCompte(
        const std::string& id,
        const std::string& titulaire,
        std::int64_t soldeInitialCentimes
    );

    // ==========================================
    // NOUVEAU : CREATION AUTOMATIQUE
    // ==========================================

    // Cree un compte et retourne son identifiant.
    // Retourne une chaine vide si la creation echoue.
    std::string creerCompte(
        const std::string& titulaire,
        std::int64_t soldeInitialCentimes
    );

    // Rechercher un compte modifiable
    Compte* rechercherCompte(const std::string& id);

    // Rechercher un compte en lecture seule
    const Compte* rechercherCompte(
        const std::string& id
    ) const;

    // Obtenir le nombre total de comptes
    std::size_t nombreComptes() const;
};

#endif
