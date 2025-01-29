/***************************************************************************
 * Nom du fichier    : ProductsWidget.cpp
 * Auteur            : Courbeyrette Maxime
 * Date de création  : 27/01/2025
 * Description       : Gestion des stocks - Fonctions liées au CRUD des produits
 *
 *
 * Dernière modification :
 * - 27/01/2025 : Finitions des détails
 *
 *
 * Copyright (c) 2025 Courbeyrette Maxime. Tous droits réservés.
 ***************************************************************************/


#include "ProductsWidget.h"
#include "ui_ProductsWindow.h"

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

ProductsWidget::ProductsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProductsWindow)
{
    ui->setupUi(this);

    QTimer::singleShot(100, this, [this]() {
        QSqlDatabase db = QSqlDatabase::database();
        if (!db.isOpen()) {
            qDebug() << "Erreur : La base de données n'est toujours pas connectée.";
        } else {
            qDebug() << "Connexion confirmée après délai.";
            loadCategoriesIntoComboBox();
            loadsupplierIntoComboBox();

            tableProduct(ui->productsTable_2);
            tableProduct(ui->productsTable_3);
            tableProduct(ui->productsTable_4);
        }
    });


    qDebug() << "Connexion signal-slot pour on_add_clicked.";
    connect(ui->add, &QPushButton::clicked, this, &ProductsWidget::on_add_clicked);
    connect(ui->search, &QPushButton::clicked, this, &ProductsWidget::on_search_clicked);
    connect(ui->modif, &QPushButton::clicked, this, &ProductsWidget::on_modif_clicked);
    connect(ui->search, &QPushButton::clicked, this, &ProductsWidget::on_search_2_clicked);
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, [this]() {
        onButtonBoxAccepted(foundId);  // Utilise la variable membre mise à jour
    });
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &ProductsWidget::onButtonBoxRejected);

}

ProductsWidget::~ProductsWidget() {
  delete ui;
}

// REDIRECTIONS AUX PAGES ANNEXES ********************************************************************

void ProductsWidget::on_home_clicked()
{
    emit goToHome(); // Émet le signal
}

//**************************

void ProductsWidget::setCurrentWidget(const QString &widgetName)
{
    if (widgetName == "b_add") {
        ui->stackedWidget->setCurrentIndex(2);
        tableProduct(ui->productsTable_2);
        tableProduct(ui->productsTable_3);
        tableProduct(ui->productsTable_4);
        loadCategoriesIntoComboBox();
        loadsupplierIntoComboBox();
        qDebug() << "Switched to add widget";
    } else if (widgetName == "c_modif"){
        ui->stackedWidget->setCurrentIndex(1);
        tableProduct(ui->productsTable_2);
        tableProduct(ui->productsTable_3);
        tableProduct(ui->productsTable_4);
        loadCategoriesIntoComboBox();
        loadsupplierIntoComboBox();
        qDebug() << "Switched to modif widget";
    } else if (widgetName == "d_delete") {
        ui->stackedWidget->setCurrentIndex(0);
        tableProduct(ui->productsTable_2);
        tableProduct(ui->productsTable_3);
        tableProduct(ui->productsTable_4);
        loadCategoriesIntoComboBox();
        loadsupplierIntoComboBox();
        qDebug() << "Switched to delete widget";
    }
}

// ********************************************************************


// TABLE ET CHARGEMENT DE TABLE & QCOMBOBOX ********************************************************************

