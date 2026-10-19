
#include <iostream>
#include <string>
#include <cstdint>
#include <limits>

#include "Montant.h"

int main() {

    // ==========================================
    // NOUVEAU : PREPARATION DES TESTS
    // ==========================================

    int testsReussis = 0;
    int testsEchoues = 0;

    auto verifier = [&](bool condition,
                        const std::string& nomTest) {

        if (condition) {
            std::cout << "[OK] " << nomTest << '\n';
            ++testsReussis;
        } else {
            std::cout << "[ECHEC] " << nomTest << '\n';
            ++testsEchoues;
        }
    };

    std::cout << "=== TESTS DU MODULE MONTANT ===\n\n";

    std::int64_t centimes = 0;

    // ==========================================
    // NOUVEAU : TESTS DES MONTANTS VALIDES
    // ==========================================

    verifier(
        Montant::convertirEnCentimes("500", centimes)
        && centimes == 50000,
        "500 EUR -> 50000 centimes"
    );

    verifier(
        Montant::convertirEnCentimes("25.50", centimes)
        && centimes == 2550,
        "25.50 EUR -> 2550 centimes"
    );

    verifier(
        Montant::convertirEnCentimes("25,50", centimes)
        && centimes == 2550,
        "25,50 EUR -> 2550 centimes"
    );

    verifier(
        Montant::convertirEnCentimes("12.5", centimes)
        && centimes == 1250,
        "12.5 EUR -> 1250 centimes"
    );

    verifier(
        Montant::convertirEnCentimes("0", centimes)
        && centimes == 0,
        "Montant nul accepte"
    );

    verifier(
        Montant::convertirEnCentimes("0.99", centimes)
        && centimes == 99,
        "0.99 EUR -> 99 centimes"
    );

    // ==========================================
    // NOUVEAU : TESTS DES SAISIES INVALIDES
    // ==========================================

    verifier(
        !Montant::convertirEnCentimes("", centimes),
        "Saisie vide refusee"
    );

    verifier(
        !Montant::convertirEnCentimes("-10", centimes),
        "Montant negatif refuse"
    );

    verifier(
        !Montant::convertirEnCentimes("abc", centimes),
        "Texte non numerique refuse"
    );

    verifier(
        !Montant::convertirEnCentimes("10.999", centimes),
        "Trois decimales refusees"
    );

    verifier(
        !Montant::convertirEnCentimes("10.", centimes),
        "Separateur final refuse"
    );

    verifier(
        !Montant::convertirEnCentimes("10.5.2", centimes),
        "Deux separateurs refuses"
    );

    verifier(
        !Montant::convertirEnCentimes("10 20", centimes),
        "Espaces refuses"
    );

    verifier(
        !Montant::convertirEnCentimes(
            "999999999999999999999999", centimes),
        "Depassement numerique refuse"
    );

    // ==========================================
    // NOUVEAU : TEST DES LIMITES NUMERIQUES
    // ==========================================

    verifier(
        Montant::convertirEnCentimes(
            "92233720368547758.07", centimes)
        && centimes ==
           std::numeric_limits<std::int64_t>::max(),
        "Montant maximum accepte"
    );

    verifier(
        !Montant::convertirEnCentimes(
            "92233720368547758.08", centimes),
        "Montant superieur au maximum refuse"
    );

    // ==========================================
    // NOUVEAU : PRESERVATION DU RESULTAT
    // ==========================================

    centimes = 12345;

    bool conversion =
        Montant::convertirEnCentimes("erreur", centimes);

    verifier(
        !conversion && centimes == 12345,
        "Une erreur ne modifie pas le resultat"
    );

    // ==========================================
    // NOUVEAU : TESTS DU FORMATAGE
    // ==========================================

    verifier(
        Montant::formater(50050) == "500,50 EUR",
        "Formatage de 50050 centimes"
    );

    verifier(
        Montant::formater(1250) == "12,50 EUR",
        "Formatage de 1250 centimes"
    );

    verifier(
        Montant::formater(5) == "0,05 EUR",
        "Formatage de 5 centimes"
    );

    verifier(
        Montant::formater(0) == "0,00 EUR",
        "Formatage du montant nul"
    );

    verifier(
        Montant::formater(-1250) == "-12,50 EUR",
        "Formatage d'un montant negatif"
    );

    // ==========================================
    // NOUVEAU : BILAN
    // ==========================================

    std::cout << "\n=== BILAN ===\n";
    std::cout << "Tests reussis : "
              << testsReussis << '\n';
    std::cout << "Tests echoues : "
              << testsEchoues << '\n';

    return testsEchoues == 0 ? 0 : 1;
}
