# MiniBank C++

**MiniBank C++** est un simulateur bancaire pédagogique développé en **C++17** et exécuté dans un terminal.

Ce projet permet de mettre en pratique la programmation orientée objet, la gestion de comptes bancaires, la validation des montants monétaires, les collections C++, les tests automatisés et l'organisation d'un projet avec Git et GitHub.

MiniBank propose actuellement **six fonctionnalités bancaires actives**, dont les virements entre comptes et la consultation de l'historique des opérations.

**État de la version actuelle : 174 tests automatisés réussis, 0 échec.**

## 1. Fonctionnalités

| Option | Fonctionnalité | État |
|---|---|---|
| `1` | Créer un compte bancaire | Fonctionnelle |
| `2` | Consulter un compte bancaire | Fonctionnelle |
| `3` | Déposer de l'argent | Fonctionnelle |
| `4` | Retirer de l'argent | Fonctionnelle |
| `5` | Effectuer un virement entre deux comptes | Fonctionnelle |
| `6` | Consulter l'historique d'un compte | Fonctionnelle |
| `7` | Afficher tous les comptes | À développer |
| `8` | Sauvegarder les données | À développer |
| `0` | Quitter MiniBank | Fonctionnelle |

### Création d'un compte

L'utilisateur saisit :

- Le nom du titulaire.
- Le solde initial, en euros.

MiniBank génère automatiquement un identifiant tel que `MB-0001`, `MB-0002`, etc.

Chaque compte possède un identifiant unique, un titulaire et un solde stocké en centimes entiers.

### Consultation d'un compte

La consultation permet de retrouver un compte grâce à son identifiant et d'afficher son titulaire ainsi que son solde.

### Dépôt d'argent

L'utilisateur choisit le compte à créditer et saisit un montant.

Un dépôt réussi augmente le solde et crée une opération `Depot` dans l'historique.

Les montants nuls ou négatifs et les dépassements de la limite numérique sont refusés.

### Retrait d'argent

L'utilisateur choisit le compte à débiter et saisit un montant.

Un retrait réussi diminue le solde et crée une opération `Retrait` dans l'historique.

Les montants nuls ou négatifs et les retraits supérieurs au solde disponible sont refusés.

### Virement bancaire

L'option `5` permet de transférer un montant entre deux comptes existants.

Le virement vérifie notamment :

- L'existence des deux comptes.
- La différence entre le compte source et le compte destinataire.
- La validité et la positivité du montant.
- Le solde disponible sur le compte source.
- L'absence de dépassement numérique sur le compte destinataire.

Un virement réussi débite le compte source, crédite le destinataire et crée deux lignes d'historique : un virement sortant et un virement entrant.

Un virement refusé ne doit modifier ni les soldes ni l'historique.

## 2. Historique des opérations bancaires

L'option `6` permet de consulter les mouvements enregistrés pour un compte bancaire.

Les opérations sont stockées en mémoire et numérotées dans leur ordre d'enregistrement.

### Types d'opérations

| Type C++ | Affichage | Sens |
|---|---|---|
| `TypeOperation::Depot` | `DEPOT` | Entrée d'argent |
| `TypeOperation::Retrait` | `RETRAIT` | Sortie d'argent |
| `TypeOperation::VirementSortant` | `VIREMENT SORTANT` | Argent envoyé |
| `TypeOperation::VirementEntrant` | `VIREMENT ENTRANT` | Argent reçu |

Chaque ligne d'historique contient :

- Un numéro d'ordre.
- Un type d'opération.
- L'identifiant du compte concerné.
- Le montant en centimes entiers.
- L'identifiant de l'autre compte lorsqu'il s'agit d'un virement.

Le montant est conservé sous forme de valeur positive. Le signe `+` ou `-` est déterminé à l'affichage selon le type de mouvement.

Un virement entre deux comptes produit **deux lignes d'historique**, une pour chaque compte, mais représente un seul transfert d'argent.

### Exemple de consultation

Après la création d'Alice avec 500 EUR, un dépôt de 100 EUR, un retrait de 50 EUR et un virement de 25 EUR vers Bob :

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

Le compte de Bob possède alors une ligne de virement entrant :

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

La consultation d'un compte sans mouvement affiche un message indiquant qu'aucune opération n'est enregistrée.

La consultation d'un identifiant inexistant affiche un message d'erreur.

**Important : l'historique est conservé uniquement en mémoire pendant l'exécution du programme. Il n'est pas sauvegardé sur disque.**

## 3. Gestion des montants monétaires

Les montants sont stockés avec le type `std::int64_t`, sous forme de centimes entiers, plutôt qu'en nombres à virgule flottante.

Par exemple :

