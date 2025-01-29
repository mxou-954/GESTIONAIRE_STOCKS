/***************************************************************************
 * Nom du fichier    : ImportWidget.cpp
 * Auteur            : Courbeyrette Maxime
 * Date de création  : 27/01/2025
 * Description       : Gestion des stocks - Fonctions liées a l'import CSV ou JSON
 *
 *
 * Dernière modification :
 * - 27/01/2025 : Finitions des détails
 *
 *
 * Copyright (c) 2025 Courbeyrette Maxime. Tous droits réservés.
 ***************************************************************************/


#include "ImportWidget.h"
#include "../include/ui_ImportWindow.h"

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

ImportWidget::ImportWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImportWindow)
{
    ui->setupUi(this);

    QTimer::singleShot(100, this, [this]() {
        QSqlDatabase db = QSqlDatabase::database();
        if (!db.isOpen()) {
            qDebug() << "Erreur : La base de données n'est toujours pas connectée.";
        } else {
            qDebug() << "Connexion confirmée après délai.";


        }
    });

    ui->dropLabel->setAcceptDrops(true);
    ui->dropLabel->setText("Déposez un fichier ici");
    ui->dropLabel->setStyleSheet("border: 2px dashed #aaa; padding: 10px; text-align: center;");

    ui->dropLabel_2->setAcceptDrops(true);
    ui->dropLabel_2->setText("Déposez un fichier ici");
    ui->dropLabel_2->setStyleSheet("border: 2px dashed #aaa; padding: 10px; text-align: center;");

}

ImportWidget::~ImportWidget() {
    delete ui;
}


// REDIRECTIONS AUX PAGES ANNEXES ********************************************************************

void ImportWidget::setCurrentWidget(const QString &widgetName)
{
    if (widgetName == "l_importXLSX") {
        ui->stackedWidget->setCurrentIndex(0);
        qDebug() << "Switched to add import widget";
    } else if (widgetName == "m_importJSON"){
        ui->stackedWidget->setCurrentIndex(1);
        qDebug() << "Switched to modif import widget";
    }
}

//**************************

void ImportWidget::on_home_clicked()
{
    emit goToHome(); // Émet le signal
}

// ********************************************************************


// CSV & JSON DRAG&DROP ********************************************************************

void ImportWidget::dropEvent(QDropEvent *event) {
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urls = mimeData->urls();
        if (!urls.isEmpty()) {
            QString filePath = urls.first().toLocalFile();
            if (filePath.endsWith(".csv", Qt::CaseInsensitive)) {
                // CSV : Charger et afficher dans dropLabel
                loadCSV(filePath);
                ui->dropLabel->setText("Fichier CSV chargé :\n" + filePath);
            } else if (filePath.endsWith(".json", Qt::CaseInsensitive)) {
                // JSON : Charger et afficher dans dropLabel_2
                importJSON(filePath);
                ui->dropLabel_2->setText("Fichier JSON chargé :\n" + filePath);
            } else {
                QMessageBox::warning(this, "Fichier non valide", "Veuillez déposer un fichier au format CSV ou JSON.");
            }
        }
    }
}

//**************************

void ImportWidget::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

// ********************************************************************


// CSV & JSON CHARGEMENT ********************************************************************

void ImportWidget::loadCSV(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        ui->dropLabel->setText("Erreur : Impossible d'ouvrir le fichier.");
        return;
    }

    QTextStream in(&file);
    QString content = in.readAll();
    file.close();

    // Afficher un aperçu du contenu
    qDebug() << "Contenu du fichier :" << content;
    ui->dropLabel->setText("Fichier chargé :\n" + filePath);
}

//**************************

void ImportWidget::loadJSON(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        ui->dropLabel_2->setText("Erreur : Impossible d'ouvrir le fichier.");
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isArray()) {
        ui->dropLabel_2->setText("Erreur : Le fichier JSON est mal formé.");
        return;
    }

    QJsonArray jsonArray = doc.array();
    QString preview;
    for (const QJsonValue &value : jsonArray) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();
            preview += obj.value("name").toString() + "\n";
        }
    }

    ui->dropLabel_2->setText("Produits dans le fichier JSON :\n" + preview);
}

