#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "itemselectiondialog.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <QKeyEvent>
#include <algorithm>
#include <QDateTime>
#include <QDir>
#include <QFile>

// CONSTRUCTOR
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , stockModel(new StockModel(this))
    , transactionModel(new TransactionModel(this))
    , confirmedTransactionModel(new ConfirmedTransactionModel(this))
    , analyticsModel(new AnalyticsModel(this))
    , howMuchModel(new HowMuchModel(this))
{
    ui->setupUi(this);
    
    // Set up the main buttons for tabs
    QVector<QPair<QPushButton*, QWidget*>> tabMappings = {
        {ui->salesButton, ui->salesTab},
        {ui->transactionsButton, ui->transactionsTab},
        {ui->loggingButton, ui->loggingTab},
        {ui->analyticsButton, ui->analyticsTab}
    };
    for (const auto& pair : tabMappings) {
        connect(pair.first, &QPushButton::clicked, this, [=]() {
            ui->stackedWidget->setCurrentWidget(pair.second);
        });
    }

    // Set up the table view for stock
    ui->stockTableView->setModel(stockModel);
    ui->stockTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->stockTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // Set up the table view for transactions
    ui->transactionTableView->setModel(transactionModel);
    ui->transactionTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Set up the analytics list views
    ui->productsToListView->setModel(analyticsModel);
    ui->howMuchToListView->setModel(howMuchModel);
    
    // Connect signals and slots for stock management
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddButtonClicked);
    connect(ui->editButton, &QPushButton::clicked, this, &MainWindow::onEditButtonClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteButtonClicked);
    connect(ui->filterLineEdit, &QLineEdit::textChanged, this, &MainWindow::onFilterTextChanged);

    // Connect signals and slots for first tab
    connect(ui->manualAddButton, &QPushButton::clicked, this, &MainWindow::onManualAddClicked);
    connect(ui->backupButton, &QPushButton::clicked, this, &MainWindow::onBackupButtonClicked);
    connect(ui->restoreButton, &QPushButton::clicked, this, &MainWindow::onRestoreButtonClicked);

    // Connect signals and slots for transaction management
    connect(ui->confirmButton, &QPushButton::clicked, this, &MainWindow::onConfirmTransactionClicked);
    connect(ui->deleteTransactionButton, &QPushButton::clicked, this, &MainWindow::onDeleteTransactionClicked);

    // Connect signals and slots for analytics
    connect(ui->timePeriodComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onTimePeriodChanged);
    connect(ui->daysToStockSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onDaysToStockChanged);

    // Initialize analytics with current data
    onTimePeriodChanged(0); // This will load data for LastWeek period

    // Icons and Shapes
    struct ButtonStyle {
        QPushButton* button;
        QString iconPath;
    };

    QList<ButtonStyle> styledButtons = {
        {ui->salesButton, ":/resources/clear-sales.png"},
        {ui->transactionsButton, ":/resources/clear-transactions.png"},
        {ui->loggingButton, ":/resources/clear-logging.png"},
        {ui->analyticsButton, ":/resources/clear-analysis.png"}
    };

    for (const auto& item : styledButtons) {
        item.button->setStyleSheet(QString(R"(
        QPushButton {
            background-color: transparent;
            border: none;
            qproperty-icon: url(%1);
            qproperty-iconSize: 50px 50px;
        }
        QPushButton:hover {
            background-color: rgba(255, 255, 255, 30);
            border-radius: 10px;
        }
    )").arg(item.iconPath));
    }
}

// DESTRUCTOR
MainWindow::~MainWindow() {
    delete ui;
    delete stockModel;
    delete transactionModel;
    delete confirmedTransactionModel;
    delete analyticsModel;
    delete howMuchModel;
}

