
#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

// ==========================================
// NOUVEAU : TESTS DE LA LISTE DES COMPTES
// ==========================================

#include "Banque.h"
#include "Compte.h"

int main() {

    // ==========================================
    // NOUVEAU : COMPTEURS DE TESTS
    // ==========================================

    int testsReussis = 0;
    int testsEchoues = 0;

    // NOUVEAU : fonction locale qui affiche
    // le resultat de chaque verification.
    auto verifier = [&](bool condition,
                        const std::string& description) {

        if (condition) {
            std::cout << "[OK] " << description << '\n';
            ++testsReussis;
        } else {
            std::cout << "[ECHEC] " << description << '\n';
            ++testsEchoues;
        }
    };

    std::cout << "=== TESTS DE LA LISTE DES COMPTES ===\n\n";

    // ==========================================
    // NOUVEAU : TESTER UNE BANQUE VIDE
    // ==========================================

    Banque banque;

    verifier(
        banque.nombreComptes() == 0,
        "01 - Nouvelle banque sans compte"
    );

    std::vector<Compte> liste =
        banque.consulterTousLesComptes();

    verifier(
        liste.empty(),
        "02 - Consultation d'une banque vide"
    );

    verifier(
        liste.size() == banque.nombreComptes(),
        "03 - Taille de la liste vide coherente"
    );

    verifier(
        banque.nombreOperations() == 0,
        "04 - Consultation vide sans historique"
    );

    // ==========================================
    // NOUVEAU : CREER LE PREMIER COMPTE
    // ==========================================

    std::string idAlice =
        banque.creerCompte("Alice Martin", 50000);

    verifier(
        idAlice == "MB-0001",
        "05 - Identifiant d'Alice correct"
    );

    verifier(
        banque.nombreComptes() == 1,
        "06 - La banque contient un compte"
    );

    liste = banque.consulterTousLesComptes();

    verifier(
        liste.size() == 1,
        "07 - Consultation avec un seul compte"
    );

    verifier(
        liste.size() == 1 &&
        liste[0].getIdentifiant() == idAlice,
        "08 - Identifiant d'Alice dans la liste"
    );

    verifier(
        liste.size() == 1 &&
        liste[0].getTitulaire() == "Alice Martin",
        "09 - Titulaire d'Alice dans la liste"
    );

    verifier(
        liste.size() == 1 &&
        liste[0].getSoldeCentimes() == 50000,
        "10 - Solde initial d'Alice dans la liste"
    );

    // ==========================================
    // NOUVEAU : CONSERVER UNE ANCIENNE COPIE
    // ==========================================

    // Cette copie doit rester independante
    // des futurs changements de la banque.
    std::vector<Compte> ancienneCopie = liste;

    // ==========================================
    // NOUVEAU : CREER LE SECOND COMPTE
    // ==========================================

    std::string idBob =
        banque.creerCompte("Bob Dupont", 120000);

    verifier(
        idBob == "MB-0002",
        "11 - Identifiant de Bob correct"
    );

    verifier(
        banque.nombreComptes() == 2,
        "12 - La banque contient deux comptes"
    );

    liste = banque.consulterTousLesComptes();

    verifier(
        liste.size() == 2,
        "13 - Consultation avec deux comptes"
    );

    verifier(
        liste.size() == 2 &&
        liste[0].getIdentifiant() == idAlice &&
        liste[1].getIdentifiant() == idBob,
        "14 - Ordre de creation conserve"
    );

    verifier(
        liste.size() == 2 &&
        liste[0].getTitulaire() == "Alice Martin" &&
        liste[1].getTitulaire() == "Bob Dupont",
        "15 - Les deux titulaires sont corrects"
    );

    verifier(
        liste.size() == 2 &&
        liste[0].getSoldeCentimes() == 50000 &&
        liste[1].getSoldeCentimes() == 120000,
        "16 - Les soldes initiaux sont corrects"
    );

    verifier(
        ancienneCopie.size() == 1,
        "17 - Ancienne copie toujours a un compte"
    );

    verifier(
        ancienneCopie.size() == 1 &&
        ancienneCopie[0].getIdentifiant() == idAlice,
        "18 - Ancienne copie conserve Alice"
    );

    // ==========================================
    // NOUVEAU : TESTER UN DEPOT
    // ==========================================

    verifier(
        banque.deposerSurCompte(idAlice, 10000),
        "19 - Depot de 100 EUR sur Alice"
    );

    liste = banque.consulterTousLesComptes();

    verifier(
        liste.size() == 2 &&
        liste[0].getSoldeCentimes() == 60000,
        "20 - Liste actualisee apres depot"
    );

    verifier(
        banque.nombreOperations() == 1,
        "21 - Depot enregistre dans l'historique"
    );

    // ==========================================
    // NOUVEAU : TESTER UN RETRAIT
    // ==========================================

    verifier(
        banque.retirerDuCompte(idAlice, 5000),
        "22 - Retrait de 50 EUR sur Alice"
    );

    liste = banque.consulterTousLesComptes();

    verifier(
        liste.size() == 2 &&
        liste[0].getSoldeCentimes() == 55000,
        "23 - Liste actualisee apres retrait"
    );

    verifier(
        banque.nombreOperations() == 2,
        "24 - Retrait enregistre dans l'historique"
    );

    // ==========================================
    // NOUVEAU : TESTER UN VIREMENT
    // ==========================================

    verifier(
        banque.virer(idAlice, idBob, 2500),
        "25 - Virement de 25 EUR vers Bob"
    );

    liste = banque.consulterTousLesComptes();

    verifier(
        liste.size() == 2 &&
        liste[0].getSoldeCentimes() == 52500 &&
        liste[1].getSoldeCentimes() == 122500,
        "26 - Soldes actualises apres virement"
    );

    verifier(
        banque.nombreOperations() == 4,
        "27 - Quatre lignes d'historique au total"
    );

    verifier(
        liste.size() == 2 &&
        liste[0].getIdentifiant() == idAlice &&
        liste[1].getIdentifiant() == idBob,
        "28 - Ordre conserve apres les mouvements"
    );

    // ==========================================
    // NOUVEAU : TESTER LA COPIE INDEPENDANTE
    // ==========================================

    // Modifier le Compte present dans la
    // copie ne doit PAS modifier la banque.
    verifier(
        liste.size() == 2 &&
        liste[0].deposer(10000),
        "29 - Modifier Alice dans la copie"
    );

    verifier(
        liste.size() == 2 &&
        liste[0].getSoldeCentimes() == 62500,
        "30 - La copie d'Alice a change"
    );

    // NOUVEAU : consulter une nouvelle fois
    // pour retrouver les donnees reelles.
    std::vector<Compte> nouvelleCopie =
        banque.consulterTousLesComptes();

    verifier(
        nouvelleCopie.size() == 2 &&
        nouvelleCopie[0].getSoldeCentimes() == 52500,
        "31 - Alice dans Banque reste a 525 EUR"
    );

    verifier(
        banque.nombreOperations() == 4,
        "32 - Modifier une copie ne cree aucun historique"
    );

    verifier(
        liste.size() == 2 &&
        liste[1].getSoldeCentimes() == 122500,
        "33 - Bob dans la copie reste inchange"
    );

    // ==========================================
    // NOUVEAU : TESTER LES CONSULTATIONS REPETEES
    // ==========================================

    const std::size_t nombreAvant =
        banque.nombreComptes();

    const std::size_t operationsAvant =
        banque.nombreOperations();

    std::vector<Compte> consultation1 =
        banque.consulterTousLesComptes();

    std::vector<Compte> consultation2 =
        banque.consulterTousLesComptes();

    verifier(
        consultation1.size() == 2 &&
        consultation2.size() == 2,
        "34 - Deux consultations consecutives"
    );

    verifier(
        banque.nombreComptes() == nombreAvant,
        "35 - Consultations sans creation de compte"
    );

    verifier(
        banque.nombreOperations() == operationsAvant,
        "36 - Consultations sans ajout d'historique"
    );

    verifier(
        consultation1.size() == 2 &&
        consultation2.size() == 2 &&
        consultation1[0].getSoldeCentimes() ==
            consultation2[0].getSoldeCentimes() &&
        consultation1[1].getSoldeCentimes() ==
            consultation2[1].getSoldeCentimes(),
        "37 - Consultations successives coherentes"
    );

    // ==========================================
    // NOUVEAU : CONSULTATION PAR REFERENCE CONST
    // ==========================================

    const Banque& banqueLecture = banque;

    std::vector<Compte> listeLecture =
        banqueLecture.consulterTousLesComptes();

    verifier(
        listeLecture.size() == 2,
        "38 - Consultation possible sur Banque const"
    );

    verifier(
        listeLecture.size() == 2 &&
        listeLecture[0].getSoldeCentimes() == 52500 &&
        listeLecture[1].getSoldeCentimes() == 122500,
        "39 - Lecture seule retourne les bons soldes"
    );

    // ==========================================
    // NOUVEAU : CREATION REFUSEE
    // ==========================================

    verifier(
        !banque.ajouterCompte(
            idAlice,
            "Doublon",
            1000
        ),
        "40 - Identifiant deja utilise refuse"
    );

    verifier(
        banque.nombreComptes() == 2 &&
        banque.consulterTousLesComptes().size() == 2,
        "41 - Creation refusee sans nouveau compte"
    );

    // ==========================================
    // NOUVEAU : VERIFIER L'ANCIENNE COPIE
    // ==========================================

    verifier(
        ancienneCopie.size() == 1 &&
        ancienneCopie[0].getSoldeCentimes() == 50000,
        "42 - Ancienne copie conserve son solde initial"
    );

    // ==========================================
    // NOUVEAU : VERIFIER L'ETAT FINAL
    // ==========================================

    const Compte* alice =
        banqueLecture.rechercherCompte(idAlice);

    const Compte* bob =
        banqueLecture.rechercherCompte(idBob);

    verifier(
        alice != nullptr && bob != nullptr,
        "43 - Comptes originaux toujours presents"
    );

    verifier(
        alice != nullptr &&
        bob != nullptr &&
        alice->getSoldeCentimes() == 52500 &&
        bob->getSoldeCentimes() == 122500,
        "44 - Soldes reels de Banque intacts"
    );

    verifier(
        banque.nombreOperations() == 4,
        "45 - Historique final conserve"
    );

    // ==========================================
    // NOUVEAU : BILAN
    // ==========================================

    std::cout << "\n=== LISTE FINALE DES COMPTES ===\n";

    const std::vector<Compte> comptesFinaux =
        banque.consulterTousLesComptes();

    for (const Compte& compte : comptesFinaux) {

        std::cout << compte.getIdentifiant()
                  << " - "
                  << compte.getTitulaire()
                  << " - "
                  << compte.getSoldeCentimes()
                  << " centimes\n";
    }

    std::cout << "\n=== BILAN DES TESTS ===\n";

    std::cout << "Tests reussis : "
              << testsReussis << '\n';

    std::cout << "Tests echoues : "
              << testsEchoues << '\n';

    return testsEchoues == 0 ? 0 : 1;
}
