# MiniBank C++ 🏦

**MiniBank C++** est un simulateur bancaire pédagogique développé en **C++17** et utilisable depuis un terminal.

Le projet permet de pratiquer la programmation orientée objet, les collections de la bibliothèque standard C++, la validation des montants monétaires, les opérations bancaires, les tests automatisés et le versionnement avec Git et GitHub.

La version actuelle propose **sept fonctionnalités bancaires actives**, de la création de comptes à la consultation de l'historique et à l'affichage de tous les comptes.

**Dernier bilan validé : 219 tests réussis, 0 échec, répartis dans huit programmes.**

## 1. Fonctionnalités

| Option | Fonctionnalité | État |
|---|---|---|
| `1` | Créer un compte bancaire | Fonctionnelle |
| `2` | Consulter un compte bancaire | Fonctionnelle |
| `3` | Déposer de l'argent | Fonctionnelle |
| `4` | Retirer de l'argent | Fonctionnelle |
| `5` | Effectuer un virement entre comptes | Fonctionnelle |
| `6` | Consulter l'historique d'un compte | Fonctionnelle |
| `7` | Afficher tous les comptes bancaires | Fonctionnelle |
| `8` | Sauvegarder les données | À développer |
| `0` | Quitter MiniBank | Fonctionnelle |

### Option 1 — Créer un compte

L'utilisateur saisit le nom du titulaire et un solde initial exprimé en euros.

MiniBank attribue automatiquement un identifiant tel que `MB-0001`, `MB-0002`, etc.

Chaque compte possède :

- Un identifiant unique.
- Un titulaire.
- Un solde enregistré en centimes entiers.

Un compte nouvellement créé ne génère pas de ligne de dépôt dans l'historique, même lorsque son solde initial est positif.

### Option 2 — Consulter un compte

La consultation permet de retrouver un compte grâce à son identifiant et d'afficher ses informations :

- Identifiant.
- Titulaire.
- Solde actuel.

Un identifiant inexistant provoque un message d'erreur.

### Option 3 — Déposer de l'argent

L'utilisateur saisit un identifiant et un montant.

Le dépôt doit être strictement positif et ne doit pas provoquer de dépassement numérique du solde.

Un dépôt réussi augmente le solde et ajoute une ligne `DEPOT` à l'historique bancaire.

### Option 4 — Retirer de l'argent

L'utilisateur saisit un identifiant et un montant.

Le retrait doit être strictement positif et ne peut pas dépasser le solde disponible.

Un retrait réussi diminue le solde et ajoute une ligne `RETRAIT` à l'historique bancaire.

### Option 5 — Effectuer un virement

Le virement transfère un montant d'un compte source vers un compte destinataire.

MiniBank vérifie notamment :

- L'existence des deux comptes.
- La différence entre leurs identifiants.
- La validité et la positivité du montant.
- Le solde disponible sur le compte source.
- La capacité du compte destinataire à recevoir le montant sans dépassement numérique.

Un virement réussi crée **deux lignes d'historique** :

- `VIREMENT SORTANT` pour le compte source.
- `VIREMENT ENTRANT` pour le compte destinataire.

Il s'agit de deux écritures représentant un seul transfert d'argent.

Un virement refusé ne doit modifier ni les soldes ni l'historique.

### Option 6 — Consulter l'historique

L'utilisateur saisit l'identifiant du compte à consulter.

MiniBank affiche les mouvements enregistrés pour ce compte, dans leur ordre chronologique d'enregistrement, avec un numéro d'opération.

Le détail du fonctionnement de l'historique est présenté dans la section 3.

### Option 7 — Afficher tous les comptes

Cette fonctionnalité affiche l'ensemble des comptes enregistrés dans la banque, **sans avoir à saisir leurs identifiants individuellement**.

Pour chaque compte, MiniBank affiche :

- Un numéro d'affichage.
- L'identifiant bancaire.
- Le nom du titulaire.
- Le solde actuel, formaté en euros.

Les comptes sont affichés dans leur **ordre de création**.

La consultation est en lecture seule : elle ne modifie ni les soldes, ni les comptes internes, ni l'historique bancaire.

## 2. Liste de tous les comptes

### Exemple d'utilisation de l'option 7

Supposons que les comptes suivants aient été créés :

| Identifiant | Titulaire | Solde initial |
|---|---|---:|
| `MB-0001` | Alice Martin | 500,00 EUR |
| `MB-0002` | Bob Dupont | 1200,00 EUR |

