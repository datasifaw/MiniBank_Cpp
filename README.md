# 🏦 MiniBank C++

**MiniBank C++** est une application de simulation bancaire fictive développée en **C++17**, avec une interface interactive en ligne de commande (CLI).

Ce projet pédagogique permet d'apprendre progressivement la programmation orientée objet, la gestion des comptes bancaires, les validations numériques, les tests logiciels et le versionnement avec Git et GitHub.

**Statut :** Projet en développement — Création, consultation, dépôts et retraits opérationnels.

> ⚠️ **Avertissement :** MiniBank est un projet éducatif. Il ne gère aucun argent réel, aucune transaction bancaire réelle et n'est pas destiné à un environnement de production.

---

## 🎯 Objectifs pédagogiques

- Apprendre les fondamentaux du langage C++17.
- Concevoir des classes et des objets.
- Comprendre l'encapsulation, les constructeurs, les méthodes et les pointeurs.
- Organiser le code en plusieurs fichiers `.h` et `.cpp`.
- Manipuler plusieurs comptes avec `std::vector`.
- Construire un menu interactif dans le terminal.
- Valider les saisies utilisateur et gérer les erreurs.
- Manipuler les montants en centimes entiers.
- Créer des tests unitaires et des tests d'intégration.
- Utiliser Git, GitHub et GitHub Desktop pour suivre l'évolution du projet.

---

## ✅ Fonctionnalités disponibles

### 1. Création d'un compte bancaire fictif

L'option `1` permet de créer un compte en saisissant le nom du titulaire et le solde initial en euros.

La classe `Banque` génère automatiquement un identifiant unique parmi les comptes actuellement présents.

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

Les comptes suivants reçoivent des identifiants comme `MB-0002`, `MB-0003`, etc.

La création refuse les titulaires vides ou composés uniquement d'espaces ainsi que les montants invalides.

Un solde initial de `0` euro est autorisé.

### 2. Consultation d'un compte

L'option `2` permet de consulter un compte à partir de son identifiant.

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

Si aucun compte ne correspond à l'identifiant saisi, l'application affiche un message d'erreur.

La consultation utilise un pointeur en lecture seule (`const Compte*`) et ne modifie pas les informations du compte.

### 3. Dépôt d'argent

L'option `3` permet d'ajouter de l'argent sur un compte existant.

Le montant saisi en euros est converti en centimes, puis transmis à la méthode `Compte::deposer()`.

**Exemple :**

```text
Votre choix : 3

========================================
             DEPOT D'ARGENT
========================================

Identifiant du compte : MB-0001
Montant du depot (EUR) : 100,50

Depot effectue avec succes !

Identifiant    : MB-0001
Titulaire      : Alice Martin
Ancien solde   : 500,50 EUR
Montant depose : 100,50 EUR
Nouveau solde  : 601,00 EUR
```

**Règles appliquées :**

- Le compte doit exister.
- Le montant doit être valide et strictement supérieur à zéro.
- Un dépôt ne doit pas provoquer de dépassement de la capacité numérique de `std::int64_t`.
- Si le dépôt est refusé, le solde reste inchangé.

### 4. Retrait d'argent

L'option `4` permet de retirer de l'argent d'un compte existant.

Le retrait est réalisé par la méthode `Compte::retirer()`, qui vérifie que le montant est autorisé et que le solde disponible est suffisant.

**Exemple :**

```text
Votre choix : 4

========================================
            RETRAIT D'ARGENT
========================================

Identifiant du compte : MB-0001
Montant du retrait (EUR) : 50

Retrait effectue avec succes !

Identifiant    : MB-0001
Titulaire      : Alice Martin
Ancien solde   : 601,00 EUR
Montant retire : 50,00 EUR
Nouveau solde  : 551,00 EUR
```

**Règles appliquées :**

- Le compte doit exister.
- Le montant doit être strictement positif.
- Le solde doit être suffisant.
- Le retrait ne doit pas rendre le solde négatif.
- Une opération refusée ne modifie pas le solde.

### 5. Génération automatique des identifiants

