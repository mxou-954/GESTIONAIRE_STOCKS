/***************************************************************************
 * Nom du fichier    : mainwindow.cpp
 * Auteur            : Courbeyrette Maxime
 * Date de création  : 27/01/2025
 * Description       : Gestion des stocks - fichier principal
 *
 *
 * Dernière modification :
 * - 27/01/2025 : Finitions des détails
 *
 *
 * Copyright (c) 2025 Courbeyrette Maxime. Tous droits réservés.
 ***************************************************************************/


// IMPORTATIONS .H ANNEXES ********************************************************************

#include "mainwindow.h"
#include "ProductsWidget.h"
#include "CategoryWidget.h"
#include "SupplierWidget.h"
#include "ImportWidget.h"
#include "LogsWidget.h"
#include "statsWidget.h"

// ********************************************************************

// FICHIER GENERE PAR LA COMPILATION POUR SERVIR DE PONT ENTRE LE .H ET LE .UI ********************************************************************

#include "../include/ui_mainwindow.h"

// ********************************************************************

// IMPORTATIONS VIA QT ********************************************************************

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

// ********************************************************************

MainWindow::MainWindow(QWidget *parent)

    // CONNEXION AUX DIFFERENTS .H ********************************************************************

    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , productsWidget(new ProductsWidget(this))
    , categoryWidget(new CategoryWidget(this))
    , supplierWidget(new SupplierWidget(this))
    , importWidget(new ImportWidget(this))
    , logsWidget(new LogsWidget(this))
    , statsWidget(new StatsWidget(this))

    // ********************************************************************

{
    ui->setupUi(this);

    // TIMER POUR LANCER LES FILTRES APRES LA CONNEXION A LA DB ********************************************************************

    QTimer::singleShot(100, this, [this]() {
        QSqlDatabase db = QSqlDatabase::database();
        if (!db.isOpen()) {
            qDebug() << "Erreur : La base de données n'est toujours pas connectée.";
        } else {
            qDebug() << "Connexion confirmée après délai.";

            filter();
            ui->filter->addItem("aucun");
            ui->filter->addItem("nom : a - z");
            ui->filter->addItem("nom : z - a");
            ui->filter->addItem("id : 0 - 1");
            ui->filter->addItem("id : 1 - 0");
            ui->filter->addItem("catégorie : a - z");
            ui->filter->addItem("catégorie : z - a");
            ui->filter->addItem("stock : 0 - 1");
            ui->filter->addItem("stock : 1 - 0");
            ui->filter->addItem("prix : 0 - 1");
            ui->filter->addItem("prix : 1 - 0");

            tableProduct(ui->productsTable);

        }
    });

    // ********************************************************************

    // TIMER POUR LANCER LA DETECTIONS DES STOCKS LOW APRES LA CONNEXION A LA DB ********************************************************************

    QTimer::singleShot(200, this, [this]() {
        checkLowStock();
    });

    // ********************************************************************

    // FERMETURE DES WIDGETS ANNEXES POUR NE MONTRER QUE LA HOME PAGE ********************************************************************

    productsWidget->hide();
    categoryWidget->hide();
    supplierWidget->hide();
    importWidget->hide();
    logsWidget->hide();
    statsWidget->hide();

    // ********************************************************************

    // GESTION DES SIGNAUX ENTRE LE .H ET .CPP ********************************************************************

    connect(ui->export_button, &QPushButton::clicked, this, &MainWindow::on_export_button_clicked);
    connect(ui->export_button_2, &QPushButton::clicked, this, &MainWindow::on_export_button_2_clicked);
    connect(ui->filter, &QComboBox::currentIndexChanged, this, &MainWindow::filter);
    connect(ui->searchProduct, &QLineEdit::textChanged, this, &MainWindow::searchProducts);


    connect(productsWidget, &ProductsWidget::goToHome, this, &MainWindow::showHomePage);
    connect(categoryWidget, &CategoryWidget::goToHome, this, &MainWindow::showHomePage);
    connect(supplierWidget, &SupplierWidget::goToHome, this, &MainWindow::showHomePage);
    connect(logsWidget, &LogsWidget::goToHome, this, &MainWindow::showHomePage);
    connect(importWidget, &ImportWidget::goToHome, this, &MainWindow::showHomePage);
    connect(statsWidget, &StatsWidget::goToHome, this, &MainWindow::showHomePage);

    connect(ui->buttonAddProduct, &QPushButton::clicked, this, &MainWindow::showProductAddPage);
    connect(ui->buttoModifProduct, &QPushButton::clicked, this, &MainWindow::showProductModifPage);
    connect(ui->buttonDeleteProduct, &QPushButton::clicked, this, &MainWindow::showProductDeletePage);

    connect(ui->buttonAddCategory_2, &QPushButton::clicked, this, &MainWindow::showCategoryAddPage);
    connect(ui->buttonModifCategory, &QPushButton::clicked, this, &MainWindow::showCategoryModifPage);
    connect(ui->buttonDeleteCategory, &QPushButton::clicked, this, &MainWindow::showCategoryDeletePage);

    connect(ui->buttonAddCategory, &QPushButton::clicked, this, &MainWindow::showSupplierAddPage);
    connect(ui->buttonModifCategory_2, &QPushButton::clicked, this, &MainWindow::showSupplierModifPage);
    connect(ui->buttonDeleteCategory_2, &QPushButton::clicked, this, &MainWindow::showSupplierDeletePage);

    connect(ui->import_button, &QPushButton::clicked, this, &MainWindow::showImportCSVPage);
    connect(ui->import_button_2, &QPushButton::clicked, this, &MainWindow::showImportJSONPage);

    connect(ui->logs, &QPushButton::clicked, this, &MainWindow::showLogsPage);

    connect(ui->stats, &QPushButton::clicked, this, &MainWindow::showStatsPage);

    // ********************************************************************
}