| Montant en euros | Valeur interne |
|---|---:|
| `10,00 EUR` | `1000` centimes |
| `25,50 EUR` | `2550` centimes |
| `100,00 EUR` | `10000` centimes |

Le module `Montant` gère la conversion des saisies et l'affichage.

Exemples de saisies acceptées :

```text
100
100.50
100,50
25.5
```

Les formats invalides, les valeurs négatives saisies comme montants d'opération et les valeurs dépassant la capacité numérique sont refusés selon les règles des fonctionnalités concernées.

## 4. Technologies utilisées

- **Langage :** C++17.
- **Compilateur :** G++ (environnement MSYS2 sous Windows).
- **Éditeur :** Visual Studio Code.
- **Terminal :** PowerShell.
- **Versionnement :** Git et GitHub Desktop.
- **Hébergement du code :** GitHub.

Le projet utilise uniquement la bibliothèque standard C++ pour les fonctionnalités présentées.

## 5. Organisation du projet

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
|   `-- test_historique.cpp
|
|-- data/
|   `-- .gitkeep
|
|-- .gitattributes
|-- .gitignore
`-- README.md
```

### Rôle des principaux fichiers

| Fichier | Responsabilité |
|---|---|
| `include/Compte.h` et `src/Compte.cpp` | Modéliser un compte, consulter et modifier son solde selon les règles prévues |
| `include/Banque.h` et `src/Banque.cpp` | Gérer les comptes, générer leurs identifiants, effectuer les opérations bancaires et enregistrer l'historique |
| `include/Montant.h` et `src/Montant.cpp` | Convertir et formater les montants monétaires |
| `include/Operation.h` | Définir les types et les données d'une opération bancaire |
| `src/main.cpp` | Afficher le menu et gérer les interactions avec l'utilisateur |
| `tests/` | Contenir les programmes de tests automatisés |

### Architecture de l'historique

Le modèle `Operation` contient les informations de chaque mouvement.

La classe `Banque` conserve les lignes d'historique dans un `std::vector<Operation>` et expose notamment :

```cpp
deposerSurCompte(...)
retirerDuCompte(...)
virer(...)
consulterHistorique(...)
nombreOperations()
```

Les options `3`, `4` et `5` du menu passent par `Banque` pour exécuter les opérations et assurer leur enregistrement.

L'option `6` consulte l'historique en lecture seule.

## 6. Compilation et exécution sous Windows

### Prérequis

Disposer de :

- Visual Studio Code.
- Un compilateur G++ prenant en charge C++17, par exemple via MSYS2.
- PowerShell.

Ouvrir le dossier du projet dans Visual Studio Code, puis sélectionner **Terminal → Nouveau terminal**.

### Compiler MiniBank

Depuis la racine du projet :

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude src/main.cpp src/Banque.cpp src/Compte.cpp src/Montant.cpp -o MiniBank.exe
```

### Exécuter MiniBank

```powershell
.\MiniBank.exe
```

Le menu interactif s'affiche dans le terminal.

Les fichiers d'en-tête `.h` sont inclus par les modules C++ : `Operation.h` ne nécessite pas de fichier `Operation.cpp` supplémentaire.

## 7. Exemple de parcours bancaire

Dans une même exécution de MiniBank :

1. Créer Alice Martin avec un solde initial de `500` EUR. Son identifiant attendu est `MB-0001`.
2. Créer Bob Dupont avec un solde initial de `1200` EUR. Son identifiant attendu est `MB-0002`.
3. Déposer `100` EUR sur le compte `MB-0001`.
4. Retirer `50` EUR du compte `MB-0001`.
5. Virer `25` EUR de `MB-0001` vers `MB-0002`.
6. Consulter l'historique d'Alice avec l'option `6` : trois opérations sont affichées.
7. Consulter l'historique de Bob avec l'option `6` : une opération est affichée.
8. Consulter les soldes avec l'option `2`.

Résultat attendu :

| Compte | Solde final | Nombre d'opérations |
|---|---:|---:|
| Alice (`MB-0001`) | `525,00 EUR` | 3 |
| Bob (`MB-0002`) | `1225,00 EUR` | 1 |

Les opérations refusées ne doivent pas ajouter de mouvements à l'historique.

## 8. Tests automatisés

Le projet dispose de **sept programmes de tests**, représentant **174 vérifications réussies et 0 échec** lors de la dernière validation de la phase 3.7.

| Programme | Nombre de vérifications |
|---|---:|
| `test_compte.cpp` | 6 |
| `test_banque.cpp` | 13 |
| `test_identifiants.cpp` | 12 |
| `test_montant.cpp` | 22 |
| `test_parcours.cpp` | 33 |
| `test_virement.cpp` | 33 |
| `test_historique.cpp` | 55 |
| **Total** | **174** |

### Test des comptes

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_compte.cpp src/Compte.cpp -o test_compte.exe
if ($LASTEXITCODE -eq 0) { .\test_compte.exe }
```

