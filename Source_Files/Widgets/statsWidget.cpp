/***************************************************************************
 * Nom du fichier    : statsWidget.cpp
 * Auteur            : Courbeyrette Maxime
 * Date de création  : 28/01/2025
 * Description       : Gestion des stocks - Fonctions liées a l'affichage des statistiques
 *
 *
 * Dernière modification :
 * - 28/01/2025 : Création et ajout des statistiques
 * - 29/01/2025 : Ajout du graphique produit / temps
 *
 * Copyright (c) 2025 Courbeyrette Maxime. Tous droits réservés.
 ***************************************************************************/




#include "statsWidget.h"
#include "../include/ui_StatsWindow.h"

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

#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>


StatsWidget::StatsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StatsWindow)
    , chartView(new QChartView(this))

{
    ui->setupUi(this);


    QTimer::singleShot(100, this, [this]() {
        QSqlDatabase db = QSqlDatabase::database();
        if (!db.isOpen()) {
            qDebug() << "Erreur : La base de données n'est toujours pas connectée.";
        } else {
            qDebug() << "Connexion confirmée après délai.";

            searchStatsProductCategorySupplier();
            countPriceStock();
            productAndTime();

            // Récupérer les produits par catégorie
            QMap<QString, int> productCategoryCounts;
            QSqlQuery productQuery("SELECT category, COUNT(*) FROM products GROUP BY category");
            if (productQuery.exec()) {
                while (productQuery.next()) {
                    QString category = productQuery.value(0).toString();
                    int count = productQuery.value(1).toInt();
                    productCategoryCounts[category] = count;
                }
            } else {
                qDebug() << "Erreur lors de l'exécution de la requête pour les produits :" << productQuery.lastError().text();
            }

            // Récupérer les fournisseurs par type
            QMap<QString, int> supplierTypeCounts;
            QSqlQuery supplierQuery("SELECT supplier, COUNT(*) FROM products GROUP BY supplier");
            if (supplierQuery.exec()) {
                while (supplierQuery.next()) {
                    QString type = supplierQuery.value(0).toString();
                    int count = supplierQuery.value(1).toInt();
                    supplierTypeCounts[type] = count;
                }
            } else {
                qDebug() << "Erreur lors de l'exécution de la requête pour les fournisseurs :" << supplierQuery.lastError().text();
            }

            // **********************
            // Graphique pour les produits par catégorie
            // **********************
            QPieSeries *productSeries = new QPieSeries();
            for (auto it = productCategoryCounts.begin(); it != productCategoryCounts.end(); ++it) {
                productSeries->append(it.key(), it.value());
            }

            QChart *productChart = new QChart();
            productChart->addSeries(productSeries);
            productChart->setTitle("Produits par catégorie");
            productChart->legend()->setAlignment(Qt::AlignBottom);

            QChartView *productChartView = new QChartView(productChart);
            productChartView->setRenderHint(QPainter::Antialiasing);

            if (ui->Layout) {
                ui->Layout->addWidget(productChartView);
            } else {
                qDebug() << "Erreur : Le layout 'Layout' n'existe pas.";
            }

            // **********************
            // Graphique pour les fournisseurs par type
            // **********************
            QPieSeries *supplierSeries = new QPieSeries();
            for (auto it = supplierTypeCounts.begin(); it != supplierTypeCounts.end(); ++it) {
                supplierSeries->append(it.key(), it.value());
            }

            QChart *supplierChart = new QChart();
            supplierChart->addSeries(supplierSeries);
            supplierChart->setTitle("Produits par fournisseurs");
            supplierChart->legend()->setAlignment(Qt::AlignBottom);

            QChartView *supplierChartView = new QChartView(supplierChart);
            supplierChartView->setRenderHint(QPainter::Antialiasing);

            if (ui->Layout2) {
                ui->Layout2->addWidget(supplierChartView);
            } else {
                qDebug() << "Erreur : Le layout 'Layout2' n'existe pas.";
            }
        }
    });

}

StatsWidget::~StatsWidget() {
    delete ui;
}