// TAB 3
void MainWindow::onAddButtonClicked() {
    bool ok;
    QString productName;
    double price = 0.0;
    int stock = 0;

    // Shared stylesheet
    QString inputDialogStyle = R"(
        QInputDialog {
            background-color: rgb(211, 211, 211);
            font: 700 9pt "Montserrat";
            color: black;
            border: 1px solid rgb(220, 220, 220);
            border-radius: none;
        }
        QLineEdit {
            background-color: white;
            border: 1px solid rgb(200, 200, 200);
            padding: 4px;
            font: 700 9pt "Montserrat";
            color: black;
        }
        QLabel {
            background-color: rgb(0, 71, 255);
            padding: 4px;
            border-radius: 2px;
            font: 700 9pt "Montserrat";
            color: rgb(255, 255, 255);
        }
        QPushButton {
            background-color: rgb(255, 255, 255);
            font: 700 9pt "Montserrat";
            color: rgb(0, 71, 255);
            border: none;
            border-radius: 5px;
            padding: 4px 10px;
        }
        QPushButton:hover {
            background-color: rgb(0,71,255);
            border: 1px solid rgb(0, 71, 255);
            color: rgb(255,255,255);
        }
        QSpinBox {
            color: black;
            background-color: white;
            font: 700 9pt "Montserrat";
            border: 1px solid rgb(200, 200, 200);
            border-radius: 4px;
        }
        QDoubleSpinBox {
            color: black;
            background-color: white;
            font: 700 9pt "Montserrat";
            border: 1px solid rgb(200, 200, 200);
            border-radius: 4px;
        }

    )";

    // Product Name
    QInputDialog nameDialog(this);
    nameDialog.setWindowTitle("Add Product");
    nameDialog.setLabelText("Product Name:");
    nameDialog.setTextValue("");
    nameDialog.setStyleSheet(inputDialogStyle);

    if (nameDialog.exec() == QDialog::Accepted) {
        productName = nameDialog.textValue();
        if (productName.isEmpty())
            return;
    } else {
        return;
    }

    // Price
    QInputDialog priceDialog(this);
    priceDialog.setWindowTitle("Add Product");
    priceDialog.setLabelText("Price:");
    priceDialog.setInputMode(QInputDialog::DoubleInput);
    priceDialog.setDoubleDecimals(2);
    priceDialog.setDoubleRange(0.0, 1000000.0);
    priceDialog.setDoubleValue(0.0);
    priceDialog.setStyleSheet(inputDialogStyle);

    if (priceDialog.exec() == QDialog::Accepted) {
        price = priceDialog.doubleValue();
    } else {
        return;
    }

    // Stock
    QInputDialog stockDialog(this);
    stockDialog.setWindowTitle("Add Product");
    stockDialog.setLabelText("Initial Stock:");
    stockDialog.setInputMode(QInputDialog::IntInput);
    stockDialog.setIntRange(0, 1000000);
    stockDialog.setIntValue(0);
    stockDialog.setStyleSheet(inputDialogStyle);

    if (stockDialog.exec() == QDialog::Accepted) {
        stock = stockDialog.intValue();
    } else {
        return;
    }

    // Prepare new StockItem
    StockItem item;
    std::vector<int> ids;
    for (int i = 0; i < this->stockModel->rowCount(); i++) {
        ids.push_back(this->stockModel->getItem(i).id);
    }

    int newId = 1;
    for (int i = 0; i < ids.size(); i++) {
        if (std::find(ids.begin(), ids.end(), newId) != ids.end()) {
            newId++;
        } else {
            break;
        }
    }

    item.id = newId;
    item.productName = productName;
    item.price = price;
    item.stock = stock;
    item.remaining = stock;
    item.sold = 0;

    stockModel->addItem(item);
}