// SUPPRESSION DES FORMS ********************************************************************

MainWindow::~MainWindow()
{
    delete ui;
    delete productsWidget;
    delete categoryWidget;
    delete supplierWidget;
    delete importWidget;
    delete logsWidget;
    delete statsWidget;

}

// ********************************************************************

// REDIRECTIONS AUX PAGES ANNEXES ********************************************************************

void MainWindow::showHomePage()
{
    productsWidget->hide();
    categoryWidget->hide();
    supplierWidget->hide();
    importWidget->hide();
    logsWidget->hide();
    statsWidget->hide();
    ui->centralwidget->show();
    tableProduct(ui->productsTable);
}


void MainWindow::showProductAddPage()
{
    ui->centralwidget->hide();
    productsWidget->setCurrentWidget("b_add");
    productsWidget->show();
    productsWidget->raise();
}

void MainWindow::showProductModifPage()
{
    ui->centralwidget->hide();
    productsWidget->setCurrentWidget("c_modif");
    productsWidget->show();
    productsWidget->raise();
}

void MainWindow::showProductDeletePage()
{
    ui->centralwidget->hide();
    productsWidget->setCurrentWidget("d_delete");
    productsWidget->show();
    productsWidget->raise();
}

void MainWindow::showCategoryAddPage()
{
    ui->centralwidget->hide();
    categoryWidget->setCurrentWidget("e_addCategory_2");
    categoryWidget->show();
    categoryWidget->raise();
}

void MainWindow::showCategoryModifPage()
{
    ui->centralwidget->hide();
    categoryWidget->setCurrentWidget("f_modifCategory_2");
    categoryWidget->show();
    categoryWidget->raise();
}

void MainWindow::showCategoryDeletePage()
{
    ui->centralwidget->hide();
    categoryWidget->setCurrentWidget("g_deleteCategory_2");
    categoryWidget->show();
    categoryWidget->raise();
}

void MainWindow::showSupplierAddPage()
{
    ui->centralwidget->hide();
    supplierWidget->setCurrentWidget("i_add_supplier");
    supplierWidget->show();
    supplierWidget->raise();
}

