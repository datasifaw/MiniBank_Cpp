#include <cstdint>
#include <iostream>
#include <string>

#include "Banque.h"
#include "Persistance.h"

// ==========================================
// NOUVEAU : TESTS RESTAURATION ATOMIQUE
// Etape 3.9.4.5 - aucun acces disque.
// ==========================================

namespace {

int total = 0;
int reussis = 0;

void verifier(bool ok, const std::string& nom) {
    ++total;
    if (ok) { ++reussis; }
    std::cout << (ok ? "[OK]    " : "[ECHEC] ") << nom << '\n';
}

std::string empreinte(const Banque& banque) {
    const auto resultat = Persistance::serialiser(banque.exporterEtat());
    return resultat.estUnSucces() ? resultat.texte : "";
}

// NOUVEAU : comparer tous les comptes, operations et compteurs
// grace au format canonique deja teste.
void verifierEtat(const Banque& banque, const EtatBanque& attendu,
                 const std::string& nom) {
    const auto reference = Persistance::serialiser(attendu);
    verifier(reference.estUnSucces(), nom + " : etat attendu valide");
    verifier(reference.estUnSucces() && empreinte(banque) == reference.texte,
             nom + " : etat entier identique");
}

// NOUVEAU : chaque refus doit laisser intacts comptes,
// historique et compteurs de la banque active.
void verifierRefus(Banque& banque, const EtatBanque& invalide,
                   const std::string& nom) {
    const std::string avant = empreinte(banque);
    const auto resultat = banque.restaurerEtat(invalide);
    verifier(resultat.statut == StatutRestauration::EtatInvalide,
             nom + " : statut EtatInvalide");
    verifier(!resultat.estUnSucces() && !resultat.message.empty(),
             nom + " : refus et message");
    verifier(!avant.empty() && empreinte(banque) == avant,
             nom + " : banque intacte");
}

void testerBanqueVide() {
    std::cout << "\n=== BANQUE VIDE ===\n";
    Banque active;
    active.creerCompte("Ancien", 1000);
    Banque vide;
    const EtatBanque attendu = vide.exporterEtat();
    const auto r = active.restaurerEtat(attendu);
    verifier(r.estUnSucces(), "Restaurer la banque vide");
    verifierEtat(active, attendu, "Banque vide");
    verifier(active.creerCompte("Nouveau", 20) == "MB-0001",
             "Compte numero 1 apres etat vide");
}

void testerRemplacementEtHistorique() {
    std::cout << "\n=== REMPLACEMENT ET HISTORIQUE ===\n";
    Banque source;
    const std::string alice = source.creerCompte("Alice", 50000);
    const std::string bob = source.creerCompte("Bob", 120000);
    verifier(alice == "MB-0001" && bob == "MB-0002",
             "Identifiants de reference");
    verifier(source.deposerSurCompte(alice, 10000), "Depot source");
    verifier(source.retirerDuCompte(alice, 5000), "Retrait source");
    verifier(source.virer(alice, bob, 2500), "Virement source");

    const EtatBanque attendu = source.exporterEtat();
    Banque active;
    active.creerCompte("Ancien", 40);
    const auto r = active.restaurerEtat(attendu);
    verifier(r.estUnSucces() && !r.message.empty(), "Restauration valide");
    verifierEtat(active, attendu, "Comptes et historique");
    verifier(active.nombreComptes() == 2, "Anciens comptes remplaces");
    verifier(active.nombreOperations() == 4, "Quatre operations conservees");
    verifier(active.rechercherCompte("MB-0001") != nullptr,
             "Alice retrouvee");

    verifier(active.creerCompte("Claire", 200) == "MB-0003",
             "Identifiant suivant MB-0003");
    verifier(active.deposerSurCompte(alice, 100),
             "Depot possible apres restauration");
    const EtatBanque apres = active.exporterEtat();
    verifier(apres.operations.size() == 5,
             "Nouvelle ligne d'historique");
    verifier(!apres.operations.empty() && apres.operations.back().numero == 5,
             "Numero operation suivant : 5");
    verifier(apres.prochainNumeroCompte == 4 &&
             apres.prochainNumeroOperation == 6,
             "Deux compteurs correctement avances");
}

void testerCompteursAvances() {
    std::cout << "\n=== COMPTEURS AVANCES ===\n";
    Banque source;
    source.creerCompte("Alice", 100);
    EtatBanque etat = source.exporterEtat();
    // NOUVEAU : les compteurs peuvent avoir des trous legitimes.
    etat.prochainNumeroCompte = 12;
    etat.prochainNumeroOperation = 17;
    Banque cible;
    verifier(cible.restaurerEtat(etat).estUnSucces(),
             "Accepte les compteurs avances");
    verifier(cible.creerCompte("Bob", 50) == "MB-0012",
             "Reprend le compteur de compte 12");
    verifier(cible.deposerSurCompte("MB-0001", 1),
             "Depot possible avec compteur avance");
    const EtatBanque apres = cible.exporterEtat();
    verifier(!apres.operations.empty() && apres.operations.back().numero == 17,
             "Operation numero 17");
    verifier(apres.prochainNumeroCompte == 13 &&
             apres.prochainNumeroOperation == 18,
             "Compteurs 13 et 18");
}

void testerEtatsInvalides() {
    std::cout << "\n=== REFUS SANS MODIFICATION ===\n";
    Banque banque;
    const auto id = banque.creerCompte("Protege", 5000);
    banque.deposerSurCompte(id, 200);
    const EtatBanque bon = banque.exporterEtat();

    EtatBanque invalide = bon;
    invalide.prochainNumeroCompte = 0;
    verifierRefus(banque, invalide, "Compteur compte nul");

    invalide = bon;
    invalide.prochainNumeroOperation = 0;
    verifierRefus(banque, invalide, "Compteur operation nul");

    invalide = bon;
    invalide.prochainNumeroCompte = 1;
    verifierRefus(banque, invalide, "Collision identifiant futur");

    invalide = bon;
    invalide.comptes.push_back(invalide.comptes.front());
    verifierRefus(banque, invalide, "Identifiant duplique");

    invalide = bon;
    invalide.operations.front().numero = 0;
    verifierRefus(banque, invalide, "Numero operation nul");

    invalide = bon;
    invalide.operations.front().identifiantCompte = "ABSENT";
    verifierRefus(banque, invalide, "Compte operation inexistant");

    invalide = bon;
    invalide.operations.front().montantCentimes = 0;
    verifierRefus(banque, invalide, "Montant operation nul");

    invalide = bon;
    invalide.operations.front().identifiantContrepartie = id;
    verifierRefus(banque, invalide, "Depot avec contrepartie");
}

void testerEtatDeserialise() {
    std::cout << "\n=== ETAT DESERIALISE ===\n";
    Banque origine;
    const std::string id = origine.creerCompte("Alice", 12345);
    origine.deposerSurCompte(id, 50);
    const auto texte = Persistance::serialiser(origine.exporterEtat());
    verifier(texte.estUnSucces(), "Texte MINIBANK|1 disponible");
    if (!texte.estUnSucces()) { return; }
    const auto charge = Persistance::deserialiser(texte.texte);
    verifier(charge.estUnSucces(), "Etat deserialise et valide");
    if (!charge.estUnSucces()) { return; }
    Banque cible;
    cible.creerCompte("Ancien", 1);
    verifier(cible.restaurerEtat(charge.etat).estUnSucces(),
             "Restauration depuis deserialisation");
    verifierEtat(cible, origine.exporterEtat(), "Reconstruction exacte");
}

} // namespace

int main() {
    std::cout << "=== RESTAURATION ATOMIQUE 3.9.4.5 ===\n";
    testerBanqueVide();
    testerRemplacementEtHistorique();
    testerCompteursAvances();
    testerEtatsInvalides();
    testerEtatDeserialise();
    std::cout << "\nTests executes : " << total
              << "\nTests reussis : " << reussis
              << "\nTests echoues : " << total - reussis << '\n';
    return total == reussis ? 0 : 1;
}
