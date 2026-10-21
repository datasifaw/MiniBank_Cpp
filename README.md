# MiniBank C++ 🏦

MiniBank est une application bancaire pédagogique en **C++17**, exécutée en console sous **Windows**. Elle permet de gérer des comptes et des mouvements bancaires, de consulter les informations enregistrées en mémoire, puis de sauvegarder ou restaurer l'état de la banque.

> **Projet pédagogique :** MiniBank ne traite pas de véritables transactions bancaires et n'est pas destiné à stocker des informations bancaires réelles.

## Fonctionnalités

- Création et recherche de comptes ; consultation des comptes.
- Gestion de soldes exprimés en **centimes** pour éviter les imprécisions des nombres flottants.
- Dépôts, retraits et virements entre comptes, avec contrôles métier.
- Consultation de l'historique des opérations.
- Export et restauration de l'état complet de la banque : comptes, historique et compteurs d'identifiants.
- Sauvegarde sur disque et chargement depuis le fichier principal.
- Récupération **explicite** d'une sauvegarde précédente `.bak`.

## Environnement

- Windows 11 (environnement de développement et de tests du projet).
- **g++ compatible C++17** (par exemple MSYS2 / MinGW-w64).
- VS Code et PowerShell pour le développement et l'exécution.

## Structure du projet

```text
MiniBank_Cpp/
├── include/
│   ├── Banque.h
│   ├── Compte.h
│   ├── Montant.h
│   ├── Operation.h
│   ├── EtatBanque.h
│   ├── Persistance.h
│   └── MenuPersistance.h
├── src/
│   ├── main.cpp
│   ├── Banque.cpp
│   ├── Compte.cpp
│   ├── Montant.cpp
│   ├── Persistance.cpp
│   ├── Deserialisation.cpp
│   ├── Chargement.cpp
│   ├── Restauration.cpp
│   └── Recuperation.cpp
├── tests/
├── data/
└── README.md
```

`MenuPersistance.h` contient le sous-menu de persistance raccordé au menu principal.

## Compilation (PowerShell)

Ouvrir PowerShell **à la racine du projet**, puis exécuter :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude `
    src/main.cpp `
    src/Banque.cpp `
    src/Compte.cpp `
    src/Montant.cpp `
    src/Persistance.cpp `
    src/Deserialisation.cpp `
    src/Chargement.cpp `
    src/Restauration.cpp `
    src/Recuperation.cpp `
    -o MiniBank.exe