void MainWindow::onEditButtonClicked()
{
    // Local shared stylesheet
    QString inputDialogStyle = R"(
        QInputDialog {
            background-color: rgb(211,211,211);
            font: 700 9pt "Montserrat";
            color: black;
            border: 1px solid rgb(220,220,220);
        }
        QLineEdit {
            background-color: white;
            border: 1px solid rgb(200,200,200);
            padding: 4px;
            font: 700 9pt "Montserrat";
            color: black;
        }
        QLabel {
            background-color: rgb(0,71,255);
            padding: 4px;
            border-radius: 2px;
            font: 700 9pt "Montserrat";
            color: rgb(255,255,255);
        }
        QPushButton {
            background-color: rgb(255,255,255);
            font: 700 9pt "Montserrat";
            color: rgb(0,71,255);
            border: none;
            border-radius: 5px;
            padding: 4px 10px;
        }
        QPushButton:hover {
            background-color: rgb(0,71,255);
            border: 1px solid rgb(0,71,255);
            color: rgb(255,255,255);
        }
        QSpinBox, QDoubleSpinBox {
            color: black;
            background-color: white;
            font: 700 9pt "Montserrat";
            border: 1px solid rgb(200,200,200);
            border-radius: 4px;
        }
    )";


    QModelIndex currentIndex = ui->stockTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox warningBox(this);
        warningBox.setWindowTitle("Edit");
        warningBox.setText("Please select an item to edit. ✋");
        warningBox.setIcon(QMessageBox::Warning);
        warningBox.setStandardButtons(QMessageBox::Ok);

        // Unique styleSheet
        warningBox.setStyleSheet(R"(
            QLabel {
                color: rgb(255,255,255);
            }
            QMessageBox {
                background-color: rgb(0,71,255);
                font: 700 9pt "Montserrat";
                color: rgb(255,255,255);
                border: none;
            }
            QPushButton {
                background-color: rgb(255,0,0);
                font: 700 9pt "Montserrat";
                color: rgb(255,255,255);
                border: 1px solid rgb(255,0,0);
                border-radius: 5px;
                padding: 4px 10px;
            }
            QPushButton:hover {
                background-color: rgb(255,255,255);
                color: rgb(255,0,0);
            }
        )");
        warningBox.exec();
        return;
    }


    StockItem item = stockModel->getItem(currentIndex.row());
    bool ok;

    // Product Name
    QInputDialog nameDialog(this);
    nameDialog.setWindowTitle("Edit Product");
    nameDialog.setLabelText("Product Name:");
    nameDialog.setTextValue(item.productName);
    nameDialog.setStyleSheet(inputDialogStyle);
    if (nameDialog.exec() == QDialog::Accepted) {
        item.productName = nameDialog.textValue();
    } else {
        return;
    }

    // Price
    QInputDialog priceDialog(this);
    priceDialog.setWindowTitle("Edit Product");
    priceDialog.setLabelText("Price:");
    priceDialog.setInputMode(QInputDialog::DoubleInput);
    priceDialog.setDoubleDecimals(2);
    priceDialog.setDoubleRange(0.0, 1000000.0);
    priceDialog.setDoubleValue(item.price);
    priceDialog.setStyleSheet(inputDialogStyle);
    if (priceDialog.exec() == QDialog::Accepted) {
        item.price = priceDialog.doubleValue();
    } else {
        return;
    }

    // Stock
    QInputDialog stockDialog(this);
    stockDialog.setWindowTitle("Edit Product");
    stockDialog.setLabelText("Stock:");
    stockDialog.setInputMode(QInputDialog::IntInput);
    stockDialog.setIntRange(0, 1000000);
    stockDialog.setIntValue(item.stock);
    stockDialog.setStyleSheet(inputDialogStyle);
    if (stockDialog.exec() == QDialog::Accepted) {
        item.stock = stockDialog.intValue();
        item.remaining = item.stock - item.sold;
    } else {
        return;
    }

    stockModel->updateItem(currentIndex.row(), item);
}

