#include "itemselectiondialog.h"
#include <QLabel>
#include <QItemSelectionModel>

ItemSelectionDialog::ItemSelectionDialog(StockModel *model, QWidget *parent) : QDialog(parent) , stockModel(model) {
    setWindowTitle("Select Item");
    setMinimumWidth(400);

    // Create table view
    tableView = new QTableView(this);
    tableView->setModel(stockModel);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->verticalHeader()->setVisible(false);

    // Create quantity spinbox
    quantitySpinBox = new QSpinBox(this);
    quantitySpinBox->setMinimum(1);
    quantitySpinBox->setMaximum(9999);
    quantitySpinBox->setValue(1);

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
