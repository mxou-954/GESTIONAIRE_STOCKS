/***************************************************************************
 * Nom du fichier    : LogsWidget.cpp
 * Auteur            : Courbeyrette Maxime
 * Date de création  : 27/01/2025
 * Description       : Gestion des stocks - Fonctions liées a la gestion des logs
 *
 *
 * Dernière modification :
 * - 27/01/2025 : Finitions des détails
 *
 *
 * Copyright (c) 2025 Courbeyrette Maxime. Tous droits réservés.
 ***************************************************************************/



#include "LogsWidget.h"
#include "../include/ui_LogsWindow.h"

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <OpenXLSX.hpp>
#include <QtCore/QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QTimer>
#include <QProgressBar>
#include <QMessageBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>

LogsWidget::LogsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LogsWindow)
{
    ui->setupUi(this);
    QTimer::singleShot(100, this, [this]() {
        QSqlDatabase db = QSqlDatabase::database();
        if (!db.isOpen()) {
            qDebug() << "Erreur : La base de données n'est toujours pas connectée.";
        } else {
            qDebug() << "Connexion confirmée après délai.";
            logsTable(ui->categoryTable_3);
        }
    });
}

LogsWidget::~LogsWidget() {
    delete ui;
}


// REDIRECTIONS AUX PAGES ANNEXES ********************************************************************

void LogsWidget::setCurrentWidget(const QString &widgetName)
{
    if (widgetName == "h_LOGS") {
        ui->stackedWidget->setCurrentIndex(0);
        qDebug() << "Switched to add supplier widget";
    }
}

//**************************

void LogsWidget::on_home_clicked()
{
    emit goToHome(); // Émet le signal
}

// ********************************************************************


// TABLE ET CHARGEMENT DE TABLE ********************************************************************

void LogsWidget::logsTable(QTableWidget *tableWidget) {
    // Nettoie le tableau
    tableWidget->clear();
    tableWidget->setRowCount(0);

    // Définit les en-têtes des colonnes
    QStringList headers = {"ID", "TEXT", "DATE"};
    tableWidget->setColumnCount(headers.size());
    tableWidget->setHorizontalHeaderLabels(headers);

    // Requête SQL pour récupérer tous les logs en ordre décroissant
    QSqlQuery query("SELECT id, text, created_at FROM logs ORDER BY created_at DESC");

    int row = 0; // Compteur pour les lignes du tableau
    while (query.next()) {
        // Récupérer les données de chaque log
        int id = query.value("id").toInt();
        QString name = query.value("text").toString();
        QString date = query.value("created_at").toString();

        // Ajouter une nouvelle ligne dans le tableau
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
        tableWidget->setItem(row, 1, new QTableWidgetItem(name));
        tableWidget->setItem(row, 2, new QTableWidgetItem(date));

        row++; // Passe à la ligne suivante
    }

    // Log pour débogage
    qDebug() << "Tableau des logs mis à jour avec" << row << "lignes.";
}

// ********************************************************************
