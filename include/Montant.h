
#ifndef MONTANT_H
#define MONTANT_H

#include <cstdint>
#include <string>

// ==========================================
// NOUVEAU : MODULE DE GESTION DES MONTANTS
// ==========================================

namespace Montant {

    // NOUVEAU : convertir une saisie en euros
    // vers un montant entier en centimes.
    //
    // Exemples :
    // "100"    -> 10000
    // "25.50"  -> 2550
    // "25,50"  -> 2550
    //
    // Retourne true si la conversion reussit.
    // Retourne false si la saisie est invalide.
    //
    // En cas d'echec, centimes reste inchange.
    bool convertirEnCentimes(
        const std::string& texte,
        std::int64_t& centimes
    );

    // NOUVEAU : transformer des centimes
    // en chaine de caracteres lisible.
    //
    // Exemple :
    // 2550 -> "25,50 EUR"
    std::string formater(
        std::int64_t centimes
    );

}

#endif
