/***************************************************************************
 * Nom du fichier    : statsWidget.h
 * Auteur            : Courbeyrette Maxime
 * Date de création  : 28/01/2025
 * Description       : Gestion des stocks - Fichier lié à l'affichage des statistiques
 *
 *
 * Dernière modification :
 * - 28/01/2025 : Création
 *
 *
 * Copyright (c) 2025 Courbeyrette Maxime. Tous droits réservés.
 ***************************************************************************/

#ifndef STATSWIDGET_H
#define STATSWIDGET_H

// IMPORTATION DES LIBRAIRIES QT ********************************************************************

#include <QTableWidget>
#include <QMainWindow>
#include <QString>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>

// ********************************************************************

QT_BEGIN_NAMESPACE
namespace Ui { class StatsWindow; }
QT_END_NAMESPACE

/**
 * @class StatsWidget
 * @brief Widget pour la gestion des statistiques.
 *
 * Cette classe permet d'afficher différentes statistiques liées aux produits, aux catégories,
 * aux fournisseurs et à l'évolution des stocks dans l'application de gestion des stocks.
 * Elle permet d'afficher des graphiques comme des graphiques à barres et à secteurs.
 */
class StatsWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructeur de StatsWidget.
     * @param parent Widget parent, par défaut nullptr.
     */
    explicit StatsWidget(QWidget *parent = nullptr);

    /**
     * @brief Destructeur de StatsWidget.
     */
    ~StatsWidget();

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
    /** @brief Retourne à la page d’accueil via un bouton. */
    void on_home_clicked();

private:
    Ui::StatsWindow *ui; ///< Interface utilisateur associée à StatsWidget.

    QChartView *chartView;  ///< Widget pour afficher les graphiques.

    /**
     * @brief Recherche et affiche les statistiques des produits, des catégories et des fournisseurs.
     */
    void searchStatsProductCategorySupplier();

    /**
     * @brief Compte et affiche les prix totaux et les stocks totaux des produits.
     */
    void countPriceStock();

    /**
     * @brief Affiche un graphique des produits ajoutés par date (évolution des produits au fil du temps).
     */
    void productAndTime();
};

#endif