La méthode `Banque::creerCompte()` attribue des identifiants au format :

```text
MB-0001
MB-0002
MB-0003
...
```

Avant d'utiliser un identifiant, la banque vérifie qu'il n'est pas déjà attribué.

Les identifiants sont actuellement uniques au sein d'un objet `Banque` pendant son exécution.

La sauvegarde et la restauration de l'état de la banque seront développées ultérieurement.

### 6. Conversion et affichage des montants

Le module `Montant` permet de convertir les euros en centimes entiers et inversement.

| Saisie en euros | Valeur stockée |
|---|---:|
| `100` | 10000 centimes |
| `25.50` | 2550 centimes |
| `25,50` | 2550 centimes |
| `0.99` | 99 centimes |
| `0` | 0 centime |

Les fonctions principales sont :

```cpp
Montant::convertirEnCentimes(...)
Montant::formater(...)
```

La conversion repose sur l'analyse du texte et des calculs entiers, sans utiliser `float` ou `double`.

Les montants mal formés, les valeurs négatives saisies et les dépassements numériques sont refusés par la fonction de conversion.

---

## 📋 État du menu principal

| Option | Description | Statut |
|---|---|---|
| `1` | Créer un compte | ✅ Opérationnelle |
| `2` | Consulter un compte | ✅ Opérationnelle |
| `3` | Déposer de l'argent | ✅ Opérationnelle |
| `4` | Retirer de l'argent | ✅ Opérationnelle |
| `5` | Effectuer un virement | ⏳ À développer |
| `6` | Consulter l'historique | ⏳ À développer |
| `7` | Afficher tous les comptes | ⏳ À développer |
| `8` | Sauvegarder les données | ⏳ À développer |
| `0` | Quitter le programme | ✅ Opérationnelle |

**Important :** les comptes sont conservés uniquement en mémoire. Ils ne sont pas encore restaurés lors d'un nouveau lancement de MiniBank.

---

## 🛠️ Technologies utilisées

| Technologie | Utilisation |
|---|---|
| C++17 | Langage de programmation |
| GCC / G++ | Compilation |
| MSYS2 | Environnement de compilation sous Windows |
| Visual Studio Code | Éditeur de code |
| PowerShell | Terminal |
| Git | Gestion des versions |
| GitHub Desktop | Commits et push |
| GitHub | Hébergement du projet |

MiniBank utilise actuellement la bibliothèque standard de C++ et ne nécessite aucune bibliothèque externe.

---

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
│   ├── test_montant.cpp
│   └── test_parcours.cpp
│
├── data/
│   └── .gitkeep
│
├── .gitignore
├── .gitattributes
└── README.md
```

### Responsabilité des fichiers

| Fichier | Rôle |
|---|---|
| `include/Compte.h` | Déclaration de la classe `Compte` |
| `src/Compte.cpp` | Dépôts, retraits et gestion du solde |
| `include/Banque.h` | Déclaration de la classe `Banque` |
| `src/Banque.cpp` | Création, recherche et gestion des identifiants |
| `include/Montant.h` | Déclaration des fonctions de conversion |
| `src/Montant.cpp` | Conversion et formatage des montants |
| `src/main.cpp` | Interface terminal et traitement du menu |
| `tests/test_compte.cpp` | Tests de la classe `Compte` |
| `tests/test_banque.cpp` | Tests de la classe `Banque` |
| `tests/test_identifiants.cpp` | Tests des identifiants automatiques |
| `tests/test_montant.cpp` | Tests de conversion monétaire |
| `tests/test_parcours.cpp` | Tests d'intégration du parcours bancaire |
| `data/` | Futur stockage local des données fictives |

---

## 🧱 Architecture orientée objet

MiniBank sépare la gestion du terminal, les règles bancaires et les conversions monétaires.

```text
Utilisateur
    |
    v
main.cpp
    |
    +----> Montant
    |      Conversion et affichage
    |
    +----> Banque
              |
              +----> Compte (Alice)
              |
              +----> Compte (Bob)
              |
              +----> Autres comptes
