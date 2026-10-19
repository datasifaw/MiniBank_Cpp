# 🏦 MiniBank C++

**MiniBank C++** est une application de simulation bancaire fictive développée en **C++17**, avec une interface interactive en ligne de commande.

Ce projet pédagogique permet de découvrir progressivement la programmation orientée objet, la gestion de comptes bancaires, les validations numériques, les tests logiciels et le versionnement avec Git et GitHub.

**État actuel :** cinq opérations bancaires fonctionnelles, six programmes de tests et **119 vérifications réussies**.

> ⚠️ **Avertissement :** MiniBank est un projet éducatif. Il ne gère aucun argent réel, ne communique avec aucun établissement bancaire et n'est pas conçu pour un environnement de production.

---

## 🎯 Objectifs pédagogiques

- Apprendre les fondamentaux de C++17.
- Comprendre les classes, les objets et l'encapsulation.
- Organiser un projet en fichiers `.h` et `.cpp`.
- Utiliser `std::vector` pour gérer plusieurs comptes.
- Comprendre les pointeurs et les méthodes `const`.
- Construire une interface interactive dans le terminal.
- Vérifier les saisies et les règles de gestion.
- Manipuler les montants monétaires avec des entiers.
- Implémenter des transferts entre deux objets.
- Comprendre l'atomicité logique des opérations.
- Créer des tests unitaires et des tests d'intégration.
- Suivre les changements avec Git et GitHub Desktop.

---

## ✅ Fonctionnalités disponibles

### 1. Créer un compte bancaire

L'option `1` permet de créer un compte fictif en saisissant le nom du titulaire et son solde initial en euros.

La classe `Banque` attribue automatiquement un identifiant.

**Exemple :**

```text
Votre choix : 1

Nom du titulaire : Alice Martin
Solde initial (EUR) : 551

Compte cree avec succes !

Identifiant : MB-0001
Titulaire   : Alice Martin
Solde       : 551,00 EUR
```

**Règles :**

- Le titulaire ne doit pas être vide.
- Le menu refuse un nom constitué uniquement d'espaces.
- Le solde initial ne doit pas être négatif.
- Un solde initial de zéro est autorisé.
- L'identifiant est généré automatiquement.

### 2. Consulter un compte

L'option `2` permet de retrouver un compte grâce à son identifiant.

**Exemple :**

```text
Votre choix : 2

Identifiant du compte : MB-0001

Compte trouve !

Identifiant : MB-0001
Titulaire   : Alice Martin
Solde       : 551,00 EUR
```

Si le compte n'existe pas, un message d'erreur est affiché.

La consultation utilise un accès en lecture seule afin de ne pas modifier les informations du compte.

### 3. Déposer de l'argent

L'option `3` permet d'ajouter un montant à un compte existant.

Le programme convertit le montant saisi en centimes, puis utilise `Compte::deposer()`.

**Exemple :**

```text
Votre choix : 3

Identifiant du compte : MB-0001
Montant du depot (EUR) : 100,50

Depot effectue avec succes !

Ancien solde   : 551,00 EUR
Montant depose : 100,50 EUR
Nouveau solde  : 651,50 EUR
```

**Règles :**

- Le compte doit exister.
- Le montant doit être valide et strictement positif.
- Le dépôt ne doit pas provoquer de dépassement numérique.
- Si l'opération échoue, le solde reste inchangé.

### 4. Retirer de l'argent

L'option `4` permet de retirer une somme d'un compte existant.

Le programme utilise `Compte::retirer()`.

**Exemple :**

```text
Votre choix : 4

Identifiant du compte : MB-0001
Montant du retrait (EUR) : 50

Retrait effectue avec succes !

Ancien solde   : 651,50 EUR
Montant retire : 50,00 EUR
Nouveau solde  : 601,50 EUR
```

**Règles :**

- Le compte doit exister.
- Le montant doit être strictement positif.
- Le solde doit être suffisant.
- Le retrait ne doit pas rendre le solde négatif.
- Un retrait refusé ne modifie pas le solde.

