#include "stockmodel.h"
#include <QDebug>
#include <QDir>
#include <fstream>
#include <sstream>
#include <iostream>

// CONSTRUCTOR
StockModel::StockModel(QObject *parent) : QAbstractTableModel(parent) {
    readDataFromFile();
}

// NECESSARY OVERRIDES
int StockModel::rowCount(const QModelIndex &parent) const {

    // Must be implemented when inheriting QAbstractTableModel
    if (parent.isValid()) // DO NOT CHANGE, this is in documentation
        return 0;
    return filteredItems.size();
}

int StockModel::columnCount(const QModelIndex &parent) const {

    // see rowCount method
    if (parent.isValid()) //DO NOT CHANGE
        return 0;
    return 6; // id, productName, price, stock, remaining, sold
}

QVariant StockModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= filteredItems.size() || index.column() >= 6) // In documentation
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const StockItem &item = filteredItems[index.row()];
        switch (index.column()) { // Our data
            case 0: return item.id;
            case 1: return item.productName;
            case 2: return QString::number(item.price, 'f', 2);
            case 3: return item.stock;
            case 4: return item.remaining;
            case 5: return item.sold;
            default: return QVariant(); // In documentation
        }
    }
    return QVariant(); // In documentation
}

QVariant StockModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0: return "ID";
            case 1: return "Product Name";
            case 2: return "Price";
            case 3: return "Stock";
            case 4: return "Remaining";
            case 5: return "Sold";
            default: return QVariant();
        }
    }
    return QVariant();
}

bool StockModel::setData(const QModelIndex &index, const QVariant &value, int role) {

    // Reimplementation of setData because our Model is editable

    if (!index.isValid() || role != Qt::EditRole)
        return false;

    if (index.row() >= filteredItems.size() || index.column() >= 6)
        return false;

    StockItem &item = filteredItems[index.row()];

    // What the function returns
    bool success = false;

    switch (index.column()) { // Set the return value to match
        case 0:
            item.id = value.toInt(&success);
            break;
        case 1:
            item.productName = value.toString();
            success = true;
            break;
        case 2:
            item.price = value.toDouble(&success);
            break;
        case 3:
            item.stock = value.toInt(&success);
            break;
        case 4:
            item.remaining = value.toInt(&success);
            break;
        case 5:
            item.sold = value.toInt(&success);
            break;
    }

    if (success) {
        emit dataChanged(index, index);

        // Update the main items list
        int mainIndex = items.indexOf(item);
        if (mainIndex != -1) {
            items[mainIndex] = item;
        }
    }

    return success;
}