```

### Classe `Compte`

Elle représente un compte bancaire fictif.

Attributs principaux :

- `identifiant`
- `titulaire`
- `soldeCentimes`

Méthodes :

```cpp
getIdentifiant()
getTitulaire()
getSoldeCentimes()
deposer(...)
retirer(...)
```

### Classe `Banque`

Elle gère les comptes à l'aide de `std::vector<Compte>`.

Elle permet d'ajouter, de créer automatiquement, de rechercher et de compter les comptes.

### Module `Montant`

Il assure la validation, la conversion et le formatage des montants.

### Fichier `main.cpp`

Il affiche le menu, récupère les saisies et utilise les classes déjà développées.

Les opérations bancaires ne sont pas calculées directement dans le menu : les règles sont appliquées par les classes concernées.

---

## ▶️ Compilation et exécution

### Prérequis

- Windows et PowerShell.
- G++ compatible C++17.
- Le compilateur configuré dans le `PATH`.

Vérifier G++ :

```powershell
g++ --version
```

### Compiler MiniBank

Ouvrir un terminal à la racine du projet, puis exécuter :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude src/main.cpp src/Banque.cpp src/Compte.cpp src/Montant.cpp -o MiniBank.exe
```

### Exécuter MiniBank

```powershell
.\MiniBank.exe
```

Le menu principal s'affiche :

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

Les options `1` à `4` fonctionnent. Les options `5` à `8` affichent encore un message indiquant que la fonctionnalité est à venir.

---

## 🧪 Tests

Le projet contient cinq programmes de tests indépendants.

### 1. Tests de la classe Compte

Compilation :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_compte.cpp src/Compte.cpp -o test_compte.exe
```

Exécution :

```powershell
.\test_compte.exe
```

**Résultat précédemment validé : 6 tests réussis.**

### 2. Tests de la classe Banque

Compilation :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_banque.cpp src/Banque.cpp src/Compte.cpp -o test_banque.exe
```

Exécution :

```powershell
.\test_banque.exe
```

**Résultat précédemment validé : 13 tests réussis.**

### 3. Tests des identifiants automatiques

Compilation :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_identifiants.cpp src/Banque.cpp src/Compte.cpp -o test_identifiants.exe
```

Exécution :

```powershell
.\test_identifiants.exe
```

**Résultat précédemment validé : 12 tests réussis.**

### 4. Tests du module Montant

Compilation :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_montant.cpp src/Montant.cpp -o test_montant.exe
```

Exécution :

```powershell
.\test_montant.exe
```

**Résultat précédemment validé : 22 tests réussis.**

### 5. Tests du parcours bancaire complet

Le programme `test_parcours.cpp` vérifie le fonctionnement combiné des classes `Banque`, `Compte` et du module `Montant`.

Compilation :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_parcours.cpp src/Banque.cpp src/Compte.cpp src/Montant.cpp -o test_parcours.exe
```

Exécution :

```powershell
.\test_parcours.exe
```

**Dernier résultat validé :**

```text
=== SOLDES FINAUX ===
Alice : 551,00 EUR
Bob   : 1200,00 EUR

