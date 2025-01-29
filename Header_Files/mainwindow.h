/***************************************************************************
 * Nom du fichier    : mainwindow.h
 * Auteur            : Courbeyrette Maxime
 * Date de création  : 27/01/2025
 * Description       : Gestion des stocks - fichier principal
 *
 *
 * Dernière modification :
 * - 27/01/2025 : Finitions des détails
 *
 *
 * Copyright (c) 2025 Courbeyrette Maxime. Tous droits réservés.
 ***************************************************************************/

/**
 * @file mainwindow.h
 * @brief Fichier principal pour la gestion des stocks.
 *
 * Contient la déclaration de la classe MainWindow et des structures associées.
 */

// PROTECTIONS CONTRE LES INCLUSIONS MULTIPLES ********************************************************************

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// ********************************************************************

// IMPORTATION ET CENTRALISATION DES .H ANNEXES ********************************************************************

#include "Widgets/ProductsWidget.h"
#include "Widgets/SupplierWidget.h"
#include "Widgets/CategoryWidget.h"
#include "Widgets/ImportWidget.h"
#include "Widgets/LogsWidget.h"
#include "Widgets/statsWidget.h"

// ********************************************************************

// IMPORTATION DES LIBRAIRIES QT ********************************************************************

#include <QTableWidget>
#include <QMainWindow>
#include <QString>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>

// ********************************************************************

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @class MainWindow
 * @brief Fenêtre principale de l'application de gestion des stocks.
 *
 * Cette classe gère les différents widgets et les interactions utilisateur.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructeur de MainWindow.
     * @param parent Widget parent, par défaut nullptr.
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructeur de MainWindow.
     */
    ~MainWindow();

private slots:

    // GESTION DES PAGES ********************************************************************

    /** @brief Affiche la page d'ajout de produit. */
    void showProductAddPage();

    /** @brief Affiche la page de modification de produit. */
    void showProductModifPage();

    /** @brief Affiche la page de suppression de produit. */
    void showProductDeletePage();

    /** @brief Affiche la page d'ajout de catégorie. */
    void showCategoryAddPage();

    /** @brief Affiche la page de modification de catégorie. */
    void showCategoryModifPage();

    /** @brief Affiche la page de suppression de catégorie. */
    void showCategoryDeletePage();

    /** @brief Affiche la page d'ajout de fournisseur. */
    void showSupplierAddPage();

    /** @brief Affiche la page de modification de fournisseur. */
    void showSupplierModifPage();

    /** @brief Affiche la page de suppression de fournisseur. */
    void showSupplierDeletePage();

    /** @brief Affiche la page d'importation CSV. */
    void showImportCSVPage();

    /** @brief Affiche la page d'importation JSON. */
    void showImportJSONPage();

    /** @brief Affiche la page des logs. */
    void showLogsPage();

    /** @brief Retourne à la page d'accueil. */
    void showHomePage();

    /** @brief  Affiche la page de statistiques. */
    void showStatsPage();

    // ********************************************************************

    // GESTION DES BUTTONS D'EXPORTATIONS ********************************************************************

    /** @brief Exporte les données via le premier bouton. */
    void on_export_button_clicked();

    /** @brief Exporte les données via le deuxième bouton. */
    void on_export_button_2_clicked();

    // ********************************************************************

    // GESTION DES RECHERCHES, FILTRES, NOTIFICATIONS ********************************************************************

    /** @brief Applique un filtre sur les produits. */
    void filter();

    /** @brief Recherche un produit spécifique. */
    void searchProducts();

    /** @brief Vérifie les stocks faibles et notifie l'utilisateur. */
    void checkLowStock();

    // ********************************************************************

private:
    Ui::MainWindow *ui;

    // STRUCTURES DES PRODUITS ********************************************************************

    /**
     * @struct Product
     * @brief Représente un produit dans l'application.
     *
     * Contient toutes les informations nécessaires pour gérer un produit :
     * - Identifiant unique.
     * - Code QR.
     * - Nom, description, stock, prix.
     * - Catégorie et fournisseur.
     */
    struct Product {
        int id;               ///< Identifiant unique
        QString qr;           ///< Code barre unique
        QString name;         ///< Nom du produit
        QString description;  ///< Description du produit
        int stock;            ///< Quantité en stock
        double price;         ///< Prix unitaire
        QString category;     ///< Catégorie du produit
        QString supplier;     ///< Fournisseur
    };

    QList<Product> products;

    // ********************************************************************

    // GESTIONS DES TABLES ********************************************************************

    /**
     * @brief Gère l'affichage des produits dans une table.
     * @param tableWidget Table où afficher les produits.
     * @param orderBy Critère de tri, par défaut vide.
     */
    void tableProduct(QTableWidget *tableWidget, const QString &orderBy="");

    // ********************************************************************

    // INITIALISATION DES WIDGETS ANNEXES ********************************************************************

    ProductsWidget *productsWidget;    ///< Widget pour la gestion des produits
    SupplierWidget *supplierWidget;    ///< Widget pour la gestion des fournisseurs
    CategoryWidget *categoryWidget;    ///< Widget pour la gestion des catégories
    ImportWidget *importWidget;        ///< Widget pour la gestion des imports
    LogsWidget *logsWidget;            ///< Widget pour l'affichage des logs
    StatsWidget *statsWidget;          ///< Widget pour l'affichage les statistiques

    // ********************************************************************

};

#endif
