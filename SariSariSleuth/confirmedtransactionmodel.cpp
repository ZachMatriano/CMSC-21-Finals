#include "confirmedtransactionmodel.h"
#include <QDebug>

// CONSTRUCTOR
ConfirmedTransactionModel::ConfirmedTransactionModel(QObject *parent)
    : QAbstractTableModel(parent)
    , nextTransactionId(1)
{
}

// NECESSARY OVERRIDES, see stockmodel.cpp and stockmodel.h for more complete documentation
int ConfirmedTransactionModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return transactions.size();
}

int ConfirmedTransactionModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return 5; // Transaction ID, Product Name, Price, Quantity, Timestamp
}

QVariant ConfirmedTransactionModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= transactions.size() || index.column() >= 5)
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const ConfirmedTransaction &transaction = transactions[index.row()];
        switch (index.column()) {
            case 0: return transaction.transactionId;
            case 1: return transaction.item.productName;
            case 2: return QString::number(transaction.item.price, 'f', 2);
            case 3: return transaction.quantity;
            case 4: return transaction.timestamp.toString("yyyy-MM-dd hh:mm:ss");
            default: return QVariant();
        }
    }
    return QVariant();
}

QVariant ConfirmedTransactionModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return "Transaction ID";
            case 1: return "Product Name";
            case 2: return "Price";
            case 3: return "Quantity";
            case 4: return "Timestamp";
            default: return QVariant();
        }
    }
    return QVariant();
}

// ACTUAL IMPLEMENTED FUNCTIONS
void ConfirmedTransactionModel::addTransaction(const StockItem &item, int quantity) {
    beginInsertRows(QModelIndex(), transactions.size(), transactions.size());
    
    ConfirmedTransaction transaction;
    transaction.transactionId = nextTransactionId++;
    transaction.item = item;
    transaction.quantity = quantity;
    transaction.timestamp = QDateTime::currentDateTime();
    
    transactions.append(transaction);
    endInsertRows();
}

ConfirmedTransaction ConfirmedTransactionModel::getTransaction(int row) const {
    if (row >= 0 && row < transactions.size())
        return transactions[row];
    return ConfirmedTransaction();
}

void ConfirmedTransactionModel::clearTransactions() {
    beginResetModel();
    transactions.clear();
    nextTransactionId = 1;
    endResetModel();
} 
