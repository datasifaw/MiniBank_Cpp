#include "Persistance.h"

// ==========================================
// NOUVEAU : RECUPERATION EXPLICITE .bak
// Etape 3.9.4.6 - lecture sans ecriture
// ==========================================

#include <cstddef>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <new>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

namespace {
namespace fs = std::filesystem;
constexpr std::size_t LIMITE = 8 * 1024 * 1024;

ResultatRecuperation erreur(
    StatutRecuperation statut, const char* message) {
    ResultatRecuperation resultat;
    resultat.statut = statut;
    resultat.message = message;
    return resultat;
}
} // namespace

ResultatRecuperation Persistance::chargerSauvegardePrecedente(
    const std::string& cheminPrincipal) {
    // NOUVEAU : aucun repli automatique depuis charger().
    // L'utilisateur doit appeler volontairement cette fonction.
    if (cheminPrincipal.empty()) {
        return erreur(StatutRecuperation::CheminInvalide,
                      "Chemin principal vide.");
    }

    try {
        const fs::path principal = fs::u8path(cheminPrincipal);
        if (principal.filename().empty() || principal.extension() != ".txt") {
            return erreur(StatutRecuperation::CheminInvalide,
                          "Le chemin principal doit finir par .txt.");
        }

        // NOUVEAU : le moteur Windows remplace .txt par .bak.
        // Exemple : minibank.txt -> minibank.bak.
        fs::path secours = principal;
        secours.replace_extension(".bak");

        std::error_code ec;
        const fs::file_status statut = fs::symlink_status(secours, ec);
        if (ec == std::errc::no_such_file_or_directory ||
            (!ec && statut.type() == fs::file_type::not_found)) {
            return erreur(StatutRecuperation::SauvegardeAbsente,
                          "Aucune sauvegarde precedente .bak.");
        }
        if (ec) {
            return erreur(StatutRecuperation::ErreurLecture,
                          "Impossible d'examiner la sauvegarde .bak.");
        }
        // NOUVEAU : refus des dossiers et liens symboliques.
        if (statut.type() != fs::file_type::regular) {
            return erreur(StatutRecuperation::FichierNonRegulier,
                          "La sauvegarde .bak n'est pas un fichier regulier.");
        }

        const std::uintmax_t taille = fs::file_size(secours, ec);
        if (ec) {
            return erreur(StatutRecuperation::ErreurLecture,
                          "Impossible de mesurer la sauvegarde .bak.");
        }
        if (taille > LIMITE) {
            return erreur(StatutRecuperation::FichierTropGrand,
                          "Sauvegarde .bak superieure a 8 Mio.");
        }

        std::ifstream entree(secours, std::ios::in | std::ios::binary);
        if (!entree.is_open()) {
            return erreur(StatutRecuperation::ErreurOuverture,
                          "Impossible d'ouvrir la sauvegarde .bak.");
        }

        // NOUVEAU : relire par blocs avec verification de limite,
        // meme si le fichier change entre file_size et l'ouverture.
        std::string contenu;
        contenu.reserve(static_cast<std::size_t>(taille));
        char tampon[8192];
        while (true) {
            entree.read(tampon, static_cast<std::streamsize>(sizeof(tampon)));
            const std::streamsize lus = entree.gcount();
            if (lus > 0) {
                const std::size_t n = static_cast<std::size_t>(lus);
                if (contenu.size() > LIMITE - n) {
                    return erreur(StatutRecuperation::FichierTropGrand,
                                  "Sauvegarde .bak agrandie au-dela de 8 Mio.");
                }
                contenu.append(tampon, n);
            }
            if (entree.bad()) {
                return erreur(StatutRecuperation::ErreurLecture,
                              "Erreur de lecture de la sauvegarde .bak.");
            }
            if (entree.eof()) break;
            if (entree.fail()) {
                return erreur(StatutRecuperation::ErreurLecture,
                              "Lecture incomplete de la sauvegarde .bak.");
            }
        }

        // NOUVEAU : effacer eofbit/failbit attendus en fin normale.
        entree.clear();
        entree.close();
        if (entree.fail()) {
            return erreur(StatutRecuperation::ErreurLecture,
                          "Fermeture de la sauvegarde .bak impossible.");
        }

        const ResultatDeserialisation decodage = deserialiser(contenu);
        if (!decodage.estUnSucces()) {
            ResultatRecuperation resultat;
            resultat.statut = StatutRecuperation::DonneesInvalides;
            resultat.statutDeserialisation = decodage.statut;
            resultat.message = "Sauvegarde .bak refusee : " + decodage.message;
            return resultat;
        }

        // NOUVEAU : toutes les copies et allocations AVANT
        // de signaler Succes ; aucun etat partiel retourne.
        ResultatRecuperation resultat;
        resultat.etat = decodage.etat;
        resultat.message = "Sauvegarde precedente lue en memoire.";
        resultat.statutDeserialisation = StatutDeserialisation::Succes;
        resultat.statut = StatutRecuperation::Succes;
        return resultat;
    } catch (const std::bad_alloc&) {
        return erreur(StatutRecuperation::RessourcesInsuffisantes,
                      "Memoire insuffisante.");
    } catch (const std::length_error&) {
        return erreur(StatutRecuperation::RessourcesInsuffisantes,
                      "Donnees trop longues.");
    } catch (const std::exception&) {
        return erreur(StatutRecuperation::ErreurLecture,
                      "Erreur pendant la lecture du secours.");
    }
}
