
#include <iostream>
#include <string>

#include "Banque.h"
#include "Persistance.h"

// ==========================================
// NOUVEAU : TESTS DE DESERIALISATION
// Etape 3.9.4.2 - MINIBANK|1
//
// Aucun acces disque.
// ==========================================

namespace {

int total = 0;
int reussis = 0;

// ==========================================
// NOUVEAU : VERIFIER UNE ASSERTION
// ==========================================

void verifier(
    bool ok,
    const std::string& nom
) {
    ++total;

    if (ok) {
        ++reussis;
    }

    std::cout
        << (ok ? "[OK]    " : "[ECHEC] ")
        << nom
        << '\n';
}

// ==========================================
// NOUVEAU : VERIFIER UN REFUS
// ==========================================

void rejet(
    const std::string& texte,
    StatutDeserialisation statut,
    const std::string& nom
) {
    const auto r =
        Persistance::deserialiser(texte);

    verifier(
        !r.estUnSucces()
            && r.statut == statut
            && r.etat.comptes.empty()
            && r.etat.operations.empty()
            && !r.message.empty(),
        nom
    );
}

// ==========================================
// NOUVEAU : TESTER UN ALLER-RETOUR
// ==========================================

void allerRetour(
    const Banque& banque,
    const std::string& nom
) {
    const auto serialisation =
        Persistance::serialiser(
            banque.exporterEtat()
        );

    verifier(
        serialisation.estUnSucces(),
        nom + " : texte produit"
    );

    if (!serialisation.estUnSucces()) {
        return;
    }

    const auto r =
        Persistance::deserialiser(
            serialisation.texte
        );

    verifier(
        r.estUnSucces(),
        nom + " : deserialisation reussie"
    );

    if (!r.estUnSucces()) {
        return;
    }

    const auto texteFinal =
        Persistance::serialiser(r.etat);

    verifier(
        texteFinal.estUnSucces()
            && texteFinal.texte ==
                serialisation.texte,
        nom + " : aller-retour exact"
    );
}

} // namespace

// ==========================================
// NOUVEAU : PROGRAMME DE TEST
// ==========================================