void MainWindow::onDeleteButtonClicked()
{
    QModelIndex currentIndex = ui->stockTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox warningBox(this);
        warningBox.setWindowTitle("Delete");
        warningBox.setText("Please select an item to delete. ❌");
        warningBox.setIcon(QMessageBox::Warning);
        warningBox.setStandardButtons(QMessageBox::Ok);

        // unique styleSheet
        warningBox.setStyleSheet(R"(
            QLabel {
                color: rgb(255,255,255);
            }
            QMessageBox {
                background-color: rgb(0,71,255);
                font: 700 9pt "Montserrat";
                color: rgb(255,255,255);
                border: none;
            }
            QPushButton {
                background-color: rgb(255,0,0);
                font: 700 9pt "Montserrat";
                color: rgb(255,255,255);
                border: 1px solid rgb(255,0,0);
                border-radius: 5px;
                padding: 4px 10px;
            }
            QPushButton:hover {
                background-color: rgb(255,255,255);
                color: rgb(255,0,0);
            }
        )");
        warningBox.exec();
        return;
    }

    QMessageBox confirmBox(this);
    confirmBox.setWindowTitle("Delete");
    confirmBox.setText("Are you sure you want to delete this item? 🗑️");
    confirmBox.setIcon(QMessageBox::Question);
    confirmBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    confirmBox.setDefaultButton(QMessageBox::No);

    // unique styleSheet
    confirmBox.setStyleSheet(R"(
        QLabel {
            color: rgb(255,255,255);
        }
        QMessageBox {
            background-color: rgb(0,71,255);
            font: 700 9pt "Montserrat";
            color: rgb(255,255,255);
            border: none;
        }
        QPushButton {
            background-color: rgb(255,255,255);
            font: 700 9pt "Montserrat";
            color: rgb(0,71,255);
            border: none;
            border-radius: 5px;
            padding: 4px 10px;
        }
        QPushButton:hover {
            color: rgb(255,0,0);
        }
    )");

    if (confirmBox.exec() == QMessageBox::Yes) {
        stockModel->removeItem(currentIndex.row());
    }
}

void MainWindow::onFilterTextChanged(const QString &text) {
    stockModel->filterItems(text);
}

// TAB 1
void MainWindow::onManualAddClicked() {
    ItemSelectionDialog dialog(stockModel, this);
    if (dialog.exec() == QDialog::Accepted) {
        StockItem selectedItem = dialog.getSelectedItem();
        int quantity = dialog.getQuantity();

        //Add transactions
        transactionModel->addTransaction(selectedItem, quantity);
    }
}

void MainWindow::onBackupButtonClicked() {
    // Create backup directory with timestamp
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString timestamp = currentDateTime.toString("yyyy-MM-dd_hh-mm-ss");
    QString backupDir = "Backup/" + timestamp;
    
    // Create the backup directory
    QDir dir;
    if (!dir.mkpath(backupDir)) {
        QMessageBox::critical(this, "Backup Error", "Failed to create backup directory.");
        return;
    }
    
    // Copy all files from Data directory to backup directory
    QDir dataDir("Data");
    if (!dataDir.exists()) {
        QMessageBox::critical(this, "Backup Error", "Data directory not found.");
        return;
    }
    
    QStringList files = dataDir.entryList(QDir::Files);
    bool success = true;
    
    for (const QString &file : files) {
        QString sourcePath = dataDir.filePath(file);
        QString destPath = backupDir + "/" + file;
        if (!QFile::copy(sourcePath, destPath)) {
            success = false;
            break;
        }
    }
    
    if (success) {
        QMessageBox::information(this, "Backup Complete", "Data has been successfully backed up to: " + backupDir);
    } else {
        QMessageBox::critical(this, "Backup Error", "Failed to copy some files during backup.");
    }
}

