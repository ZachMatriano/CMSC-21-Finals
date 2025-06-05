#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "itemselectiondialog.h"
#include <QMessageBox>
#include <QInputDialog>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , stockModel(new StockModel(this))
    , transactionModel(new TransactionModel(this))
    , confirmedTransactionModel(new ConfirmedTransactionModel(this))
{
    ui->setupUi(this);
    ui->tabWidget->tabBar()->hide(); //Hides tabs to make it cooler

    // Set up the table view for stock
    ui->stockTableView->setModel(stockModel);
    ui->stockTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // Set up the table view for transactions
    ui->transactionTableView->setModel(transactionModel);
    ui->transactionTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
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

    // BUTTONS TO OPEN TABWIDGET
    connect(ui->pushSales, &QPushButton::clicked, this, [=]() {
        ui->tabWidget->setCurrentIndex(0);
    });
    connect(ui->pushStock, &QPushButton::clicked, this, [=]() {
        ui->tabWidget->setCurrentIndex(1);
    });
    connect(ui->pushTransactions, &QPushButton::clicked, this, [=]() {
        ui->tabWidget->setCurrentIndex(2);
    });
    connect(ui->pushAnalytics, &QPushButton::clicked, this, [=]() {
        ui->tabWidget->setCurrentIndex(3);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

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
    std::vector<int> ids;
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

void MainWindow::onOpenCameraClicked()
{
    // TODO: Implement camera functionality
    QMessageBox::information(this, "Camera", "Camera functionality will be implemented in the next phase.");
}

void MainWindow::onManualAddClicked()
{
    ItemSelectionDialog dialog(stockModel, this);
    if (dialog.exec() == QDialog::Accepted) {
        StockItem selectedItem = dialog.getSelectedItem();
        int quantity = dialog.getQuantity();

        // Add transaction
        transactionModel->addTransaction(selectedItem, quantity);
    }
}

void MainWindow::onConfirmTransactionClicked()
{
    QModelIndex currentIndex = ui->transactionTableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Confirm", "Please select a transaction to confirm.");
        return;
    }

    Transaction transaction = transactionModel->getTransaction(currentIndex.row());
    
    // Add to confirmed transactions
    confirmedTransactionModel->addTransaction(transaction.item, transaction.quantity);

    // Update stock
    StockItem item = transaction.item;
    item.remaining -= transaction.quantity;
    item.sold += transaction.quantity;
    
    // Find the item in the stock model and update it
    for (int i = 0; i < stockModel->rowCount(); i++) {
        if (stockModel->getItem(i).id == item.id) {
            stockModel->updateItem(i, item);
            break;
        }
    }

    // Remove from pending transactions
    transactionModel->removeTransaction(currentIndex.row());
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
