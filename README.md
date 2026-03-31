# Gestionnaire de Stocks

Application desktop de gestion de stocks produits, développée avec **Qt 6** et **C++**, utilisant SQLite pour la persistance des données et OpenXLSX pour l'export Excel.

---

## Stack technique

- **Qt 6** (Qt Creator)
- **C++17**
- **SQLite** — base de données locale
- **OpenXLSX** — lecture/écriture de fichiers Excel
- **CMake** — build system
- **MSVC** — compilateur

---

## Prérequis

- Qt 6 avec Qt Creator
- Compilateur MSVC
- CMake ≥ 3.15

---

## Installation

```bash
git clone https://github.com/mxou-954/GESTIONAIRE_STOCKS.git
```

1. Ouvrir Qt Creator
2. Charger le projet via `CMakeLists.txt` (pas de fichier `.pro`)
3. Vérifier que le kit **MSVC** est bien sélectionné
4. Compiler et lancer

> ⚠️ La base de données SQLite est générée automatiquement dans le dossier `build/` à la première exécution. Si le dossier `build/` n'est pas créé automatiquement, vérifier la configuration de Qt Creator.

---

## Structure du projet

```
📂 GESTIONNAIRE_STOCKS
├── 📁 Forms            # Fichiers UI (Qt Designer)
├── 📁 Header_Files     # Fichiers d'en-tête C++ (.h)
├── 📁 Source_Files     # Fichiers source C++ (.cpp)
├── 📁 OpenXLSX         # Bibliothèque de gestion Excel
├── 📁 docs             # Documentation générée (Doxygen)
├── 📁 build            # Généré à la compilation
└── CMakeLists.txt
```

---

## Documentation

La documentation complète est disponible dans `docs/html/index.html`.  
Ouvrir ce fichier directement dans un navigateur.

---

## Version & contributions

Version actuelle : **1.0** — des bugs peuvent subsister.  
Les retours et contributions sont les bienvenus :

- Signaler un bug via les Issues
- Proposer une fonctionnalité
- Soumettre une Pull Request
