# 🏦 MiniBank C++

**MiniBank C++** est une application de simulation bancaire développée en C++17, avec une interface en ligne de commande (CLI).

Ce projet pédagogique a pour objectif d'apprendre progressivement la programmation orientée objet, la gestion de collections, la validation des données, les tests logiciels et le versionnement avec Git et GitHub.

> **Statut :** En développement — Phase 3, gestion de plusieurs comptes.
>
> **Important :** MiniBank est une simulation éducative. Aucun argent réel, compte bancaire réel ou service financier n'est utilisé.

---

## 🎯 Objectifs du projet

- Apprendre les fondamentaux de C++17.
- Appliquer la programmation orientée objet (POO).
- Comprendre les classes, objets, constructeurs et méthodes.
- Utiliser l'encapsulation avec `private` et `public`.
- Gérer plusieurs objets avec `std::vector`.
- Développer une interface interactive dans le terminal.
- Écrire et exécuter des tests logiciels.
- Organiser un projet C++ en plusieurs fichiers.
- Utiliser Git et GitHub pour suivre les versions.

## ✅ Fonctionnalités réalisées

### Classe `Compte`

- Création d'un compte fictif avec identifiant, titulaire et solde initial.
- Consultation de l'identifiant, du titulaire et du solde.
- Dépôt d'argent.
- Retrait d'argent.
- Refus des opérations de montant nul ou négatif.
- Protection contre les retraits supérieurs au solde.
- Protection contre les dépassements de capacité lors des dépôts.
- Validation du constructeur pour les informations invalides.

### Classe `Banque`

- Gestion d'une collection de comptes avec `std::vector<Compte>`.
- Ajout de plusieurs comptes.
- Refus des identifiants en double.
- Recherche d'un compte par identifiant.
- Recherche en lecture seule avec `const`.
- Consultation du nombre de comptes.
- Refus des créations invalides.

### Interface terminal

- Affichage du menu principal.
- Saisie du choix utilisateur.
- Gestion des choix invalides.
- Sortie du programme avec l'option `0`.
- Initialisation d'un objet `Banque`.

**À noter :** les méthodes des classes `Compte` et `Banque` sont implémentées, mais les opérations bancaires ne sont pas encore connectées aux options du menu.

## 🚧 Fonctionnalités à venir

- Création de comptes depuis le menu.
- Consultation d'un compte depuis le menu.
- Dépôts et retraits interactifs.
- Liste détaillée des comptes.
- Génération automatique des identifiants.
- Virements entre comptes.
- Historique des transactions.
- Sauvegarde et chargement des données locales au format CSV.
- Tests complémentaires et améliorations de robustesse.

---

## 🛠️ Technologies utilisées

| Technologie | Utilisation |
|---|---|
| C++17 | Langage de programmation |
| GCC / G++ | Compilation |
| Visual Studio Code | Éditeur |
| Git | Gestion des versions |
| GitHub | Hébergement du code |
| GitHub Desktop | Gestion des commits et push |
| PowerShell | Terminal sous Windows |

Le projet utilise actuellement la bibliothèque standard C++ et ne nécessite pas de bibliothèque externe.

## 📂 Structure du projet

```text
MiniBank_Cpp/
│
├── include/
│   ├── Compte.h
│   └── Banque.h
│
├── src/
│   ├── main.cpp
│   ├── Compte.cpp
│   └── Banque.cpp
│
├── tests/
│   ├── test_compte.cpp
│   └── test_banque.cpp
│
├── data/
│   └── .gitkeep
│
├── .gitignore
├── .gitattributes
└── README.md
```

### Description des composants

**`include/Compte.h`** : déclaration de la classe `Compte`, de ses attributs privés et de ses méthodes publiques.

**`src/Compte.cpp`** : implémentation du constructeur, des getters, des dépôts, des retraits et des contrôles associés.

**`include/Banque.h`** : déclaration de la classe `Banque` et de sa collection de comptes.

**`src/Banque.cpp`** : implémentation des méthodes d'ajout, de recherche et de comptage.

**`src/main.cpp`** : point d'entrée du programme et interface terminal.

**`tests/test_compte.cpp`** : tests pédagogiques de la classe `Compte`.

**`tests/test_banque.cpp`** : tests de la classe `Banque`.

**`data/`** : dossier réservé aux futures données de simulation sauvegardées localement.

---

## ▶️ Compilation et exécution

### Prérequis

- Windows.
- G++ compatible C++17, configuré dans le `PATH`.
- Un terminal PowerShell.

Vérifier le compilateur :

```powershell
g++ --version
```

### Compiler MiniBank

