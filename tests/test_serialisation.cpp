
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "Banque.h"
#include "Compte.h"
#include "EtatBanque.h"
#include "Operation.h"
#include "Persistance.h"

// ==========================================
// TESTS DE SERIALISATION MINIBANK|1
// Etape 3.9.3.1 - VERSION CORRIGEE
//
// Aucun fichier bancaire n'est cree.
// ==========================================

namespace {

int nombreTests = 0;
int nombreReussites = 0;

// ==========================================
// VERIFIER UNE CONDITION
// ==========================================

void verifier(
    bool condition,
    const std::string& description
) {
    ++nombreTests;

    if (condition) {
        ++nombreReussites;
        std::cout << "[OK]    "
                  << description << '\n';
    } else {
        std::cout << "[ECHEC] "
                  << description << '\n';
    }
}

// ==========================================
// RECHERCHER DU TEXTE
// ==========================================

bool contient(
    const std::string& texte,
    const std::string& extrait
) {
    return texte.find(extrait) !=
        std::string::npos;
}

// ==========================================
// VERIFIER LA FIN DU DOCUMENT
// ==========================================

bool seTerminePar(
    const std::string& texte,
    const std::string& fin
) {
    return texte.size() >= fin.size()
        && texte.compare(
            texte.size() - fin.size(),
            fin.size(),
            fin
        ) == 0;
}

// ==========================================
// VERIFIER UN REFUS DE SERIALISATION
// ==========================================

bool estRefuse(
    const ResultatSerialisation& resultat
) {
    return resultat.statut ==
               StatutSerialisation::DonneesInvalides
        && !resultat.estUnSucces()
        && resultat.texte.empty()
        && !resultat.message.empty();
}

// ==========================================
// TESTER UNE BANQUE VIDE
// ==========================================

void testerBanqueVide() {
    std::cout << "\n=== BANQUE VIDE ===\n";

    const Banque banque;
    const EtatBanque etat = banque.exporterEtat();

    const ResultatSerialisation resultat =
        Persistance::serialiser(etat);

    verifier(
        resultat.estUnSucces(),
        "Banque vide : serialisation reussie"
    );

    verifier(
        resultat.statut ==
            StatutSerialisation::Succes,
        "Banque vide : statut Succes"
    );

    verifier(
        resultat.texte ==
            "MINIBANK|1\n"
            "COUNTERS|1|1\n"
            "ACCOUNTS|0\n"
            "OPERATIONS|0\n"
            "END\n",
        "Banque vide : format exact"
    );

    verifier(
        !resultat.message.empty(),
        "Banque vide : message present"
    );

    verifier(
        etat.comptes.empty()
            && etat.operations.empty(),
        "Banque vide : instantane inchange"
    );
}

// ==========================================
// TESTER DEUX COMPTES
// ==========================================

void testerDeuxComptes() {
    std::cout << "\n=== DEUX COMPTES ===\n";

    Banque banque;

    const std::string alice =
        banque.creerCompte("Alice", 50000);

    const std::string bob =
        banque.creerCompte("Bob", 120000);

    verifier(
        alice == "MB-0001"
            && bob == "MB-0002",
        "Identifiants Alice et Bob"
    );

    const EtatBanque etat = banque.exporterEtat();

    const ResultatSerialisation resultat =
        Persistance::serialiser(etat);

    verifier(
        resultat.estUnSucces(),
        "Deux comptes : serialisation reussie"
    );

    verifier(
        contient(resultat.texte, "MINIBANK|1\n"),
        "Deux comptes : signature MINIBANK|1"
    );

    verifier(
        contient(
            resultat.texte,
            "COUNTERS|3|1\n"
        ),
        "Deux comptes : compteurs corrects"
    );

    verifier(
        contient(
            resultat.texte,
            "ACCOUNTS|2\n"
        ),
        "Deux comptes : nombre de comptes"
    );

    verifier(
        contient(
            resultat.texte,
            "ACCOUNT|4D422D30303031|"
            "416C696365|50000\n"
        ),
        "Alice : identifiant, nom et solde"
    );

    verifier(
        contient(
            resultat.texte,
            "ACCOUNT|4D422D30303032|"
            "426F62|120000\n"
        ),
        "Bob : identifiant, nom et solde"
    );

    verifier(
        contient(
            resultat.texte,
            "OPERATIONS|0\n"
        ),
        "Deux comptes : historique vide"
    );

    verifier(
        seTerminePar(resultat.texte, "END\n"),
        "Deux comptes : marqueur de fin"
    );

    verifier(
        etat.comptes.size() == 2
            && etat.operations.empty(),
        "Deux comptes : instantane preserve"
    );
}

// ==========================================
// TESTER L'HISTORIQUE BANCAIRE
// ==========================================

void testerHistorique() {
    std::cout << "\n=== HISTORIQUE BANCAIRE ===\n";

    Banque banque;

    const std::string alice =
        banque.creerCompte("Alice", 50000);

    const std::string bob =
        banque.creerCompte("Bob", 120000);

    const bool operationsOK =
        !alice.empty()
        && !bob.empty()
        && banque.deposerSurCompte(alice, 10000)
        && banque.retirerDuCompte(alice, 5000)
        && banque.virer(alice, bob, 2500);

    verifier(
        operationsOK,
        "Depot, retrait et virement executes"
    );

    const EtatBanque etat = banque.exporterEtat();

    const ResultatSerialisation resultat =
        Persistance::serialiser(etat);

    verifier(
        resultat.estUnSucces(),
        "Historique : serialisation reussie"
    );

    verifier(
        contient(
            resultat.texte,
            "COUNTERS|3|5\n"
        ),
        "Historique : compteurs 3 et 5"
    );

    verifier(
        contient(
            resultat.texte,
            "OPERATIONS|4\n"
        ),
        "Historique : quatre operations"
    );

    verifier(
        contient(
            resultat.texte,
            "ACCOUNT|4D422D30303031|"
            "416C696365|52500\n"
        ),
        "Historique : solde final Alice"
    );

    verifier(
        contient(
            resultat.texte,
            "ACCOUNT|4D422D30303032|"
            "426F62|122500\n"
        ),
        "Historique : solde final Bob"
    );

    const std::string depot =
        "OP|1|DEPOT|4D422D30303031|10000|-\n";

    const std::string retrait =
        "OP|2|RETRAIT|4D422D30303031|5000|-\n";

    const std::string sortant =
        "OP|3|VIREMENT_SORTANT|"
        "4D422D30303031|2500|4D422D30303032\n";

    const std::string entrant =
        "OP|4|VIREMENT_ENTRANT|"
        "4D422D30303032|2500|4D422D30303031\n";

    verifier(
        contient(resultat.texte, depot),
        "Historique : depot encode"
    );

    verifier(
        contient(resultat.texte, retrait),
        "Historique : retrait encode"
    );

    verifier(
        contient(resultat.texte, sortant),
        "Historique : virement sortant encode"
    );

    verifier(
        contient(resultat.texte, entrant),
        "Historique : virement entrant encode"
    );

    const std::size_t positionDepot =
        resultat.texte.find(depot);

    const std::size_t positionRetrait =
        resultat.texte.find(retrait);

    const std::size_t positionSortant =
        resultat.texte.find(sortant);

    const std::size_t positionEntrant =
        resultat.texte.find(entrant);

    verifier(
        positionDepot != std::string::npos
            && positionRetrait != std::string::npos
            && positionSortant != std::string::npos
            && positionEntrant != std::string::npos
            && positionDepot < positionRetrait
            && positionRetrait < positionSortant
            && positionSortant < positionEntrant,
        "Historique : ordre global conserve"
    );

    verifier(
        etat.operations.size() == 4
            && banque.nombreOperations() == 4,
        "Historique : quatre operations inchangees"
    );
}

// ==========================================
// TESTER LES CARACTERES SPECIAUX
// ==========================================

void testerCaracteresSpeciaux() {
    std::cout << "\n=== CARACTERES SPECIAUX ===\n";

    Banque banque;

    const std::string id =
        banque.creerCompte("Jean | Martin", 12345);

    const EtatBanque etat = banque.exporterEtat();

    const ResultatSerialisation resultat =
        Persistance::serialiser(etat);

    verifier(
        !id.empty() && resultat.estUnSucces(),
        "Nom avec separateur : serialisation"
    );

    verifier(
        contient(
            resultat.texte,
            "4A65616E207C204D617274696E"
        ),
        "Nom avec separateur : hexadecimal exact"
    );

    verifier(
        !contient(resultat.texte, "Jean | Martin"),
        "Nom avec separateur : aucun texte brut"
    );

    verifier(
        contient(resultat.texte, "|12345\n"),
        "Nom avec separateur : solde conserve"
    );

    // NOUVEAU : utiliser les octets UTF-8
    // directement pour rendre le test stable.
    Banque banqueAccent;

    const std::string nomAccent =
        std::string("El") + "\xC3\xA9" + "a";

    const std::string idAccent =
        banqueAccent.creerCompte(nomAccent, 0);

    const ResultatSerialisation accent =
        Persistance::serialiser(
            banqueAccent.exporterEtat()
        );

    verifier(
        !idAccent.empty() && accent.estUnSucces(),
        "Nom accentue : serialisation"
    );

    verifier(
        contient(accent.texte, "456CC3A961"),
        "Nom accentue : octets UTF-8 preserves"
    );
}

// ==========================================
// TESTER LA REPRODUCTIBILITE
// ==========================================

void testerReproductibilite() {
    std::cout << "\n=== REPRODUCTIBILITE ===\n";

    Banque banque;

    banque.creerCompte("Alice", 50000);

    const EtatBanque etat = banque.exporterEtat();

    const ResultatSerialisation premier =
        Persistance::serialiser(etat);

    const ResultatSerialisation deuxieme =
        Persistance::serialiser(etat);

    verifier(
        premier.estUnSucces()
            && deuxieme.estUnSucces(),
        "Deux serialisations reussies"
    );

    verifier(
        premier.texte == deuxieme.texte,
        "Memes donnees : texte identique"
    );

    verifier(
        banque.nombreComptes() == 1
            && banque.nombreOperations() == 0,
        "Serialiser ne modifie pas la banque"
    );
}

// ==========================================
// NOUVEAU : TESTS INVALIDES CORRIGES
// ==========================================

void testerDonneesInvalides() {
    std::cout << "\n=== DONNEES INVALIDES ===\n";

    Banque banque;

    const std::string alice =
        banque.creerCompte("Alice", 50000);

    const std::string bob =
        banque.creerCompte("Bob", 120000);

    const EtatBanque valide =
        banque.exporterEtat();

    // NOUVEAU : tester les compteurs invalides
    // directement dans des copies d'EtatBanque.
    EtatBanque compteurCompteNul = valide;
    compteurCompteNul.prochainNumeroCompte = 0;

    verifier(
        estRefuse(
            Persistance::serialiser(
                compteurCompteNul
            )
        ),
        "Refus : compteur de compte nul"
    );

    EtatBanque compteurOperationNul = valide;
    compteurOperationNul.prochainNumeroOperation = 0;

    verifier(
        estRefuse(
            Persistance::serialiser(
                compteurOperationNul
            )
        ),
        "Refus : compteur d'operation nul"
    );

    // NOUVEAU : le doublon est constructible
    // dans l'instantane, sans contourner Compte.
    EtatBanque doublon = valide;

    doublon.comptes.push_back(
        doublon.comptes.front()
    );

    verifier(
        estRefuse(
            Persistance::serialiser(doublon)
        ),
        "Refus : identifiant en double"
    );

    // ======================================
    // NOUVEAU : CORRECTION DE 3.9.3.1
    // ======================================
    //
    // Les anciens tests qui construisaient :
    //
    //   Compte("", "Titulaire", 0)
    //   Compte("MB-9999", "", 0)
    //
    // ont ete retires.
    //
    // Le constructeur Compte refuse deja
    // ces valeurs avec std::invalid_argument.
    // Leur construction interrompait donc
    // le programme avant la serialisation.
    //
    // Nous testons ici seulement des etats
    // invalides pouvant etre construits
    // sans enfreindre ce contrat.

    // ======================================
    // NOUVEAU : PREPARER UNE OPERATION
    // ======================================

    Operation operation;

    operation.numero = 1;
    operation.type = TypeOperation::Depot;
    operation.identifiantCompte = alice;
    operation.montantCentimes = 100;
    operation.identifiantContrepartie = "";

    EtatBanque montantNul = valide;
    montantNul.operations.push_back(operation);
    montantNul.prochainNumeroOperation = 2;
    montantNul.operations[0].montantCentimes = 0;

    verifier(
        estRefuse(
            Persistance::serialiser(montantNul)
        ),
        "Refus : montant d'operation nul"
    );

    EtatBanque montantNegatif = valide;
    montantNegatif.operations.push_back(operation);
    montantNegatif.prochainNumeroOperation = 2;
    montantNegatif.operations[0].montantCentimes = -1;

    verifier(
        estRefuse(
            Persistance::serialiser(montantNegatif)
        ),
        "Refus : montant d'operation negatif"
    );

    EtatBanque compteInexistant = valide;
    compteInexistant.operations.push_back(operation);
    compteInexistant.prochainNumeroOperation = 2;

    compteInexistant.operations[0]
        .identifiantCompte = "INCONNU";

    verifier(
        estRefuse(
            Persistance::serialiser(
                compteInexistant
            )
        ),
        "Refus : operation sans compte existant"
    );

    EtatBanque numeroNul = valide;
    numeroNul.operations.push_back(operation);
    numeroNul.prochainNumeroOperation = 2;
    numeroNul.operations[0].numero = 0;

    verifier(
        estRefuse(
            Persistance::serialiser(numeroNul)
        ),
        "Refus : numero d'operation nul"
    );

    EtatBanque compteurIncoherent = valide;
    compteurIncoherent.operations.push_back(operation);
    compteurIncoherent.prochainNumeroOperation = 1;

    verifier(
        estRefuse(
            Persistance::serialiser(
                compteurIncoherent
            )
        ),
        "Refus : compteur d'historique incoherent"
    );

    EtatBanque numerosDupliques = valide;
    numerosDupliques.operations.push_back(operation);
    numerosDupliques.operations.push_back(operation);
    numerosDupliques.prochainNumeroOperation = 3;

    verifier(
        estRefuse(
            Persistance::serialiser(
                numerosDupliques
            )
        ),
        "Refus : numeros d'operations dupliques"
    );

    // NOUVEAU : verifier un ordre decroissant
    // avec deux numeros distincts : 2 puis 1.
    EtatBanque numeroDecroissant = valide;

    Operation operationNumeroDeux = operation;
    operationNumeroDeux.numero = 2;

    numeroDecroissant.operations.push_back(
        operationNumeroDeux
    );

    numeroDecroissant.operations.push_back(
        operation
    );

    numeroDecroissant.prochainNumeroOperation = 3;

    verifier(
        estRefuse(
            Persistance::serialiser(
                numeroDecroissant
            )
        ),
        "Refus : numerotation non croissante"
    );

    EtatBanque depotAvecContrepartie = valide;
    depotAvecContrepartie.operations.push_back(
        operation
    );
    depotAvecContrepartie.prochainNumeroOperation = 2;

    depotAvecContrepartie.operations[0]
        .identifiantContrepartie = bob;

    verifier(
        estRefuse(
            Persistance::serialiser(
                depotAvecContrepartie
            )
        ),
        "Refus : depot avec contrepartie"
    );

    EtatBanque typeInconnu = valide;
    typeInconnu.operations.push_back(operation);
    typeInconnu.prochainNumeroOperation = 2;

    typeInconnu.operations[0].type =
        static_cast<TypeOperation>(999);

    verifier(
        estRefuse(
            Persistance::serialiser(
                typeInconnu
            )
        ),
        "Refus : type d'operation inconnu"
    );

    EtatBanque virementSansContrepartie = valide;
    virementSansContrepartie.operations.push_back(
        operation
    );
    virementSansContrepartie.prochainNumeroOperation = 2;

    virementSansContrepartie.operations[0].type =
        TypeOperation::VirementSortant;

    verifier(
        estRefuse(
            Persistance::serialiser(
                virementSansContrepartie
            )
        ),
        "Refus : virement sans contrepartie"
    );

    EtatBanque virementVersSoi = valide;
    virementVersSoi.operations.push_back(operation);
    virementVersSoi.prochainNumeroOperation = 2;

    virementVersSoi.operations[0].type =
        TypeOperation::VirementSortant;

    virementVersSoi.operations[0]
        .identifiantContrepartie = alice;

    verifier(
        estRefuse(
            Persistance::serialiser(
                virementVersSoi
            )
        ),
        "Refus : virement vers le meme compte"
    );

    EtatBanque contrepartieInexistante = valide;
    contrepartieInexistante.operations.push_back(
        operation
    );
    contrepartieInexistante.prochainNumeroOperation = 2;

    contrepartieInexistante.operations[0].type =
        TypeOperation::VirementEntrant;

    contrepartieInexistante.operations[0]
        .identifiantContrepartie = "INCONNU";

    verifier(
        estRefuse(
            Persistance::serialiser(
                contrepartieInexistante
            )
        ),
        "Refus : contrepartie inexistante"
    );
}

} // namespace

// ==========================================
// PROGRAMME PRINCIPAL DE TEST
// ==========================================

int main() {
    std::cout
        << "====================================\n"
        << " TESTS SERIALISATION MINIBANK|1\n"
        << " Etape 3.9.3.1 - CORRIGEE\n"
        << "====================================\n";

    testerBanqueVide();
    testerDeuxComptes();
    testerHistorique();
    testerCaracteresSpeciaux();
    testerReproductibilite();
    testerDonneesInvalides();

    std::cout
        << "\n====================================\n"
        << "Tests executes : " << nombreTests << '\n'
        << "Tests reussis : " << nombreReussites << '\n'
        << "Tests echoues : "
        << nombreTests - nombreReussites << '\n'
        << "====================================\n";

    // NOUVEAU : signaler tout echec
    // a PowerShell par un code non nul.
    return nombreTests == nombreReussites ? 0 : 1;
}