Après un dépôt de 100 EUR sur le compte d'Alice, un retrait de 50 EUR et un virement de 25 EUR d'Alice vers Bob, l'option `7` affiche :

```text
Votre choix : 7

========================================
          LISTE DES COMPTES
========================================

Compte n. 1
Identifiant : MB-0001
Titulaire   : Alice Martin
Solde       : 525,00 EUR

----------------------------------------

Compte n. 2
Identifiant : MB-0002
Titulaire   : Bob Dupont
Solde       : 1225,00 EUR

----------------------------------------

Nombre total de comptes : 2
```

### Consultation d'une banque vide

Si aucun compte n'a été créé, l'option `7` affiche :

```text
========================================
          LISTE DES COMPTES
========================================

Aucun compte bancaire enregistre.

Nombre total de comptes : 0
```

Le programme retourne ensuite au menu principal.

### Fonctionnement technique

La classe `Banque` expose la méthode suivante :

```cpp
std::vector<Compte> consulterTousLesComptes() const;
```

Cette méthode retourne une **copie du vecteur de comptes** stocké dans la banque.

Ses principales propriétés sont :

1. **Ordre conservé :** les comptes restent dans leur ordre d'insertion.
2. **Copie indépendante :** modifier un compte dans la liste retournée ne modifie pas le compte original.
3. **Lecture seule :** la méthode ne change pas l'état de la banque.
4. **Banque vide :** la méthode retourne un vecteur vide.
5. **Soldes actualisés :** une nouvelle consultation reflète les dernières opérations effectuées.

Une copie obtenue avant une opération bancaire ne se met pas automatiquement à jour. Il faut appeler de nouveau `consulterTousLesComptes()` pour obtenir les soldes les plus récents.

Le menu utilise `Montant::formater()` pour afficher les soldes de manière lisible.

## 3. Historique des opérations bancaires

MiniBank conserve les mouvements bancaires dans un historique en mémoire.

Chaque mouvement enregistré possède :

- Un numéro d'ordre.
- Un type d'opération.
- L'identifiant du compte concerné.
- Un montant exprimé en centimes entiers.
- L'identifiant de l'autre compte lorsqu'il s'agit d'un virement.

### Types d'opérations

| Type C++ | Affichage | Nature |
|---|---|---|
| `TypeOperation::Depot` | `DEPOT` | Entrée d'argent |
| `TypeOperation::Retrait` | `RETRAIT` | Sortie d'argent |
| `TypeOperation::VirementSortant` | `VIREMENT SORTANT` | Argent envoyé |
| `TypeOperation::VirementEntrant` | `VIREMENT ENTRANT` | Argent reçu |

Le montant est stocké sous forme positive. Le signe `+` ou `-` est déterminé lors de l'affichage selon le type d'opération.

### Exemple d'historique d'Alice

```text
Votre choix : 6

========================================
       HISTORIQUE DES OPERATIONS
========================================

Identifiant du compte : MB-0001

Titulaire : Alice Martin
Compte    : MB-0001

----------------------------
Operation n. 1
Type     : DEPOT
Montant  : +100,00 EUR

----------------------------
Operation n. 2
Type     : RETRAIT
Montant  : -50,00 EUR

----------------------------
Operation n. 3
Type     : VIREMENT SORTANT
Vers     : MB-0002
Montant  : -25,00 EUR

----------------------------
Nombre d'operations : 3
```

### Exemple d'historique de Bob

```text
Titulaire : Bob Dupont
Compte    : MB-0002

----------------------------
Operation n. 4
Type     : VIREMENT ENTRANT
Depuis   : MB-0001
Montant  : +25,00 EUR

----------------------------
Nombre d'operations : 1
```

### Règles de l'historique

- Seules les opérations réussies sont enregistrées.
- Les opérations refusées n'ajoutent aucune ligne d'historique.
- Un virement réussi crée deux lignes portant des numéros distincts.
- Les numéros sont attribués dans l'ordre global d'enregistrement.
- La création d'un compte avec un solde initial ne crée pas de dépôt.
- La consultation de l'historique ne modifie pas les soldes.
- La consultation de la liste des comptes ne modifie pas non plus l'historique.
- Un compte sans mouvement possède un historique vide.

**Important : l'historique est uniquement conservé en mémoire et disparaît lorsque le programme se termine.**

## 4. Gestion des montants monétaires