### 5. Effectuer un virement bancaire

L'option `5` permet de transférer un montant d'un compte source vers un compte destinataire.

Le menu recueille les identifiants des deux comptes et le montant en euros, puis appelle `Banque::virer()`.

**Exemple :**

Alice possède initialement **551,00 EUR** et Bob **1 200,00 EUR**.

```text
Votre choix : 5

========================================
           VIREMENT BANCAIRE
========================================

Compte source       : MB-0001
Compte destinataire : MB-0002
Montant (EUR)       : 100

Virement effectue avec succes !

Source       : Alice Martin (MB-0001)
Destinataire : Bob Dupont (MB-0002)
Montant      : 100,00 EUR

Solde source avant       : 551,00 EUR
Solde source apres       : 451,00 EUR
Solde destinataire avant : 1200,00 EUR
Solde destinataire apres : 1300,00 EUR
```

**Résultat :**

| Compte | Avant | Après |
|---|---:|---:|
| Alice | 551,00 EUR | 451,00 EUR |
| Bob | 1 200,00 EUR | 1 300,00 EUR |
| **Total** | **1 751,00 EUR** | **1 751,00 EUR** |

Le montant total des deux comptes reste identique.

#### Règles des virements

Un virement est refusé si :

- Le montant est nul ou négatif.
- Le compte source n'existe pas.
- Le compte destinataire n'existe pas.
- Les identifiants source et destinataire sont identiques.
- Un identifiant est vide.
- Le solde source est insuffisant.
- Le crédit provoquerait un dépassement numérique chez le destinataire.

En cas de refus, les deux soldes doivent rester inchangés.

#### Atomicité logique

MiniBank vérifie les conditions nécessaires avant de transférer les fonds.

Ainsi, les scénarios de refus testés ne produisent pas de transfert partiel.

**Limite importante :** cette atomicité concerne la simulation actuelle en mémoire, dans une exécution normale et sans accès concurrent. Elle ne constitue pas une transaction bancaire durable et ne garantit pas la récupération après un arrêt brutal du programme.

---

## 🔢 Identifiants automatiques

La classe `Banque` génère les identifiants au format :

```text
MB-0001
MB-0002
MB-0003
...
```

La génération vérifie qu'un identifiant n'est pas déjà utilisé dans la banque courante.

Les identifiants sont uniques parmi les comptes présents au sein de l'objet `Banque`.

La conservation de ces identifiants entre deux exécutions n'est pas encore implémentée.

---

## 💶 Gestion des montants

MiniBank stocke les montants en **centimes entiers** grâce au type `std::int64_t`.

Cette approche évite les problèmes d'arrondi habituels des types à virgule flottante pour les opérations actuellement implémentées.

Le module `Montant` fournit deux fonctions :

```cpp
Montant::convertirEnCentimes(...)
Montant::formater(...)
```

### Exemples de conversion

| Saisie utilisateur | Valeur interne |
|---|---:|
| `100` | 10000 centimes |
| `25.50` | 2550 centimes |
| `25,50` | 2550 centimes |
| `0.99` | 99 centimes |
| `0` | 0 centime |

La conversion accepte un séparateur décimal `.` ou `,` et au maximum deux chiffres après le séparateur.

Elle refuse les textes incorrects, les nombres négatifs saisis, les valeurs mal formées et les dépassements numériques.

Les opérations de dépôt, retrait et virement exigent ensuite un montant strictement supérieur à zéro.

---

## 📋 Menu principal

| Option | Description | Statut |
|---|---|---|
| `1` | Créer un compte | ✅ Fonctionnelle |
| `2` | Consulter un compte | ✅ Fonctionnelle |
| `3` | Déposer de l'argent | ✅ Fonctionnelle |
| `4` | Retirer de l'argent | ✅ Fonctionnelle |
| `5` | Effectuer un virement | ✅ Fonctionnelle |
| `6` | Consulter l'historique | ⏳ À développer |
| `7` | Afficher tous les comptes | ⏳ À développer |
| `8` | Sauvegarder les données | ⏳ À développer |
| `0` | Quitter MiniBank | ✅ Fonctionnelle |

