#include "itemselectiondialog.h"
#include <QLabel>
#include <QItemSelectionModel>

ItemSelectionDialog::ItemSelectionDialog(StockModel *model, QWidget *parent) : QDialog(parent) , stockModel(model) {
    setWindowTitle("Select Item");
    setMinimumWidth(700);

    // Create search bar
    searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("🔎 Search items...");
    searchBar->setStyleSheet(
        "QLineEdit {"
        "    color: rgb(0, 71, 255);"
        "    background-color: rgb(255, 255, 255);"
        "    font: 500 9pt \"Montserrat\";"
        "    padding: 6px;"
        "    border: 1px solid rgb(200, 200, 200);"
        "    border-radius: 4px;"
        "}"
    );

    // Create proxy model for filtering
    proxyModel = new QSortFilterProxyModel(this);
    proxyModel->setSourceModel(stockModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(1);  // Filter only on the product name column
    proxyModel->setFilterRole(Qt::DisplayRole);  // Filter on the display text

    // Create table view
    tableView = new QTableView(this);
    tableView->setModel(stockModel);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->verticalHeader()->setVisible(false);

    tableView->setStyleSheet(
        "QTableView {"
        "    color: black;"
        "    background-color: rgb(211, 211, 211);"
        "    font: 500 9pt \"Montserrat\";"
        "    border: none;"
        "    border-bottom-left-radius: 10px;"
        "    border-bottom-right-radius: 10px;"
        "    gridline-color: rgb(220, 220, 220);"
        "}"
        "QTableView::item:selected {"
        "    background-color: rgb(38,255,0);"
        "    color: rgb(11,129,0);"
        "}"
        "QHeaderView::section {"
        //"    margin-top: 2px;  "
        "    color:rgb(0, 71, 255);"
        "    background-color: rgb(255, 255, 255);"
        "    font: 800 9pt \"Montserrat\";"
        "    padding: 4px;"
        "    border: none;"
        "}"
        );

    // Quantity spinbox
    quantitySpinBox = new QSpinBox(this);
    quantitySpinBox->setMinimum(1);
    quantitySpinBox->setMaximum(9999);
    quantitySpinBox->setValue(1);
    quantitySpinBox->setStyleSheet(
        "QSpinBox {"
        "    color: rgb(0, 71, 255);"
        "    background-color: rgb(255, 255, 255);"
        "    selection-background-color: rgb(230, 240, 255);"
        "    selection-color: rgb(0, 71, 255);"
        "    font: 700 9pt \"Montserrat\";"
        "    padding-right: 20px;"  // Reserve space for the buttons
        "    border: 1px solid rgb(200, 200, 200);"
        "    border-radius: 4px;"
        "}"

        "QSpinBox::up-button {"
        "    subcontrol-origin: border;"
        "    subcontrol-position: top right;"
        "    width: 16px;"
        "    height: 10px;"
        "    background-color: rgb(240, 240, 240);"
        "    border-left: 1px solid rgb(200, 200, 200);"
        "    border-bottom: 1px solid rgb(200, 200, 200);"
        "}"

        "QSpinBox::down-button {"
        "    subcontrol-origin: border;"
        "    subcontrol-position: bottom right;"
        "    width: 16px;"
        "    height: 10px;"
        "    background-color: rgb(240, 240, 240);"
        "    border-left: 1px solid rgb(200, 200, 200);"
        "    border-top: 1px solid rgb(200, 200, 200);"
        "}"

        "QSpinBox::up-arrow, QSpinBox::down-arrow {"
        "    width: 7px;"
        "    height: 7px;"
        "}"
        );

    // Create buttons
    acceptButton = new QPushButton("ORDER 💸", this);
    acceptButton->setStyleSheet(
        "QPushButton {"
        "    background-color: rgb(38,255,0);"    // green
        "    color: rgb(21,139,0);"
        "    border: none;"
        "    padding: 6px 12px;"
        "    border-radius: 5px;"
        "    font: 800 9pt \"Montserrat\";"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        );

    cancelButton = new QPushButton("CANCEL ⛔", this);
    cancelButton->setStyleSheet(
        "QPushButton {"
        "    background-color: rgb(255,0,0);"    // red
        "    color: white;"
        "    border: none;"
        "    padding: 6px 12px;"
        "    border-radius: 5px;"
        "    font: 800 9pt \"Montserrat\";"
        "}"
        "QPushButton:hover {"
        "    background-color: #d32f2f;"
        "}"
        );

    // Layouts
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *bottomRowLayout = new QHBoxLayout();
    mainLayout->addWidget(tableView);
    bottomRowLayout->addStretch();  // Optional: adds space between spinbox and buttons
    bottomRowLayout->addWidget(quantitySpinBox);
    bottomRowLayout->addWidget(acceptButton);
    bottomRowLayout->addWidget(cancelButton);

    mainLayout->addLayout(bottomRowLayout);

    // Connect signals and slots
    connect(tableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &ItemSelectionDialog::onItemSelected);
    connect(acceptButton, &QPushButton::clicked, this, &ItemSelectionDialog::onAcceptClicked);
    connect(cancelButton, &QPushButton::clicked, this, &ItemSelectionDialog::onCancelClicked);
}

void ItemSelectionDialog::onItemSelected(const QItemSelection &selected, const QItemSelection &deselected) {
    if (!selected.indexes().isEmpty()) {
        QModelIndex index = selected.indexes().first();
        selectedItem = stockModel->getItem(index.row());
    }
}

void ItemSelectionDialog::onAcceptClicked() {
    if (tableView->selectionModel()->hasSelection()) {  // Guard
        accept();
    }
}

void ItemSelectionDialog::onCancelClicked() {
    reject();
} 
