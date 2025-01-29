/***************************************************************************
 * Nom du fichier    : ProductsWidget.h
 * Auteur            : Courbeyrette Maxime
 * Date de création  : 27/01/2025
 * Description       : Gestion des stocks - Fichier lié à la gestion des produits
 *
 *
 * Dernière modification :
 * - 27/01/2025 : Finitions des détails
 *
 *
 * Copyright (c) 2025 Courbeyrette Maxime. Tous droits réservés.
 ***************************************************************************/

#ifndef PRODUCTSWIDGET_H
#define PRODUCTSWIDGET_H

// IMPORTATION DES LIBRAIRIES QT ********************************************************************

#include <QTableWidget>
#include <QMainWindow>
#include <QString>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QWidget>

// ********************************************************************

QT_BEGIN_NAMESPACE
namespace Ui { class ProductsWindow; }
QT_END_NAMESPACE

/**
 * @class ProductsWidget
 * @brief Widget pour la gestion des produits.
 *
 * Cette classe permet d’afficher, ajouter, modifier et rechercher des produits
 * dans l’application de gestion des stocks.
 */
class ProductsWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructeur de ProductsWidget.
     * @param parent Widget parent, par défaut nullptr.
     */
    explicit ProductsWidget(QWidget *parent = nullptr);

    /**
     * @brief Destructeur de ProductsWidget.
     */
    ~ProductsWidget();

    /**
     * @brief Change le widget actif dans le conteneur.
     * @param widgetName Nom du widget à afficher.
     */
    void setCurrentWidget(const QString &widgetName);

signals:
    /**
     * @brief Signal pour retourner à la page d’accueil.
     */
    void goToHome();

private slots:

    /** @brief Ajoute un produit. */
    void on_add_clicked();

    /** @brief Retourne à la page d’accueil via un bouton. */
    void on_home_clicked();

    /** @brief Recherche un produit via un bouton. */
    void on_search_clicked();

    /** @brief Modifie un produit sélectionné. */
    void on_modif_clicked();

    /** @brief Recherche avancée de produits. */
    void on_search_2_clicked();

    /** @brief Charge les catégories dans une liste déroulante. */
    void loadCategoriesIntoComboBox();

    /** @brief Charge les fournisseurs dans une liste déroulante. */
    void loadsupplierIntoComboBox();

private:
    Ui::ProductsWindow *ui; ///< Interface utilisateur associée à ProductsWidget.

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

    QList<Product> products; ///< Liste des produits gérés.

    /**
     * @brief Affiche les produits dans une table.
     * @param tableWidget Widget de la table où afficher les données.
     * @param orderBy Critère de tri, par défaut vide.
     */
    void tableProduct(QTableWidget *tableWidget, const QString &orderBy="");

    /**
     * @brief Recherche un produit par ID.
     * @param productId ID du produit.
     * @param msg Label où afficher un message en cas d'erreur.
     * @return ID du produit trouvé, ou -1 si non trouvé.
     */
    int SearchID(QSpinBox *productId, QLabel *msg);

    /**
     * @brief Action à effectuer lorsque l'utilisateur confirme une opération.
     * @param foundId ID du produit confirmé.
     */
    void onButtonBoxAccepted(int foundId);

    /**
     * @brief Action à effectuer lorsque l'utilisateur annule une opération.
     */
    void onButtonBoxRejected();

    int foundId; ///< Identifiant du produit trouvé.
    int lastProductId = 0; ///< Identifiant du dernier produit ajouté.
};

#endif