void MainWindow::onRestoreButtonClicked() {
    // Prompt user to select a backup directory
    QString backupDir = QFileDialog::getExistingDirectory(this, "Select Backup Directory", "Backup",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    
    if (backupDir.isEmpty()) {
        return; // User cancelled
    }
    
    // Verify the directory contains the expected files
    QDir dir(backupDir);
    QStringList files = dir.entryList(QDir::Files);
    if (files.isEmpty()) {
        QMessageBox::critical(this, "Restore Error", "Selected directory is empty.");
        return;
    }
    
    // Confirm with user before proceeding
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Restore",
        "This will replace all current data with the backup data. Are you sure you want to continue?",
        QMessageBox::Yes | QMessageBox::No);
    
    if (reply != QMessageBox::Yes) {
        return;
    }
    
    // Clear current data
    stockModel->clear();
    transactionModel->clearTransactions();
    confirmedTransactionModel->clearTransactions();
    
    // Copy files from backup to Data directory
    QDir dataDir("Data");
    if (!dataDir.exists()) {
        if (!dataDir.mkpath(".")) {
            QMessageBox::critical(this, "Restore Error", "Failed to create Data directory.");
            return;
        }
    }
    
    bool success = true;
    for (const QString &file : files) {
        QString sourcePath = dir.filePath(file);
        QString destPath = dataDir.filePath(file);
        
        // Remove existing file if it exists
        if (QFile::exists(destPath)) {
            QFile::remove(destPath);
        }
        
        if (!QFile::copy(sourcePath, destPath)) {
            success = false;
            break;
        }
    }
    
    if (success) {
        // Reload models with new data
        stockModel->readDataFromFile();
        transactionModel->readDataFromFile();
        confirmedTransactionModel->readDataFromFile();
        
        // Update analytics
        onTimePeriodChanged(ui->timePeriodComboBox->currentIndex());
        
        QMessageBox::information(this, "Restore Complete", "Data has been successfully restored from: " + backupDir);
    } else {
        QMessageBox::critical(this, "Restore Error", "Failed to restore some files.");
    }
}

// TAB 2
void MainWindow::onConfirmTransactionClicked()
{
    QModelIndex currentIndex = ui->stockTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox warningBox(this);
        warningBox.setWindowTitle("Delete");
        warningBox.setText("Please select an item to delete. ❌");
        warningBox.setIcon(QMessageBox::Warning);
        warningBox.setStandardButtons(QMessageBox::Ok);

        // unique styleSheet
        warningBox.setStyleSheet(R"(
            QLabel {
                color: rgb(255,255,255);
            }
            QMessageBox {
                background-color: rgb(0,71,255);
                font: 700 9pt "Montserrat";
                color: rgb(255,255,255);
                border: none;
            }
            QPushButton {
                background-color: rgb(255,0,0);
                font: 700 9pt "Montserrat";
                color: rgb(255,255,255);
                border: 1px solid rgb(255,0,0);
                border-radius: 5px;
                padding: 4px 10px;
            }
            QPushButton:hover {
                background-color: rgb(255,255,255);
                color: rgb(255,0,0);
            }
        )");
        warningBox.exec();
        return;
    }


    Transaction transaction = transactionModel->getTransaction(currentIndex.row());

    // Update stock
    StockItem item;
    for (int i = 0; i < stockModel->rowCount(); i++) {
        if (transaction.item.id == stockModel->getItem(i).id) {
            item = stockModel->getItem(i);
            if (item.remaining < transaction.quantity) {
                QMessageBox::StandardButton reply = QMessageBox::warning(this, "Low Stock Warning",
                    QString("This transaction would reduce stock below zero.\n"
                        "Current remaining stock: %1\n"
                        "Transaction quantity: %2\n\n"
                        "Would you like to continue? The remaining stock will be set to 0.").arg(item.remaining).arg(transaction.quantity),
                QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::No) {
                    break;
                }
                item.sold += item.remaining;
                confirmedTransactionModel->addTransaction(transaction.item, item.remaining);
                item.remaining = 0;
                stockModel->updateItem(i, item);
                break;
            } else {
                item.remaining -= transaction.quantity;
                item.sold += transaction.quantity;
                confirmedTransactionModel->addTransaction(transaction.item, transaction.quantity);
                stockModel->updateItem(i, item);
                break;
            }
        }
    }

    // Remove from pending transactions
    transactionModel->removeTransaction(currentIndex.row());

    // Update analytics with current time period
    int currentPeriodIndex = ui->timePeriodComboBox->currentIndex();
    onTimePeriodChanged(currentPeriodIndex);
}