int main() {
    std::cout
        << "=== TESTS DESERIALISATION 3.9.4.2 ===\n";

    // ======================================
    // NOUVEAU : BANQUE VIDE
    // ======================================

    Banque vide;

    allerRetour(
        vide,
        "Banque vide"
    );

    // ======================================
    // NOUVEAU : DEUX COMPTES
    // ======================================

    Banque banque;

    const std::string alice =
        banque.creerCompte(
            "Alice",
            50000
        );

    const std::string bob =
        banque.creerCompte(
            "Bob",
            120000
        );

    allerRetour(
        banque,
        "Deux comptes"
    );

    // ======================================
    // NOUVEAU : HISTORIQUE COMPLET
    // ======================================

    verifier(
        banque.deposerSurCompte(
            alice, 10000
        )
        && banque.retirerDuCompte(
            alice, 5000
        )
        && banque.virer(
            alice, bob, 2500
        ),
        "Operations de reference"
    );

    allerRetour(
        banque,
        "Historique de quatre operations"
    );

    // ======================================
    // NOUVEAU : UTF-8 ET SEPARATEUR
    // ======================================

    Banque special;

    special.creerCompte(
        std::string("El")
            + "\xC3\xA9"
            + "a | Martin",
        0
    );

    allerRetour(
        special,
        "UTF-8 et separateur"
    );

    // ======================================
    // NOUVEAU : FORMAT CANONIQUE VIDE
    // ======================================

    const std::string canon =
        "MINIBANK|1\n"
        "COUNTERS|1|1\n"
        "ACCOUNTS|0\n"
        "OPERATIONS|0\n"
        "END\n";

    // ======================================
    // NOUVEAU : FORMAT INVALIDE
    // ======================================

    rejet(
        "",
        StatutDeserialisation::FormatInvalide,
        "Texte vide"
    );

    rejet(
        "MINIBANK|2\n"
        "COUNTERS|1|1\n"
        "ACCOUNTS|0\n"
        "OPERATIONS|0\n"
        "END\n",
        StatutDeserialisation::VersionInconnue,
        "Version inconnue"
    );

    rejet(
        canon.substr(
            0,
            canon.size() - 1
        ),
        StatutDeserialisation::FormatInvalide,
        "LF final absent"
    );

    rejet(
        "MINIBANK|1\r\n"
        "COUNTERS|1|1\r\n"
        "ACCOUNTS|0\r\n"
        "OPERATIONS|0\r\n"
        "END\r\n",
        StatutDeserialisation::FormatInvalide,
        "CRLF refuse (format strict)"
    );

    rejet(
        canon + "INTRUS\n",
        StatutDeserialisation::FormatInvalide,
        "Ligne apres END"
    );

    rejet(
        "MINIBANK|1\n"
        "COUNTERS|1|1\n"
        "ACCOUNTS|1\n"
        "OPERATIONS|0\n"
        "END\n",
        StatutDeserialisation::FormatInvalide,
        "ACCOUNT manquant"
    );

    rejet(
        "MINIBANK|1\n"
        "COUNTERS|1|1\n"
        "ACCOUNTS|0\n"
        "OPERATIONS|1\n"
        "END\n",
        StatutDeserialisation::FormatInvalide,
        "OP manquante"
    );

    // ======================================
    // NOUVEAU : HEXADECIMAL INVALIDE
    // ======================================

    rejet(
        "MINIBANK|1\n"
        "COUNTERS|1|1\n"
        "ACCOUNTS|1\n"
        "ACCOUNT|4G|416C696365|0\n"
        "OPERATIONS|0\n"
        "END\n",
        StatutDeserialisation::EncodageInvalide,
        "Hex non valide"
    );

    rejet(
        "MINIBANK|1\n"
        "COUNTERS|1|1\n"
        "ACCOUNTS|1\n"
        "ACCOUNT|4D2|416C696365|0\n"
        "OPERATIONS|0\n"
        "END\n",
        StatutDeserialisation::EncodageInvalide,
        "Hex impair"
    );

    // ======================================
    // NOUVEAU : NOMBRES INVALIDES
    // ======================================

    rejet(
        "MINIBANK|1\n"
        "COUNTERS|18446744073709551616|1\n"
        "ACCOUNTS|0\n"
        "OPERATIONS|0\n"
        "END\n",
        StatutDeserialisation::NombreInvalide,
        "Depassement uint64"
    );

    rejet(
        "MINIBANK|1\n"
        "COUNTERS|01|1\n"
        "ACCOUNTS|0\n"
        "OPERATIONS|0\n"
        "END\n",
        StatutDeserialisation::NombreInvalide,
        "Compteur non canonique"
    );

    // ======================================
    // NOUVEAU : ETATS INCOHERENTS
    // ======================================

    rejet(
        "MINIBANK|1\n"
        "COUNTERS|0|1\n"
        "ACCOUNTS|0\n"
        "OPERATIONS|0\n"
        "END\n",
        StatutDeserialisation::EtatIncoherent,
        "Compteur nul"
    );

    rejet(
        "MINIBANK|1\n"
        "COUNTERS|1|1\n"
        "ACCOUNTS|1\n"
        "ACCOUNT|4D422D30303031|"
        "416C696365|50000\n"
        "OPERATIONS|0\n"
        "END\n",
        StatutDeserialisation::EtatIncoherent,
        "Compteur compte trop petit"
    );

    rejet(
        "MINIBANK|1\n"
        "COUNTERS|2|1\n"
        "ACCOUNTS|1\n"
        "ACCOUNT|4D422D30303031|"
        "416C696365|-1\n"
        "OPERATIONS|0\n"
        "END\n",
        StatutDeserialisation::EtatIncoherent,
        "Solde negatif"
    );

    rejet(
        "MINIBANK|1\n"
        "COUNTERS|2|1\n"
        "ACCOUNTS|1\n"
        "ACCOUNT|4D422D30303031|"
        "416C696365|0\n"
        "OPERATIONS|1\n"
        "OP|1|DEPOT|4D422D30303031|100|-\n"
        "END\n",
        StatutDeserialisation::EtatIncoherent,
        "Numero OP non inferieur au compteur"
    );

    rejet(
        "MINIBANK|1\n"
        "COUNTERS|1|1\n"
        "ACCOUNTS|0\n"
        "OPERATIONS|0\n"
        "END\n"
        "\n",
        StatutDeserialisation::FormatInvalide,
        "Ligne vide supplementaire"
    );

    // ======================================
    // NOUVEAU : TAILLE MAXIMALE
    // ======================================

    const std::string tropGrand(
        8 * 1024 * 1024 + 1,
        'X'
    );

    rejet(
        tropGrand,
        StatutDeserialisation::
            RessourcesInsuffisantes,
        "Taille superieure a 8 Mio"
    );

    // ======================================
    // NOUVEAU : BILAN
    // ======================================

    std::cout
        << "\nTests executes : "
        << total
        << "\nTests reussis : "
        << reussis
        << "\nTests echoues : "
        << total - reussis
        << '\n';

    return total == reussis ? 0 : 1;
}