### Test de la banque

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_banque.cpp src/Banque.cpp src/Compte.cpp -o test_banque.exe
if ($LASTEXITCODE -eq 0) { .\test_banque.exe }
```

### Test des identifiants automatiques

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_identifiants.cpp src/Banque.cpp src/Compte.cpp -o test_identifiants.exe
if ($LASTEXITCODE -eq 0) { .\test_identifiants.exe }
```

### Test de la conversion des montants

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_montant.cpp src/Montant.cpp -o test_montant.exe
if ($LASTEXITCODE -eq 0) { .\test_montant.exe }
```

### Test du parcours bancaire

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_parcours.cpp src/Banque.cpp src/Compte.cpp src/Montant.cpp -o test_parcours.exe
if ($LASTEXITCODE -eq 0) { .\test_parcours.exe }
```

### Test des virements

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_virement.cpp src/Banque.cpp src/Compte.cpp -o test_virement.exe
if ($LASTEXITCODE -eq 0) { .\test_virement.exe }
```

### Test de l'historique bancaire

```powershell
g++ -std=c++17 -Wall -Wextra -Iinclude tests/test_historique.cpp src/Banque.cpp src/Compte.cpp -o test_historique.exe
if ($LASTEXITCODE -eq 0) { .\test_historique.exe }
```

Les tests de l'historique vérifient notamment :

- La création d'une banque avec un historique vide.
- L'enregistrement des dépôts et retraits réussis.
- Les deux lignes créées lors d'un virement.
- L'ordre de numérotation des opérations.
- Le bon rattachement des mouvements à chaque compte.
- L'absence de nouvelles lignes pour les opérations refusées.
- La cohérence des soldes.
- La consultation sans modification des données.
- L'indépendance des copies d'historique.
- Le refus d'un dépôt provoquant un dépassement monétaire.

Résultat validé :

```text
Tests reussis : 174
Tests echoues : 0
```

Ce total correspond à la somme des sept programmes de tests, et non à la sortie d'un seul exécutable.

## 9. Règles et limites actuelles

MiniBank est un projet pédagogique, pas un logiciel destiné à gérer de véritables fonds bancaires.

### Règles principales

- Les identifiants des comptes sont uniques.
- Les soldes sont représentés en centimes entiers.
- Les dépôts et retraits doivent respecter les règles de validation des montants.
- Un virement ne peut pas être effectué vers le même compte.
- Un virement ne peut pas dépasser le solde disponible.
- Les mouvements réussis réalisés par les méthodes bancaires prévues sont enregistrés dans l'historique.
- La consultation de l'historique ne modifie pas les soldes.

### Limites

- Les comptes et l'historique sont stockés **uniquement en mémoire**.
- Les données disparaissent lorsque le programme se termine.
- Il n'existe pas encore de chargement ni de sauvegarde permanente.
- Le programme ne gère pas les accès simultanés.
- Le mécanisme d'historique ne constitue pas une base de données transactionnelle durable.
- Les modifications directes d'un `Compte` en dehors des méthodes de `Banque` peuvent contourner l'enregistrement de l'historique.
- Les lignes d'historique utilisent un numéro d'ordre, sans date ni heure enregistrée.
- L'ouverture d'un compte avec un solde initial ne crée pas de ligne de dépôt dans l'historique.

## 10. Feuille de route

### Fonctionnalités terminées

- [x] Modèle des comptes bancaires.
- [x] Gestion des comptes dans une banque.
- [x] Identifiants automatiques.
- [x] Conversion et formatage des montants monétaires.
- [x] Création et consultation d'un compte.
- [x] Dépôts et retraits.
- [x] Virements entre comptes.
- [x] Modèle des opérations bancaires.
- [x] Enregistrement des opérations réussies.
- [x] Consultation de l'historique depuis le menu.
- [x] Tests automatisés de l'historique.
- [x] Vérification générale des 174 tests.

### Prochaines améliorations

- [ ] **Option 7 :** afficher tous les comptes bancaires.
- [ ] **Option 8 :** sauvegarder les données sur disque.
- [ ] Étudier le chargement des comptes et de l'historique lors du démarrage.
- [ ] Améliorer progressivement l'organisation du code, la validation et les scénarios de tests.

## 11. Dépôt GitHub

Le code source du projet est disponible ici :

[MiniBank_Cpp sur GitHub](https://github.com/datasifaw/MiniBank_Cpp)

Le projet est développé progressivement, avec des tests et des validations avant chaque nouvelle fonctionnalité.

---

**MiniBank C++ — Projet pédagogique de simulation bancaire en C++17.**
