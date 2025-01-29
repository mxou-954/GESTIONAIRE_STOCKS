/***************************************************************************
 * Nom du fichier    : CategoryWidget.cpp
 * Auteur            : Courbeyrette Maxime
 * Date de création  : 27/01/2025
 * Description       : Gestion des stocks - Fonctions liées au CRUD des catégories
 *
 *
 * Dernière modification :
 * - 27/01/2025 : Finitions des détails
 *
 *
 * Copyright (c) 2025 Courbeyrette Maxime. Tous droits réservés.
 ***************************************************************************/




#include "CategoryWidget.h"
#include "../include/ui_CategoryWindow.h"

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

CategoryWidget::CategoryWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CategoryWindow)
{
    ui->setupUi(this);

    QTimer::singleShot(100, this, [this]() {
        QSqlDatabase db = QSqlDatabase::database();
        if (!db.isOpen()) {
            qDebug() << "Erreur : La base de données n'est toujours pas connectée.";
        } else {
            qDebug() << "Connexion confirmée après délai.";
            loadCategoriesIntoComboBox();

            categoryTable(ui->categoryTable);
            categoryTable(ui->categoryTable_2);
            categoryTable(ui->categoryTable_1);

        }
    });

    connect(ui->addCategory, &QPushButton::clicked, this, &CategoryWidget::on_addCategory_clicked);
    connect(ui->search_3, &QPushButton::clicked, this, &CategoryWidget::on_search_3_clicked);
    connect(ui->modif_2, &QPushButton::clicked, this, &CategoryWidget::on_modif_2_clicked);
    connect(ui->search_4, &QPushButton::clicked, this, &CategoryWidget::on_search_4_clicked);
    connect(ui->deleteCategory, &QPushButton::clicked, this, [this]() {
        foundId = ui->categoryId_2->value();
        on_deleteCategory_clicked();
    });
}

CategoryWidget::~CategoryWidget() {
    delete ui;
}


// REDIRECTIONS AUX PAGES ANNEXES ********************************************************************

void CategoryWidget::on_home_clicked()
{
    emit goToHome();
}

//**************************

void CategoryWidget::setCurrentWidget(const QString &widgetName)
{
    if (widgetName == "e_addCategory_2") {
        ui->stackedWidget->setCurrentIndex(2);
        categoryTable(ui->categoryTable);
        categoryTable(ui->categoryTable_2);
        categoryTable(ui->categoryTable_1);
        qDebug() << "Switched to add widget";
    } else if (widgetName == "f_modifCategory_2"){
        ui->stackedWidget->setCurrentIndex(1);
        categoryTable(ui->categoryTable);
        categoryTable(ui->categoryTable_2);
        categoryTable(ui->categoryTable_1);
        qDebug() << "Switched to modif widget";
    } else if (widgetName == "g_deleteCategory_2") {
        ui->stackedWidget->setCurrentIndex(0);
        categoryTable(ui->categoryTable);
        categoryTable(ui->categoryTable_2);
        categoryTable(ui->categoryTable_1);
        qDebug() << "Switched to delete widget";
    }
}

// ********************************************************************


// TABLE ET CHARGEMENT DE TABLE ********************************************************************

void CategoryWidget::loadCategoriesIntoComboBox() {
    qDebug() << "loadCategoriesIntoComboBox appelée.";

    // Vérifier que la base de données est ouverte
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen()) {
        qDebug() << "Erreur : La base de données n'est pas connectée.";
        return;
    }

    QSqlQuery query;

    // Requête SQL pour récupérer les catégories
    if (!query.exec("SELECT name FROM category")) {
        qDebug() << "Erreur lors de l'exécution de la requête SELECT :"
                 << query.lastError().text();
        return;
    }

    // Effacer les éléments existants dans le comboBox
    ui->categoryComboBox_1->clear();

    // Ajouter les catégories au comboBox
    while (query.next()) {
        QString categoryName = query.value("name").toString();
        ui->categoryComboBox_1->addItem(categoryName);
    }

    qDebug() << "Catégories chargées dans le comboBox.";
}

//**************************