void ProductsWidget::tableProduct(QTableWidget *tableWidget, const QString &orderBy) {
    // Nettoie le tableau
    tableWidget->clear();
    tableWidget->setRowCount(0);

    // Définit les en-têtes des colonnes
    QStringList headers = {"ID", "QR", "Nom", "Description", "Stock", "Prix", "Catégorie", "Fournisseur"};
    tableWidget->setColumnCount(headers.size());
    tableWidget->setHorizontalHeaderLabels(headers);

    // Construire la requête SQL avec tri
    QString queryStr = "SELECT id, qr, name, description, stock, price, category, supplier FROM products";
    if (!orderBy.isEmpty()) {
        queryStr += " ORDER BY " + orderBy;
    }

    QSqlQuery query(queryStr);
    int row = 0; // Compteur pour les lignes du tableau
    while (query.next()) {
        // Récupérer les données de chaque produit
        int id = query.value("id").toInt();
        QString qr = query.value("qr").toString();
        QString name = query.value("name").toString();
        QString description = query.value("description").toString();
        int stock = query.value("stock").toInt();
        double price = query.value("price").toDouble();
        QString category = query.value("category").toString();
        QString supplier = query.value("supplier").toString();

        // Ajouter une nouvelle ligne dans le tableau
        tableWidget->insertRow(row);
        tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(id)));       // ID
        tableWidget->setItem(row, 1, new QTableWidgetItem(qr));                        // QR
        tableWidget->setItem(row, 2, new QTableWidgetItem(name));                      // Nom
        tableWidget->setItem(row, 3, new QTableWidgetItem(description));               // Description
        tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(stock))); // Ajouter la barre à la colonne "Stock"
        tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(price, 'f', 2))); // Prix
        tableWidget->setItem(row, 6, new QTableWidgetItem(category));                      // Catégorie
        tableWidget->setItem(row, 7, new QTableWidgetItem(supplier));

        row++; // Passe à la ligne suivante
    }

    // Log pour débogage
    qDebug() << "Tableau des produits mis à jour avec" << row << "lignes.";
}

//**************************

void ProductsWidget::loadCategoriesIntoComboBox() {
    qDebug() << "loadCategoriesIntoComboBox appelée.";

    // Vérifiez que la base de données est ouverte
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
    ui->categoryComboBox->clear();
    ui->category_2->clear();

    // Ajouter les catégories au comboBox
    while (query.next()) {
        QString categoryName = query.value("name").toString();
        ui->categoryComboBox->addItem(categoryName);
        ui->category_2->addItem(categoryName);
    }

    qDebug() << "Catégories chargées dans le comboBox.";
}

//**************************

void ProductsWidget::loadsupplierIntoComboBox() {
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
    ui->supplierComboBox->clear();
    ui->supplier_2->clear();

    // Ajouter les fournisseurs au comboBox
    while (query.next()) {
        QString supplierName = query.value("name").toString();
        ui->supplierComboBox->addItem(supplierName);
        ui->supplier_2->addItem(supplierName);
    }

    qDebug() << "Fournisseurs chargées dans le comboBox.";
}

// ********************************************************************


// CRUD (ADD, MODIF, DELETE) ********************************************************************

void ProductsWidget::on_add_clicked() {
    qDebug() << "Ajout d'un nouveau produit.";
    qDebug() << "Produits existants avant ajout :" << products.size();

    Product newProduct;
    newProduct.qr = ui->qr->text();
    newProduct.name = ui->name->text();
    newProduct.description = ui->description->toPlainText();
    newProduct.stock = ui->stock->value();
    newProduct.price = ui->price->value();
    newProduct.category = ui->categoryComboBox->currentText();
    newProduct.supplier = ui->supplierComboBox->currentText();

    // Vérifiez que la catégorie est valide
    if (newProduct.category.isEmpty()) {
        qDebug() << "Erreur : Aucune catégorie sélectionnée.";
        return;
    }

    // Vérifiez si le produit existe déjà
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM products WHERE qr = :qr");
    query.bindValue(":qr", newProduct.qr);

    if (query.exec() && query.next()) {
        if (query.value(0).toInt() > 0) {
            qDebug() << "Produit déjà existant, ajout ignoré :" << newProduct.qr;
            return;
        }
    } else {
        qDebug() << "Erreur lors de la vérification des doublons :" << query.lastError().text();
        return;
    }

    // Ajout du produit dans la base de données
    query.prepare("INSERT INTO products (qr, name, description, stock, price, category, supplier) "
                  "VALUES (:qr, :name, :description, :stock, :price, :category, :supplier)");
    query.bindValue(":qr", newProduct.qr);
    query.bindValue(":name", newProduct.name);
    query.bindValue(":description", newProduct.description);
    query.bindValue(":stock", newProduct.stock);
    query.bindValue(":price", newProduct.price);
    query.bindValue(":category", newProduct.category);
    query.bindValue(":supplier", newProduct.supplier);
    if (query.exec()) {
        qDebug() << "Produit ajouté dans la base de données :" << newProduct.name;

        // Récupérez l'ID généré automatiquement
        query.exec("SELECT LAST_INSERT_ID()");
        if (query.next()) {
            newProduct.id = query.value(0).toInt();
            qDebug() << "ID généré pour le nouveau produit :" << newProduct.id;
        }

        // Ajout du produit à la liste locale
        products.append(newProduct);
        qDebug() << "Produit ajouté localement. Total produits :" << products.size();

        // Ajouter une entrée dans les logs
        QSqlQuery logQuery;
        logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
        logQuery.bindValue(":text", "Un produit (" + newProduct.name + ") a été ajouté.");
        if (logQuery.exec()) {
            qDebug() << "Log ajouté : Un produit a été ajouté.";
        } else {
            qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
        }
    } else {
        qDebug() << "Erreur lors de l'ajout du produit dans la base de données :"
                 << query.lastError().text();
    }

    // Met à jour l'affichage
    tableProduct(ui->productsTable_2);
}