**Important :** les comptes et leurs soldes sont conservés uniquement en mémoire pendant l'exécution.

Les opérations ne sont pas encore enregistrées dans un historique permanent.

---

## 🛠️ Technologies utilisées

| Technologie | Rôle |
|---|---|
| C++17 | Langage de programmation |
| GCC / G++ | Compilateur |
| MSYS2 | Environnement de compilation Windows |
| Visual Studio Code | Éditeur |
| PowerShell | Terminal |
| Git | Gestion des versions |
| GitHub Desktop | Gestion graphique des commits |
| GitHub | Hébergement du code et de la documentation |

MiniBank utilise actuellement uniquement la bibliothèque standard C++.

Aucune bibliothèque externe n'est nécessaire.

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
│   ├── test_parcours.cpp
│   └── test_virement.cpp
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
| `src/Compte.cpp` | Gestion du solde, dépôts et retraits |
| `include/Banque.h` | Déclaration de la classe `Banque` |
| `src/Banque.cpp` | Création, recherche, identifiants et virements |
| `include/Montant.h` | Déclaration des fonctions monétaires |
| `src/Montant.cpp` | Conversion et formatage des montants |
| `src/main.cpp` | Menu interactif et gestion des saisies |
| `tests/test_compte.cpp` | Tests de la classe `Compte` |
| `tests/test_banque.cpp` | Tests de la classe `Banque` |
| `tests/test_identifiants.cpp` | Tests des identifiants |
| `tests/test_montant.cpp` | Tests monétaires |
| `tests/test_parcours.cpp` | Tests du parcours bancaire complet |
| `tests/test_virement.cpp` | Tests des virements entre comptes |
| `data/` | Dossier réservé au futur stockage local |

---

## 🧱 Architecture orientée objet

MiniBank sépare l'interface utilisateur, la gestion des comptes et la manipulation des montants.

```text
             UTILISATEUR
                  |
                  v
              main.cpp
                  |
       +----------+----------+
       |                     |
       v                     v
    Montant                Banque
 Conversion /                |
  formatage                  |
                     +-------+-------+
                     |               |
                     v               v
                 Compte Alice    Compte Bob
                     |               ^
                     |               |
                     +--- VIREMENT --+
```

### Classe `Compte`

Elle représente un compte bancaire fictif et contient notamment :

- Son identifiant.
- Le nom du titulaire.
- Son solde en centimes.

Méthodes principales :

```cpp
getIdentifiant()
getTitulaire()
getSoldeCentimes()
deposer(...)
retirer(...)
```

### Classe `Banque`

Elle conserve les comptes dans un `std::vector<Compte>`.

Méthodes principales :

```cpp
ajouterCompte(...)
creerCompte(...)
rechercherCompte(...)
nombreComptes()
virer(...)
```

La méthode `virer()` centralise les règles des transferts entre comptes.

### Module `Montant`

Il convertit les euros saisis par l'utilisateur en centimes entiers et assure leur affichage.

### Fichier `main.cpp`

Il affiche le menu, reçoit les saisies et appelle les classes adaptées.

Le menu ne retire ni ne dépose directement l'argent lors d'un virement : il délègue cette responsabilité à `Banque::virer()`.

---

## ▶️ Compilation et exécution

### Prérequis

- Windows.
- Un compilateur G++ compatible C++17.
- MSYS2 configuré.
- PowerShell ou un terminal compatible.

Vérifier le compilateur :

```powershell
g++ --version
```

### Compiler MiniBank

Depuis la racine du projet :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude src/main.cpp src/Banque.cpp src/Compte.cpp src/Montant.cpp -o MiniBank.exe
```

### Lancer le programme

```powershell
.\MiniBank.exe
```

Le menu affiche :

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

Les options `1` à `5` sont opérationnelles.

Les options `6` à `8` affichent pour le moment :

```text
Fonctionnalite a venir !
```

---

## 🧪 Tests

MiniBank dispose actuellement de **six programmes de tests indépendants**.

Ils ont tous été recompilés et exécutés avec succès lors de l'étape **3.6.4.1**.

### Test 1 — Classe Compte

Compilation :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_compte.cpp src/Compte.cpp -o test_compte.exe
```

