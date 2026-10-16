
#include <iostream>
#include <stdexcept>
#include "Compte.h"

int main() {

    Compte compte("MB-TEST", "Client Test", 50000);

    std::cout << "=== TESTS COMPTE ===\n";

    // Test 1 : Depot nul
    std::cout << "Depot nul refuse : "
              << (!compte.deposer(0) ? "OK" : "ECHEC")
              << '\n';

    // Test 2 : Depot negatif
    std::cout << "Depot negatif refuse : "
              << (!compte.deposer(-5000) ? "OK" : "ECHEC")
              << '\n';

    // Test 3 : Retrait negatif
    std::cout << "Retrait negatif refuse : "
              << (!compte.retirer(-2000) ? "OK" : "ECHEC")
              << '\n';

    // Test 4 : Solde insuffisant
    std::cout << "Solde insuffisant : "
              << (!compte.retirer(100000) ? "OK" : "ECHEC")
              << '\n';

    // Test 5 : Solde initial negatif
    bool exceptionDetectee = false;

    try {
        Compte invalide("MB-TEST2", "Test", -100);
    }
    catch (const std::invalid_argument&) {
        exceptionDetectee = true;
    }

    std::cout << "Solde initial negatif refuse : "
              << (exceptionDetectee ? "OK" : "ECHEC")
              << '\n';

    // Test 6 : Solde inchange apres les refus
    std::cout << "Solde inchange : "
              << (compte.getSoldeCentimes() == 50000
                      ? "OK" : "ECHEC")
              << '\n';

    return 0;
}
