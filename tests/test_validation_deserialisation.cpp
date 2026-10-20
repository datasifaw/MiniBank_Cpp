
#include <iostream>
#include <string>

#include "Banque.h"
#include "Persistance.h"

// ==========================================
// NOUVEAU : VALIDATION RIGOUREUSE
// Etape 3.9.4.3 - MINIBANK|1
//
// Tests exclusivement en memoire.
// Aucun acces aux fichiers bancaires.
// ==========================================

namespace {

int tests = 0;
int reussites = 0;
int refusTestes = 0;

// ==========================================
// NOUVEAU : ASSERTION
// ==========================================

void verifier(
    bool condition,
    const std::string& nom
) {
    ++tests;

    if (condition) {
        ++reussites;
    }

    std::cout
        << (condition ? "[OK]    " : "[ECHEC] ")
        << nom << '\n';
}

// ==========================================
// NOUVEAU : REMPLACER UN FRAGMENT
// ==========================================

std::string remplacer(
    const std::string& texte,
    const std::string& ancien,
    const std::string& nouveau
) {
    const std::size_t position =
        texte.find(ancien);

    if (position == std::string::npos) {
        return "";
    }

    std::string copie = texte;
    copie.replace(position, ancien.size(), nouveau);
    return copie;
}

// ==========================================
// NOUVEAU : DOCUMENTS DE REFERENCE
// ==========================================

const std::string VIDE =
    "MINIBANK|1\n"
    "COUNTERS|1|1\n"
    "ACCOUNTS|0\n"
    "OPERATIONS|0\n"
    "END\n";

const std::string COMPTES =
    "MINIBANK|1\n"
    "COUNTERS|3|1\n"
    "ACCOUNTS|2\n"
    "ACCOUNT|4D422D30303031|416C696365|50000\n"
    "ACCOUNT|4D422D30303032|426F62|120000\n"
    "OPERATIONS|0\n"
    "END\n";

const std::string LIGNE_DEPOT =
    "OP|1|DEPOT|4D422D30303031|100|-";

const std::string AVEC_DEPOT =
    "MINIBANK|1\n"
    "COUNTERS|3|2\n"
    "ACCOUNTS|2\n"
    "ACCOUNT|4D422D30303031|416C696365|50000\n"
    "ACCOUNT|4D422D30303032|426F62|120000\n"
    "OPERATIONS|1\n"
    "OP|1|DEPOT|4D422D30303031|100|-\n"
    "END\n";

// ==========================================
// NOUVEAU : TESTER UN REFUS
// ==========================================

void refus(
    const std::string& nom,
    const std::string& texte,
    StatutDeserialisation attendu
) {
    ++refusTestes;

    const auto resultat =
        Persistance::deserialiser(texte);

    verifier(
        !resultat.estUnSucces()
            && resultat.statut == attendu,
        nom + " : statut de refus"
    );

    verifier(
        resultat.etat.comptes.empty()
            && resultat.etat.operations.empty(),
        nom + " : aucun etat partiel"
    );

    verifier(
        !resultat.message.empty(),
        nom + " : diagnostic"
    );

    // NOUVEAU : faciliter le diagnostic
    // si le statut ne correspond pas.
    if (resultat.statut != attendu) {
        std::cout
            << "Diagnostic : "
            << resultat.message << '\n';
    }
}

// ==========================================
// NOUVEAU : TESTER UN ALLER-RETOUR
// ==========================================

void allerRetour(
    const Banque& banque,
    const std::string& nom
) {
    const auto etat = banque.exporterEtat();

    const auto original =
        Persistance::serialiser(etat);

    verifier(
        original.estUnSucces(),
        nom + " : serialisation"
    );

    if (!original.estUnSucces()) {
        return;
    }

    const auto resultat =
        Persistance::deserialiser(original.texte);

    verifier(
        resultat.estUnSucces(),
        nom + " : deserialisation"
    );

    if (!resultat.estUnSucces()) {
        return;
    }

    const auto copie =
        Persistance::serialiser(resultat.etat);

    verifier(
        copie.estUnSucces()
            && copie.texte == original.texte,
        nom + " : texte identique"
    );

    verifier(
        resultat.etat.comptes.size() ==
            etat.comptes.size()
        && resultat.etat.operations.size() ==
            etat.operations.size()
        && resultat.etat.prochainNumeroCompte ==
            etat.prochainNumeroCompte
        && resultat.etat.prochainNumeroOperation ==
            etat.prochainNumeroOperation,
        nom + " : etat complet"
    );
}

// ==========================================
// NOUVEAU : CAS VALIDES
// ==========================================

void testerCasValides() {
    std::cout << "\n=== CAS VALIDES ===\n";

    Banque vide;
    allerRetour(vide, "Banque vide");

    Banque banque;

    const std::string alice =
        banque.creerCompte("Alice", 50000);

    const std::string bob =
        banque.creerCompte("Bob", 120000);

    allerRetour(banque, "Deux comptes");

    const bool operationsOK =
        banque.deposerSurCompte(alice, 10000)
        && banque.retirerDuCompte(alice, 5000)
        && banque.virer(alice, bob, 2500);

    verifier(
        operationsOK,
        "Depot, retrait et virement"
    );

    if (operationsOK) {
        allerRetour(banque, "Historique complet");
    }

    Banque utf8;

    utf8.creerCompte(
        std::string("El") + "\xC3\xA9"
            + "a | Martin",
        12345
    );

    allerRetour(utf8, "Nom UTF-8 et separateur");

    Banque zero;
    zero.creerCompte("Solde nul", 0);
    allerRetour(zero, "Solde nul");
}

// ==========================================
// NOUVEAU : STRUCTURES INVALIDES
// ==========================================

void testerStructure() {
    std::cout << "\n=== STRUCTURE ===\n";

    using S = StatutDeserialisation;

    refus("Texte vide", "", S::FormatInvalide);

    refus(
        "Version 2",
        remplacer(VIDE, "MINIBANK|1", "MINIBANK|2"),
        S::VersionInconnue
    );

    refus(
        "Signature inconnue",
        remplacer(VIDE, "MINIBANK|1", "AUTRE|1"),
        S::VersionInconnue
    );

    refus(
        "LF final absent",
        VIDE.substr(0, VIDE.size() - 1),
        S::FormatInvalide
    );

    refus(
        "CRLF",
        remplacer(
            VIDE,
            "MINIBANK|1\n",
            "MINIBANK|1\r\n"
        ),
        S::FormatInvalide
    );

    refus(
        "Octet NUL",
        VIDE + std::string(1, '\0'),
        S::FormatInvalide
    );

    refus(
        "Ligne apres END",
        VIDE + "INTRUS\n",
        S::FormatInvalide
    );

    refus(
        "Deux END",
        VIDE + "END\n",
        S::FormatInvalide
    );

    refus(
        "Ligne vide finale",
        VIDE + "\n",
        S::FormatInvalide
    );

    refus(
        "COUNTERS absent",
        remplacer(VIDE, "COUNTERS|1|1\n", ""),
        S::FormatInvalide
    );

    refus(
        "ACCOUNTS absent",
        remplacer(VIDE, "ACCOUNTS|0\n", ""),
        S::FormatInvalide
    );

    refus(
        "OPERATIONS absent",
        remplacer(VIDE, "OPERATIONS|0\n", ""),
        S::FormatInvalide
    );

    refus(
        "Champ COUNTERS supplementaire",
        remplacer(
            VIDE,
            "COUNTERS|1|1",
            "COUNTERS|1|1|EXTRA"
        ),
        S::FormatInvalide
    );

    refus(
        "Champ ACCOUNT supplementaire",
        remplacer(
            COMPTES,
            "416C696365|50000",
            "416C696365|50000|EXTRA"
        ),
        S::FormatInvalide
    );

    refus(
        "Champ OP supplementaire",
        remplacer(
            AVEC_DEPOT,
            "|100|-\n",
            "|100|-|EXTRA\n"
        ),
        S::FormatInvalide
    );

    refus(
        "Compte manquant",
        remplacer(
            VIDE,
            "ACCOUNTS|0",
            "ACCOUNTS|1"
        ),
        S::FormatInvalide
    );

    refus(
        "Operation manquante",
        remplacer(
            VIDE,
            "OPERATIONS|0",
            "OPERATIONS|1"
        ),
        S::FormatInvalide
    );

    refus(
        "Compte excedentaire",
        remplacer(
            COMPTES,
            "ACCOUNTS|2",
            "ACCOUNTS|1"
        ),
        S::FormatInvalide
    );

    refus(
        "Mauvais marqueur END",
        remplacer(VIDE, "END\n", "FIN\n"),
        S::FormatInvalide
    );
}

// ==========================================
// NOUVEAU : HEXADECIMAL
// ==========================================

void testerHexadecimal() {
    std::cout << "\n=== HEXADECIMAL ===\n";

    using S = StatutDeserialisation;

    refus(
        "Longueur impaire",
        remplacer(
            COMPTES,
            "4D422D30303031",
            "4D422D3030303"
        ),
        S::EncodageInvalide
    );

    refus(
        "Caractere interdit",
        remplacer(
            COMPTES,
            "4D422D30303031",
            "4G422D30303031"
        ),
        S::EncodageInvalide
    );

    // ======================================
    // NOUVEAU : CORRECTION 3.9.4.3
    // ======================================
    //
    // Le decodeur accepte 0-9 et A-F.
    // Une lettre a-f est donc rejetee
    // comme un encodage hexadecimal invalide.
    //
    // AVANT : S::FormatInvalide
    // APRES : S::EncodageInvalide

    refus(
        "Hexadecimal minuscule",
        remplacer(
            COMPTES,
            "416C696365",
            "416c696365"
        ),
        S::EncodageInvalide
    );

    refus(
        "Identifiant vide",
        remplacer(
            COMPTES,
            "ACCOUNT|4D422D30303031|",
            "ACCOUNT||"
        ),
        S::EtatIncoherent
    );

    refus(
        "Titulaire vide",
        remplacer(
            COMPTES,
            "|416C696365|50000",
            "||50000"
        ),
        S::EtatIncoherent
    );
}

// ==========================================
// NOUVEAU : NOMBRES ET LIMITES
// ==========================================

void testerNombres() {
    std::cout << "\n=== NOMBRES ET LIMITES ===\n";

    using S = StatutDeserialisation;

    refus(
        "Debordement uint64",
        remplacer(
            VIDE,
            "COUNTERS|1|1",
            "COUNTERS|18446744073709551616|1"
        ),
        S::NombreInvalide
    );

    refus(
        "Compteur negatif",
        remplacer(
            VIDE,
            "COUNTERS|1|1",
            "COUNTERS|-1|1"
        ),
        S::NombreInvalide
    );

    refus(
        "Signe plus",
        remplacer(
            VIDE,
            "COUNTERS|1|1",
            "COUNTERS|+1|1"
        ),
        S::NombreInvalide
    );

    refus(
        "Zero initial",
        remplacer(
            VIDE,
            "COUNTERS|1|1",
            "COUNTERS|01|1"
        ),
        S::NombreInvalide
    );

    refus(
        "Espace numerique",
        remplacer(
            VIDE,
            "COUNTERS|1|1",
            "COUNTERS|1 |1"
        ),
        S::NombreInvalide
    );

    refus(
        "Lettres dans un compteur",
        remplacer(
            VIDE,
            "COUNTERS|1|1",
            "COUNTERS|abc|1"
        ),
        S::NombreInvalide
    );

    refus(
        "Depassement int64 positif",
        remplacer(
            COMPTES,
            "416C696365|50000",
            "416C696365|9223372036854775808"
        ),
        S::NombreInvalide
    );

    refus(
        "Depassement int64 negatif",
        remplacer(
            COMPTES,
            "416C696365|50000",
            "416C696365|-9223372036854775809"
        ),
        S::NombreInvalide
    );

    refus(
        "Nombre de comptes excessif",
        remplacer(
            VIDE,
            "ACCOUNTS|0",
            "ACCOUNTS|100001"
        ),
        S::FormatInvalide
    );

    refus(
        "Nombre d'operations excessif",
        remplacer(
            VIDE,
            "OPERATIONS|0",
            "OPERATIONS|100001"
        ),
        S::FormatInvalide
    );

    refus(
        "Texte superieur a 8 Mio",
        std::string(8 * 1024 * 1024 + 1, 'X'),
        S::RessourcesInsuffisantes
    );
}

// ==========================================
// NOUVEAU : COHERENCE DES COMPTES
// ==========================================

void testerComptes() {
    std::cout << "\n=== COHERENCE DES COMPTES ===\n";

    using S = StatutDeserialisation;

    refus(
        "Identifiant duplique",
        remplacer(
            COMPTES,
            "ACCOUNT|4D422D30303032|426F62|120000",
            "ACCOUNT|4D422D30303031|426F62|120000"
        ),
        S::EtatIncoherent
    );

    refus(
        "Solde negatif",
        remplacer(
            COMPTES,
            "416C696365|50000",
            "416C696365|-1"
        ),
        S::EtatIncoherent
    );

    refus(
        "Compteur compte nul",
        remplacer(
            COMPTES,
            "COUNTERS|3|1",
            "COUNTERS|0|1"
        ),
        S::EtatIncoherent
    );

    refus(
        "Compteur compte trop petit",
        remplacer(
            COMPTES,
            "COUNTERS|3|1",
            "COUNTERS|2|1"
        ),
        S::EtatIncoherent
    );

    refus(
        "Numero MB-0000",
        remplacer(
            COMPTES,
            "4D422D30303031",
            "4D422D30303030"
        ),
        S::EtatIncoherent
    );
}

// ==========================================
// NOUVEAU : COHERENCE DES OPERATIONS
// ==========================================

void testerOperations() {
    std::cout << "\n=== COHERENCE DES OPERATIONS ===\n";

    using S = StatutDeserialisation;

    refus(
        "Numero operation nul",
        remplacer(
            AVEC_DEPOT,
            "OP|1|DEPOT",
            "OP|0|DEPOT"
        ),
        S::EtatIncoherent
    );

    refus(
        "Compteur operation incoherent",
        remplacer(
            AVEC_DEPOT,
            "COUNTERS|3|2",
            "COUNTERS|3|1"
        ),
        S::EtatIncoherent
    );

    refus(
        "Montant nul",
        remplacer(
            AVEC_DEPOT,
            "|100|-",
            "|0|-"
        ),
        S::EtatIncoherent
    );

    refus(
        "Montant negatif",
        remplacer(
            AVEC_DEPOT,
            "|100|-",
            "|-100|-"
        ),
        S::EtatIncoherent
    );

    refus(
        "Compte inexistant",
        remplacer(
            AVEC_DEPOT,
            "OP|1|DEPOT|4D422D30303031",
            "OP|1|DEPOT|494E434F4E4E55"
        ),
        S::EtatIncoherent
    );

    refus(
        "Type operation inconnu",
        remplacer(
            AVEC_DEPOT,
            "|DEPOT|",
            "|INCONNU|"
        ),
        S::FormatInvalide
    );

    refus(
        "Depot avec contrepartie",
        remplacer(
            AVEC_DEPOT,
            "|100|-",
            "|100|4D422D30303032"
        ),
        S::EtatIncoherent
    );

    refus(
        "Retrait avec contrepartie",
        remplacer(
            remplacer(
                AVEC_DEPOT,
                "|DEPOT|",
                "|RETRAIT|"
            ),
            "|100|-",
            "|100|4D422D30303032"
        ),
        S::EtatIncoherent
    );

    refus(
        "Virement sans contrepartie",
        remplacer(
            AVEC_DEPOT,
            "|DEPOT|",
            "|VIREMENT_SORTANT|"
        ),
        S::EtatIncoherent
    );

    refus(
        "Virement vers soi",
        remplacer(
            remplacer(
                AVEC_DEPOT,
                "|DEPOT|",
                "|VIREMENT_SORTANT|"
            ),
            "|100|-",
            "|100|4D422D30303031"
        ),
        S::EtatIncoherent
    );

    refus(
        "Virement vers compte inconnu",
        remplacer(
            remplacer(
                AVEC_DEPOT,
                "|DEPOT|",
                "|VIREMENT_ENTRANT|"
            ),
            "|100|-",
            "|100|494E434F4E4E55"
        ),
        S::EtatIncoherent
    );

    // NOUVEAU : construire deux operations
    // avec le meme numero.
    const std::string deuxOperations =
        remplacer(
            remplacer(
                AVEC_DEPOT,
                "COUNTERS|3|2",
                "COUNTERS|3|3"
            ),
            "OPERATIONS|1\n",
            "OPERATIONS|2\n"
        );

    refus(
        "Numeros d'operations dupliques",
        remplacer(
            deuxOperations,
            LIGNE_DEPOT + "\nEND\n",
            LIGNE_DEPOT + "\n"
            + LIGNE_DEPOT + "\nEND\n"
        ),
        S::EtatIncoherent
    );

    // NOUVEAU : ordre decroissant 2 puis 1.
    const std::string premiereNumeroDeux =
        "OP|2|DEPOT|4D422D30303031|100|-";

    refus(
        "Numeros d'operations decroissants",
        remplacer(
            deuxOperations,
            LIGNE_DEPOT + "\nEND\n",
            premiereNumeroDeux + "\n"
            + LIGNE_DEPOT + "\nEND\n"
        ),
        S::EtatIncoherent
    );
}

// ==========================================
// NOUVEAU : BANQUE D'ORIGINE INCHANGEE
// ==========================================

void testerPreservation() {
    std::cout << "\n=== PRESERVATION ===\n";

    Banque banque;
    banque.creerCompte("Alice", 50000);

    const auto avant =
        Persistance::serialiser(
            banque.exporterEtat()
        );

    const auto invalide =
        Persistance::deserialiser(
            remplacer(
                VIDE,
                "COUNTERS|1|1",
                "COUNTERS|0|0"
            )
        );

    const auto apres =
        Persistance::serialiser(
            banque.exporterEtat()
        );

    verifier(
        !invalide.estUnSucces(),
        "Etat corrompu refuse"
    );

    verifier(
        avant.estUnSucces()
            && apres.estUnSucces()
            && avant.texte == apres.texte,
        "Banque d'origine inchangee"
    );
}

} // namespace

// ==========================================
// NOUVEAU : PROGRAMME PRINCIPAL
// ==========================================

int main() {
    std::cout
        << "====================================\n"
        << " VALIDATION DESERIALISATION\n"
        << " Etape 3.9.4.3\n"
        << "====================================\n";

    testerCasValides();
    testerStructure();
    testerHexadecimal();
    testerNombres();
    testerComptes();
    testerOperations();
    testerPreservation();

    std::cout
        << "\n====================================\n"
        << "Scenarios refuses : "
        << refusTestes << '\n'
        << "Tests executes : "
        << tests << '\n'
        << "Tests reussis : "
        << reussites << '\n'
        << "Tests echoues : "
        << tests - reussites << '\n'
        << "====================================\n";

    return tests == reussites ? 0 : 1;
}