Exécution :

```powershell
.\test_compte.exe
```

**Résultat validé : 6 tests réussis, 0 échec.**

### Test 2 — Classe Banque

Compilation :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_banque.cpp src/Banque.cpp src/Compte.cpp -o test_banque.exe
```

Exécution :

```powershell
.\test_banque.exe
```

**Résultat validé : 13 tests réussis, 0 échec.**

### Test 3 — Identifiants automatiques

Compilation :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_identifiants.cpp src/Banque.cpp src/Compte.cpp -o test_identifiants.exe
```

Exécution :

```powershell
.\test_identifiants.exe
```

**Résultat validé : 12 tests réussis, 0 échec.**

### Test 4 — Module Montant

Compilation :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_montant.cpp src/Montant.cpp -o test_montant.exe
```

Exécution :

```powershell
.\test_montant.exe
```

**Résultat validé : 22 tests réussis, 0 échec.**

### Test 5 — Parcours bancaire complet

Compilation :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_parcours.cpp src/Banque.cpp src/Compte.cpp src/Montant.cpp -o test_parcours.exe
```

Exécution :

```powershell
.\test_parcours.exe
```

**Résultat validé :**

```text
=== SOLDES FINAUX ===
Alice : 551,00 EUR
Bob   : 1200,00 EUR

=== BILAN DES TESTS ===
Tests reussis : 33
Tests echoues : 0
```

Ce programme vérifie notamment la création des comptes, la consultation, les dépôts, les retraits, les erreurs de saisie et la conservation des soldes après une opération refusée.

### Test 6 — Virements bancaires

Le fichier `tests/test_virement.cpp` vérifie directement la méthode `Banque::virer()`.

Compilation :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_virement.cpp src/Banque.cpp src/Compte.cpp -o test_virement.exe
```

Exécution :

```powershell
.\test_virement.exe
```

**Résultat validé :**

```text
=== SOLDES FINAUX ===
Alice : 0 centimes
Bob   : 175100 centimes

