# 🏦 MiniBank C++

**MiniBank C++** est une application de simulation bancaire fictive développée en **C++17**, avec une interface interactive dans le terminal.

Ce projet pédagogique permet d'apprendre progressivement la programmation orientée objet, la gestion des comptes, la validation des données, les tests logiciels et le versionnement avec Git et GitHub.

**Statut actuel :** En développement — Création et consultation des comptes opérationnelles.

> ⚠️ **Avertissement :** MiniBank est un projet éducatif. Il ne gère aucun compte bancaire réel, aucun paiement réel et aucune donnée financière sensible.

---

## 🎯 Objectifs pédagogiques

- Apprendre les fondamentaux de C++17.
- Concevoir des classes et des objets.
- Comprendre l'encapsulation, les constructeurs et les méthodes.
- Organiser le code en fichiers `.h` et `.cpp`.
- Manipuler plusieurs comptes avec `std::vector`.
- Gérer les saisies et les erreurs dans le terminal.
- Représenter les montants en centimes entiers.
- Écrire des tests pour vérifier les fonctionnalités.
- Utiliser Git et GitHub pour suivre l'évolution du projet.

## ✅ Fonctionnalités disponibles

### 1. Création interactive de comptes

L'option `1` du menu permet de créer un compte bancaire fictif.

L'utilisateur saisit :

- Le nom du titulaire.
- Le solde initial en euros.

MiniBank génère automatiquement un identifiant, puis conserve le compte en mémoire.

**Exemple :**

```text
Votre choix : 1

========================================
       CREATION D'UN COMPTE
========================================

Nom du titulaire : Alice Martin
Solde initial (EUR) : 500,50

Compte cree avec succes !

Identifiant : MB-0001
Titulaire   : Alice Martin
Solde       : 500,50 EUR

Nombre total de comptes : 1
```

Un deuxième compte créé dans la même banque reçoit normalement l'identifiant `MB-0002`.

### 2. Consultation des comptes

L'option `2` du menu permet de retrouver un compte à partir de son identifiant.

**Exemple :**

```text
Votre choix : 2

========================================
       CONSULTATION D'UN COMPTE
========================================

Identifiant du compte : MB-0001

Compte trouve !

Identifiant : MB-0001
Titulaire   : Alice Martin
Solde       : 500,50 EUR
```

Si l'identifiant n'existe pas, MiniBank affiche un message d'erreur et revient au menu principal.

La consultation ne modifie pas les informations du compte.

### 3. Génération automatique d'identifiants

Les comptes créés par `Banque::creerCompte()` reçoivent des identifiants au format :

```text
MB-0001
MB-0002
MB-0003
...
```

La classe `Banque` vérifie qu'un identifiant n'est pas déjà utilisé avant de l'attribuer.

L'unicité est actuellement garantie au sein d'un objet `Banque` pendant son utilisation en mémoire. La persistance du compteur sera traitée lors de l'ajout de la sauvegarde.

### 4. Gestion sécurisée des montants

Le module `Montant` convertit les sommes saisies en euros vers des centimes entiers de type `std::int64_t`.

| Saisie en euros | Valeur en centimes |
|---|---:|
| `100` | 10000 |
| `25.50` | 2550 |
| `25,50` | 2550 |
| `0.99` | 99 |
| `0` | 0 |

Le module permet également l'affichage inverse : `2550` centimes deviennent `25,50 EUR`.

La conversion n'utilise pas `float` ou `double`. Elle vérifie notamment les caractères autorisés, le nombre de décimales et les dépassements numériques.

### 5. Gestion des comptes dans les classes C++

La classe `Compte` dispose de méthodes pour :

- Obtenir l'identifiant du compte.
- Obtenir le titulaire.
- Consulter le solde en centimes.
- Effectuer un dépôt.
- Effectuer un retrait.

La classe `Banque` permet notamment de :

- Conserver plusieurs comptes avec `std::vector<Compte>`.
- Ajouter un compte avec un identifiant fourni.
- Créer un compte avec un identifiant automatique.
- Rechercher un compte par identifiant.
- Connaître le nombre de comptes.

> **Important :** les dépôts et retraits sont implémentés et testés au niveau de la classe `Compte`, mais les options `3` et `4` du menu ne sont pas encore opérationnelles.

---

## 🚧 Fonctionnalités à développer

| Option | Fonctionnalité | État |
|---|---|---|
| `1` | Créer un compte | ✅ Fonctionnelle |
| `2` | Consulter un compte | ✅ Fonctionnelle |
| `3` | Déposer de l'argent | ⏳ À connecter au menu |
| `4` | Retirer de l'argent | ⏳ À connecter au menu |
| `5` | Effectuer un virement | ⏳ À développer |
| `6` | Consulter l'historique | ⏳ À développer |
| `7` | Afficher tous les comptes | ⏳ À développer |
| `8` | Sauvegarder les données | ⏳ À développer |
| `0` | Quitter MiniBank | ✅ Fonctionnelle |