//**************************

void ProductsWidget::on_modif_clicked() {
    int id = ui->productId->value();

    // Préparer les nouvelles données
    QString qr = ui->qr_2->text();
    QString name = ui->name_2->text();
    QString description = ui->description_2->toPlainText();
    int stock = ui->stock_2->value();
    double price = ui->price_2->value();
    QString category = ui->category_2->currentText();
    QString supplier = ui->supplier_2->currentText();

    // Requête SQL pour mettre à jour le produit
    QSqlQuery query;
    query.prepare("UPDATE products SET qr = :qr, name = :name, description = :description, "
                  "stock = :stock, price = :price, category = :category, supplier = :supplier WHERE id = :id");
    query.bindValue(":qr", qr);
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    query.bindValue(":stock", stock);
    query.bindValue(":price", price);
    query.bindValue(":category", category);
    query.bindValue(":supplier", supplier);
    query.bindValue(":id", id);

    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            // Produit modifié avec succès
            qDebug() << "Produit modifié avec succès : ID" << id;
            ui->msg->setText("Produit modifié avec succès.");

            QSqlQuery logQuery;
            logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
            logQuery.bindValue(":text", "Un produit a été modifié (" + name + ").");
            if (logQuery.exec()) {
                qDebug() << "Log ajouté : Un produit a été ajouté.";
            } else {
                qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
            }

        } else {
            // Aucun produit correspondant trouvé
            qDebug() << "Aucun produit trouvé pour modification : ID" << id;
            ui->msg->setText("Produit introuvable pour modification...");
        }
    } else {
        // Erreur lors de l'exécution de la requête
        qDebug() << "Erreur lors de la modification du produit :" << query.lastError().text();
        ui->msg->setText("Erreur lors de la modification du produit.");
    }

    // Rafraîchir l'affichage
    tableProduct(ui->productsTable_3);
}

//**************************

void ProductsWidget::onButtonBoxAccepted(int foundId) {
    if (foundId == -1) {
        qDebug() << "Aucun produit à supprimer.";
        ui->msg_2->setText("Aucun produit sélectionné pour suppression.");
        return;
    }

    // Requête SQL pour supprimer le produit
    QSqlQuery query;
    query.prepare("DELETE FROM products WHERE id = :id");
    query.bindValue(":id", foundId);

    if (query.exec()) {
        if (query.numRowsAffected() > 0) {
            qDebug() << "Produit supprimé avec succès. ID :" << foundId;
            ui->msg_2->setText("Produit supprimé avec succès.");

            QSqlQuery logQuery;
            logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
            logQuery.bindValue(":text", "Un produit avec l'ID (" + QString::number(foundId) + ") a été supprimé.");
            if (logQuery.exec()) {
                qDebug() << "Log ajouté : Un produit a été ajouté.";
            } else {
                qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
            }

        } else {
            qDebug() << "Aucun produit trouvé pour suppression. ID :" << foundId;
            ui->msg_2->setText("Aucun produit trouvé pour suppression.");
        }
    } else {
        qDebug() << "Erreur lors de la suppression du produit :" << query.lastError().text();
        ui->msg_2->setText("Erreur lors de la suppression.");
    }
    tableProduct(ui->productsTable_4);
}