void CategoryWidget::categoryTable(QTableWidget *tableWidget) {
    // Nettoie le tableau
    tableWidget->clear();
    tableWidget->setRowCount(0);

    // Définit les en-têtes des colonnes
    QStringList headers = {"ID", "Nom"};
    tableWidget->setColumnCount(headers.size());
    tableWidget->setHorizontalHeaderLabels(headers);

    // Requête SQL pour récupérer toutes les catégories
    QSqlQuery query("SELECT id, name FROM category");

    int row = 0; // Compteur pour les lignes du tableau
    while (query.next()) {
        // Récupérer les données de chaque catégories
        int id = query.value("id").toInt();
        QString name = query.value("name").toString();

        // Ajouter une nouvelle ligne dans le tableau
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
        tableWidget->setItem(row, 1, new QTableWidgetItem(name));

        row++; // Passe à la ligne suivante
    }

    // Log pour débogage
    qDebug() << "Tableau des catégories mis à jour avec" << row << "lignes.";
}

// ********************************************************************


// CRUD (ADD, MODIF, DELETE) ********************************************************************

void CategoryWidget::on_addCategory_clicked() {
    QSqlQuery query;
    QString newCategory = ui->categoryInput->text().trimmed();

    if (newCategory.isEmpty()) {
        qDebug() << "La catégorie ne peut pas être vide.";
        return;
    }

    // Vérifie si la catégorie existe déjà dans la base de données
    query.prepare("SELECT COUNT(*) FROM category WHERE name = :name");
    query.bindValue(":name", newCategory);
    if (!query.exec() || !query.next()) {
        qDebug() << "Erreur lors de la vérification de la catégorie :"
                 << query.lastError().text();
        return;
    }
    if (query.value(0).toInt() > 0) {
        qDebug() << "La catégorie existe déjà dans la base de données.";
        return;
    }

    // Insére la nouvelle catégorie dans la base de données
    query.prepare("INSERT INTO category (name) VALUES (:name)");
    query.bindValue(":name", newCategory);
    if (!query.exec()) {
        qDebug() << "Erreur lors de l'ajout de la catégorie :"
                 << query.lastError().text();
        return;
    }

    // Ajoute la catégorie dans la liste locale et le menu déroulant après succès
    customCategories.append(newCategory);
    ui->categoryComboBox_1->addItem(newCategory);

    categoryTable(ui->categoryTable);
    categoryTable(ui->categoryTable_1);
    categoryTable(ui->categoryTable_2);

    qDebug() << "Nouvelle catégorie ajoutée avec succès :" << newCategory;

    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
    logQuery.bindValue(":text", "Une catégorie (" + newCategory + ") a été ajouté.");
    if (logQuery.exec()) {
        qDebug() << "Log ajouté : Une catégorie a été ajouté.";
    } else {
        qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
    }

}

//**************************

void CategoryWidget::on_modif_2_clicked() {
    int id = ui->categoryId->value();

    // Préparer les nouvelles données
    QString name = ui->categoryInput_2->text();

    // Requête SQL pour mettre à jour la catégorie
    QSqlQuery query;
    query.prepare("UPDATE category SET name = :name WHERE id = :id");
    query.bindValue(":name", name);
    query.bindValue(":id", id);

    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            // Catégorie modifié avec succès
            qDebug() << "Catégorie modifiée avec succès : ID" << id;
            ui->msg_3->setText("Catégorie modifié avec succès.");

            QSqlQuery logQuery;
            logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
            logQuery.bindValue(":text", "Une catégorie a été modifiée (" + name + ").");
            if (logQuery.exec()) {
                qDebug() << "Log ajouté : Une Catégorie a été ajoutée.";
            } else {
                qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
            }

        } else {
            // Aucune catégorie correspondante trouvée
            qDebug() << "Aucune catégorie trouvée pour modification : ID" << id;
            ui->msg_3->setText("Catégorie introuvable pour modification...");
        }
    } else {
        // Erreur lors de l'exécution de la requête
        qDebug() << "Erreur lors de la modification de la catégorie :" << query.lastError().text();
        ui->msg_3->setText("Erreur lors de la modification de la catégorie.");
    }

    categoryTable(ui->categoryTable);
    categoryTable(ui->categoryTable_1);
    categoryTable(ui->categoryTable_2);

}

