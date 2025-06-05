#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "itemselectiondialog.h"
#include <QMessageBox>
#include <QInputDialog>
#include <algorithm>

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
    
    QVector<QPair<QPushButton*, QWidget*>> tabMappings = {
        {ui->salesButton, ui->sales},
        {ui->transactionsButton, ui->transactions},
        {ui->loggingButton, ui->stocklogging},
        {ui->analysisButton, ui->tab4}
    };

    for (const auto& pair : tabMappings) {
        connect(pair.first, &QPushButton::clicked, this, [=]() {
            ui->tabs->setCurrentWidget(pair.second);
        });
    }

    // Set up the table view for stock
    ui->stockTableView->setModel(stockModel);
    ui->stockTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
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
    connect(ui->openCameraButton, &QPushButton::clicked, this, &MainWindow::onOpenCameraClicked);
    connect(ui->manualAddButton, &QPushButton::clicked, this, &MainWindow::onManualAddClicked);

    // Connect signals and slots for transaction management
    connect(ui->confirmButton, &QPushButton::clicked, this, &MainWindow::onConfirmTransactionClicked);
    connect(ui->deleteTransactionButton, &QPushButton::clicked, this, &MainWindow::onDeleteTransactionClicked);

    // === SHAPES AND ICON STYLES ===
    struct ButtonStyle {
        QPushButton* button;
        QString iconPath;
    };

    QList<ButtonStyle> styledButtons = {
        {ui->salesButton, ":/resources/clear-sales.png"},
        {ui->transactionsButton, ":/resources/clear-transactions.png"},
        {ui->loggingButton, ":/resources/clear-logging.png"},
        {ui->analysisButton, ":/resources/clear-analysis.png"}
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

MainWindow::~MainWindow()
{
    delete ui;
}

// TAB 3
void MainWindow::onAddButtonClicked() {
    bool ok;
    QString productName = QInputDialog::getText(this, "Add Product", "Product Name:", QLineEdit::Normal, "", &ok);
    if (!ok || productName.isEmpty())
        return;

    double price = QInputDialog::getDouble(this, "Add Product", "Price:", 0.0, 0.0, 1000000.0, 2, &ok);
    if (!ok)
        return;

    int stock = QInputDialog::getInt(this, "Add Product", "Initial Stock:", 0, 0, 1000000, 1, &ok);
    if (!ok)
        return;

    StockItem item;
    std::vector<int> ids; // Unique ID but also slot filling
    for (int i = 0; i < this->stockModel->rowCount(); i++) {
        ids.push_back(this->stockModel->getItem(i).id);
    }
    int newId = 1;
    for (int i = 0; i < ids.size(); i++) {
        if(std::find(ids.begin(), ids.end(), newId) != ids.end()) {
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
    QModelIndex currentIndex = ui->stockTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Edit", "Please select an item to edit.");
        return;
    }

    StockItem item = stockModel->getItem(currentIndex.row());
    bool ok;

    QString productName = QInputDialog::getText(this, "Edit Product", "Product Name:", QLineEdit::Normal, item.productName, &ok);
    if (!ok) return;

    double price = QInputDialog::getDouble(this, "Edit Product", "Price:", item.price, 0.0, 1000000.0, 2, &ok);
    if (!ok) return;

    int stock = QInputDialog::getInt(this, "Edit Product", "Stock:", item.stock, 0, 1000000, 1, &ok);
    if (!ok) return;

    item.productName = productName;
    item.price = price;
    item.stock = stock;
    item.remaining = stock - item.sold;

    stockModel->updateItem(currentIndex.row(), item);
}

void MainWindow::onDeleteButtonClicked()
{
    QModelIndex currentIndex = ui->stockTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Delete", "Please select an item to delete.");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete", "Are you sure you want to delete this item?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) stockModel->removeItem(currentIndex.row());
}

void MainWindow::onFilterTextChanged(const QString &text) {
    stockModel->filterItems(text);
}

// TAB 1
void MainWindow::onOpenCameraClicked()
{
    // TODO: Implement camera functionality
    QMessageBox::information(this, "Camera", "Camera functionality will be implemented in the next phase.");
}

void MainWindow::onManualAddClicked() {
    ItemSelectionDialog dialog(stockModel, this);
    if (dialog.exec() == QDialog::Accepted) {
        StockItem selectedItem = dialog.getSelectedItem();
        int quantity = dialog.getQuantity();

        // Add transaction
        transactionModel->addTransaction(selectedItem, quantity);
    }
}

// TAB 2
void MainWindow::onConfirmTransactionClicked()
{
    QModelIndex currentIndex = ui->transactionTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Confirm", "Please select a transaction to confirm.");
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
    QModelIndex currentIndex = ui->transactionTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Delete", "Please select a transaction to delete.");
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