Depuis la racine du projet :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude src/main.cpp src/Banque.cpp src/Compte.cpp -o MiniBank.exe
```

### Exécuter MiniBank

```powershell
.\MiniBank.exe
```

### Menu principal

```text
========================================
              MINIBANK C++
========================================

1. Creer un compte
2. Consulter un compte
3. Deposer de l'argent
4. Retirer de l'argent
5. Effectuer un virement
6. Consulter l'historique
7. Afficher tous les comptes
8. Sauvegarder les donnees
0. Quitter

Votre choix :
```

À ce stade, les options `1` à `8` sont affichées mais restent en attente d'intégration. L'option `0` permet de quitter le programme.

---

## 🧪 Tests

### Tester la classe Compte

Compilation :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_compte.cpp src/Compte.cpp -o test_compte.exe
```

Exécution :

```powershell
.\test_compte.exe
```

**Résultat observé :** 6 tests affichant `OK`.

Les tests couvrent notamment les dépôts invalides, les retraits invalides, les soldes insuffisants et le rejet d'un solde initial négatif.

### Tester la classe Banque

Compilation :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_banque.cpp src/Banque.cpp src/Compte.cpp -o test_banque.exe
```

Exécution :

```powershell
.\test_banque.exe
```

**Dernier résultat observé :**

```text
=== BILAN ===
Tests reussis : 13
Tests echoues : 0
```

Ces tests vérifient notamment l'ajout de comptes, la recherche, le refus des identifiants dupliqués et le nombre de comptes.

Les tests actuels ne couvrent pas encore toutes les situations possibles.

---

## 🔐 Règles de gestion

- Un compte doit posséder un identifiant et un titulaire non vides.
- Un solde initial ne peut pas être négatif.
- Deux comptes d'une même banque ne peuvent pas partager le même identifiant.
- Les montants sont représentés par des entiers en centimes.
- Les dépôts et retraits doivent être strictement positifs.
- Un retrait ne peut pas dépasser le solde disponible.
- Une opération refusée ne doit pas modifier le solde.
- Les valeurs dépassant les limites du type entier doivent être contrôlées.

La génération automatique des identifiants et les règles relatives aux virements seront ajoutées lors de prochaines étapes.

## 🧱 Architecture orientée objet

### Classe Compte

Responsable des informations et opérations associées à un compte bancaire individuel.

Attributs principaux :

- `identifiant`
- `titulaire`
- `soldeCentimes`

Méthodes principales :

- `getIdentifiant()`
- `getTitulaire()`
- `getSoldeCentimes()`
- `deposer()`
- `retirer()`

### Classe Banque

Responsable de la gestion de plusieurs comptes.

Elle utilise une collection `std::vector<Compte>`.

Méthodes principales :

- `ajouterCompte()`
- `rechercherCompte()`
- `nombreComptes()`

Cette architecture sépare la logique d'un compte individuel de la gestion globale de la banque.

---

## 🗺️ Feuille de route

- [x] Initialiser le projet C++.
- [x] Configurer Git et GitHub.
- [x] Créer le menu principal.
- [x] Développer la classe `Compte`.
- [x] Ajouter les dépôts et retraits.
- [x] Tester la classe `Compte`.
- [x] Développer la classe `Banque`.
- [x] Tester la gestion de plusieurs comptes.
- [x] Préparer `main.cpp` pour intégrer `Banque`.
- [ ] Connecter la création des comptes au menu.
- [ ] Connecter la consultation des comptes.
- [ ] Connecter les dépôts et les retraits.
- [ ] Afficher tous les comptes.
- [ ] Ajouter les virements.
- [ ] Implémenter l'historique.
- [ ] Sauvegarder et charger les données CSV.
- [ ] Finaliser les tests et publier la version 1.0.

## 🌐 Git et GitHub

Le code source est versionné avec Git et hébergé sur GitHub.

Les modifications sont enregistrées progressivement à l'aide de commits décrivant les fonctionnalités, les corrections et les tests.

Les exécutables et fichiers de compilation sont exclus du dépôt grâce à `.gitignore`.

Les futures données bancaires fictives produites localement seront également exclues du versionnement.

**Dépôt GitHub :** https://github.com/datasifaw/MiniBank_Cpp

---

## 📄 Licence

Aucune licence open source n'a encore été choisie pour ce projet.

Le dépôt est public, mais cela ne signifie pas automatiquement que son code peut être réutilisé ou redistribué sans autorisation.

---

## 📌 Statut actuel

**MiniBank C++ : projet pédagogique en cours de développement.**

Dernière étape technique validée : **3.5.1 — Préparation de `main.cpp` pour la classe `Banque`.**

Prochaine étape prévue : **3.5.2 : Création d'un compte depuis le menu interactif !!**
