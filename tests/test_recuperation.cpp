#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

#include "Banque.h"
#include "Persistance.h"

// ==========================================
// NOUVEAU : TESTS RECUPERATION EXPLICITE
// Etape 3.9.4.6 - dossier isole et vide
// ==========================================
namespace {
namespace fs = std::filesystem;
int total = 0;
int reussis = 0;

void verifier(bool condition, const std::string& nom) {
    ++total;
    if (condition) ++reussis;
    std::cout << (condition ? "[OK]    " : "[ECHEC] ") << nom << '\n';
}

// NOUVEAU : ecriture uniquement dans le dossier de test.
bool ecrire(const fs::path& chemin, const std::string& texte) {
    if (fs::exists(chemin)) return false;
    std::ofstream sortie(chemin, std::ios::binary);
    if (!sortie) return false;
    sortie.write(texte.data(), static_cast<std::streamsize>(texte.size()));
    const bool ok = sortie.good();
    sortie.close();
    return ok && !sortie.fail();
}

std::string lire(const fs::path& chemin) {
    std::ifstream entree(chemin, std::ios::binary);
    if (!entree) return "";
    return std::string(std::istreambuf_iterator<char>(entree),
                       std::istreambuf_iterator<char>());
}

bool videApresErreur(const ResultatRecuperation& r) {
    return !r.estUnSucces() && r.etat.comptes.empty()
        && r.etat.operations.empty()
        && r.etat.prochainNumeroCompte == 1
        && r.etat.prochainNumeroOperation == 1
        && !r.message.empty();
}

const std::string CANONIQUE =
    "MINIBANK|1\n"
    "COUNTERS|1|1\n"
    "ACCOUNTS|0\n"
    "OPERATIONS|0\n"
    "END\n";

void testerSecoursValide(const fs::path& dossier) {
    Banque source;
    const std::string alice = source.creerCompte("Alice", 50000);
    const std::string bob = source.creerCompte("Bob", 20000);
    verifier(!alice.empty() && !bob.empty(), "Comptes sources crees");
    verifier(source.deposerSurCompte(alice, 1000)
        && source.virer(alice, bob, 250), "Operations sources executees");

    const auto original = Persistance::serialiser(source.exporterEtat());
    verifier(original.estUnSucces(), "Etat source serialise");
    if (!original.estUnSucces()) return;

    const fs::path principal = dossier / "valide.txt";
    const fs::path secours = dossier / "valide.bak";
    const std::string principalCorrompu = "PRINCIPAL ENDOMMAGE\n";
    verifier(ecrire(principal, principalCorrompu), "Principal corrompu cree");
    verifier(ecrire(secours, original.texte), "Secours valide cree");

    // NOUVEAU : charger() ne consulte PAS .bak.
    const auto principalCharge = Persistance::charger(principal.u8string());
    verifier(!principalCharge.estUnSucces(), "Principal corrompu refuse");

    const auto r = Persistance::chargerSauvegardePrecedente(principal.u8string());
    verifier(r.estUnSucces(), "Secours explicite accepte");
    verifier(r.statutDeserialisation == StatutDeserialisation::Succes,
             "Statut de deserialisation correct");
    if (r.estUnSucces()) {
        const auto reserialise = Persistance::serialiser(r.etat);
        verifier(reserialise.estUnSucces() && reserialise.texte == original.texte,
                 "Etat de secours strictement identique");
    } else {
        verifier(false, "Etat de secours strictement identique");
    }
    verifier(lire(principal) == principalCorrompu, "Principal preserve");
    verifier(lire(secours) == original.texte, "Secours preserve");
}

void testerAbsence(const fs::path& dossier) {
    const auto r = Persistance::chargerSauvegardePrecedente(
        (dossier / "absent.txt").u8string());
    verifier(r.statut == StatutRecuperation::SauvegardeAbsente,
             "Secours absent distingue");
    verifier(videApresErreur(r), "Absence sans etat partiel");
    verifier(!fs::exists(dossier / "absent.bak"),
             "Aucun secours cree par la lecture");
}

void testerChemins(const fs::path& dossier) {
    const auto vide = Persistance::chargerSauvegardePrecedente("");
    verifier(vide.statut == StatutRecuperation::CheminInvalide,
             "Chemin vide refuse");
    verifier(videApresErreur(vide), "Chemin vide sans etat");
    const auto extension = Persistance::chargerSauvegardePrecedente(
        (dossier / "mauvais.bak").u8string());
    verifier(extension.statut == StatutRecuperation::CheminInvalide,
             "Extension principale invalide refusee");

    const fs::path dossierSecours = dossier / "dossier.bak";
    verifier(fs::create_directory(dossierSecours), "Dossier secours prepare");
    const auto nonRegulier = Persistance::chargerSauvegardePrecedente(
        (dossier / "dossier.txt").u8string());
    verifier(nonRegulier.statut == StatutRecuperation::FichierNonRegulier,
             "Dossier secours refuse");
    verifier(fs::is_directory(dossierSecours), "Dossier secours preserve");
}

void testerCorruption(const fs::path& dossier, const std::string& nom,
                      const std::string& texte, StatutDeserialisation attendu) {
    const fs::path principal = dossier / (nom + ".txt");
    const fs::path secours = dossier / (nom + ".bak");
    verifier(ecrire(secours, texte), nom + " : fichier prepare");
    const auto r = Persistance::chargerSauvegardePrecedente(principal.u8string());
    verifier(r.statut == StatutRecuperation::DonneesInvalides,
             nom + " : refuse");
    verifier(r.statutDeserialisation == attendu,
             nom + " : diagnostic exact");
    verifier(videApresErreur(r), nom + " : sans etat partiel");
    verifier(lire(secours) == texte && !fs::exists(principal),
             nom + " : fichiers inchanges");
}

void testerPrincipalValide(const fs::path& dossier) {
    const fs::path principal = dossier / "principal_valide.txt";
    const fs::path secours = dossier / "principal_valide.bak";
    verifier(ecrire(principal, CANONIQUE), "Principal valide cree");
    verifier(ecrire(secours, "SAUVEGARDE INVALIDE\n"), "Secours invalide cree");
    const auto charge = Persistance::charger(principal.u8string());
    verifier(charge.estUnSucces(), "Principal valide charge normalement");
    const auto r = Persistance::chargerSauvegardePrecedente(principal.u8string());
    verifier(r.statut == StatutRecuperation::DonneesInvalides,
             "Demande explicite de secours toujours controlee");
    verifier(lire(principal) == CANONIQUE && lire(secours) == "SAUVEGARDE INVALIDE\n",
             "Principal valide et secours non modifies");
}

void testerTaille(const fs::path& dossier) {
    const fs::path principal = dossier / "grand.txt";
    const fs::path secours = dossier / "grand.bak";
    const std::string grand(8 * 1024 * 1024 + 1, 'X');
    verifier(ecrire(secours, grand), "Grand secours prepare");
    const auto r = Persistance::chargerSauvegardePrecedente(principal.u8string());
    verifier(r.statut == StatutRecuperation::FichierTropGrand,
             "Grand secours refuse");
    verifier(videApresErreur(r), "Grand secours sans etat");
    verifier(fs::file_size(secours) == grand.size(), "Grand secours non tronque");
}

void testerBanqueInchangee(const fs::path& dossier) {
    Banque active;
    active.creerCompte("Actif", 65000);
    const auto avant = Persistance::serialiser(active.exporterEtat());
    const auto r = Persistance::chargerSauvegardePrecedente(
        (dossier / "valide.txt").u8string());
    const auto apres = Persistance::serialiser(active.exporterEtat());
    verifier(r.estUnSucces(), "Secours disponible pour une autre banque");
    verifier(avant.estUnSucces() && apres.estUnSucces()
             && avant.texte == apres.texte,
             "Banque active inchangee sans restaurerEtat");
}
} // namespace

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Fournir un dossier de test neuf et vide.\n";
        return 2;
    }
    try {
        const fs::path dossier = fs::u8path(argv[1]);
        if (!fs::is_directory(dossier) || !fs::is_empty(dossier)) {
            std::cerr << "Le dossier de test doit exister et etre vide.\n";
            return 2;
        }
        testerSecoursValide(dossier);
        testerAbsence(dossier);
        testerChemins(dossier);
        testerCorruption(dossier, "vide", "", StatutDeserialisation::FormatInvalide);
        testerCorruption(dossier, "version", "MINIBANK|2\nCOUNTERS|1|1\nACCOUNTS|0\nOPERATIONS|0\nEND\n",
                         StatutDeserialisation::VersionInconnue);
        testerCorruption(dossier, "tronque", "MINIBANK|1\nCOUNTERS|1|1\nACCOUNTS|0\n",
                         StatutDeserialisation::FormatInvalide);
        testerCorruption(dossier, "incoherent", "MINIBANK|1\nCOUNTERS|0|1\nACCOUNTS|0\nOPERATIONS|0\nEND\n",
                         StatutDeserialisation::EtatIncoherent);
        testerPrincipalValide(dossier);
        testerTaille(dossier);
        testerBanqueInchangee(dossier);
        std::cout << "\nTests executes : " << total
                  << "\nTests reussis : " << reussis
                  << "\nTests echoues : " << total - reussis << '\n';
        return total == reussis ? 0 : 1;
    } catch (const std::exception& e) {
        std::cerr << "Erreur de preparation des tests : " << e.what() << '\n';
        return 2;
    }
}
