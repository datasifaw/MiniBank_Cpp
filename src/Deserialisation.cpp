
#include "Persistance.h"

// ==========================================
// NOUVEAU : DESERIALISATION EN MEMOIRE
// Etape 3.9.4.2 - format strict MINIBANK|1
// ==========================================

#include <charconv>
#include <cstdint>
#include <limits>
#include <new>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace {

// NOUVEAU : limites de protection.
constexpr std::size_t TAILLE_MAX =
    8 * 1024 * 1024;

constexpr std::uint64_t ENREGISTREMENTS_MAX =
    100000;

enum class Erreur {
    Format,
    Hex,
    Nombre,
    Coherence
};

struct ErreurLecture {
    Erreur type;
    const char* message;
};

// ==========================================
// NOUVEAU : SIGNALER UNE ERREUR
// ==========================================

[[noreturn]] void refuser(
    Erreur type,
    const char* message
) {
    throw ErreurLecture{type, message};
}

// ==========================================
// NOUVEAU : DECOUPER LES LIGNES
// ==========================================

std::vector<std::string> lignesDuTexte(
    const std::string& texte
) {
    // NOUVEAU : LF final obligatoire.
    // CRLF et octets NUL refuses.
    if (
        texte.empty() ||
        texte.back() != '\n' ||
        texte.find('\r') != std::string::npos ||
        texte.find('\0') != std::string::npos
    ) {
        refuser(
            Erreur::Format,
            "Fin de ligne ou octet interdit."
        );
    }

    std::vector<std::string> lignes;
    std::size_t debut = 0;

    while (debut < texte.size()) {
        const auto fin = texte.find('\n', debut);

        if (fin == std::string::npos) {
            refuser(
                Erreur::Format,
                "Ligne non terminee."
            );
        }

        lignes.emplace_back(
            texte.substr(debut, fin - debut)
        );

        debut = fin + 1;
    }

    return lignes;
}

// ==========================================
// NOUVEAU : DECOUPER LES CHAMPS
// ==========================================

std::vector<std::string> champs(
    const std::string& ligne
) {
    // NOUVEAU : conserver les champs vides.
    std::vector<std::string> resultat;
    std::size_t debut = 0;

    while (true) {
        const auto fin = ligne.find('|', debut);

        if (fin == std::string::npos) {
            resultat.emplace_back(
                ligne.substr(debut)
            );
            return resultat;
        }

        resultat.emplace_back(
            ligne.substr(debut, fin - debut)
        );

        debut = fin + 1;
    }
}

// ==========================================
// NOUVEAU : CONVERTIR UN NOMBRE EXACT
// ==========================================

template <typename T>
T entier(const std::string& texte) {
    T nombre{};

    // NOUVEAU : refuser les representations
    // non canoniques.
    if (
        texte.empty() ||
        texte.front() == '+' ||
        (
            texte.size() > 1 &&
            texte.front() == '0'
        ) ||
        texte == "-0" ||
        texte == "-"
    ) {
        refuser(
            Erreur::Nombre,
            "Nombre non canonique."
        );
    }

    const auto resultat = std::from_chars(
        texte.data(),
        texte.data() + texte.size(),
        nombre,
        10
    );

    if (
        resultat.ec != std::errc{} ||
        resultat.ptr != texte.data() + texte.size()
    ) {
        refuser(
            Erreur::Nombre,
            "Nombre incorrect ou hors limites."
        );
    }

    return nombre;
}

// ==========================================
// NOUVEAU : DECODEUR HEXADECIMAL
// ==========================================

int valeurHex(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }

    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }

    return -1;
}

std::string decoderHex(
    const std::string& texte
) {
    if (texte.size() % 2 != 0) {
        refuser(
            Erreur::Hex,
            "Longueur hexadecimale impaire."
        );
    }

    std::string sortie;
    sortie.reserve(texte.size() / 2);

    for (
        std::size_t i = 0;
        i < texte.size();
        i += 2
    ) {
        const int haut = valeurHex(texte[i]);
        const int bas = valeurHex(texte[i + 1]);

        if (haut < 0 || bas < 0) {
            refuser(
                Erreur::Hex,
                "Caractere hexadecimal incorrect."
            );
        }

        sortie.push_back(
            static_cast<char>((haut << 4) | bas)
        );
    }

    return sortie;
}

// ==========================================
// NOUVEAU : VERIFIER LES IDS GENERES
// ==========================================

