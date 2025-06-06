#include "transactionmodel.h"
#include <QDebug>
#include <QDir>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

// CONSTRUCTOR
TransactionModel::TransactionModel(QObject *parent) : QAbstractTableModel(parent), nextTransactionId(1) {
    readDataFromFile();
}

//NECESSARY OVERRIDES, better explanations exist in the stockmodel.h file since that was created first
int TransactionModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return transactions.size();
}

int TransactionModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return 5; // Transaction ID, Product Name, Price, Quantity, Timestamp
}

QVariant TransactionModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= transactions.size() || index.column() >= 5)
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const Transaction &transaction = transactions[index.row()];
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

QVariant TransactionModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return "Transaction ID 🎫";
            case 1: return "Product Name 🛍️";
            case 2: return "Price 💥";
            case 3: return "Quantity 📦";
            case 4: return "Timestamp ⏰";
            default: return QVariant();
        }
    }
    return QVariant();
}


// ACTUAL IMPLEMENTED FUNCTIONS
void TransactionModel::addTransaction(const StockItem &item, int quantity) {
    beginInsertRows(QModelIndex(), transactions.size(), transactions.size());
    
    Transaction transaction;
    
    // Find the first available transaction ID
    std::vector<int> ids;
    for (const Transaction& t : transactions) {
        ids.push_back(t.transactionId);
    }
    int newId = 1;
    for (int i = 0; i < ids.size(); i++) {
        if(std::find(ids.begin(), ids.end(), newId) != ids.end()) {
            newId++;
        } else {
            break;
        }
    }
    
    transaction.transactionId = newId;
    transaction.item = item;
    transaction.quantity = quantity;
    transaction.timestamp = QDateTime::currentDateTime();
    
    transactions.append(transaction);
    writeTransactionToFile(transaction);
    endInsertRows();
}

Transaction TransactionModel::getTransaction(int row) const {
    if (row >= 0 && row < transactions.size())
        return transactions[row];
    return Transaction();
}

void TransactionModel::removeTransaction(int row) {

    // Called by onConfirmTransactionClicked and onDeleteTransactionClicked

    if (row >= 0 && row < transactions.size()) {
        beginRemoveRows(QModelIndex(), row, row);
        deleteTransactionFromFile(transactions.at(row).transactionId);
        transactions.removeAt(row);
        endRemoveRows();
    }
}

void TransactionModel::clearTransactions() { // TODO: WHERE????
    beginResetModel();
    transactions.clear();
    nextTransactionId = 1;
    endResetModel();
}

void TransactionModel::writeTransactionToFile(const Transaction &transaction) {
    // Create Data directory if it doesn't exist
    QDir().mkpath("Data");
    
    std::ofstream file(DATA_FILE.toStdString(), std::ios::app);
    if (file.is_open()) {
        file << transaction.transactionId << "|"
             << transaction.item.id << "|"
             << transaction.item.productName.toStdString() << "|"
             << transaction.item.price << "|"
             << transaction.item.stock << "|"
             << transaction.item.remaining << "|"
             << transaction.item.sold << "|"
             << transaction.quantity << "|"
             << transaction.timestamp.toString("yyyy-MM-dd hh:mm:ss").toStdString() << std::endl;
        file.close();
    }
}

void TransactionModel::deleteTransactionFromFile(int id) {
    std::ifstream inFile(DATA_FILE.toStdString());
    std::ofstream outFile("Data/temp.txt");
    std::string line;
    bool found = false;
    
    if (inFile.is_open() && outFile.is_open()) {
        while (std::getline(inFile, line)) {
            std::istringstream iss(line);
            std::string idStr;
            std::getline(iss, idStr, '|');
            int currentId = std::stoi(idStr);
            
            if (currentId != id) {
                outFile << line << std::endl;
            } else {
                found = true;
            }
        }
        
        inFile.close();
        outFile.close();
        
        if (found) {
            // Replace the original file with the temporary file
            std::remove(DATA_FILE.toStdString().c_str());
            std::rename("Data/temp.txt", DATA_FILE.toStdString().c_str());
        } else {
            // If item wasn't found, remove the temporary file
            std::remove("Data/temp.txt");
        }
    }
}

void TransactionModel::readDataFromFile() {
    // Create Data directory if it doesn't exist
    QDir().mkpath("Data");
    
    std::ifstream file(DATA_FILE.toStdString());
    if (!file.is_open()) {
        return; // File doesn't exist yet, that's okay
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string thing;
        Transaction transaction;

        // Read Transaction ID
        std::getline(iss, thing, '|');
        transaction.transactionId = std::stoi(thing);
        nextTransactionId = std::max(nextTransactionId, transaction.transactionId + 1);

        // Read StockItem data
        std::getline(iss, thing, '|');
        transaction.item.id = std::stoi(thing);

        std::getline(iss, thing, '|');
        transaction.item.productName = QString::fromStdString(thing);

        std::getline(iss, thing, '|');
        transaction.item.price = std::stod(thing);

        std::getline(iss, thing, '|');
        transaction.item.stock = std::stoi(thing);

        std::getline(iss, thing, '|');
        transaction.item.remaining = std::stoi(thing);

        std::getline(iss, thing, '|');
        transaction.item.sold = std::stoi(thing);

        // Read Quantity
        std::getline(iss, thing, '|');
        transaction.quantity = std::stoi(thing);

        // Read Timestamp
        std::getline(iss, thing, '|');
        transaction.timestamp = QDateTime::fromString(QString::fromStdString(thing), "yyyy-MM-dd hh:mm:ss");

        // Add to transactions list
        transactions.append(transaction);
    }

    file.close();
}