// ********************************************************************


// IMPORTATION CSV & JSON ********************************************************************

void ImportWidget::importCSV(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Erreur : Impossible d'ouvrir le fichier CSV :" << filePath;
        return;
    }

    QTextStream in(&file);
    int lineNumber = 0;

    while (!in.atEnd()) {
        QString line = in.readLine();
        lineNumber++;

        // Ignorer la première ligne si c'est une en-tête
        if (lineNumber == 1 && line.startsWith("id,qr,name,description,stock,price,category,supplier")) {
            continue;
        }

        QStringList fields = line.split(";"); // Séparer les colonnes par une virgule
        if (fields.size() != 7) {
            qDebug() << "Ligne mal formatée à la ligne" << lineNumber << ":" << line;
            continue;
        }

        // Extraire les données
        int id = fields[0].toInt();
        QString qr = fields[1].trimmed();
        QString name = fields[2].trimmed();
        QString description = fields[3].trimmed();
        int stock = fields[4].toInt();
        double price = fields[5].toDouble();
        QString category = fields[6].trimmed();
        QString supplier = fields[6].trimmed();


        // Insérer les données dans la base de données
        QSqlQuery query;
        query.prepare("INSERT INTO products (id, qr, name, description, stock, price, category, supplier) "
                      "VALUES (:id, :qr, :name, :description, :stock, :price, :category, :supplier)");
        query.bindValue(":id", id);
        query.bindValue(":qr", qr);
        query.bindValue(":name", name);
        query.bindValue(":description", description);
        query.bindValue(":stock", stock);
        query.bindValue(":price", price);
        query.bindValue(":category", category);
        query.bindValue(":supplier", supplier);


        if (!query.exec()) {
            qDebug() << "Erreur lors de l'insertion dans la base de données à la ligne" << lineNumber
                     << ":" << query.lastError().text();
        } else {
            qDebug() << "Produit inséré : " << name;
        }
    }

    file.close();
    qDebug() << "Importation CSV terminée.";

}

//**************************

void ImportWidget::importJSON(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Erreur : Impossible d'ouvrir le fichier JSON :" << filePath;
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isArray()) {
        qDebug() << "Erreur : Le fichier JSON est mal formaté.";
        return;
    }

    QJsonArray jsonArray = doc.array();
    for (const QJsonValue &value : jsonArray) {
        if (!value.isObject()) {
            qDebug() << "Erreur : Ligne JSON invalide, ignorée.";
            continue;
        }

        QJsonObject obj = value.toObject();

        // Extraire les données
        int id = obj["id"].toInt();
        QString qr = obj["qr"].toString();
        QString name = obj["name"].toString();
        QString description = obj["description"].toString();
        int stock = obj["stock"].toInt();
        double price = obj["price"].toDouble();
        QString category = obj["category"].toString();
        QString supplier = obj["supplier"].toString();

        // Insérer dans la base de données
        QSqlQuery query;
        query.prepare("INSERT INTO products (id, qr, name, description, stock, price, category, supplier) "
                      "VALUES (:id, :qr, :name, :description, :stock, :price, :category, :supplier)");
        query.bindValue(":id", id);
        query.bindValue(":qr", qr);
        query.bindValue(":name", name);
        query.bindValue(":description", description);
        query.bindValue(":stock", stock);
        query.bindValue(":price", price);
        query.bindValue(":category", category);
        query.bindValue(":supplier", supplier);

        if (!query.exec()) {
            qDebug() << "Erreur lors de l'insertion dans la base de données pour le produit :" << name
                     << ":" << query.lastError().text();
        } else {
            qDebug() << "Produit inséré : " << name;
        }
    }

    // Log pour l'importation JSON
    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
    logQuery.bindValue(":text", "Un fichier JSON a été importé.");
    if (logQuery.exec()) {
        qDebug() << "Log ajouté : un fichier JSON a été importé.";
    } else {
        qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
    }

    qDebug() << "Importation JSON terminée.";
}

// ********************************************************************
