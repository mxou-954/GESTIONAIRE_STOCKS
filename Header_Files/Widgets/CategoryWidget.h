/***************************************************************************
 * Nom du fichier    : CategoryWidget.h
 * Auteur            : Courbeyrette Maxime
 * Date de création  : 27/01/2025
 * Description       : Gestion des stocks - Fichier lié à la gestion des catégories
 *
 *
 * Dernière modification :
 * - 27/01/2025 : Finitions des détails
 *
 *
 * Copyright (c) 2025 Courbeyrette Maxime. Tous droits réservés.
 ***************************************************************************/

#ifndef CATEGORYSWIDGET_H
#define CATEGORYSWIDGET_H

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
namespace Ui { class CategoryWindow; }
QT_END_NAMESPACE

/**
 * @class CategoryWidget
 * @brief Widget pour la gestion des catégories.
 *
 * Cette classe permet d’ajouter, modifier, supprimer et rechercher des catégories
 * dans l’application de gestion des stocks.
 */
class CategoryWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructeur de CategoryWidget.
     * @param parent Widget parent, par défaut nullptr.
     */
    explicit CategoryWidget(QWidget *parent = nullptr);

    /**
     * @brief Destructeur de CategoryWidget.
     */
    ~CategoryWidget();

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
    /** @brief Ajoute une catégorie. */
    void on_addCategory_clicked();

    /** @brief Charge les catégories dans une liste déroulante. */
    void loadCategoriesIntoComboBox();

    /** @brief Supprime une catégorie sélectionnée. */
    void on_deleteCategory_clicked();

    /** @brief Retourne à la page d’accueil via un bouton. */
    void on_home_clicked();

    /** @brief Recherche une catégorie via un bouton. */
    void on_search_3_clicked();

    /** @brief Modifie une catégorie sélectionnée. */
    void on_modif_2_clicked();

    /** @brief Recherche avancée de catégories. */
    void on_search_4_clicked();

private:
    Ui::CategoryWindow *ui; ///< Interface utilisateur associée à CategoryWidget.

    /**
     * @brief Affiche les catégories dans une table.
     * @param tableWidget Widget de la table où afficher les données.
     */
    void categoryTable(QTableWidget *tableWidget);

    /**
     * @brief Recherche une catégorie par ID.
     * @param categoryId ID de la catégorie.
     * @param msg_3 Label où afficher un message en cas d'erreur.
     * @return ID de la catégorie trouvée, ou -1 si non trouvée.
     */
    int SearchIDCategory(QSpinBox *categoryId, QLabel *msg_3);

    int foundId; ///< Identifiant de la catégorie trouvée.
    QStringList customCategories; ///< Liste des catégories personnalisées.
};

#endif
