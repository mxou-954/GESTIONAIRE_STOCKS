/***************************************************************************
 * Nom du fichier    : SupplierWidget.cpp
 * Auteur            : Courbeyrette Maxime
 * Date de création  : 27/01/2025
 * Description       : Gestion des stocks - Fonctions liées au CRUD des fournisseurs
 *
 *
 * Dernière modification :
 * - 27/01/2025 : Finitions des détails
 *
 *
 * Copyright (c) 2025 Courbeyrette Maxime. Tous droits réservés.
 ***************************************************************************/




#include "SupplierWidget.h"
#include "../include/ui_SupplierWindow.h"

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

SupplierWidget::SupplierWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SupplierWindow)
{
    ui->setupUi(this);


    QTimer::singleShot(100, this, [this]() {
        QSqlDatabase db = QSqlDatabase::database();
        if (!db.isOpen()) {
            qDebug() << "Erreur : La base de données n'est toujours pas connectée.";
        } else {
            qDebug() << "Connexion confirmée après délai.";

            supplierTable(ui->supplierTable_4);
            supplierTable(ui->supplierTable_5);
            supplierTable(ui->supplierTable_6);

        }
    });
    connect(ui->addSupplier, &QPushButton::clicked, this, &SupplierWidget::on_addSupplier_clicked);
    connect(ui->search_5, &QPushButton::clicked, this, &SupplierWidget::on_search_5_clicked);
    connect(ui->modif_3, &QPushButton::clicked, this, &SupplierWidget::on_modif_3_clicked);
    connect(ui->search_6, &QPushButton::clicked, this, &SupplierWidget::on_search_6_clicked);
    connect(ui->deleteSupplier_2, &QPushButton::clicked, this, &SupplierWidget::on_deleteSupplier_2_clicked);

}

SupplierWidget::~SupplierWidget() {
    delete ui;
}



// REDIRECTIONS AUX PAGES ANNEXES ********************************************************************

void SupplierWidget::setCurrentWidget(const QString &widgetName)
{
    if (widgetName == "i_add_supplier") {
        ui->stackedWidget->setCurrentIndex(0);
        supplierTable(ui->supplierTable_4);
        supplierTable(ui->supplierTable_5);
        supplierTable(ui->supplierTable_6);
        qDebug() << "Switched to add supplier widget";
    } else if (widgetName == "j_modif_supplier"){
        ui->stackedWidget->setCurrentIndex(1);
        supplierTable(ui->supplierTable_4);
        supplierTable(ui->supplierTable_5);
        supplierTable(ui->supplierTable_6);
        qDebug() << "Switched to modif supplier widget";
    } else if (widgetName == "k_delete_supplier") {
        ui->stackedWidget->setCurrentIndex(2);
        supplierTable(ui->supplierTable_4);
        supplierTable(ui->supplierTable_5);
        supplierTable(ui->supplierTable_6);
        qDebug() << "Switched to delete supplier widget";
    }
}

//**************************

void SupplierWidget::on_home_clicked()
{
    emit goToHome(); // Émet le signal
}

// ********************************************************************


// TABLE ET CHARGEMENT DE TABLE ********************************************************************

void SupplierWidget::supplierTable(QTableWidget *tableWidget) {
    // Nettoie le tableau
    tableWidget->clear();
    tableWidget->setRowCount(0);

    // Définit les en-têtes des colonnes
    QStringList headers = {"ID", "Nom"};
    tableWidget->setColumnCount(headers.size());
    tableWidget->setHorizontalHeaderLabels(headers);

    // Requête SQL pour récupérer tous les Fournisseur
    QSqlQuery query("SELECT id, name FROM supplier");

    int row = 0; // Compteur pour les lignes du tableau
    while (query.next()) {
        // Récupérer les données de chaque Fournisseur
        int id = query.value("id").toInt();
        QString name = query.value("name").toString();

        // Ajouter une nouvelle ligne dans le tableau
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
        tableWidget->setItem(row, 1, new QTableWidgetItem(name));

        row++; // Passe à la ligne suivante
    }

    // Log pour débogage
    qDebug() << "Tableau des Fournisseur mis à jour avec" << row << "lignes.";
}

