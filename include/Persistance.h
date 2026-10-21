#ifndef PERSISTANCE_H
#define PERSISTANCE_H

#include <string>
#include "EtatBanque.h"

enum class StatutSauvegarde {
    Succes, NonImplemente, CheminInvalide,
    ErreurSerialisation, ErreurEcriture, ErreurRemplacement,
    SauvegardePrecedenteExistante, FichierPrincipalInvalide
};
struct ResultatSauvegarde {
    StatutSauvegarde statut = StatutSauvegarde::NonImplemente;
    std::string message;
    bool estUnSucces() const { return statut == StatutSauvegarde::Succes; }
};

enum class StatutSerialisation { Succes, DonneesInvalides, ErreurMemoire };
struct ResultatSerialisation {
    StatutSerialisation statut = StatutSerialisation::DonneesInvalides;
    std::string texte;
    std::string message;
    bool estUnSucces() const { return statut == StatutSerialisation::Succes; }
};

enum class StatutEcritureTemporaire {
    Succes, CheminInvalide, FichierDejaExistant,
    ErreurSerialisation, ErreurOuverture, ErreurEcriture,
    ErreurFermeture, ErreurLecture, ContenuDifferent
};
struct ResultatEcritureTemporaire {
    StatutEcritureTemporaire statut = StatutEcritureTemporaire::ErreurEcriture;
    std::string message;
    bool estUnSucces() const { return statut == StatutEcritureTemporaire::Succes; }
};

enum class StatutDeserialisation {
    Succes, VersionInconnue, FormatInvalide, EncodageInvalide,
    NombreInvalide, EtatIncoherent, RessourcesInsuffisantes
};
struct ResultatDeserialisation {
    StatutDeserialisation statut = StatutDeserialisation::FormatInvalide;
    EtatBanque etat;
    std::string message;
    bool estUnSucces() const { return statut == StatutDeserialisation::Succes; }
};

enum class StatutChargement {
    Succes, CheminInvalide, FichierIntrouvable, FichierNonRegulier,
    ErreurOuverture, ErreurLecture, FichierTropGrand,
    ErreurDeserialisation, RessourcesInsuffisantes
};
struct ResultatChargement {
    StatutChargement statut = StatutChargement::ErreurLecture;
    StatutDeserialisation statutDeserialisation = StatutDeserialisation::FormatInvalide;
    EtatBanque etat;
    std::string message;
    bool estUnSucces() const { return statut == StatutChargement::Succes; }
};

// NOUVEAU (3.9.4.6) : statut distinct du chargement principal.
enum class StatutRecuperation {
    Succes, CheminInvalide, SauvegardeAbsente, FichierNonRegulier,
    ErreurOuverture, ErreurLecture, FichierTropGrand,
    DonneesInvalides, RessourcesInsuffisantes
};

// NOUVEAU : conserver le diagnostic de deserialisation.
struct ResultatRecuperation {
    StatutRecuperation statut = StatutRecuperation::ErreurLecture;
    StatutDeserialisation statutDeserialisation = StatutDeserialisation::FormatInvalide;
    EtatBanque etat;
    std::string message;
    bool estUnSucces() const { return statut == StatutRecuperation::Succes; }
};

class Persistance {
public:
    static ResultatSerialisation serialiser(const EtatBanque& etat);
    static ResultatEcritureTemporaire ecrireFichierTemporaire(
        const EtatBanque& etat, const std::string& cheminTemporaire);
    static ResultatSauvegarde sauvegarder(
        const EtatBanque& etat, const std::string& chemin);
    static ResultatDeserialisation deserialiser(const std::string& texte);
    static ResultatChargement charger(const std::string& cheminPrincipal);

    // NOUVEAU : lecture EXPLICITE du chemin principal dont .txt est remplace par .bak.
    // Ne change aucun fichier ni aucun objet Banque.
    static ResultatRecuperation chargerSauvegardePrecedente(
        const std::string& cheminPrincipal);
};

#endif