if ($LASTEXITCODE -ne 0) {
    throw "La compilation MiniBank a echoue."
}
```

Lancer ensuite l'application :

```powershell
.\MiniBank.exe
```

Les chemins `data/...` sont relatifs au **répertoire de lancement** de l'application. Pour travailler avec les données du projet, lancer MiniBank depuis la racine du projet. Pour les essais, utiliser un répertoire temporaire distinct avec son propre dossier `data`.

## Utilisation du menu

Le menu principal permet la gestion des comptes, des mouvements et de leur consultation.

L'**option `8`** ouvre le sous-menu « Sauvegarde et restauration » :

| Choix | Action |
| --- | --- |
| `1` | Sauvegarder la banque active dans `data/minibank.txt` |
| `2` | Charger `data/minibank.txt` et, après confirmation, restaurer son état |
| `3` | Demander explicitement la récupération de `data/minibank.bak` et, après confirmation, restaurer son état |
| `0` | Revenir au menu principal |

Une lecture réussie ne suffit pas à remplacer la banque active : **la restauration nécessite une confirmation**. Refuser cette confirmation conserve l'état actuellement en mémoire.

## Sauvegardes et restauration

### Fichier principal

Le fichier principal est :

```text
data/minibank.txt
```

La persistance utilise un format texte versionné (`MINIBANK|1`) qui représente les comptes, les opérations et les compteurs utiles à la continuité des identifiants. L'intégrité du contenu est vérifiée lors du chargement.

### Sauvegarde précédente

Lorsqu'une deuxième sauvegarde remplace un fichier principal existant, le moteur Windows conserve sa version antérieure sous le nom :

```text
data/minibank.bak
```

**Attention :** le nom est `minibank.bak`, **pas** `minibank.txt.bak`.

Si un `.bak` existe déjà, MiniBank **refuse une nouvelle sauvegarde** plutôt que de remplacer silencieusement cette copie précédente. Une troisième sauvegarde sur les mêmes chemins n'est donc pas acceptée tant que le `.bak` existe.

### Règles de sécurité

- Le chargement du fichier principal **ne bascule jamais automatiquement** vers le `.bak`.
- La récupération du `.bak` n'est proposée que sur **choix explicite**.
- Un fichier absent, illisible, non régulier, trop volumineux ou invalide est refusé.
- En cas d'échec du chargement, la banque active reste inchangée.
- La restauration de l'état est atomique : un état invalide ne doit pas remplacer partiellement les données actives.
- Une lecture ou une restauration en mémoire ne doit modifier ni `minibank.txt` ni `minibank.bak`.
- Une restauration depuis `.bak` replace l'état de la banque **à la date de cette ancienne sauvegarde** : les opérations plus récentes ne font pas partie de l'état récupéré.

**Ne pas supprimer ou renommer manuellement une sauvegarde réelle pour contourner un refus sans avoir préalablement vérifié et conservé les données.**

## Tests et validation

Le projet comprend **18 suites de tests** couvrant notamment comptes, montants, virements, historique, sérialisation, publication Windows, désérialisation, chargement, restauration et récupération du `.bak`.

À l'issue de la phase 3.9.4, la vérification globale a réussi : **18 suites sur 18**. Les **15 premières suites** comptent **773 assertions de référence** ; la suite d'intégration de restauration compte **47 vérifications réussies**. Le total consolidé des assertions des 18 suites n'est pas indiqué ici, car les compteurs individuels de deux suites supplémentaires n'ont pas été établis.

Les essais manuels du menu `8` ont également confirmé : la sauvegarde initiale, le chargement du principal, la création de `minibank.bak`, sa récupération explicite, le refus des restaurations invalides, l'annulation et le refus d'écrasement d'une sauvegarde précédente.

Pour les tests de fichiers, créer un dossier neuf et isolé. **Ne jamais exécuter des tests destructifs sur `data/` contenant des données à préserver.**

## Gestion du dépôt

Le code et les suites de tests peuvent évoluer localement. La consultation de l'état du dépôt s'effectue avec :

```powershell
git status --short
```

Aucune commande de commit ou de push n'est nécessaire pour compiler ou utiliser MiniBank.

## Limites

- Application de démonstration locale en console, et non service bancaire réel.
- Persistance fondée sur des fichiers locaux, sans gestion multi-utilisateur ou transactions concurrentes.
- Conservation limitée de l'historique des sauvegardes : un principal et, au plus, une sauvegarde précédente protégée `.bak`.

---
## 🚀 Évolutions envisagées

MiniBank C++ v1.0.0 constitue une première version stable du projet. Plusieurs améliorations pourraient être envisagées à l'avenir :

- **Relevés bancaires :** export de l'historique des opérations au format CSV.
- **Interface graphique :** développement d'une interface Windows pour faciliter l'utilisation de l'application.
- **Sécurité :** amélioration de la protection des données et de la gestion des sauvegardes.
- **Gestion avancée :** recherche d'opérations, filtres par date et statistiques sur les comptes.
- **Tests automatisés :** extension de la couverture des tests pour les futures fonctionnalités.

Ces évolutions représentent des pistes possibles et ne constituent pas des fonctionnalités disponibles dans la version v1.0.0.


**MiniBank C++ — apprentissage de la programmation orientée objet, de la validation des données et de la persistance sécurisée en C++17.**
