/***************************************************************************
 * Nom du fichier    : ImportWidget.h
 * Auteur            : Courbeyrette Maxime
 * Date de création  : 27/01/2025
 * Description       : Gestion des stocks - Fichier lié à la gestion des importations
 *
 *
 * Dernière modification :
 * - 27/01/2025 : Finitions des détails
 *
 *
 * Copyright (c) 2025 Courbeyrette Maxime. Tous droits réservés.
 ***************************************************************************/

#ifndef IMPORTWIDGET_H
#define IMPORTWIDGET_H

// IMPORTATION DES LIBRAIRIES QT ********************************************************************

#include <QTableWidget>
#include <QMainWindow>
#include <QString>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QWidget>
#include <QDragEnterEvent>
#include <QDropEvent>

// ********************************************************************

QT_BEGIN_NAMESPACE
namespace Ui { class ImportWindow; }
QT_END_NAMESPACE

/**
 * @class ImportWidget
 * @brief Widget pour la gestion des importations.
 *
 * Cette classe permet de gérer l'importation de fichiers CSV et JSON dans l'application
 * de gestion des stocks.
 */
class ImportWidget : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructeur de ImportWidget.
     * @param parent Widget parent, par défaut nullptr.
     */
    explicit ImportWidget(QWidget *parent = nullptr);

    /**
     * @brief Destructeur de ImportWidget.
     */
    ~ImportWidget();

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
    Ui::ImportWindow *ui; ///< Interface utilisateur associée à ImportWidget.

    /**
     * @brief Gestion de l'événement de glisser-déposer.
     * @param event L'événement de glissement entrant.
     */
    void dragEnterEvent(QDragEnterEvent *event) override;

    /**
     * @brief Gestion de l'événement de dépôt.
     * @param event L'événement de dépôt.
     */
    void dropEvent(QDropEvent *event) override;

    /**
     * @brief Charge un fichier CSV à partir d'un chemin donné.
     * @param filePath Chemin du fichier CSV.
     */
    void loadCSV(const QString &filePath);

    /**
     * @brief Importe les données d'un fichier CSV dans l'application.
     * @param filePath Chemin du fichier CSV.
     */
    void importCSV(const QString &filePath);

    /**
     * @brief Charge un fichier JSON à partir d'un chemin donné.
     * @param filePath Chemin du fichier JSON.
     */
    void loadJSON(const QString &filePath);

    /**
     * @brief Gestion de l'événement de dépôt pour les fichiers JSON.
     * @param event L'événement de dépôt.
     */
    void dropEventJSON(QDropEvent *event);

    /**
     * @brief Importe les données d'un fichier JSON dans l'application.
     * @param filePath Chemin du fichier JSON.
     */
    void importJSON(const QString &filePath);
};

#endif
