
#include "Persistance.h"

// ==========================================
// NOUVEAU : CHARGEMENT CONTROLE
// Etape 3.9.4.4
//
// Lecture exclusivement non destructive.
// Le fichier .bak n'est pas utilise.
// ==========================================

#include <algorithm>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <fstream>
#include <new>
#include <string>
#include <system_error>
#include <utility>

namespace {

namespace fs = std::filesystem;

// NOUVEAU : meme limite que le deserialiseur.
constexpr std::size_t TAILLE_MAXIMALE =
    8 * 1024 * 1024;

// ==========================================
// NOUVEAU : CONSTRUIRE UN RESULTAT D'ERREUR
// ==========================================

ResultatChargement erreurChargement(
    StatutChargement statut,
    const std::string& message
) {
    ResultatChargement resultat;
    resultat.statut = statut;
    resultat.message = message;

    return resultat;
}

// ==========================================
// NOUVEAU : RESULTAT D'UN FICHIER TROP GRAND
// ==========================================

ResultatChargement erreurTaille() {
    return erreurChargement(
        StatutChargement::FichierTropGrand,
        "Fichier superieur a 8 Mio."
    );
}

} // namespace

// ==========================================
// NOUVEAU : LIRE LE FICHIER PRINCIPAL
// ==========================================