MiniBank représente les soldes et les montants avec le type `std::int64_t`, exprimé en centimes entiers.

Cette approche évite les imprécisions habituelles des nombres à virgule flottante pour les montants monétaires.

| Montant affiché | Valeur interne |
|---|---:|
| `10,00 EUR` | `1000` centimes |
| `25,50 EUR` | `2550` centimes |
| `100,00 EUR` | `10000` centimes |
| `525,00 EUR` | `52500` centimes |

Le module `Montant` gère la conversion des saisies et le formatage des valeurs.

Exemples de saisies acceptées :

```text
100
100.50
100,50
25.5
```

Les formats invalides sont refusés. Les règles métier vérifient également la positivité des dépôts, retraits et virements.

## 5. Technologies utilisées

- **Langage :** C++17.
- **Compilateur :** G++ avec MSYS2 sous Windows.
- **Éditeur :** Visual Studio Code.
- **Terminal :** PowerShell.
- **Versionnement :** Git.
- **Application Git :** GitHub Desktop.
- **Dépôt distant :** GitHub.

Les fonctionnalités présentées utilisent la bibliothèque standard C++.

## 6. Organisation du projet

```text
MiniBank_Cpp/
|
|-- include/
|   |-- Banque.h
|   |-- Compte.h
|   |-- Montant.h
|   `-- Operation.h
|
|-- src/
|   |-- Banque.cpp
|   |-- Compte.cpp
|   |-- Montant.cpp
|   `-- main.cpp
|
|-- tests/
|   |-- test_compte.cpp
|   |-- test_banque.cpp
|   |-- test_identifiants.cpp
|   |-- test_montant.cpp
|   |-- test_parcours.cpp
|   |-- test_virement.cpp
|   |-- test_historique.cpp
|   `-- test_liste_comptes.cpp
|
|-- data/
|   `-- .gitkeep
|
|-- .gitattributes
|-- .gitignore
`-- README.md
```

Les fichiers exécutables `.exe` sont générés lors des compilations, mais ne doivent pas être publiés dans le dépôt Git.

### Rôle des modules

| Fichier | Responsabilité |
|---|---|
| `include/Compte.h` et `src/Compte.cpp` | Modéliser un compte et les opérations élémentaires sur son solde |
| `include/Banque.h` et `src/Banque.cpp` | Gérer les comptes, les identifiants, les mouvements bancaires, l'historique et la consultation de tous les comptes |
| `include/Montant.h` et `src/Montant.cpp` | Convertir et formater les montants monétaires |
| `include/Operation.h` | Définir les types et les informations des opérations |
| `src/main.cpp` | Proposer le menu interactif et gérer les saisies de l'utilisateur |
| `tests/` | Contenir les huit programmes de tests automatisés |

### Méthodes principales de Banque

La classe `Banque` expose notamment :

```cpp
std::string creerCompte(
    const std::string& titulaire,
    std::int64_t soldeInitialCentimes
);

std::size_t nombreComptes() const;

std::vector<Compte> consulterTousLesComptes() const;

bool deposerSurCompte(
    const std::string& idCompte,
    std::int64_t montantCentimes
);

bool retirerDuCompte(
    const std::string& idCompte,
    std::int64_t montantCentimes
);

bool virer(
    const std::string& idSource,
    const std::string& idDestination,
    std::int64_t montantCentimes
);

std::vector<Operation> consulterHistorique(
    const std::string& idCompte
) const;

std::size_t nombreOperations() const;
```

Les options `3`, `4` et `5` passent par les méthodes de `Banque` qui enregistrent les mouvements réussis.

L'option `6` consulte l'historique.

L'option `7` obtient une copie de la liste des comptes et affiche leur état actuel, sans effectuer de transaction.

## 7. Compiler et exécuter MiniBank sous Windows

### Prérequis

Installer et configurer :

- Visual Studio Code.
- Un compilateur G++ prenant en charge C++17, par exemple via MSYS2.
- PowerShell.

Ouvrir le dossier `MiniBank_Cpp` dans Visual Studio Code, puis sélectionner **Terminal → Nouveau terminal**.

### Se placer dans le projet

Adapter le chemin si le dossier se trouve à un autre emplacement :

```powershell
cd "C:\Users\saide\Desktop\ISOSET\SaZ-Projet_Cpp\MiniBank_Cpp"
```

### Compiler le programme

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude src/main.cpp src/Banque.cpp src/Compte.cpp src/Montant.cpp -o MiniBank.exe
```