void verifierCompteurComptes(
    const EtatBanque& etat
) {
    for (const Compte& compte : etat.comptes) {
        const std::string& id =
            compte.getIdentifiant();

        // NOUVEAU : reconnaitre les
        // identifiants generes de type
        // MB-0001, MB-0002, MB-10000...
        if (
            id.size() < 7 ||
            id.compare(0, 3, "MB-") != 0
        ) {
            continue;
        }

        bool chiffres = true;

        for (
            std::size_t i = 3;
            i < id.size();
            ++i
        ) {
            if (
                id[i] < '0' ||
                id[i] > '9'
            ) {
                chiffres = false;
                break;
            }
        }

        if (!chiffres) {
            continue;
        }

        // NOUVEAU : les zeros initiaux sont
        // autorises dans les identifiants.
        std::uint64_t valeur{};

        const std::string chiffresId =
            id.substr(3);

        const auto conversion = std::from_chars(
            chiffresId.data(),
            chiffresId.data() + chiffresId.size(),
            valeur
        );

        if (
            conversion.ec != std::errc{} ||
            conversion.ptr !=
                chiffresId.data() + chiffresId.size()
        ) {
            refuser(
                Erreur::Coherence,
                "Identifiant de compte trop grand."
            );
        }

        if (
            valeur == 0 ||
            valeur >= etat.prochainNumeroCompte
        ) {
            refuser(
                Erreur::Coherence,
                "Compteur de comptes incoherent."
            );
        }
    }
}

// ==========================================
// NOUVEAU : LIRE LA LIGNE SUIVANTE
// ==========================================

const std::string& prendre(
    const std::vector<std::string>& lignes,
    std::size_t& position
) {
    if (position >= lignes.size()) {
        refuser(
            Erreur::Format,
            "Section incomplete."
        );
    }

    return lignes[position++];
}

} // namespace

// ==========================================
// NOUVEAU : FONCTION PUBLIQUE
// ==========================================