//**************************

void CategoryWidget::on_deleteCategory_clicked() {
    if (foundId == -1) {
        qDebug() << "Aucune categorie à supprimer.";
        ui->msg_4->setText("Aucune categorie sélectionné pour suppression.");
        return;
    }
    // Requête SQL pour supprimer la catégorie
    QSqlQuery query;
    query.prepare("DELETE FROM category WHERE id = :id");
    query.bindValue(":id", foundId);
    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            qDebug() << "categorie supprimé avec succès. ID :" << foundId;
            ui->msg_4->setText("categorie supprimé avec succès.");
            QSqlQuery logQuery;
            logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
            logQuery.bindValue(":text", "Une catégorie avec l'ID (" + QString::number(foundId) + ") a été supprimé.");
            if (logQuery.exec()) {
                qDebug() << "Log ajouté : Une catégorie a été ajouté.";
            } else {
                qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
            }
        } else {
            qDebug() << "Aucun categorie trouvé pour suppression. ID :" << foundId;
            ui->msg_4->setText("Aucun categorie trouvé pour suppression.");
        }
    } else {
        qDebug() << "Erreur lors de la suppression du categorie :" << query.lastError().text();
        ui->msg_4->setText("Erreur lors de la suppression.");
    }
    categoryTable(ui->categoryTable);
    categoryTable(ui->categoryTable_1);
    categoryTable(ui->categoryTable_2);
}

// ********************************************************************


// FONCTIONS DE RECHERCHES ********************************************************************

int CategoryWidget::SearchIDCategory(QSpinBox *categoryId, QLabel *msg_3) {
    int id = categoryId->value();

    // Requête SQL pour rechercher la catégorie par ID
    QSqlQuery query;
    query.prepare("SELECT name FROM category WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        // Catégorie trouvé, mettre à jour l'interface utilisateur
        QString name = query.value("name").toString();

        // Met à jour les champs de l'interface
        msg_3->setText("Catégorie trouvé : " + name);
        ui->categoryInput_2->setText(name);

        QSqlQuery logQuery;
        logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
        logQuery.bindValue(":text", "Une catégorie a été recherché (" + name + ") pour être modifié.");
        if (logQuery.exec()) {
            qDebug() << "Log ajouté : Une Catégorie a été ajouté.";
        } else {
            qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
        }

        return id;  // Retourne l'ID trouvé
    } else {
        // Catégorie non trouvé ou erreur dans la requête
        msg_3->setText("Impossible de trouver le catégorie...");
        qDebug() << "Erreur lors de la recherche SQL :" << query.lastError().text();
        return -1;  // Retourne -1 si aucun catégorie n'est trouvé
    }
}

//**************************

void CategoryWidget::on_search_3_clicked() {
    SearchIDCategory(ui->categoryId, ui->msg_3);
}

//**************************

void CategoryWidget::on_search_4_clicked() {
    int searchId = ui->categoryId_2->value();

    // Requête SQL pour chercher la catégorie
    QSqlQuery query;
    query.prepare("SELECT id, name FROM category WHERE id = :id");
    query.bindValue(":id", searchId);

    if (query.exec() && query.next()) {
        foundId = query.value("id").toInt();  // Mets à jour la variable membre
        QString name = query.value("name").toString();

        qDebug() << "Catégorie trouvé :" << name << "avec ID :" << foundId;
        ui->msg_4->setText("Catégorie trouvé : " + name);

        QSqlQuery logQuery;
        logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
        logQuery.bindValue(":text", "Une catégorie a été recherché (" + name + ") pour être supprimé.");
        if (logQuery.exec()) {
            qDebug() << "Log ajouté : Une Catégorie a été ajouté.";
        } else {
            qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
        }

    } else {
        qDebug() << "Aucune Catégorie trouvée avec l'ID :" << searchId;
        ui->msg_4->setText("Aucune Catégorie trouvée.");
        foundId = -1;  // Réinitialise foundId si aucun Catégorie n'est trouvé
    }}

// ********************************************************************