//**************************

void SupplierWidget::loadsupplierIntoComboBox() {
    qDebug() << "loadSupplierIntoComboBox appelée.";

    // Vérifiez que la base de données est ouverte
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Erreur : La base de données n'est pas connectée.";
        return;
    }

    QSqlQuery query;

    // Requête SQL pour récupérer les fournisseurs
    if (!query.exec("SELECT name FROM supplier")) {
        qDebug() << "Erreur lors de l'exécution de la requête SELECT :"
                 << query.lastError().text();
        return;
    }

    // Effacer les éléments existants dans le comboBox
    ui->supplierComboBox_2->clear();

    // Ajouter les fournisseurs au comboBox
    while (query.next()) {
        QString supplierName = query.value("name").toString();
        ui->supplierComboBox_2->addItem(supplierName);
    }

    qDebug() << "Fournisseurs chargées dans le comboBox.";
}

// ********************************************************************


// CRUD (ADD, MODIF, DELETE) ********************************************************************

void SupplierWidget::on_addSupplier_clicked() {

    QSqlQuery query;
    QString name = ui->supplierInput_3->text();



    // Vérifiez que l'entrée n'est pas vide
    if (name.isEmpty()) {
        qDebug() << "le fournisseur ne peut pas être vide.";
        return;
    }

    // Vérifiez si le fournisseur existe déjà dans la base de données
    query.prepare("SELECT COUNT(*) FROM supplier WHERE name = :name");
    query.bindValue(":name", name);
    if (!query.exec() || !query.next()) {
        qDebug() << "Erreur lors de la vérification de le fournisseur :"
                 << query.lastError().text();
        return;
    }
    if (query.value(0).toInt() > 0) {
        qDebug() << "Le fournisseur existe déjà dans la base de données.";
        return;
    }

    // Insérez la nouvelle le fournisseur dans la base de données
    query.prepare("INSERT INTO supplier (name) VALUES (:name)");
    query.bindValue(":name", name);
    if (!query.exec()) {
        qDebug() << "Erreur lors de l'ajout de le fournisseur :"
                 << query.lastError().text();
        return;
    }

    // Ajoutez le fournisseur dans la liste locale et le menu déroulant après succès
    customCategories.append(name);
    ui->supplierComboBox_2->addItem(name);

    qDebug() << "Nouvelle le fournisseur ajoutée avec succès :" << name;

    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
    logQuery.bindValue(":text", "Un fournisseur " + name + " a été ajouté.");
    if (logQuery.exec()) {
        qDebug() << "Log ajouté : Un fournisseur a été ajouté.";
    } else {
        qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
    }

    supplierTable(ui->supplierTable_4);
    supplierTable(ui->supplierTable_5);
    supplierTable(ui->supplierTable_6);
}

//**************************

void SupplierWidget::on_modif_3_clicked() {
    int id = ui->supplierId_3->value();

    // Préparer les nouvelles données
    QString name = ui->supplierInput_4->text();

    // Requête SQL pour mettre à jour le Fournisseur
    QSqlQuery query;
    query.prepare("UPDATE supplier SET name = :name WHERE id = :id");
    query.bindValue(":name", name);
    query.bindValue(":id", id);

    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            // Fournisseur modifié avec succès
            qDebug() << "Fournisseur modifié avec succès : ID" << id;
            ui->msg_5->setText("Fournisseur modifié avec succès.");

            QSqlQuery logQuery;
            logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
            logQuery.bindValue(":text", "Un fournisseur " + name + " a été modifié.");
            if (logQuery.exec()) {
                qDebug() << "Log ajouté : Un fournisseur a été ajouté.";
            } else {
                qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
            }

        } else {
            // Aucun Fournisseur correspondant trouvé
            qDebug() << "Aucun Fournisseur trouvé pour modification : ID" << id;
            ui->msg_5->setText("Fournisseur introuvable pour modification...");
        }
    } else {
        // Erreur lors de l'exécution de la requête
        qDebug() << "Erreur lors de la modification du Fournisseur :" << query.lastError().text();
        ui->msg_5->setText("Erreur lors de la modification du Fournisseur.");
    }

    supplierTable(ui->supplierTable_4);
    supplierTable(ui->supplierTable_5);
    supplierTable(ui->supplierTable_6);
}

