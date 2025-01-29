/***************************************************************************
 * Nom du fichier    : LogsWidget.h
 * Auteur            : Courbeyrette Maxime
 * Date de création  : 27/01/2025
 * Description       : Gestion des stocks - Fichier lié à la gestion des logs
 *
 *
 * Dernière modification :
 * - 27/01/2025 : Finitions des détails
 *
 *
 * Copyright (c) 2025 Courbeyrette Maxime. Tous droits réservés.
 ***************************************************************************/

#ifndef LOGSWIDGET_H
#define LOGSWIDGET_H

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
namespace Ui { class LogsWindow; }
QT_END_NAMESPACE

/**
 * @class LogsWidget
 * @brief Widget pour la gestion des logs.
 *
 * Cette classe permet d’afficher les journaux d’activité et de naviguer dans l’historique des logs
 * pour l’application de gestion des stocks.
 */
class LogsWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructeur de LogsWidget.
     * @param parent Widget parent, par défaut nullptr.
     */
    explicit LogsWidget(QWidget *parent = nullptr);

    /**
     * @brief Destructeur de LogsWidget.
     */
    ~LogsWidget();

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
    Ui::LogsWindow *ui; ///< Interface utilisateur associée à LogsWidget.

    /**
     * @brief Affiche les journaux d'activité dans une table.
     * @param tableWidget Widget de la table où afficher les logs.
     */
    void logsTable(QTableWidget *tableWidget);
};

#endif
