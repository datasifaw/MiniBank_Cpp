
#ifndef OPERATION_H
#define OPERATION_H

// ==========================================
// NOUVEAU : MODELE D'OPERATION BANCAIRE
// ==========================================

#include <cstdint>
#include <string>

// ==========================================
// NOUVEAU : TYPES D'OPERATIONS
// ==========================================

// enum class definit un ensemble de valeurs
// nommees et fortement typees.
//
// Nous distinguons quatre types de mouvements
// afin de pouvoir afficher les operations
// du point de vue de chaque compte.

enum class TypeOperation {

    Depot,

    Retrait,

    VirementSortant,

    VirementEntrant
};

// ==========================================
// NOUVEAU : STRUCTURE OPERATION
// ==========================================

// Cette structure represente UNE ligne
// de l'historique d'un compte bancaire.
//
// Elle ne modifie aucun solde.
// Elle contient uniquement les informations
// necessaires pour decrire un mouvement.

struct Operation {

    // NOUVEAU : ordre chronologique logique.
    // Une operation plus recente aura un
    // numero d'ordre plus eleve.
    std::uint64_t numero = 0;

    // NOUVEAU : nature de l'operation.
    TypeOperation type = TypeOperation::Depot;

    // NOUVEAU : identifiant du compte auquel
    // appartient cette ligne d'historique.
    std::string identifiantCompte;

    // NOUVEAU : montant de l'operation
    // exprime en centimes.
    //
    // Convention retenue :
    // le montant est toujours POSITIF.
    // Le type d'operation permet de savoir
    // s'il s'agit d'une entree ou d'une sortie.
    std::int64_t montantCentimes = 0;

    // NOUVEAU : compte associe au mouvement.
    //
    // Depot / Retrait :
    // chaine vide.
    //
    // Virement sortant :
    // identifiant du destinataire.
    //
    // Virement entrant :
    // identifiant du compte source.
    std::string identifiantContrepartie;
};

// ==========================================
// NOUVEAU : CONVERTIR UN TYPE EN TEXTE
// ==========================================

// inline permet de definir cette fonction
// directement dans un fichier .h sans
// provoquer de definitions multiples lorsque
// plusieurs fichiers .cpp l'utilisent.

inline std::string typeOperationEnTexte(
    TypeOperation type
) {

    switch (type) {

        case TypeOperation::Depot:
            return "DEPOT";

        case TypeOperation::Retrait:
            return "RETRAIT";

        case TypeOperation::VirementSortant:
            return "VIREMENT SORTANT";

        case TypeOperation::VirementEntrant:
            return "VIREMENT ENTRANT";
    }

    // Protection pour une valeur non reconnue.
    return "OPERATION INCONNUE";
}

#endif
