
#ifndef COMPTE_H
#define COMPTE_H

#include <string>
#include <cstdint>

class Compte {

private:
    std::string identifiant;
    std::string titulaire;
    std::int64_t soldeCentimes;

public:
    Compte(const std::string& id,
           const std::string& nom,
           std::int64_t soldeInitialCentimes);

    //Ajouter les méthodes de dépôt et de retrait
    bool deposer(std::int64_t montantCentimes);
    bool retirer(std::int64_t montantCentimes);


    std::string getIdentifiant() const;
    std::string getTitulaire() const;
    std::int64_t getSoldeCentimes() const;
};

#endif
