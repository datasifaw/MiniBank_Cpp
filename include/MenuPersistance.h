#ifndef MENU_PERSISTANCE_H
#define MENU_PERSISTANCE_H

// NOUVEAU : sous-menu de persistance, sans changer les autres options.
#include <iostream>
#include <string>
#include "Banque.h"
#include "Persistance.h"

namespace menu_persistance {

// NOUVEAU : demander une confirmation explicite avant une operation.
inline bool confirmer(const std::string& question) {
    std::cout << question << " (O pour confirmer, autre chose pour annuler) : ";
    std::string reponse;
    if (!std::getline(std::cin >> std::ws, reponse)) {
        std::cout << "Entree interrompue. Operation annulee.\n";
        return false;
    }
    return reponse == "O" || reponse == "o";
}

// NOUVEAU : conserver le meme chemin principal pour les trois actions.
inline constexpr const char* cheminPrincipal = "data/minibank.txt";

// NOUVEAU : ne remplacer la banque QUE si la lecture et la confirmation
// ont reussi. Le fichier principal ou le .bak n'est jamais reecrit ici.
inline void chargerPrincipal(Banque& banque) {
    const auto lecture = Persistance::charger(cheminPrincipal);
    if (!lecture.estUnSucces()) {
        std::cout << "Chargement refuse : " << lecture.message << '\n';
        return;
    }
    if (!confirmer("Remplacer la banque en memoire par le fichier principal ?")) {
        std::cout << "Restauration annulee.\n";
        return;
    }
    const auto restauration = banque.restaurerEtat(lecture.etat);
    std::cout << (restauration.estUnSucces() ? "Restauration reussie : "
                                                : "Restauration refusee : ")
              << restauration.message << '\n';
}

// NOUVEAU : recours au .bak exclusivement a la demande de l'utilisateur.
inline void chargerSecours(Banque& banque) {
    const auto lecture =
        Persistance::chargerSauvegardePrecedente(cheminPrincipal);
    if (!lecture.estUnSucces()) {
        std::cout << "Recuperation refusee : " << lecture.message << '\n';
        return;
    }
    if (!confirmer("Remplacer la banque en memoire par la version .bak ?")) {
        std::cout << "Recuperation annulee.\n";
        return;
    }
    const auto restauration = banque.restaurerEtat(lecture.etat);
    std::cout << (restauration.estUnSucces() ? "Recuperation reussie : "
                                                : "Recuperation refusee : ")
              << restauration.message << '\n';
}

// NOUVEAU : sous-menu independant pour garder intact le menu principal.
inline void afficher(Banque& banque) {
    for (;;) {
        std::cout << "\n=== SAUVEGARDE ET RESTAURATION ===\n"
                  << "1. Sauvegarder la banque actuelle\n"
                  << "2. Charger data/minibank.txt\n"
                  << "3. Recuperer explicitement data/minibank.bak\n"
                  << "0. Retour au menu principal\n"
                  << "Votre choix : ";
        std::string choix;
        if (!std::getline(std::cin >> std::ws, choix)) {
            std::cout << "Fin de saisie : retour au menu principal.\n";
            return;
        }
        if (choix == "0") return;
        if (choix == "1") {
            if (!confirmer("Sauvegarder l'etat actuel sur disque ?")) {
                std::cout << "Sauvegarde annulee.\n";
                continue;
            }
            const auto sauvegarde =
                Persistance::sauvegarder(banque.exporterEtat(), cheminPrincipal);
            std::cout << (sauvegarde.estUnSucces() ? "Sauvegarde reussie : "
                                                    : "Sauvegarde refusee : ")
                      << sauvegarde.message << '\n';
        } else if (choix == "2") {
            chargerPrincipal(banque);
        } else if (choix == "3") {
            chargerSecours(banque);
        } else {
            std::cout << "Choix invalide.\n";
        }
    }
}

} // namespace menu_persistance

#endif // MENU_PERSISTANCE_H
