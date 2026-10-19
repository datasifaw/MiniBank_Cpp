
#include "Montant.h"

#include <limits>
#include <string>

// ==========================================
// NOUVEAU : CONVERSION EUROS -> CENTIMES
// ==========================================

bool Montant::convertirEnCentimes(
    const std::string& texte,
    std::int64_t& centimes
) {
    // NOUVEAU : refuser une saisie vide.
    if (texte.empty()) {
        return false;
    }

    // NOUVEAU : utiliser un entier non signe
    // pour construire un montant positif.
    std::uint64_t euros = 0;
    std::uint64_t partieCentimes = 0;

    bool separateurTrouve = false;
    int nombreDecimales = 0;
    bool chiffreTrouve = false;

    const std::uint64_t maximum =
        static_cast<std::uint64_t>(
            std::numeric_limits<std::int64_t>::max()
        );

    // NOUVEAU : lire chaque caractere.
    for (char caractere : texte) {

        if (caractere >= '0' && caractere <= '9') {

            chiffreTrouve = true;

            int chiffre = caractere - '0';

            if (!separateurTrouve) {

                // NOUVEAU : detecter un depassement
                // avant de multiplier par 10.
                if (euros > (maximum / 100 -
                             chiffre) / 10) {
                    return false;
                }

                euros = euros * 10 + chiffre;

            } else {

                // NOUVEAU : maximum deux decimales.
                if (nombreDecimales >= 2) {
                    return false;
                }

                partieCentimes =
                    partieCentimes * 10 + chiffre;

                ++nombreDecimales;
            }

        } else if (
            (caractere == '.' || caractere == ',') &&
            !separateurTrouve
        ) {
            // NOUVEAU : accepter un seul separateur,
            // et seulement apres des chiffres.
            if (!chiffreTrouve) {
                return false;
            }

            separateurTrouve = true;

        } else {
            // NOUVEAU : refuser lettres, espaces,
            // signes et autres caracteres.
            return false;
        }
    }

    if (!chiffreTrouve) {
        return false;
    }

    // NOUVEAU : imposer au moins un chiffre
    // apres un separateur decimal.
    if (separateurTrouve && nombreDecimales == 0) {
        return false;
    }

    // NOUVEAU : "12.5" signifie 12,50 EUR.
    if (nombreDecimales == 1) {
        partieCentimes *= 10;
    }

    // NOUVEAU : verifier le total avant addition.
    if (partieCentimes > maximum - euros * 100) {
        return false;
    }

    // NOUVEAU : affecter le resultat uniquement
    // lorsque toute la saisie a ete validee.
    centimes = static_cast<std::int64_t>(
        euros * 100 + partieCentimes
    );

    return true;
}

// ==========================================
// NOUVEAU : FORMATAGE CENTIMES -> EUROS
// ==========================================

std::string Montant::formater(
    std::int64_t centimes
) {
    // NOUVEAU : gerer les montants negatifs
    // sans provoquer de depassement numerique.
    bool negatif = centimes < 0;

    std::uint64_t valeur;

    if (negatif) {
        valeur = static_cast<std::uint64_t>(
            -(centimes + 1)
        ) + 1;
    } else {
        valeur = static_cast<std::uint64_t>(
            centimes
        );
    }

    std::uint64_t euros = valeur / 100;
    std::uint64_t reste = valeur % 100;

    // NOUVEAU : ajouter un zero devant
    // les centimes inferieurs a 10.
    std::string decimales =
        (reste < 10 ? "0" : "") +
        std::to_string(reste);

    return (negatif ? "-" : "") +
           std::to_string(euros) +
           "," +
           decimales +
           " EUR";
}