//**************************

void ProductsWidget::onButtonBoxRejected() {
    ui->stackedWidget->setCurrentIndex(1);
}

// ********************************************************************


// FONCTIONS DE RECHERCHES ********************************************************************

int ProductsWidget::SearchID(QSpinBox *productId, QLabel *msg) {
    int id = productId->value();

    // Requête SQL pour rechercher le produit par ID
    QSqlQuery query;
    query.prepare("SELECT qr, name, description, stock, price, category, supplier FROM products WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        // Produit trouvé, récupérer les données
        QString qr = query.value("qr").toString();
        QString name = query.value("name").toString();
        QString description = query.value("description").toString();
        int stock = query.value("stock").toInt();
        double price = query.value("price").toDouble();
        QString category = query.value("category").toString();
        QString supplier = query.value("supplier").toString();

        qDebug() << "Catégorie récupérée de la base de données : " << category;

        // Met à jour les champs de l'interface utilisateur
        msg->setText("Produit trouvé : " + name);
        ui->qr_2->setText(qr);
        ui->name_2->setText(name);
        ui->description_2->setText(description);
        ui->stock_2->setValue(stock);
        ui->price_2->setValue(price);

        // Vérifie si la catégorie est dans la QComboBox
        int categoryIndex = ui->category_2->findText(category);
        int supplierIndex = ui->supplier_2->findText(supplier);
        qDebug() << "Index trouvé pour la catégorie " << category << ": " << categoryIndex;

        if (categoryIndex != -1 && supplierIndex != -1) {
            ui->category_2->setCurrentText(category); // Définit directement le texte si trouvé
            ui->supplier_2->setCurrentText(supplier); // Définit directement le texte si trouvé
        } else {
            qDebug() << "Ajout de la catégorie manquante : " << category;
            ui->category_2->addItem(category);        // Ajoute la catégorie si elle n'existe pas
            ui->category_2->setCurrentText(category); // Définit ensuite la catégorie comme sélectionnée

            ui->supplier_2->addItem(supplier);        // Ajoute la catégorie si elle n'existe pas
            ui->supplier_2->setCurrentText(supplier); // Définit ensuite la catégorie comme sélectionnée
        }

        QSqlQuery logQuery;
        logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
        logQuery.bindValue(":text", "Un produit a été recherché (" + name + ") pour être modifié.");
        if (logQuery.exec()) {
            qDebug() << "Log ajouté : Un produit a été ajouté.";
        } else {
            qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
        }

        return id;  // Retourne l'ID trouvé
    } else {
        // Produit non trouvé ou erreur dans la requête
        msg->setText("Impossible de trouver le produit...");
        qDebug() << "Erreur lors de la recherche SQL :" << query.lastError().text();
        return -1;  // Retourne -1 si aucun produit n'est trouvé
    }
}

//**************************

void ProductsWidget::on_search_clicked() {
    SearchID(ui->productId, ui->msg);
}

//**************************

void ProductsWidget::on_search_2_clicked() {
    int searchId = ui->productId_2->value();

    // Requête SQL pour chercher le produit
    QSqlQuery query;
    query.prepare("SELECT id, name FROM products WHERE id = :id");
    query.bindValue(":id", searchId);

    if (query.exec() && query.next()) {
        foundId = query.value("id").toInt();  // Mets à jour la variable membre
        QString name = query.value("name").toString();

        qDebug() << "Produit trouvé :" << name << "avec ID :" << foundId;
        ui->msg_2->setText("Produit trouvé : " + name);

        QSqlQuery logQuery;
        logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
        logQuery.bindValue(":text", "Un produit a été recherché (" + name + ") pour être supprimé.");
        if (logQuery.exec()) {
            qDebug() << "Log ajouté : Un produit a été ajouté.";
        } else {
            qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
        }

    } else {
        qDebug() << "Aucun produit trouvé avec l'ID :" << searchId;
        ui->msg_2->setText("Aucun produit trouvé.");
        foundId = -1;  // Réinitialise foundId si aucun produit n'est trouvé
    }
}

// ********************************************************************