La sauvegarde et le chargement des comptes au format CSV sont prévus pour une étape ultérieure.

**Actuellement, les comptes disparaissent à la fermeture du programme.**

---

## 🛠️ Technologies et outils

| Technologie | Utilisation |
|---|---|
| C++17 | Langage du projet |
| GCC / G++ | Compilateur |
| MSYS2 | Environnement de compilation utilisé sous Windows |
| Visual Studio Code | Éditeur de code |
| PowerShell | Exécution des commandes |
| Git | Gestion des versions |
| GitHub Desktop | Commits et synchronisation |
| GitHub | Hébergement du dépôt |

Le projet utilise actuellement la bibliothèque standard C++ et ne nécessite aucune bibliothèque externe.

## 📂 Structure du projet

```text
MiniBank_Cpp/
│
├── include/
│   ├── Compte.h
│   ├── Banque.h
│   └── Montant.h
│
├── src/
│   ├── main.cpp
│   ├── Compte.cpp
│   ├── Banque.cpp
│   └── Montant.cpp
│
├── tests/
│   ├── test_compte.cpp
│   ├── test_banque.cpp
│   ├── test_identifiants.cpp
│   └── test_montant.cpp
│
├── data/
│   └── .gitkeep
│
├── .gitignore
├── .gitattributes
└── README.md
```

### Rôle des fichiers

| Fichier | Responsabilité |
|---|---|
| `include/Compte.h` | Déclaration de la classe `Compte` |
| `src/Compte.cpp` | Gestion du solde, des dépôts et des retraits |
| `include/Banque.h` | Déclaration de la classe `Banque` |
| `src/Banque.cpp` | Collection, création, recherche et identifiants |
| `include/Montant.h` | Déclaration des fonctions de conversion |
| `src/Montant.cpp` | Conversion et formatage des montants |
| `src/main.cpp` | Menu interactif et connexion des modules |
| `tests/test_compte.cpp` | Tests de la classe `Compte` |
| `tests/test_banque.cpp` | Tests de la classe `Banque` |
| `tests/test_identifiants.cpp` | Tests des identifiants automatiques |
| `tests/test_montant.cpp` | Tests des conversions monétaires |
| `data/` | Dossier réservé aux futures données locales |

---

## 🧱 Architecture du programme

MiniBank sépare les responsabilités entre plusieurs composants.

```text
Utilisateur
    |
    v
main.cpp
    |
    +----> Montant
    |      Conversion EUR / centimes
    |
    +----> Banque
              |
              +----> Compte
              +----> Compte
              +----> Compte
```

### Classe `Compte`

Elle représente un compte bancaire fictif.

Ses attributs principaux sont :

- `identifiant`
- `titulaire`
- `soldeCentimes`

Les informations sont privées et accessibles à travers des méthodes publiques.

### Classe `Banque`

Elle gère une collection de comptes et les identifiants automatiques.

Elle s'appuie sur `std::vector<Compte>`.

### Module `Montant`

Il fournit deux fonctions principales :

```cpp
Montant::convertirEnCentimes(...)
Montant::formater(...)
```

Elles assurent la conversion entre les montants textuels en euros et leur représentation interne en centimes.

### Programme principal

`main.cpp` affiche le menu, lit les saisies de l'utilisateur et appelle les modules appropriés.

Les opérations de dépôt et de retrait seront progressivement connectées au menu dans les prochaines étapes.

---

## ▶️ Compiler et lancer MiniBank

### Prérequis

- Windows avec PowerShell.
- Un compilateur `g++` compatible C++17.
- Le compilateur accessible depuis le terminal.

Vérifier l'installation :

```powershell
g++ --version
```

### Compiler l'application

Depuis le dossier racine `MiniBank_Cpp` :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude src/main.cpp src/Banque.cpp src/Compte.cpp src/Montant.cpp -o MiniBank.exe
```

### Lancer le programme

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

Pour tester la consultation, crée d'abord un compte avec l'option `1`, puis recherche son identifiant avec l'option `2`, sans quitter le programme.

---

## 🧪 Tests du projet

MiniBank possède actuellement quatre programmes de test indépendants.

### Tests de la classe Compte

Compiler :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_compte.cpp src/Compte.cpp -o test_compte.exe
```

Exécuter :

```powershell
.\test_compte.exe
```

**Résultat précédemment validé : 6 tests réussis.**

### Tests de la classe Banque

