/***************************************************************************
 * Nom du fichier    : main.cpp
 * Auteur            : Courbeyrette Maxime
 * Date de création  : 27/01/2025
 * Description       : Gestion des stocks - fichier principal
 *
 *
 * Dernière modification :
 * - 27/01/2025 : Finitions des détails
 * - 27/01/2025 : Changement de base de donnée (MYSQL -> SQLite) pour une meilleure portabilité
 *
 *
 * Copyright (c) 2025 Courbeyrette Maxime. Tous droits réservés.
 ***************************************************************************/

#include "mainwindow.h"
#include <OpenXLSX.hpp>
#include <QtCore/QCoreApplication>

#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>
#include <QSqlQuery>

using namespace std;
using namespace OpenXLSX;

// Gestionnaire personnalisé des messages Qt
void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    fprintf(stdout, "%s\n", localMsg.constData());
    fflush(stdout);
}

// Fonction pour initialiser la base de données SQLite
bool initializeDatabase(QSqlDatabase &db) {
    QSqlQuery query;

    // Création de la table "category"
    if (!query.exec("CREATE TABLE IF NOT EXISTS category ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name TEXT NOT NULL)")) {
        qDebug() << "Erreur lors de la création de la table 'category':" << query.lastError().text();
        return false;
    }

    // Création de la table "supplier"
    if (!query.exec("CREATE TABLE IF NOT EXISTS supplier ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "name TEXT NOT NULL)")) {
        qDebug() << "Erreur lors de la création de la table 'supplier':" << query.lastError().text();
        return false;
    }

    // Création de la table "logs"
    if (!query.exec("CREATE TABLE IF NOT EXISTS logs ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "text TEXT NOT NULL, "
                    "created_at DATETIME DEFAULT CURRENT_TIMESTAMP)")) {
        qDebug() << "Erreur lors de la création de la table 'logs':" << query.lastError().text();
        return false;
    }

    // Création de la table "products"
    if (!query.exec("CREATE TABLE IF NOT EXISTS products ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "qr TEXT UNIQUE, "
                    "name TEXT NOT NULL, "
                    "description TEXT, "
                    "stock INTEGER DEFAULT 0, "
                    "price REAL DEFAULT 0.0, "
                    "category INTEGER, "
                    "supplier INTEGER, "
                    "created_at TEXT DEFAULT (CURRENT_DATE), "  // Ajout de la colonne created_at
                    "FOREIGN KEY(category) REFERENCES category(id), "
                    "FOREIGN KEY(supplier) REFERENCES supplier(id))")) {
        qDebug() << "Erreur lors de la création de la table 'products':" << query.lastError().text();
        return false;
    }

    qDebug() << "Base de données initialisée avec succès.";
    return true;
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(customMessageHandler); // Activer le gestionnaire personnalisé

    QApplication a(argc, argv);

    // Afficher les drivers SQL disponibles
    qDebug() << "Drivers SQL disponibles :" << QSqlDatabase::drivers();

    // Connexion à la base de données SQLite
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("stock_qt.db"); // Nom du fichier SQLite (il sera créé s'il n'existe pas)

    qDebug() << "Tentative de connexion à SQLite...";
    if (!db.open()) {
        qDebug() << "Erreur lors de l'ouverture de la base de données SQLite :" << db.lastError().text();
        return -1; // Quitte si la connexion échoue
    }

    // Initialiser la base de données (créer les tables si nécessaire)
    if (!initializeDatabase(db)) {
        qDebug() << "Erreur lors de l'initialisation de la base de données SQLite.";
        return -1; // Quitte si l'initialisation échoue
    }

    qDebug() << "Connexion réussie à SQLite.";

    MainWindow w;
    w.show();

    return a.exec();
}
