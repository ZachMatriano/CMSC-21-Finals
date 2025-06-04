#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , stockModel(new StockModel(this))
{
    ui->setupUi(this);
    
    // Set up the table view
    ui->stockTableView->setModel(stockModel);
    ui->stockTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    // Connect signals and slots
    connect(ui->addButton, &QPushButton::clicked, this, &MainWindow::onAddButtonClicked);
    connect(ui->editButton, &QPushButton::clicked, this, &MainWindow::onEditButtonClicked);
    connect(ui->deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteButtonClicked);
    connect(ui->filterLineEdit, &QLineEdit::textChanged, this, &MainWindow::onFilterTextChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAddButtonClicked()
{
    bool ok;
    QString productName = QInputDialog::getText(this, "Add Product",
                                              "Product Name:", QLineEdit::Normal,
                                              "", &ok);
    if (!ok || productName.isEmpty())
        return;

    double price = QInputDialog::getDouble(this, "Add Product",
                                         "Price:", 0.0, 0.0, 1000000.0, 2, &ok);
    if (!ok)
        return;

    int stock = QInputDialog::getInt(this, "Add Product",
                                   "Initial Stock:", 0, 0, 1000000, 1, &ok);
    if (!ok)
        return;

    StockItem item;
    item.id = stockModel->rowCount() + 1; // Simple ID generation
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

    QString productName = QInputDialog::getText(this, "Edit Product",
                                              "Product Name:", QLineEdit::Normal,
                                              item.productName, &ok);
    if (!ok)
        return;

    double price = QInputDialog::getDouble(this, "Edit Product",
                                         "Price:", item.price, 0.0, 1000000.0, 2, &ok);
    if (!ok)
        return;

    int stock = QInputDialog::getInt(this, "Edit Product",
                                   "Stock:", item.stock, 0, 1000000, 1, &ok);
    if (!ok)
        return;

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

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete",
                                                            "Are you sure you want to delete this item?",
                                                            QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        stockModel->removeItem(currentIndex.row());
    }
}

void MainWindow::onFilterTextChanged(const QString &text)
{
    stockModel->filterItems(text);
}