ResultatChargement Persistance::charger(
    const std::string& cheminPrincipal
) {
    // NOUVEAU : refuser tout chemin vide
    // avant de consulter le systeme de fichiers.
    if (cheminPrincipal.empty()) {
        return erreurChargement(
            StatutChargement::CheminInvalide,
            "Chemin de chargement vide."
        );
    }

    try {
        // NOUVEAU : interpreter les chemins
        // recus comme du texte UTF-8.
        const fs::path chemin =
            fs::u8path(cheminPrincipal);

        // NOUVEAU : cette methode ne concerne
        // que les fichiers principaux .txt.
        if (
            chemin.filename().empty() ||
            chemin.extension() != ".txt"
        ) {
            return erreurChargement(
                StatutChargement::CheminInvalide,
                "Le fichier principal doit "
                "posseder l'extension .txt."
            );
        }

        // ==================================
        // NOUVEAU : EXAMINER SANS SUIVRE
        // LES LIENS SYMBOLIQUES
        // ==================================

        std::error_code erreurStatut;

        const fs::file_status statut =
            fs::symlink_status(
                chemin,
                erreurStatut
            );

        if (
            erreurStatut ==
            std::errc::no_such_file_or_directory
        ) {
            return erreurChargement(
                StatutChargement::FichierIntrouvable,
                "Fichier de sauvegarde introuvable."
            );
        }

        if (erreurStatut) {
            return erreurChargement(
                StatutChargement::ErreurLecture,
                "Impossible d'examiner le fichier."
            );
        }

        if (
            statut.type() ==
            fs::file_type::not_found
        ) {
            return erreurChargement(
                StatutChargement::FichierIntrouvable,
                "Fichier de sauvegarde introuvable."
            );
        }

        if (
            statut.type() !=
            fs::file_type::regular
        ) {
            // NOUVEAU : refuser les dossiers,
            // liens symboliques et autres types.
            return erreurChargement(
                StatutChargement::FichierNonRegulier,
                "Le chemin ne designe pas "
                "un fichier regulier."
            );
        }

        // ==================================
        // NOUVEAU : CONTROLER LA TAILLE
        // AVANT L'OUVERTURE
        // ==================================

        std::error_code erreurTailleFichier;

        const std::uintmax_t tailleDisque =
            fs::file_size(
                chemin,
                erreurTailleFichier
            );

        if (erreurTailleFichier) {
            return erreurChargement(
                StatutChargement::ErreurLecture,
                "Impossible de connaitre "
                "la taille du fichier."
            );
        }

        if (tailleDisque > TAILLE_MAXIMALE) {
            return erreurTaille();
        }

        // ==================================
        // NOUVEAU : OUVERTURE BINAIRE
        // ==================================

        std::ifstream entree(
            chemin,
            std::ios::in | std::ios::binary
        );

        if (!entree.is_open()) {
            return erreurChargement(
                StatutChargement::ErreurOuverture,
                "Impossible d'ouvrir "
                "le fichier principal."
            );
        }

        // ==================================
        // NOUVEAU : LECTURE CONTROLEE
        // ==================================

        std::string contenu;

        contenu.reserve(
            static_cast<std::size_t>(tailleDisque)
        );

        // NOUVEAU : lire par blocs afin
        // de ne jamais depasser la limite.
        //
        // Nous revérifions la taille PENDANT
        // la lecture, car le fichier pourrait
        // changer entre file_size et read.
        char tampon[8192];

        while (true) {
            entree.read(
                tampon,
                static_cast<std::streamsize>(
                    sizeof(tampon)
                )
            );

            const std::streamsize lus =
                entree.gcount();

            if (lus > 0) {
                const std::size_t tailleLue =
                    static_cast<std::size_t>(lus);

                if (
                    contenu.size() >
                    TAILLE_MAXIMALE - tailleLue
                ) {
                    // NOUVEAU : aucune
                    // deserialisation ne sera faite.
                    return erreurTaille();
                }

                contenu.append(
                    tampon,
                    tailleLue
                );
            }

            // NOUVEAU : badbit indique
            // une erreur de lecture.
            if (entree.bad()) {
                return erreurChargement(
                    StatutChargement::ErreurLecture,
                    "Erreur pendant la lecture "
                    "du fichier."
                );
            }

            // NOUVEAU : eofbit est attendu
            // apres la derniere lecture.
            if (entree.eof()) {
                break;
            }

            // NOUVEAU : failbit sans eofbit
            // ne constitue pas une fin normale.
            if (entree.fail()) {
                return erreurChargement(
                    StatutChargement::ErreurLecture,
                    "Lecture incomplete du fichier."
                );
            }
        }

        // ==================================
        // NOUVEAU : VERIFIER LA FERMETURE
        // ==================================

        // NOUVEAU : supprimer eofbit et failbit
        // qui peuvent resulter d'une fin
        // de fichier parfaitement normale.
        entree.clear();
        entree.close();

        if (entree.fail()) {
            return erreurChargement(
                StatutChargement::ErreurLecture,
                "Fermeture du fichier impossible."
            );
        }

        // ==================================
        // NOUVEAU : DESERIALISER LE TEXTE
        // ==================================

        const ResultatDeserialisation donnees =
            Persistance::deserialiser(contenu);

        if (!donnees.estUnSucces()) {
            ResultatChargement resultat;

            resultat.statut =
                StatutChargement::
                    ErreurDeserialisation;

            resultat.statutDeserialisation =
                donnees.statut;

            resultat.message =
                "Deserialisation refusee : "
                + donnees.message;

            // NOUVEAU : resultat.etat reste vide.
            return resultat;
        }

        // ==================================
        // NOUVEAU : PUBLIER LE RESULTAT
        // ==================================

        ResultatChargement resultat;

        resultat.etat = donnees.etat;

        resultat.statut =
            StatutChargement::Succes;

        resultat.statutDeserialisation =
            StatutDeserialisation::Succes;

        resultat.message =
            "Chargement en memoire reussi.";

        return resultat;

    } catch (const std::bad_alloc&) {
        return erreurChargement(
            StatutChargement::RessourcesInsuffisantes,
            "Memoire insuffisante pour "
            "charger le fichier."
        );

    } catch (const std::length_error&) {
        return erreurChargement(
            StatutChargement::RessourcesInsuffisantes,
            "Taille des donnees excessive."
        );

    } catch (const fs::filesystem_error&) {
        return erreurChargement(
            StatutChargement::ErreurLecture,
            "Erreur du systeme de fichiers."
        );

    } catch (const std::exception&) {
        return erreurChargement(
            StatutChargement::ErreurLecture,
            "Erreur inattendue pendant "
            "le chargement."
        );
    }
}