// REDIRECTIONS AUX PAGES ANNEXES ********************************************************************

void StatsWidget::setCurrentWidget(const QString &widgetName)
{
    if (widgetName == "Statistiques") {
        ui->stackedWidget->setCurrentIndex(0);
        qDebug() << "Switched to stats";
    }
}

//**************************

void StatsWidget::on_home_clicked()
{
    emit goToHome(); // Émet le signal
}

// ********************************************************************


// CREATION DES GRAPHIQUES PRODUITS ********************************************************************

void StatsWidget::searchStatsProductCategorySupplier() {
    QSqlQuery query("SELECT COUNT(*), COUNT(DISTINCT category), COUNT(DISTINCT supplier) FROM products");

    if (query.exec() && query.next()) {
        int totalProducts = query.value(0).toInt();  // Nombre total de produits
        int totalCategories = query.value(1).toInt();  // Nombre de catégories distinctes
        int totalSuppliers = query.value(2).toInt();  // Nombre de fournisseurs distincts

        ui->nbProduit->setText(QString::number(totalProducts));
        ui->nbCategory->setText(QString::number(totalCategories));
        ui->nbFournisseurs->setText(QString::number(totalSuppliers));
    } else {
        qDebug() << "Erreur SQL :" << query.lastError().text();
    }
}

//**************************

void StatsWidget::countPriceStock() {
    QSqlQuery query("SELECT SUM(price), SUM(stock) FROM products");

    if (query.exec() && query.next()) {
        double totalPrice = query.value(0).toDouble();  // Somme des prix
        int totalStock = query.value(1).toInt();  // Somme des quantités en stock

        // Afficher dans l'interface utilisateur
        ui->nbPrix->setText(QString::number(totalPrice, 'f', 2) + " $");  // Afficher avec 2 décimales
        ui->nbStock->setText(QString::number(totalStock));
    } else {
        qDebug() << "Erreur SQL :" << query.lastError().text();
    }
}

//**************************

void StatsWidget::productAndTime() {
    // Requête SQL pour récupérer les produits par jour
    QSqlQuery query("SELECT DATE(created_at) AS day, COUNT(*) AS product_count FROM products GROUP BY day ORDER BY day");

    QBarSeries *series = new QBarSeries();
    QBarSet *set = new QBarSet("Produits");

    QStringList categories;

    // Vérifier si la requête est exécutée avec succès
    if (query.exec()) {
        while (query.next()) {
            QString day = query.value(0).toString();
            int productCount = query.value(1).toInt();

            // Ajouter une barre pour chaque jour
            *set << productCount;

            // Ajouter le jour à la liste des catégories
            categories << day;
        }

        // Ajouter les données à la série
        series->append(set);

        // Créer le graphique
        QChart *chart = new QChart();
        chart->addSeries(series);

        // Définir les catégories pour l'axe X (dates)
        QBarCategoryAxis *axisX = new QBarCategoryAxis();
        axisX->append(categories);
        chart->setAxisX(axisX, series);

        // Calculer la valeur maximale sur le QBarSet
        qreal maxProductCount = 0;
        for (int i = 0; i < set->count(); ++i) {
            maxProductCount = std::max(maxProductCount, set->at(i));  // Comparaison avec qreal
        }

        // Définir l'axe Y pour les produits avec un incrément de 2 en 2
        QValueAxis *axisY = new QValueAxis();
        axisY->setRange(0, maxProductCount);  // Plage de l'axe Y
        axisY->setTickCount(6);  // Nombre de ticks
        axisY->setMinorTickCount(1);  // Sous-divisions pour les ticks
        axisY->setLabelFormat("%d");
        chart->setAxisY(axisY, series);

        // Créer et configurer le graphique
        chart->setTitle("Produits ajoutés par jour");
        chart->setAnimationOptions(QChart::SeriesAnimations);

        // Affichage du graphique dans un QChartView
        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        ui->Layout3->addWidget(chartView);
    } else {
        qDebug() << "Erreur lors de l'exécution de la requête SQL : " << query.lastError().text();
    }
}

// ********************************************************************








