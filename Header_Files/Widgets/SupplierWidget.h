/***************************************************************************
 * Nom du fichier    : SupplierWidget.h
 * Auteur            : Courbeyrette Maxime
 * Date de création  : 27/01/2025
 * Description       : Gestion des stocks - Fichier lié à la gestion des fournisseurs
 *
 *
 * Dernière modification :
 * - 27/01/2025 : Finitions des détails
 *
 *
 * Copyright (c) 2025 Courbeyrette Maxime. Tous droits réservés.
 ***************************************************************************/

#ifndef SUPPLIERWIDGET_H
#define SUPPLIERWIDGET_H

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
namespace Ui { class SupplierWindow; }
QT_END_NAMESPACE

/**
 * @class SupplierWidget
 * @brief Widget pour la gestion des fournisseurs.
 *
 * Cette classe permet d’afficher, ajouter, modifier et supprimer les fournisseurs
 * dans l’application de gestion des stocks.
 */
class SupplierWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructeur de SupplierWidget.
     * @param parent Widget parent, par défaut nullptr.
     */
    explicit SupplierWidget(QWidget *parent = nullptr);

    /**
     * @brief Destructeur de SupplierWidget.
     */
    ~SupplierWidget();

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
    /** @brief Ajoute un fournisseur. */
    void on_addSupplier_clicked();

    /** @brief Charge les fournisseurs dans une liste déroulante. */
    void loadsupplierIntoComboBox();

    /** @brief Recherche un fournisseur via un bouton. */
    void on_search_5_clicked();

    /** @brief Modifie un fournisseur sélectionné. */
    void on_modif_3_clicked();

    /** @brief Recherche avancée de fournisseurs. */
    void on_search_6_clicked();

    /** @brief Supprime un fournisseur sélectionné. */
    void on_deleteSupplier_2_clicked();

    /** @brief Retourne à la page d’accueil via un bouton. */
    void on_home_clicked();

private:
    Ui::SupplierWindow *ui; ///< Interface utilisateur associée à SupplierWidget.

    /**
     * @brief Affiche les fournisseurs dans une table.
     * @param tableWidget Widget de la table où afficher les données.
     */
    void supplierTable(QTableWidget *tableWidget);

    int foundId; ///< Identifiant du fournisseur trouvé.
    QStringList customCategories; ///< Catégories personnalisées pour les fournisseurs.
};

#endif
