#include "Banque.h"
#include "Persistance.h"

// ==========================================
// NOUVEAU : RESTAURATION ATOMIQUE EN MEMOIRE
// Etape 3.9.4.5
// ==========================================

#include <exception>
#include <new>
#include <stdexcept>
#include <utility>

ResultatRestauration Banque::restaurerEtat(
    const EtatBanque& etat
) {
    try {
        // NOUVEAU : verifier les invariants de la banque.
        // serialiser() refuse les doublons, operations
        // incoherentes, montants invalides, etc.
        const ResultatSerialisation serialisation =
            Persistance::serialiser(etat);

        if (!serialisation.estUnSucces()) {
            return {
                StatutRestauration::EtatInvalide,
                "Etat bancaire invalide : " + serialisation.message
            };
        }

        // NOUVEAU : appliquer exactement le contrat de
        // restauration MINIBANK|1, notamment le controle
        // du compteur MB-xxxx et les limites de ressources.
        const ResultatDeserialisation verification =
            Persistance::deserialiser(serialisation.texte);

        if (!verification.estUnSucces()) {
            return {
                StatutRestauration::EtatInvalide,
                "Etat non restaurable : " + verification.message
            };
        }

        // NOUVEAU : tout le travail potentiellement
        // susceptible d'allouer est termine AVANT de
        // toucher aux membres internes de Banque.
        // La copie ci-dessous preserve egalement les
        // comptes, operations et compteurs exacts.
        EtatBanque prepare = verification.etat;

        // NOUVEAU : construire le resultat de succes AVANT
        // la publication (evite toute allocation de message
        // une fois l'etat interne remplace).
        ResultatRestauration succes{
            StatutRestauration::Succes,
            "Restauration atomique reussie."
        };

        // NOUVEAU : avec std::vector et son allocateur
        // standard, swap est noexcept en C++17.
        // Aucun code utilisateur ni allocation entre
        // les deux permutations ou les affectations.
        comptes.swap(prepare.comptes);
        historique.swap(prepare.operations);
        prochainNumero = prepare.prochainNumeroCompte;
        prochainNumeroOperation = prepare.prochainNumeroOperation;

        return succes;

    } catch (const std::bad_alloc&) {
        return {
            StatutRestauration::RessourcesInsuffisantes,
            "Memoire insuffisante."
        };
    } catch (const std::length_error&) {
        return {
            StatutRestauration::RessourcesInsuffisantes,
            "Taille excessive."
        };
    } catch (const std::exception&) {
        return {
            StatutRestauration::ErreurInterne,
            "Erreur pendant la preparation de la restauration."
        };
    }
}