//**************************

void SupplierWidget::on_deleteSupplier_2_clicked() {
    if (foundId == -1) {
        qDebug() << "Aucune Fournisseur à supprimer.";
        ui->msg_6->setText("Aucune Fournisseur sélectionné pour suppression.");
        return;
    }

    // Requête SQL pour supprimer le Fournisseur
    QSqlQuery query;
    query.prepare("DELETE FROM supplier WHERE id = :id");
    query.bindValue(":id", foundId);

    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            qDebug() << "Fournisseur supprimé avec succès. ID :" << foundId;
            ui->msg_6->setText("Fournisseur supprimé avec succès.");

            QSqlQuery logQuery;
            logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
            logQuery.bindValue(":text", "Un fournisseur " + QString::number(foundId) + " a été supprimé.");
            if (logQuery.exec()) {
                qDebug() << "Log ajouté : Un fournisseur a été ajouté.";
            } else {
                qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
            }

        } else {
            qDebug() << "Aucun Fournisseur trouvé pour suppression. ID :" << foundId;
            ui->msg_6->setText("Aucun Fournisseur trouvé pour suppression.");
        }
    } else {
        qDebug() << "Erreur lors de la suppression du Fournisseur :" << query.lastError().text();
        ui->msg_6->setText("Erreur lors de la suppression.");
    }

    supplierTable(ui->supplierTable_4);
    supplierTable(ui->supplierTable_5);
    supplierTable(ui->supplierTable_6);

}

// ********************************************************************


// FONCTIONS DE RECHERCHES ********************************************************************

void SupplierWidget::on_search_6_clicked() {
    int searchId = ui->supplierId_4->value();

    // Requête SQL pour chercher le Fournisseur
    QSqlQuery query;
    query.prepare("SELECT id, name FROM supplier WHERE id = :id");
    query.bindValue(":id", searchId);

    if (query.exec() && query.next()) {
        foundId = query.value("id").toInt();  // Mets à jour la variable membre
        QString name = query.value("name").toString();

        qDebug() << "Fournisseur trouvé :" << name << "avec ID :" << foundId;
        ui->msg_6->setText("Fournisseur trouvé : " + name);

        QSqlQuery logQuery;
        logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
        logQuery.bindValue(":text", "Un fournisseur " + name + " a été cherché pour etre supprimé.");
        if (logQuery.exec()) {
            qDebug() << "Log ajouté : Un fournisseur a été ajouté.";
        } else {
            qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
        }

    } else {
        qDebug() << "Aucun Fournisseur trouvé avec l'ID :" << searchId;
        ui->msg_6->setText("Aucun Fournisseur trouvé.");
        foundId = -1;  // Réinitialise foundId si aucun Fournisseur n'est trouvé
    }
}

//**************************

void SupplierWidget::on_search_5_clicked() {
    int searchId = ui->supplierId_3->value();

    // Requête SQL pour chercher le Fournisseur
    QSqlQuery query;
    query.prepare("SELECT id, name FROM supplier WHERE id = :id");
    query.bindValue(":id", searchId);

    if (query.exec() && query.next()) {
        foundId = query.value("id").toInt();  // Mets à jour la variable membre
        QString name = query.value("name").toString();

        ui->supplierInput_4->setText(name);

        qDebug() << "Fournisseur trouvé :" << name << "avec ID :" << foundId;
        ui->msg_5->setText("Fournisseur trouvé : " + name);

        QSqlQuery logQuery;
        logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
        logQuery.bindValue(":text", "Un fournisseur " + name + " a été cherché pour etre modifié.");
        if (logQuery.exec()) {
            qDebug() << "Log ajouté : Un fournisseur a été ajouté.";
        } else {
            qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
        }

    } else {
        qDebug() << "Aucune Fournisseur trouvée avec l'ID :" << searchId;
        ui->msg_5->setText("Aucune Fournisseur trouvée.");
        foundId = -1;  // Réinitialise foundId si aucun Fournisseur n'est trouvé
    }}

// ********************************************************************