=== BILAN DES TESTS ===
Tests reussis : 33
Tests echoues : 0
```

Les 33 vérifications couvrent notamment :

- La création des comptes source et destinataire.
- Le virement réussi d'Alice vers Bob.
- La diminution du solde source.
- L'augmentation du solde destinataire.
- La conservation du total des soldes.
- Le refus des montants nuls ou négatifs.
- Le refus d'un solde insuffisant.
- Le refus des identifiants inexistants.
- Le refus d'un transfert vers le même compte.
- Le refus d'un dépassement numérique.
- La conservation des deux soldes après un refus.
- Un virement dans le sens inverse.
- Le transfert de la totalité du solde.

### Bilan général des tests

| Programme | Vérifications réussies | Échecs |
|---|---:|---:|
| `test_compte.cpp` | 6 | 0 |
| `test_banque.cpp` | 13 | 0 |
| `test_identifiants.cpp` | 12 | 0 |
| `test_montant.cpp` | 22 | 0 |
| `test_parcours.cpp` | 33 | 0 |
| `test_virement.cpp` | 33 | 0 |
| **Total** | **119** | **0** |

**Dernière vérification générale : étape 3.6.4.1, réussie.**

Les 119 vérifications ont été exécutées à l'aide de six programmes distincts, et non par une commande unique ou un système d'intégration continue.

Le fonctionnement des virements a également été vérifié manuellement via l'option `5` du menu.

Ces résultats montrent que les scénarios couverts par les tests réussissent. Ils ne garantissent pas l'absence de tous les défauts possibles.

---

## 🔐 Règles de gestion

MiniBank applique les règles suivantes :

- Chaque compte possède un identifiant non vide et un titulaire.
- Le menu refuse les noms de titulaires composés uniquement d'espaces.
- Un solde initial doit être positif ou nul.
- Un dépôt doit être strictement positif.
- Un retrait doit être strictement positif.
- Un retrait ne peut pas dépasser le solde disponible.
- Un dépôt doit respecter les limites numériques.
- Les deux comptes d'un virement doivent exister.
- La source et la destination d'un virement doivent être différentes.
- Le montant du virement doit être strictement positif.
- La source doit posséder les fonds nécessaires.
- La destination doit pouvoir recevoir le montant sans dépassement numérique.
- Les opérations refusées ne doivent pas modifier les soldes.
- Le total des deux comptes impliqués est conservé après un virement réussi.
- Les montants sont stockés en centimes entiers avec `std::int64_t`.

---

## 🗺️ Feuille de route

### Étapes terminées

- [x] Initialiser le projet C++17.
- [x] Configurer Git et GitHub.
- [x] Créer le menu principal.
- [x] Développer la classe `Compte`.
- [x] Implémenter les dépôts et les retraits.
- [x] Tester la classe `Compte`.
- [x] Développer la classe `Banque`.
- [x] Tester la gestion des comptes.
- [x] Générer les identifiants automatiquement.
- [x] Tester les identifiants.
- [x] Développer le module `Montant`.
- [x] Tester les conversions monétaires.
- [x] Activer la création de comptes dans le menu.
- [x] Activer la consultation des comptes.
- [x] Activer les dépôts.
- [x] Activer les retraits.
- [x] Valider les tests du parcours bancaire.
- [x] Implémenter `Banque::virer()`.
- [x] Valider les 33 tests de virements.
- [x] Activer l'option `5` du menu.
- [x] Tester manuellement un virement entre Alice et Bob.
- [x] Réussir la vérification générale des 119 tests.

### Prochaines étapes

- [ ] Publier la version documentée des virements sur GitHub.
- [ ] Développer l'historique des opérations bancaires.
- [ ] Connecter l'historique à l'option `6`.
- [ ] Afficher tous les comptes dans l'option `7`.
- [ ] Sauvegarder les données fictives dans des fichiers CSV.
- [ ] Charger les données au démarrage.
- [ ] Ajouter des tests pour les nouvelles fonctionnalités.
- [ ] Améliorer progressivement la robustesse de l'application.
- [ ] Préparer une première version stable.

---

## 💾 Sauvegarde et persistance

**MiniBank ne sauvegarde pas encore les comptes.**

Les comptes, les identifiants générés et les soldes existent uniquement durant l'exécution du programme.

Lorsque le programme est fermé puis relancé, une nouvelle banque vide est créée.

Le dossier `data/` est réservé à une future fonctionnalité de sauvegarde locale des données fictives.

L'enregistrement CSV et le chargement des comptes feront l'objet d'étapes ultérieures.

---

## 🌐 Git et GitHub

Le projet utilise Git pour conserver l'historique des changements et GitHub pour publier le code.

Les commits et les push peuvent être effectués avec GitHub Desktop.

Le fichier `.gitignore` exclut les exécutables, les fichiers objets, les dossiers de compilation temporaires et les futures données locales.

**Dépôt GitHub :**

https://github.com/datasifaw/MiniBank_Cpp

---

## 📄 Licence

Aucune licence open source n'a encore été définie pour ce projet.

Un dépôt public sur GitHub n'accorde pas automatiquement le droit de réutiliser, modifier ou redistribuer son contenu.

---

## 📌 État actuel du projet

**Dernière étape technique validée : 3.6.4.1 — Vérification générale des 119 tests.**

MiniBank C++ possède cinq opérations bancaires fonctionnelles : création, consultation, dépôt, retrait et virement.

Les six programmes de tests ont été recompilés et exécutés avec succès :

**119 tests réussis — 0 échec.**

**Étape documentaire en cours : 3.6.4.2 — Actualisation du README.**

**Étape suivante prévue : 3.6.4.3 — Commit et push GitHub des virements et de la documentation.**

Ensuite, le développement pourra se poursuivre avec l'historique des opérations bancaires.

MiniBank C++ reste un projet pédagogique en cours de développement.