void MainWindow::showSupplierModifPage()
{
    ui->centralwidget->hide();
    supplierWidget->setCurrentWidget("j_modif_supplier");
    supplierWidget->show();
    supplierWidget->raise();
}

void MainWindow::showSupplierDeletePage()
{
    ui->centralwidget->hide();
    supplierWidget->setCurrentWidget("k_delete_supplier");
    supplierWidget->show();
    supplierWidget->raise();
}

void MainWindow::showImportCSVPage()
{
    ui->centralwidget->hide();
    importWidget->setCurrentWidget("l_importXLSX");
    importWidget->show();
    importWidget->raise();
}

void MainWindow::showImportJSONPage()
{
    ui->centralwidget->hide();
    importWidget->setCurrentWidget("m_importJSON");
    importWidget->show();
    importWidget->raise();
}

void MainWindow::showLogsPage()
{
    ui->centralwidget->hide();
    logsWidget->setCurrentWidget("h_LOGS");
    logsWidget->show();
    logsWidget->raise();
}

void MainWindow::showStatsPage()
{
    ui->centralwidget->hide();
    statsWidget->setCurrentWidget("Statistiques");
    statsWidget->show();
    statsWidget->raise();
}

// ********************************************************************

// AFFICHAGE DES PRODUITS DANS UN QTABLEWIDGET ********************************************************************

void MainWindow::tableProduct(QTableWidget *tableWidget, const QString &orderBy) {
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

// ********************************************************************

// FONCTION D'EXPORTATIONS CSV ********************************************************************

void MainWindow::on_export_button_clicked() {
    // Créer un nouveau document Excel
    OpenXLSX::XLDocument doc;
    doc.create("./rapport_stocks.xlsx", OpenXLSX::XLForceOverwrite);
    auto wks = doc.workbook().worksheet("Sheet1");

    // Ajouter les en-têtes de colonnes
    wks.cell("A1").value() = "id";
    wks.cell("B1").value() = "qr";
    wks.cell("C1").value() = "name";
    wks.cell("D1").value() = "description";
    wks.cell("E1").value() = "stock";
    wks.cell("F1").value() = "price";
    wks.cell("G1").value() = "category";
    wks.cell("H1").value() = "supplier";

    // Préparer la requête SQL
    QSqlQuery query;
    query.prepare("SELECT * FROM products;");
    query.exec();

    int row = 2; // Compteur pour les lignes du tableau
    while (query.next()) {
        // Récupérer les données de chaque produit
        int id = query.value("id").toInt();
        QString qr = query.value("qr").toString();
        QString name = query.value("name").toString();
        QString description = query.value("description").toString();
        int stock = query.value("stock").toInt();
        double price = query.value("price").toDouble();
        int category = query.value("category").toInt();
        int supplier = query.value("supplier").toInt();

        wks.cell(QString("A%1").arg(row).toStdString()).value() = static_cast<double>(id);         // Colonne A
        wks.cell(QString("B%1").arg(row).toStdString()).value() = qr.toStdString();                 // Colonne B
        wks.cell(QString("C%1").arg(row).toStdString()).value() = name.toStdString();               // Colonne C
        wks.cell(QString("D%1").arg(row).toStdString()).value() = description.toStdString();        // Colonne D
        wks.cell(QString("E%1").arg(row).toStdString()).value() = static_cast<double>(stock);        // Colonne E
        wks.cell(QString("F%1").arg(row).toStdString()).value() = price;                             // Colonne F
        wks.cell(QString("G%1").arg(row).toStdString()).value() = static_cast<double>(category);     // Colonne G
        wks.cell(QString("H%1").arg(row).toStdString()).value() = static_cast<double>(supplier);     // Colonne G

        row++; // Passe à la ligne suivante
    }

    // Sauvegarder et fermer le document
    doc.save();
    doc.close();

    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
    logQuery.bindValue(":text", "Un rapport excel a été généré.");
    if (logQuery.exec()) {
        qDebug() << "Log ajouté : un rapport excel a été généré.";
    } else {
        qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
    }
}

// ********************************************************************

// FONCTION D'EXPORTATIONS JSON ********************************************************************

void MainWindow::on_export_button_2_clicked() {
    QSqlQuery query;
    query.prepare("SELECT * FROM products;");
    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête :" << query.lastError().text();
        return;
    }

    // Créer un tableau JSON pour stocker les produits
    QJsonArray productsArray;

    while (query.next()) {
        // Récupérer les données de chaque produit
        QJsonObject productObject;
        productObject["id"] = query.value("id").toInt();
        productObject["qr"] = query.value("qr").toString();
        productObject["name"] = query.value("name").toString();
        productObject["description"] = query.value("description").toString();
        productObject["stock"] = query.value("stock").toInt();
        productObject["price"] = query.value("price").toDouble();
        productObject["category"] = query.value("category").toInt();
        productObject["supplier"] = query.value("supplier").toInt();

        // Ajouter l'objet produit au tableau JSON
        productsArray.append(productObject);
    }

    // Créer un objet JSON racine
    QJsonObject rootObject;
    rootObject["products"] = productsArray;

    // Convertir l'objet JSON en document JSON
    QJsonDocument jsonDoc(rootObject);

    // Sauvegarder le document JSON dans un fichier
    QFile file("./rapport_stocks.json");
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Erreur lors de l'ouverture du fichier JSON pour l'écriture :"
                 << file.errorString();
        return;
    }

    file.write(jsonDoc.toJson(QJsonDocument::Indented));
    file.close();

    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
    logQuery.bindValue(":text", "Un rapport JSON a été généré.");
    if (logQuery.exec()) {
        qDebug() << "Log ajouté : Un rapport json a été généré.";
    } else {
        qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
    }

    qDebug() << "Exportation des données en JSON réussie.";
}