### Exécuter MiniBank

```powershell
.\MiniBank.exe
```

Le menu interactif s'affiche dans le terminal.

Les fichiers d'en-tête `.h` sont inclus par le compilateur via les fichiers sources. Il n'est pas nécessaire de compiler séparément `Operation.h`.

## 8. Exemple de parcours bancaire complet

Dans **une seule exécution** de MiniBank :

1. Choisir l'option `7` pour vérifier que la banque est vide.
2. Créer Alice Martin avec un solde initial de `500` EUR : identifiant `MB-0001`.
3. Créer Bob Dupont avec un solde initial de `1200` EUR : identifiant `MB-0002`.
4. Choisir l'option `7` pour afficher les deux comptes.
5. Déposer `100` EUR sur `MB-0001`.
6. Retirer `50` EUR de `MB-0001`.
7. Virer `25` EUR de `MB-0001` vers `MB-0002`.
8. Choisir l'option `7` pour afficher les soldes actualisés.
9. Choisir l'option `6` pour consulter les historiques d'Alice et de Bob.
10. Choisir l'option `0` pour quitter MiniBank.

### Résultats attendus

| Compte | Solde final | Opérations enregistrées |
|---|---:|---:|
| Alice (`MB-0001`) | `525,00 EUR` | 3 |
| Bob (`MB-0002`) | `1225,00 EUR` | 1 |

Le virement crée deux lignes d'historique, une sur chaque compte.

La somme des soldes finaux est de `1750,00 EUR`, après un dépôt externe de 100 EUR et un retrait externe de 50 EUR.

## 9. Tests automatisés

MiniBank dispose de **huit programmes de tests automatisés**.

Le dernier contrôle général validé a confirmé **219 tests réussis et 0 échec**.

| Programme | Nombre de vérifications |
|---|---:|
| `test_compte.cpp` | 6 |
| `test_banque.cpp` | 13 |
| `test_identifiants.cpp` | 12 |
| `test_montant.cpp` | 22 |
| `test_parcours.cpp` | 33 |
| `test_virement.cpp` | 33 |
| `test_historique.cpp` | 55 |
| `test_liste_comptes.cpp` | 45 |
| **Total** | **219** |

Les programmes de tests peuvent être compilés et exécutés séparément dans PowerShell.

### 9.1. Tests des comptes — 6 vérifications

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_compte.cpp src/Compte.cpp -o test_compte.exe
if ($LASTEXITCODE -eq 0) { .\test_compte.exe }
```

### 9.2. Tests de la banque — 13 vérifications

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_banque.cpp src/Banque.cpp src/Compte.cpp -o test_banque.exe
if ($LASTEXITCODE -eq 0) { .\test_banque.exe }
```

### 9.3. Tests des identifiants — 12 vérifications

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_identifiants.cpp src/Banque.cpp src/Compte.cpp -o test_identifiants.exe
if ($LASTEXITCODE -eq 0) { .\test_identifiants.exe }
```

### 9.4. Tests des montants — 22 vérifications

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_montant.cpp src/Montant.cpp -o test_montant.exe
if ($LASTEXITCODE -eq 0) { .\test_montant.exe }
```

### 9.5. Tests du parcours bancaire — 33 vérifications

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_parcours.cpp src/Banque.cpp src/Compte.cpp src/Montant.cpp -o test_parcours.exe
if ($LASTEXITCODE -eq 0) { .\test_parcours.exe }
```

### 9.6. Tests des virements — 33 vérifications

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_virement.cpp src/Banque.cpp src/Compte.cpp -o test_virement.exe
if ($LASTEXITCODE -eq 0) { .\test_virement.exe }
```

### 9.7. Tests de l'historique — 55 vérifications

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_historique.cpp src/Banque.cpp src/Compte.cpp -o test_historique.exe
if ($LASTEXITCODE -eq 0) { .\test_historique.exe }
```

Ces tests couvrent notamment :

- L'enregistrement des dépôts et retraits réussis.
- Les deux mouvements associés à un virement.
- La numérotation des opérations.
- La consultation de l'historique d'un compte.
- L'absence d'historique pour les mouvements refusés.
- La cohérence des soldes et des copies d'historique.

### 9.8. Tests de la liste des comptes — 45 vérifications

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_liste_comptes.cpp src/Banque.cpp src/Compte.cpp -o test_liste_comptes.exe
if ($LASTEXITCODE -eq 0) { .\test_liste_comptes.exe }
```