void MainWindow::onDeleteTransactionClicked()
{
    QModelIndex currentIndex = ui->stockTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox warningBox(this);
        warningBox.setWindowTitle("Delete");
        warningBox.setText("Please select an item to delete. ❌");
        warningBox.setIcon(QMessageBox::Warning);
        warningBox.setStandardButtons(QMessageBox::Ok);

        // unique styleSheet
        warningBox.setStyleSheet(R"(
            QLabel {
                color: rgb(255,255,255);
            }
            QMessageBox {
                background-color: rgb(0,71,255);
                font: 700 9pt "Montserrat";
                color: rgb(255,255,255);
                border: none;
            }
            QPushButton {
                background-color: rgb(255,0,0);
                font: 700 9pt "Montserrat";
                color: rgb(255,255,255);
                border: 1px solid rgb(255,0,0);
                border-radius: 5px;
                padding: 4px 10px;
            }
            QPushButton:hover {
                background-color: rgb(255,255,255);
                color: rgb(255,0,0);
            }
        )");
        warningBox.exec();
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete", "Are you sure you want to delete this transaction?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        transactionModel->removeTransaction(currentIndex.row());
    }
}

// TAB 4
void MainWindow::onTimePeriodChanged(int index)
{
    TimePeriod period;
    switch (index) {
        case 0: period = TimePeriod::LastWeek; break;
        case 1: period = TimePeriod::LastMonth; break;
        case 2: period = TimePeriod::LastYear; break;
        default: period = TimePeriod::LastWeek;
    }
    
    // Get all transactions from confirmedTransactionModel
    QVector<ConfirmedTransaction> transactions;
    for (int i = 0; i < confirmedTransactionModel->rowCount(); ++i) {
        transactions.append(confirmedTransactionModel->getTransaction(i));
    }
    
    analyticsModel->updateAnalytics(transactions, period);
    
    // Update the how much to stock list
    QVector<ProductAnalytics> analytics;
    for (int i = 0; i < analyticsModel->rowCount(); ++i) {
        QModelIndex idx = analyticsModel->index(i);
        QString text = analyticsModel->data(idx).toString();
        QStringList parts = text.split(" - Sold: ");
        QString name = parts[0];
        QStringList soldParts = parts[1].split(" (Rate: ");
        int sold = soldParts[0].toInt();
        QString rateStr = soldParts[1].split("/day")[0];
        double rate = rateStr.toDouble();
        
        ProductAnalytics analytic;
        analytic.productName = name;
        analytic.totalSold = sold;
        analytic.salesRate = rate;
        analytics.append(analytic);
    }
    
    howMuchModel->updateRecommendations(analytics, ui->daysToStockSpinBox->value(), stockModel);
}

void MainWindow::onDaysToStockChanged(int days)
{
    // Get current analytics data
    QVector<ProductAnalytics> analytics;
    for (int i = 0; i < analyticsModel->rowCount(); ++i) {
        QModelIndex idx = analyticsModel->index(i);
        QString text = analyticsModel->data(idx).toString();
        QStringList parts = text.split(" - Sold: ");
        QString name = parts[0];
        QStringList soldParts = parts[1].split(" (Rate: ");
        int sold = soldParts[0].toInt();
        QString rateStr = soldParts[1].split("/day")[0];
        double rate = rateStr.toDouble();
        
        ProductAnalytics analytic;
        analytic.productName = name;
        analytic.totalSold = sold;
        analytic.salesRate = rate;
        analytics.append(analytic);
    }
    
    howMuchModel->updateRecommendations(analytics, days, stockModel);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_1:
            ui->stackedWidget->setCurrentWidget(ui->salesTab);
            break;
        case Qt::Key_2:
            ui->stackedWidget->setCurrentWidget(ui->transactionsTab);
            break;
        case Qt::Key_3:
            ui->stackedWidget->setCurrentWidget(ui->loggingTab);
            break;
        case Qt::Key_4:
            ui->stackedWidget->setCurrentWidget(ui->analyticsTab);
            break;
        default:
            QMainWindow::keyPressEvent(event);
    }
}