ResultatDeserialisation Persistance::deserialiser(
    const std::string& texte
) {
    ResultatDeserialisation resultat;

    // NOUVEAU : construire un EtatBanque local
    // puis le publier uniquement apres validation.
    try {
        if (texte.size() > TAILLE_MAX) {
            resultat.statut =
                StatutDeserialisation::
                    RessourcesInsuffisantes;

            resultat.message =
                "Texte superieur a 8 Mio.";

            return resultat;
        }

        const auto lignes =
            lignesDuTexte(texte);

        std::size_t position = 0;

        // ==================================
        // NOUVEAU : SIGNATURE
        // ==================================

        if (
            prendre(lignes, position) !=
            "MINIBANK|1"
        ) {
            resultat.statut =
                StatutDeserialisation::
                    VersionInconnue;

            resultat.message =
                "Signature ou version inconnue.";

            return resultat;
        }

        EtatBanque temporaire;

        // ==================================
        // NOUVEAU : COMPTEURS
        // ==================================

        const auto compteurs = champs(
            prendre(lignes, position)
        );

        if (
            compteurs.size() != 3 ||
            compteurs[0] != "COUNTERS"
        ) {
            refuser(
                Erreur::Format,
                "Ligne COUNTERS incorrecte."
            );
        }

        temporaire.prochainNumeroCompte =
            entier<std::uint64_t>(
                compteurs[1]
            );

        temporaire.prochainNumeroOperation =
            entier<std::uint64_t>(
                compteurs[2]
            );

        // ==================================
        // NOUVEAU : NOMBRE DE COMPTES
        // ==================================

        const auto enteteComptes = champs(
            prendre(lignes, position)
        );

        if (
            enteteComptes.size() != 2 ||
            enteteComptes[0] != "ACCOUNTS"
        ) {
            refuser(
                Erreur::Format,
                "Ligne ACCOUNTS incorrecte."
            );
        }

        const auto nComptes =
            entier<std::uint64_t>(
                enteteComptes[1]
            );

        if (
            nComptes > ENREGISTREMENTS_MAX ||
            nComptes > lignes.size() - position
        ) {
            refuser(
                Erreur::Format,
                "Nombre de comptes excessif ou tronque."
            );
        }

        // ==================================
        // NOUVEAU : LIGNES ACCOUNT
        // ==================================

        for (
            std::uint64_t i = 0;
            i < nComptes;
            ++i
        ) {
            const auto c = champs(
                prendre(lignes, position)
            );

            if (
                c.size() != 4 ||
                c[0] != "ACCOUNT"
            ) {
                refuser(
                    Erreur::Format,
                    "Enregistrement ACCOUNT incorrect."
                );
            }

            const std::string id =
                decoderHex(c[1]);

            const std::string nom =
                decoderHex(c[2]);

            const auto solde =
                entier<std::int64_t>(
                    c[3]
                );

            try {
                temporaire.comptes.emplace_back(
                    id,
                    nom,
                    solde
                );

            } catch (const std::invalid_argument&) {
                refuser(
                    Erreur::Coherence,
                    "Compte invalide."
                );
            }
        }

        // ==================================
        // NOUVEAU : NOMBRE D'OPERATIONS
        // ==================================

        const auto enteteOperations = champs(
            prendre(lignes, position)
        );

        if (
            enteteOperations.size() != 2 ||
            enteteOperations[0] != "OPERATIONS"
        ) {
            refuser(
                Erreur::Format,
                "Ligne OPERATIONS incorrecte."
            );
        }

        const auto nOperations =
            entier<std::uint64_t>(
                enteteOperations[1]
            );

        if (
            nOperations > ENREGISTREMENTS_MAX ||
            nOperations > lignes.size() - position
        ) {
            refuser(
                Erreur::Format,
                "Nombre d'operations excessif ou tronque."
            );
        }

        // ==================================
        // NOUVEAU : LIGNES OP
        // ==================================

        for (
            std::uint64_t i = 0;
            i < nOperations;
            ++i
        ) {
            const auto o = champs(
                prendre(lignes, position)
            );

            if (
                o.size() != 6 ||
                o[0] != "OP"
            ) {
                refuser(
                    Erreur::Format,
                    "Enregistrement OP incorrect."
                );
            }

            Operation op{};

            op.numero =
                entier<std::uint64_t>(
                    o[1]
                );

            if (o[2] == "DEPOT") {
                op.type = TypeOperation::Depot;

            } else if (o[2] == "RETRAIT") {
                op.type = TypeOperation::Retrait;

            } else if (
                o[2] == "VIREMENT_SORTANT"
            ) {
                op.type =
                    TypeOperation::VirementSortant;

            } else if (
                o[2] == "VIREMENT_ENTRANT"
            ) {
                op.type =
                    TypeOperation::VirementEntrant;

            } else {
                refuser(
                    Erreur::Format,
                    "Type d'operation inconnu."
                );
            }

            op.identifiantCompte =
                decoderHex(o[3]);

            op.montantCentimes =
                entier<std::int64_t>(
                    o[4]
                );

            // NOUVEAU : '-' indique
            // l'absence de contrepartie.
            op.identifiantContrepartie =
                o[5] == "-"
                    ? ""
                    : decoderHex(o[5]);

            temporaire.operations.push_back(
                std::move(op)
            );
        }

        // ==================================
        // NOUVEAU : FIN DU DOCUMENT
        // ==================================

        if (
            prendre(lignes, position) != "END" ||
            position != lignes.size()
        ) {
            refuser(
                Erreur::Format,
                "Marqueur END ou fin incorrecte."
            );
        }

        // ==================================
        // NOUVEAU : COHERENCE BANCAIRE
        // ==================================

        const auto verification =
            Persistance::serialiser(
                temporaire
            );

        if (!verification.estUnSucces()) {
            refuser(
                Erreur::Coherence,
                "Etat bancaire incoherent."
            );
        }

        // NOUVEAU : protection supplementaire
        // du compteur de comptes.
        verifierCompteurComptes(temporaire);

        // NOUVEAU : exiger exactement
        // l'ecriture produite par serialiser().
        if (verification.texte != texte) {
            refuser(
                Erreur::Format,
                "Representation non canonique."
            );
        }

        // ==================================
        // NOUVEAU : RESULTAT COMPLET
        // ==================================

        resultat.etat =
            std::move(temporaire);

        resultat.statut =
            StatutDeserialisation::Succes;

        resultat.message =
            "Deserialisation en memoire reussie.";

        return resultat;

    } catch (const ErreurLecture& e) {
        switch (e.type) {
            case Erreur::Format:
                resultat.statut =
                    StatutDeserialisation::
                        FormatInvalide;
                break;

            case Erreur::Hex:
                resultat.statut =
                    StatutDeserialisation::
                        EncodageInvalide;
                break;

            case Erreur::Nombre:
                resultat.statut =
                    StatutDeserialisation::
                        NombreInvalide;
                break;

            case Erreur::Coherence:
                resultat.statut =
                    StatutDeserialisation::
                        EtatIncoherent;
                break;
        }

        resultat.message = e.message;

    } catch (const std::bad_alloc&) {
        resultat.statut =
            StatutDeserialisation::
                RessourcesInsuffisantes;

        resultat.message =
            "Memoire insuffisante.";

    } catch (const std::length_error&) {
        resultat.statut =
            StatutDeserialisation::
                RessourcesInsuffisantes;

        resultat.message =
            "Taille excessive.";
    }

    // NOUVEAU : aucun EtatBanque partiel
    // n'est rendu utilisable apres un echec.
    resultat.etat = EtatBanque{};

    return resultat;
}