Ces 45 nouveaux tests vérifient notamment :

- La consultation d'une banque vide.
- La présence d'un ou plusieurs comptes.
- L'ordre de création des comptes.
- Les identifiants et les titulaires.
- Les soldes initiaux et actualisés.
- Les soldes après dépôt, retrait et virement.
- L'indépendance de la copie retournée.
- Les consultations répétées.
- L'utilisation sur une référence constante à `Banque`.
- L'absence de changement des comptes et de l'historique.
- La cohérence des données après une création refusée.

### Résultat général validé

```text
Huit programmes de tests

Tests reussis : 219
Tests echoues : 0
```

Ce bilan correspond à la somme des huit programmes indépendants, et non à la sortie d'un seul programme de tests.

Les contrôles manuels du menu complètent les tests automatisés.

## 10. Principes de conception

### Programmation orientée objet

La classe `Compte` représente un compte bancaire individuel.

La classe `Banque` gère les comptes et centralise les opérations bancaires.

### Montants entiers

Les soldes sont stockés en centimes sous forme d'entiers signés 64 bits.

### Consultation par copie

`Banque::consulterTousLesComptes()` et `Banque::consulterHistorique()` retournent des copies indépendantes des collections internes.

### Séparation des responsabilités

- `Compte` représente les informations et le solde d'un compte.
- `Banque` organise les comptes, les virements et l'historique.
- `Montant` assure la conversion et l'affichage des sommes.
- `Operation` décrit un mouvement bancaire.
- `main.cpp` gère le terminal et les interactions.

### Tests automatisés

Les huit programmes vérifient les comportements attendus et les principales situations d'erreur.

## 11. Règles et limites actuelles

MiniBank est un **projet pédagogique** et non un logiciel bancaire destiné à gérer de véritables fonds.

### Règles principales

- Les identifiants sont uniques au sein d'une banque.
- Les montants sont conservés en centimes entiers.
- Les dépôts, retraits et virements sont soumis à des validations.
- Les opérations refusées ne doivent pas modifier les soldes ou l'historique.
- Les consultations ne créent aucune opération.
- La liste des comptes respecte l'ordre de création.

### Limites connues

- Les comptes et les historiques sont stockés **uniquement en mémoire**.
- Les données sont perdues à la fermeture de MiniBank.
- Aucune sauvegarde permanente n'est encore disponible.
- Les numéros des mouvements représentent leur ordre d'enregistrement, sans date ni heure.
- Le programme n'est pas conçu pour gérer des opérations simultanées.
- Le mécanisme d'historique ne constitue pas une base de données transactionnelle durable.
- Les méthodes de modification directe d'un `Compte` restent accessibles et peuvent contourner le journal de `Banque` si elles sont utilisées en dehors des méthodes bancaires prévues.
- Les copies des comptes nécessitent du temps et de la mémoire supplémentaires lorsque le nombre de comptes augmente.
- Le programme ne comprend pas encore de mécanisme de connexion ou d'authentification bancaire.

## 12. Feuille de route

### Fonctionnalités réalisées

- [x] Modéliser un compte bancaire.
- [x] Gérer plusieurs comptes dans une banque.
- [x] Générer automatiquement les identifiants.
- [x] Convertir et formater les montants.
- [x] Créer et consulter un compte.
- [x] Déposer et retirer de l'argent.
- [x] Effectuer des virements entre comptes.
- [x] Enregistrer les opérations réussies.
- [x] Consulter l'historique bancaire.
- [x] Consulter tous les comptes bancaires.
- [x] Tester l'indépendance des copies de comptes.
- [x] Valider les huit programmes de tests.
- [x] Obtenir 219 tests réussis avec 0 échec.

### Prochaines évolutions

- [ ] **Option 8 :** sauvegarder les données sur disque.
- [ ] Préparer le chargement des données au démarrage.
- [ ] Étudier la persistance des comptes et de l'historique.
- [ ] Renforcer progressivement les validations et les scénarios de tests.

## 13. Dépôt GitHub

Le projet est disponible dans le dépôt :

[MiniBank_Cpp sur GitHub](https://github.com/datasifaw/MiniBank_Cpp)

Les fonctionnalités sont développées progressivement, avec des phases distinctes de conception, d'implémentation, de tests, de documentation et de publication.

---

**MiniBank C++ — Projet pédagogique de simulation bancaire en C++17.**