// ********************************************************************

// FONCTION DE FILTRAGE SUR LE QTABLEWIDGET SPECIFIQUE AUX PRODUITS ********************************************************************

void MainWindow::filter() {
    QString choix = ui->filter->currentText();
    QString orderBy;

    // Définir le tri en fonction du choix
    if (choix == "nom : a - z") {
        orderBy = "name ASC"; // Tri alphabétique croissant par le champ "name"
    } else if (choix == "nom : z - a") {
        orderBy = "name DESC"; // Tri alphabétique décroissant par le champ "name"
    } else if (choix == "id : 0 - 1") {
        orderBy = "id ASC"; // Tri croissant par ID
    } else if (choix == "id : 1 - 0") {
        orderBy = "id DESC"; // Tri décroissant par ID
    } else if (choix == "catégorie : a - z") {
        orderBy = "category ASC"; // Tri alphabétique croissant par catégorie
    } else if (choix == "catégorie : z - a") {
        orderBy = "category DESC"; // Tri alphabétique décroissant par catégorie
    } else if (choix == "stock : 0 - 1") {
        orderBy = "stock ASC"; // Tri croissant par stock
    } else if (choix == "stock : 1 - 0") {
        orderBy = "stock DESC"; // Tri décroissant par stock
    } else if (choix == "prix : 0 - 1") {
        orderBy = "price ASC"; // Tri croissant par prix
    } else if (choix == "prix : 1 - 0") {
        orderBy = "price DESC"; // Tri décroissant par prix
    } else if (choix == "fournisseur : 1 - 0") {
        orderBy = "supplier ASC"; // Tri décroissant par fournisseur
    } else if (choix == "fournisseur : 1 - 0") {
        orderBy = "supplier DESC"; // Tri décroissant par fournisseur
    } else {
        orderBy = ""; // Pas de tri
    }

    // Mettre à jour toutes les tables
    tableProduct(ui->productsTable, orderBy);

    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
    logQuery.bindValue(":text", "Un filtre a été choisi : " + choix +".");
    if (logQuery.exec()) {
        qDebug() << "Log ajouté : Une catégorie a été ajouté.";
    } else {
        qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
    }
}

