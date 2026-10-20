
#ifndef ETAT_BANQUE_H
#define ETAT_BANQUE_H

// ==========================================
// NOUVEAU : INSTANTANE DE LA BANQUE
// Etape 3.9.2.1
// ==========================================

#include <cstdint>
#include <vector>

#include "Compte.h"
#include "Operation.h"

// NOUVEAU : cette structure represente
// un etat complet de MiniBank en memoire.
//
// Elle sera utilisee plus tard par le
// module Persistance pour sauvegarder
// les donnees bancaires sur disque.
//
// Elle ne lit et n'ecrit aucun fichier.
struct EtatBanque {

    // NOUVEAU : copie de tous les comptes.
    std::vector<Compte> comptes;

    // NOUVEAU : copie de tout l'historique,
    // dans son ordre global d'enregistrement.
    std::vector<Operation> operations;

    // NOUVEAU : prochain numero servant
    // a generer les identifiants MB-xxxx.
    std::uint64_t prochainNumeroCompte = 1;

    // NOUVEAU : prochain numero global
    // pour les lignes d'historique.
    std::uint64_t prochainNumeroOperation = 1;
};

#endif
