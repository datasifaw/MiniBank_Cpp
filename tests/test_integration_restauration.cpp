#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

#include "Banque.h"
#include "Persistance.h"

// ==========================================
// NOUVEAU : INTEGRATION DE LA RESTAURATION
// MiniBank - etape 3.9.4.7
// Tous les fichiers sont dans un dossier
// NEUF fourni en argument ; aucun nettoyage.
// ==========================================

namespace {
namespace fs = std::filesystem;
int tests = 0;
int reussis = 0;

void verifier(bool condition, const std::string& nom) {
    ++tests;
    if (condition) ++reussis;
    std::cout << (condition ? "[OK]    " : "[ECHEC] ")
              << nom << '\n';
}

// NOUVEAU : capturer exactement les quatre
// composantes de l'etat bancaire dans le format
// canonique deja valide par MiniBank.
std::string empreinte(const Banque& banque) {
    const auto s = Persistance::serialiser(banque.exporterEtat());
    if (!s.estUnSucces()) return "";
    return s.texte;
}

// NOUVEAU : lire les octets sans transformer LF.
bool lireOctets(const fs::path& chemin, std::string& texte) {
    std::ifstream entree(chemin, std::ios::binary);
    if (!entree.is_open()) return false;
    texte.assign(std::istreambuf_iterator<char>(entree),
                 std::istreambuf_iterator<char>());
    if (entree.bad()) return false;
    entree.clear();
    entree.close();
    return !entree.fail();
}

// NOUVEAU : jamais ecraser un fichier de test.
bool creerFichier(const fs::path& chemin, const std::string& texte) {
    if (fs::exists(chemin)) return false;
    std::ofstream sortie(chemin, std::ios::binary);
    if (!sortie.is_open()) return false;
    sortie.write(texte.data(), static_cast<std::streamsize>(texte.size()));
    const bool ok = sortie.good();
    sortie.close();
    return ok && !sortie.fail();
}

// NOUVEAU : chaque scenario cree son sous-dossier.
fs::path nouveauDossier(const fs::path& racine,
                        const std::string& nom) {
    const fs::path p = racine / nom;
    if (!fs::create_directory(p)) {
        throw std::runtime_error("Sous-dossier deja existant : " + nom);
    }
    return p;
}

void cyclePrincipal(const fs::path& racine) {
    std::cout << "\n=== SAUVEGARDE / CHARGEMENT / RESTAURATION ===\n";
    const fs::path dir = nouveauDossier(racine, "cycle_principal");
    const fs::path fichier = dir / "minibank.txt";

    Banque origine;
    const std::string alice = origine.creerCompte("Alice", 50000);
    const std::string bob = origine.creerCompte("Bob", 120000);
    const bool mouvements = !alice.empty() && !bob.empty()
        && origine.deposerSurCompte(alice, 10000)
        && origine.retirerDuCompte(alice, 5000)
        && origine.virer(alice, bob, 2500);
    verifier(mouvements, "Operations de la banque source");
    if (!mouvements) return;

    const std::string attendu = empreinte(origine);
    verifier(!attendu.empty(), "Instantane source valide");
    const auto s = Persistance::sauvegarder(origine.exporterEtat(), fichier.u8string());
    verifier(s.estUnSucces(), "Sauvegarde initiale reussie");
    if (!s.estUnSucces()) return;

    std::string octetsAvant;
    verifier(lireOctets(fichier, octetsAvant), "Lecture binaire de reference");
    const auto lu = Persistance::charger(fichier.u8string());
    verifier(lu.estUnSucces(), "Chargement du principal reussi");
    if (!lu.estUnSucces()) return;

    Banque cible;
    const std::string ancien = "ANCIEN-TEST";
    verifier(cible.ajouterCompte(ancien, "Ancien compte", 800)
             && cible.deposerSurCompte(ancien, 100),
             "Banque cible preexistante preparee");
    const auto restaure = cible.restaurerEtat(lu.etat);
    verifier(restaure.estUnSucces(), "Restauration atomique acceptee");
    verifier(empreinte(cible) == attendu, "Etat complet identique a la source");
    verifier(cible.rechercherCompte(ancien) == nullptr,
             "Ancien compte de la cible disparu");
    verifier(cible.nombreOperations() == origine.nombreOperations(),
             "Historique global integralement conserve");
    verifier(cible.nombreComptes() == origine.nombreComptes(),
             "Nombre de comptes conserve");

    const std::string nouveau = cible.creerCompte("Charlie", 0);
    verifier(nouveau == "MB-0003", "Numerotation des comptes continue");
    verifier(cible.deposerSurCompte(nouveau, 100),
             "Depot possible apres restauration");
    if (!nouveau.empty()) {
        const auto hist = cible.consulterHistorique(nouveau);
        verifier(hist.size() == 1 && hist[0].numero == 5,
                 "Numerotation des operations continue");
    }
    std::string octetsApres;
    verifier(lireOctets(fichier, octetsApres) && octetsApres == octetsAvant,
             "Principal inchange par chargement et restauration");
    verifier(!fs::exists((dir / "minibank.bak")),
             "Aucun secours cree par une simple lecture");
}

void banqueVide(const fs::path& racine) {
    std::cout << "\n=== BANQUE VIDE ===\n";
    const fs::path dir = nouveauDossier(racine, "banque_vide");
    const fs::path fichier = dir / "vide.txt";
    Banque vide;
    const auto s = Persistance::sauvegarder(vide.exporterEtat(), fichier.u8string());
    verifier(s.estUnSucces(), "Sauvegarde banque vide");
    if (!s.estUnSucces()) return;
    const auto lu = Persistance::charger(fichier.u8string());
    verifier(lu.estUnSucces(), "Chargement banque vide");
    if (!lu.estUnSucces()) return;
    Banque cible;
    cible.creerCompte("A remplacer", 500);
    verifier(cible.restaurerEtat(lu.etat).estUnSucces(), "Restauration banque vide");
    verifier(cible.nombreComptes() == 0 && cible.nombreOperations() == 0,
             "Ancien etat totalement efface");
    verifier(cible.creerCompte("Premier", 0) == "MB-0001",
             "Compteur initial conserve");
}

void principalCorrompu(const fs::path& racine) {
    std::cout << "\n=== PRINCIPAL CORROMPU ===\n";
    const fs::path dir = nouveauDossier(racine, "principal_corrompu");
    const fs::path fichier = dir / "minibank.txt";
    const std::string corruption = "MINIBANK|1\nCOUNTERS|0|1\nACCOUNTS|0\nOPERATIONS|0\nEND\n";
    verifier(creerFichier(fichier, corruption), "Principal corrompu prepare");
    Banque cible;
    cible.creerCompte("Original", 777);
    const std::string avant = empreinte(cible);
    const auto lu = Persistance::charger(fichier.u8string());
    verifier(!lu.estUnSucces(), "Principal corrompu refuse");
    verifier(empreinte(cible) == avant, "Banque active preservee");
    verifier(!fs::exists((dir / "minibank.bak")),
             "Pas de creation automatique de .bak");
    std::string octets;
    verifier(lireOctets(fichier, octets) && octets == corruption,
             "Principal corrompu non modifie");
}

void secoursExplicite(const fs::path& racine) {
    std::cout << "\n=== SECOURS EXPLICITE .BAK ===\n";
    const fs::path dir = nouveauDossier(racine, "secours_explicite");
    const fs::path fichier = dir / "minibank.txt";
    // NOUVEAU : convention reelle de la sauvegarde Windows.
    const fs::path bak = (dir / "minibank.bak");
    Banque origine;
    const std::string alice = origine.creerCompte("Alice", 50000);
    verifier(!alice.empty(), "Compte source cree");
    const std::string etatAncien = empreinte(origine);
    const auto premiere = Persistance::sauvegarder(
        origine.exporterEtat(), fichier.u8string());
    verifier(premiere.estUnSucces(), "Premiere sauvegarde publiee");
    if (!premiere.estUnSucces()) return;

    verifier(origine.deposerSurCompte(alice, 10000), "Etat courant modifie");
    const std::string etatActuel = empreinte(origine);
    const auto seconde = Persistance::sauvegarder(
        origine.exporterEtat(), fichier.u8string());
    verifier(seconde.estUnSucces(), "Deuxieme sauvegarde publiee");
    if (!seconde.estUnSucces()) return;

    std::string avantPrincipal, avantBak;
    const bool captures = lireOctets(fichier, avantPrincipal)
        && lireOctets(bak, avantBak);
    verifier(captures, "Principal et .bak accessibles");
    verifier(captures && avantPrincipal == etatActuel && avantBak == etatAncien,
             "Principal actuel et .bak precedent distincts");

    const auto principal = Persistance::charger(fichier.u8string());
    verifier(principal.estUnSucces(), "Chargement normal du principal");
    if (principal.estUnSucces()) {
        verifier(Persistance::serialiser(principal.etat).texte == etatActuel,
                 "Le chargement normal n'utilise pas .bak");
    }

    // NOUVEAU : appel volontaire et distinct au secours.
    const auto secours = Persistance::chargerSauvegardePrecedente(fichier.u8string());
    verifier(secours.estUnSucces(), "Lecture explicite du .bak reussie");
    if (secours.estUnSucces()) {
        Banque cible;
        cible.creerCompte("Autre", 100);
        verifier(cible.restaurerEtat(secours.etat).estUnSucces(),
                 "Restauration explicite du secours");
        verifier(empreinte(cible) == etatAncien,
                 "Ancien etat retrouve exactement");
        verifier(empreinte(cible) != etatActuel,
                 "Ancien etat distinct du principal courant");
    }

    const auto troisieme = Persistance::sauvegarder(
        origine.exporterEtat(), fichier.u8string());
    verifier(!troisieme.estUnSucces() &&
             troisieme.statut == StatutSauvegarde::SauvegardePrecedenteExistante,
             "Troisieme sauvegarde refusee si .bak existe");
    std::string apresPrincipal, apresBak;
    verifier(lireOctets(fichier, apresPrincipal) &&
             lireOctets(bak, apresBak) &&
             avantPrincipal == apresPrincipal && avantBak == apresBak,
             "Principal et .bak preserves octet par octet");
}

void corruptionAvecSecours(const fs::path& racine) {
    std::cout << "\n=== AUCUN SECOURS AUTOMATIQUE ===\n";
    const fs::path dir = nouveauDossier(racine, "principal_invalide_bak_valide");
    const fs::path principal = dir / "minibank.txt";
    // NOUVEAU : convention reelle de la sauvegarde Windows.
    const fs::path bak = (dir / "minibank.bak");
    const std::string mauvais = "MINIBANK|2\nEND\n";
    const std::string bon = "MINIBANK|1\nCOUNTERS|1|1\nACCOUNTS|0\nOPERATIONS|0\nEND\n";
    verifier(creerFichier(principal, mauvais), "Principal invalide prepare");
    verifier(creerFichier(bak, bon), "Secours valide prepare");
    Banque cible;
    cible.creerCompte("Intact", 200);
    const auto avant = empreinte(cible);
    const auto echec = Persistance::charger(principal.u8string());
    verifier(!echec.estUnSucces(), "Chargement normal refuse le principal");
    verifier(empreinte(cible) == avant, "Cible preservee apres refus");
    const auto secours = Persistance::chargerSauvegardePrecedente(principal.u8string());
    verifier(secours.estUnSucces(), "Secours accessible seulement sur demande");
    verifier(empreinte(cible) == avant,
             "La lecture du secours ne restaure pas automatiquement");
    std::string p, b;
    verifier(lireOctets(principal, p) && lireOctets(bak, b) &&
             p == mauvais && b == bon, "Deux fichiers inchanges");
}

} // namespace

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage : test_integration_restauration.exe DOSSIER_NEUF_VIDE\n";
        return 2;
    }
    try {
        const fs::path racine = fs::u8path(argv[1]);
        // NOUVEAU : ne jamais reutiliser un dossier de donnees.
        if (!fs::is_directory(racine) || !fs::is_empty(racine)) {
            std::cerr << "Dossier de test inexistant ou non vide.\n";
            return 2;
        }
        cyclePrincipal(racine);
        banqueVide(racine);
        principalCorrompu(racine);
        secoursExplicite(racine);
        corruptionAvecSecours(racine);
    } catch (const std::exception& e) {
        std::cerr << "ERREUR DE PREPARATION : " << e.what() << '\n';
        return 2;
    }
    std::cout << "\n====================================\n"
              << "Tests executes : " << tests << '\n'
              << "Tests reussis : " << reussis << '\n'
              << "Tests echoues : " << tests - reussis << '\n'
              << "====================================\n";
    return tests == reussis ? 0 : 1;
}