// ********************************************************************

// FONCTION DE RECHERCHE DES PRODUITS SUR LE QTABLEWIDGET ********************************************************************

void MainWindow::searchProducts() {
    QString searchText = ui->searchProduct->text().trimmed();

    // Si aucun texte n'est saisi, afficher un message ou réinitialiser le tableau
    if (searchText.isEmpty()) {
        tableProduct(ui->productsTable);
    }

    // Construire la requête SQL pour une recherche partielle
    QSqlQuery query;
    query.prepare("SELECT id, name, description, stock, price, category, supplier "
                  "FROM products WHERE name LIKE :searchText");
    query.bindValue(":searchText", "%" + searchText + "%"); // Utilise le joker %

    // Exécuter la requête et afficher les résultats
    if (query.exec()) {
        ui->productsTable->clear();
        ui->productsTable->setRowCount(0);

        // Parcourir les résultats
        int row = 0;
        while (query.next()) {
            int id = query.value("id").toInt();
            QString name = query.value("name").toString();
            QString description = query.value("description").toString();
            int stock = query.value("stock").toInt();
            double price = query.value("price").toDouble();
            QString category = query.value("category").toString();
            QString supplier = query.value("supplier").toString();

            // Ajouter les résultats au tableau
            ui->productsTable->insertRow(row);
            ui->productsTable->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
            ui->productsTable->setItem(row, 1, new QTableWidgetItem(name));
            ui->productsTable->setItem(row, 2, new QTableWidgetItem(description));
            ui->productsTable->setItem(row, 3, new QTableWidgetItem(QString::number(stock)));
            ui->productsTable->setItem(row, 4, new QTableWidgetItem(QString::number(price, 'f', 2)));
            ui->productsTable->setItem(row, 5, new QTableWidgetItem(category));
            ui->productsTable->setItem(row, 6, new QTableWidgetItem(supplier));

            row++;
        }

        // Log des résultats pour débogage
        qDebug() << "Produits trouvés avec le texte '" << searchText << "' : " << row;

        QSqlQuery logQuery;
        logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
        logQuery.bindValue(":text", "Une recherche a été faite dans le tableau des produits.");
        if (logQuery.exec()) {
            qDebug() << "Log ajouté : Une catégorie a été ajouté.";
        } else {
            qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
        }

    } else {
        qDebug() << "Erreur lors de la recherche de produits :" << query.lastError().text();
    }
}

// ********************************************************************

// FONCTION DE NOTIFICATION EN CAS DE STOCK BAS ********************************************************************

void MainWindow::checkLowStock() {
    QSqlQuery query("SELECT name, stock FROM products WHERE stock < 10");
    QStringList lowStockItems;

    while (query.next()) {
        QString name = query.value("name").toString();
        int stock = query.value("stock").toInt();
        lowStockItems.append(name + " (Stock: " + QString::number(stock) + ")");
    }

    if (!lowStockItems.isEmpty()) {
        QMessageBox *msgBox = new QMessageBox(QMessageBox::Warning, "Alerte Stock Faible",
                                              "Les produits suivants ont un stock inférieur à 10 :\n" +
                                                  lowStockItems.join("\n"), QMessageBox::Ok, this);
        msgBox->setWindowModality(Qt::ApplicationModal); // Assure que la boîte bloque l'interaction avec la fenêtre principale
        msgBox->raise();                                 // Amène la boîte au premier plan
        msgBox->activateWindow();                        // Active la boîte de dialogue
        msgBox->exec();                                  // Affiche la boîte de dialogue
    }

    QSqlQuery logQuery;
    logQuery.prepare("INSERT INTO logs (text) VALUES (:text)");
    logQuery.bindValue(":text", "Une alerte de stock bas a été observé.");
    if (logQuery.exec()) {
        qDebug() << "Log ajouté : Une catégorie a été ajouté.";
    } else {
        qDebug() << "Erreur lors de l'ajout dans les logs :" << logQuery.lastError().text();
    }
}

// ********************************************************************