=== BILAN DES TESTS ===
Tests reussis : 33
Tests echoues : 0
```

Ces tests vérifient notamment :

- La création de plusieurs comptes.
- La conversion des soldes initiaux.
- Les dépôts et retraits successifs.
- L'indépendance des comptes.
- Le refus des retraits excessifs.
- Le refus des opérations nulles ou négatives.
- La conservation des soldes après un échec.
- Les identifiants inexistants.
- La consultation en lecture seule.
- Les limites numériques.

### Bilan global des tests

| Ensemble | Tests réussis |
|---|---:|
| Classe `Compte` | 6 |
| Classe `Banque` | 13 |
| Identifiants automatiques | 12 |
| Module `Montant` | 22 |
| Parcours bancaire complet | 33 |
| **Total** | **86** |

Les **86 vérifications ont été validées au fil des étapes du projet**, lors de différentes exécutions.

Elles ne constituent pas une garantie de correction exhaustive ni une preuve que toutes les fonctionnalités ont été testées automatiquement dans le terminal.

Le menu a également été vérifié manuellement pour les opérations de création, consultation, dépôt et retrait.

---

## 🔐 Règles de gestion

MiniBank applique actuellement plusieurs règles :

- Un compte possède un identifiant et un titulaire non vides.
- Le menu refuse les titulaires constitués uniquement d'espaces.
- Un solde initial ne peut pas être négatif.
- Un solde initial de zéro est autorisé.
- Les dépôts et retraits doivent être strictement positifs.
- Un retrait ne peut pas dépasser le solde disponible.
- Un dépôt ne doit pas provoquer de dépassement numérique.
- Une opération refusée laisse le solde inchangé.
- Deux comptes d'une même banque ne peuvent pas partager le même identifiant.
- Les montants sont représentés en centimes entiers avec `std::int64_t`.
- La consultation ne modifie pas les comptes.

Des contrôles supplémentaires et de nouvelles règles seront ajoutés au fur et à mesure du développement.

---

## 🗺️ Feuille de route

### Étapes terminées

- [x] Initialiser le projet C++17.
- [x] Configurer Git et GitHub.
- [x] Créer le menu principal.
- [x] Développer la classe `Compte`.
- [x] Implémenter les dépôts et retraits dans `Compte`.
- [x] Tester la classe `Compte`.
- [x] Développer la classe `Banque`.
- [x] Tester la gestion de plusieurs comptes.
- [x] Ajouter les identifiants automatiques.
- [x] Tester la génération des identifiants.
- [x] Développer et tester le module `Montant`.
- [x] Connecter la création des comptes au menu.
- [x] Connecter la consultation des comptes.
- [x] Connecter les dépôts au menu.
- [x] Connecter les retraits au menu.
- [x] Réaliser les 33 tests du parcours bancaire.
- [x] Vérifier manuellement le parcours Alice/Bob.

### Étapes à venir

- [ ] Implémenter les virements entre comptes.
- [ ] Tester les virements et leurs cas d'échec.
- [ ] Connecter les virements au menu.
- [ ] Ajouter l'historique des opérations.
- [ ] Afficher tous les comptes.
- [ ] Sauvegarder les données fictives en CSV.
- [ ] Charger les données depuis un fichier CSV.
- [ ] Ajouter des tests complémentaires.
- [ ] Améliorer l'architecture et la documentation.
- [ ] Préparer une première version stable.

---

## 💾 Sauvegarde des données

**La sauvegarde des comptes n'est pas encore implémentée.**

Les comptes existent uniquement pendant l'exécution du programme.

Le dossier `data/` est réservé aux futures données fictives locales.

Les fichiers générés dans ce dossier seront exclus du dépôt Git, à l'exception de `.gitkeep`, conformément à la configuration prévue de `.gitignore`.

La sauvegarde, le chargement des comptes et la restauration des identifiants feront l'objet d'étapes ultérieures.

---

## 🌐 Git et GitHub

Le code est versionné avec Git et publié sur GitHub.

GitHub Desktop est utilisé pour enregistrer les modifications à l'aide de commits et pour les synchroniser avec le dépôt distant.

Les exécutables, les fichiers objets et les données locales sont exclus du versionnement par `.gitignore`.

**Dépôt GitHub :** https://github.com/datasifaw/MiniBank_Cpp

---

## 📄 Licence

Aucune licence open source n'a encore été définie pour ce projet.

Le fait qu'un dépôt GitHub soit public ne signifie pas automatiquement que son contenu peut être librement réutilisé ou redistribué.

---

## 📌 État actuel du développement

**Dernière étape validée : 3.5.5 — Tests du parcours bancaire complet.**

Les options `1`, `2`, `3`, `4` et `0` du menu fonctionnent.

**86 tests ont été réussis au cours du développement**, dont 33 tests d'intégration du parcours bancaire.

**Prochaine étape envisagée : 3.6.1 — Implémentation de la logique des virements dans la classe `Banque`.**

Le projet reste une simulation pédagogique en cours de développement.