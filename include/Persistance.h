
#ifndef PERSISTANCE_H
#define PERSISTANCE_H

#include <string>

#include "EtatBanque.h"

// ==========================================
// MODULE DE PERSISTANCE MINIBANK
// ==========================================

// ==========================================
// RESULTAT DE SAUVEGARDE
// ==========================================

enum class StatutSauvegarde {
    Succes,
    NonImplemente,
    CheminInvalide,
    ErreurSerialisation,
    ErreurEcriture,
    ErreurRemplacement,
    SauvegardePrecedenteExistante,
    FichierPrincipalInvalide
};

struct ResultatSauvegarde {
    StatutSauvegarde statut =
        StatutSauvegarde::NonImplemente;

    std::string message;

    bool estUnSucces() const {
        return statut == StatutSauvegarde::Succes;
    }
};

// ==========================================
// RESULTAT DE SERIALISATION
// ==========================================

enum class StatutSerialisation {
    Succes,
    DonneesInvalides,
    ErreurMemoire
};

struct ResultatSerialisation {
    StatutSerialisation statut =
        StatutSerialisation::DonneesInvalides;

    std::string texte;
    std::string message;

    bool estUnSucces() const {
        return statut == StatutSerialisation::Succes;
    }
};

// ==========================================
// RESULTAT D'ECRITURE TEMPORAIRE
// ==========================================

enum class StatutEcritureTemporaire {
    Succes,
    CheminInvalide,
    FichierDejaExistant,
    ErreurSerialisation,
    ErreurOuverture,
    ErreurEcriture,
    ErreurFermeture,
    ErreurLecture,
    ContenuDifferent
};

struct ResultatEcritureTemporaire {
    StatutEcritureTemporaire statut =
        StatutEcritureTemporaire::ErreurEcriture;

    std::string message;

    bool estUnSucces() const {
        return statut ==
            StatutEcritureTemporaire::Succes;
    }
};

// ==========================================
// RESULTAT DE DESERIALISATION
// ==========================================

enum class StatutDeserialisation {
    Succes,
    VersionInconnue,
    FormatInvalide,
    EncodageInvalide,
    NombreInvalide,
    EtatIncoherent,
    RessourcesInsuffisantes
};

struct ResultatDeserialisation {
    StatutDeserialisation statut =
        StatutDeserialisation::FormatInvalide;

    EtatBanque etat;
    std::string message;

    bool estUnSucces() const {
        return statut ==
            StatutDeserialisation::Succes;
    }
};

// ==========================================
// NOUVEAU : RESULTAT DE CHARGEMENT
// ETAPE 3.9.4.4
// ==========================================

enum class StatutChargement {
    Succes,
    CheminInvalide,
    FichierIntrouvable,
    FichierNonRegulier,
    ErreurOuverture,
    ErreurLecture,
    FichierTropGrand,
    ErreurDeserialisation,
    RessourcesInsuffisantes
};

struct ResultatChargement {
    StatutChargement statut =
        StatutChargement::ErreurLecture;

    // NOUVEAU : conserver le statut
    // detaille du deserialiseur si celui-ci
    // a ete execute.
    StatutDeserialisation statutDeserialisation =
        StatutDeserialisation::FormatInvalide;

    EtatBanque etat;
    std::string message;

    bool estUnSucces() const {
        return statut == StatutChargement::Succes;
    }
};

// ==========================================
// INTERFACE PUBLIQUE
// ==========================================

class Persistance {
public:

    static ResultatSerialisation serialiser(
        const EtatBanque& etat
    );

    static ResultatEcritureTemporaire
    ecrireFichierTemporaire(
        const EtatBanque& etat,
        const std::string& cheminTemporaire
    );

    static ResultatSauvegarde sauvegarder(
        const EtatBanque& etat,
        const std::string& chemin
    );

    static ResultatDeserialisation deserialiser(
        const std::string& texte
    );

    // NOUVEAU : lecture controlee d'un
    // fichier principal .txt.
    //
    // Ne modifie pas les fichiers.
    // Ne modifie pas un objet Banque.
    // Ne restaure pas automatiquement .bak.
    static ResultatChargement charger(
        const std::string& cheminPrincipal
    );
};

#endif