Compiler :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_banque.cpp src/Banque.cpp src/Compte.cpp -o test_banque.exe
```

Exécuter :

```powershell
.\test_banque.exe
```

**Résultat précédemment validé : 13 tests réussis.**

### Tests des identifiants automatiques

Compiler :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_identifiants.cpp src/Banque.cpp src/Compte.cpp -o test_identifiants.exe
```

Exécuter :

```powershell
.\test_identifiants.exe
```

**Résultat précédemment validé : 12 tests réussis.**

Ces tests vérifient notamment la numérotation, l'unicité, les créations invalides et la gestion d'identifiants déjà utilisés.

### Tests du module Montant

Compiler :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_montant.cpp src/Montant.cpp -o test_montant.exe
```

Exécuter :

```powershell
.\test_montant.exe
```

**Résultat précédemment validé : 22 tests réussis.**

Ces tests couvrent les montants valides, les saisies incorrectes, les décimales, les limites numériques et le formatage.

### Bilan des tests automatisés

| Module | Tests validés |
|---|---:|
| `Compte` | 6 |
| `Banque` | 13 |
| Identifiants automatiques | 12 |
| `Montant` | 22 |
| **Total** | **53** |

Ces résultats correspondent aux exécutions effectuées pendant le développement. Ils ne constituent pas une couverture exhaustive.

Les options `1` et `2` ont également été vérifiées manuellement dans le terminal.

---

## 🔐 Règles de gestion

MiniBank applique actuellement les règles suivantes :

- Le titulaire et l'identifiant ne peuvent pas être vides lors de la création des comptes.
- Le menu refuse un nom composé uniquement d'espaces.
- Le solde initial ne peut pas être négatif.
- Un solde initial nul est autorisé.
- Les dépôts et les retraits doivent avoir un montant strictement positif.
- Un retrait ne peut pas dépasser le solde disponible.
- Un dépôt ne doit pas provoquer un dépassement de capacité numérique.
- Les opérations refusées ne doivent pas modifier le solde.
- Deux comptes d'une même banque ne peuvent pas avoir le même identifiant.
- La consultation d'un compte ne modifie pas ses informations.
- Les montants financiers sont représentés par des entiers en centimes.

Ce projet ne constitue pas une application bancaire sécurisée ou destinée à la production.

---

## 🗺️ Feuille de route

### Étapes réalisées

- [x] Initialiser le projet C++17.
- [x] Configurer le dépôt GitHub.
- [x] Créer le menu principal.
- [x] Développer la classe `Compte`.
- [x] Implémenter les dépôts et retraits dans `Compte`.
- [x] Tester la classe `Compte`.
- [x] Développer la classe `Banque`.
- [x] Tester la gestion de plusieurs comptes.
- [x] Préparer `main.cpp` pour utiliser `Banque`.
- [x] Générer automatiquement les identifiants.
- [x] Tester les identifiants automatiques.
- [x] Développer le module `Montant`.
- [x] Tester les conversions monétaires.
- [x] Connecter la création des comptes au menu.
- [x] Connecter la consultation des comptes au menu.

### Étapes prévues

- [ ] Connecter les dépôts au menu.
- [ ] Connecter les retraits au menu.
- [ ] Afficher tous les comptes.
- [ ] Implémenter les virements.
- [ ] Ajouter l'historique des opérations.
- [ ] Sauvegarder les données fictives en CSV.
- [ ] Charger les données depuis un fichier CSV.
- [ ] Ajouter des tests complémentaires.
- [ ] Finaliser la documentation et préparer une version 1.0.

---

## 🌐 Git et GitHub

Le développement est suivi avec Git et GitHub Desktop.

Les commits permettent de conserver un historique des fonctionnalités, des corrections et des améliorations apportées au projet.

Les fichiers exécutables et les fichiers de compilation sont exclus du dépôt grâce à `.gitignore`.

Le dossier `data/` est prévu pour stocker les futures données fictives locales sans les publier sur GitHub.

**Dépôt public :** https://github.com/datasifaw/MiniBank_Cpp

## 📄 Licence

Aucune licence open source n'a encore été ajoutée au projet.

Un dépôt public sur GitHub ne signifie pas automatiquement que son contenu peut être librement réutilisé ou redistribué.

---

## 📌 État actuel du projet

**Dernière étape fonctionnelle validée : 3.5.3 — Consultation des comptes depuis le menu.**

Les options `1`, `2` et `0` sont opérationnelles.

Les options de dépôt, retrait, virement, historique, liste des comptes et sauvegarde restent à intégrer ou développer.

**Prochaine étape envisagée : 3.5.4.1 — Déposer de l'argent depuis le menu.**

MiniBank C++ reste un projet pédagogique en cours de développement.