Qt::ItemFlags StockModel::flags(const QModelIndex &index) const {

    // Definition of flags for the index.
    // These flags define the state that the objects can be in
    // NOTE: These are bitwise, so to combine flags use the bitwise | operator as shown below
    // This is already implemented in the parent class, but without ItemIsEditable and NoItemFlags

    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

// ACTUAL IMPLEMENTED METHODS
void StockModel::writeItemToFile(const StockItem &item) {
    // Create Data directory if it doesn't exist
    QDir().mkpath("Data");
    
    std::ofstream file(DATA_FILE.toStdString(), std::ios::app);
    if (file.is_open()) {
        std::cout << "what the heck." << std::endl;
        file << item.id << "|" << item.productName.toStdString() << "|" << item.price << "|" << item.stock << "|" << item.remaining << "|" << item.sold << std::endl;
        file.close();
    }
}

void StockModel::updateItemInFile(const StockItem &oldItem, const StockItem &newItem) {
    std::ifstream inFile(DATA_FILE.toStdString());
    std::ofstream outFile("Data/temp.txt");
    std::string line;
    
    if (inFile.is_open() && outFile.is_open()) {
        while (std::getline(inFile, line)) {
            std::istringstream iss(line);
            std::string idStr;
            std::getline(iss, idStr, '|');
            int id = std::stoi(idStr);
            
            if (id == oldItem.id) {
                // Write the new item data
                outFile << newItem.id << "|"
                       << newItem.productName.toStdString() << "|"
                       << newItem.price << "|"
                       << newItem.stock << "|"
                       << newItem.remaining << "|"
                       << newItem.sold << std::endl;
            } else {
                // Write the original line
                outFile << line << std::endl;
            }
        }
        
        inFile.close();
        outFile.close();
        
        // Replace the original file with the temporary file
        std::remove(DATA_FILE.toStdString().c_str());
        std::rename("Data/temp.txt", DATA_FILE.toStdString().c_str());
    }
}

void StockModel::deleteItemFromFile(int id) {
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

void StockModel::addItem(const StockItem &item) {

    // Only called by onAddButtonClicked

    beginInsertRows(QModelIndex(), items.size(), items.size()); // Must be called before insertion of data
    items.append(item);
    if (currentFilter.isEmpty() || item.productName.contains(currentFilter, Qt::CaseInsensitive)) {
        filteredItems.append(item);
    }
    writeItemToFile(item);
    endInsertRows(); // Must be called at the end of insertion
}

void StockModel::removeItem(int row) {

    // Only called by onDeleteButtonClicked

    if (row < 0 || row >= filteredItems.size()) // Guard
        return;

    beginRemoveRows(QModelIndex(), row, row); // Must be called BEFORE removal of data
    StockItem item = filteredItems[row];
    filteredItems.removeAt(row);
    items.removeOne(item);
    deleteItemFromFile(item.id);
    endRemoveRows(); // Must be called at the end of removal
}

void StockModel::updateItem(int row, const StockItem &item) {

    // Called by onConfirmTransactionClicked (to change the remaining and sold)
    // and by onEditButtonClicked (to edit an item)

    if (row < 0 || row >= filteredItems.size()) // Guard
        return;

    StockItem oldItem = filteredItems[row];
    filteredItems[row] = item;
    
    // Find the item in the main list
    for (int i = 0; i < items.size(); i++) {
        if (items[i].id == item.id) {
            items[i] = item;
            break;
        }
    }
    
    updateItemInFile(oldItem, item);
    emit dataChanged(index(row, 0), index(row, columnCount() - 1)); // TODO: IS THIS EVEN USED??
}

StockItem StockModel::getItem(int row) const {
    if (row >= 0 && row < filteredItems.size())
        return filteredItems[row];
    return StockItem();
}

void StockModel::filterItems(const QString &text) {

    // Called by onFilterTextChanged in the main window

    beginResetModel(); // Begins the reset of any data
    currentFilter = text; // The current filter
    filteredItems.clear(); // Resets the filtered items
    
    if (text.isEmpty()) { // In the case where the text changes to be empty
        filteredItems = items;
    } else {
        for (int i = 0; i < items.size(); i++) {
            if (items[i].productName.contains(text, Qt::CaseInsensitive)) {
                filteredItems.append(items[i]);
            }
        }
    }
    endResetModel(); // Ends the reset of any data
}

void StockModel::readDataFromFile() {
    // Create Data directory if it doesn't exist
    QDir().mkpath("Data");
    
    std::ifstream file(DATA_FILE.toStdString());
    if (!file.is_open()) {
        return; // File doesn't exist yet, that's okay
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        StockItem item;

        // Read ID
        std::getline(iss, token, '|');
        item.id = std::stoi(token);

        // Read Product Name
        std::getline(iss, token, '|');
        item.productName = QString::fromStdString(token);

        // Read Price
        std::getline(iss, token, '|');
        item.price = std::stod(token);

        // Read Stock
        std::getline(iss, token, '|');
        item.stock = std::stoi(token);

        // Read Remaining
        std::getline(iss, token, '|');
        item.remaining = std::stoi(token);

        // Read Sold
        std::getline(iss, token, '|');
        item.sold = std::stoi(token);

        // Add to both lists
        items.append(item);
        filteredItems.append(item);
    }

    file.close();
}

QString StockModel::generateQRCodeString() const {
    QString qrString;
    for (const StockItem &item : items) {
        qrString += QString("%1|%2|%3\n")
            .arg(item.productName)
            .arg(QString::number(item.price, 'f', 2))
            .arg(item.remaining);
    }
    return qrString;
} 
