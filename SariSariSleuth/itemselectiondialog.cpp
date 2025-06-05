#include "itemselectiondialog.h"
#include <QLabel>
#include <QItemSelectionModel>

ItemSelectionDialog::ItemSelectionDialog(StockModel *model, QWidget *parent) : QDialog(parent) , stockModel(model) {
    setWindowTitle("Select Item");
    setMinimumWidth(700);

    // Create table view
    tableView = new QTableView(this);
    tableView->setModel(stockModel);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->verticalHeader()->setVisible(false);

    // Apply your custom stylesheet
    tableView->setStyleSheet(
        "QTableView {"
        "    color: black;"
        "    background-color: rgb(255, 255, 255);"
        "    selection-background-color: rgb(230, 240, 255);"
        "    selection-color: rgb(0, 71, 255);"
        "    font: 700 9pt \"Montserrat\";"
        "    border: 1px solid rgb(220, 220, 220);"
        "    border-bottom-left-radius: 10px;"
        "    border-bottom-right-radius: 10px;"
        "    gridline-color: rgb(220, 220, 220);"
        "}"
        "QHeaderView::section {"
        //"    margin-top: 2px;  "
        "    color:black;"
        "    background-color: rgb(245, 245, 245);"
        "    font: 700 9pt \"Montserrat\";"
        "    padding: 4px;"
        "    border: 1px solid rgb(200, 200, 200);"
        "}"
        );

    // Create quantity spinbox
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
    acceptButton = new QPushButton("Accept", this);
    cancelButton = new QPushButton("Cancel", this);

    // Create layouts
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    mainLayout->addWidget(tableView);
    mainLayout->addWidget(quantitySpinBox);
    buttonLayout->addWidget(acceptButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

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
