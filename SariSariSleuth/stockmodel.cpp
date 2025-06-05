#include "stockmodel.h"
#include <QDebug>

// CONSTRUCTOR
StockModel::StockModel(QObject *parent) : QAbstractTableModel(parent) {
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
void StockModel::addItem(const StockItem &item) {

    // Only called by onAddButtonClicked

    beginInsertRows(QModelIndex(), items.size(), items.size()); // Must be called before insertion of data
    items.append(item);
    if (currentFilter.isEmpty() || item.productName.contains(currentFilter, Qt::CaseInsensitive)) {
        filteredItems.append(item);
    }
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
    endRemoveRows(); // Must be called at the end of removal
}

void StockModel::updateItem(int row, const StockItem &item) {

    // Called by onConfirmTransactionClicked (to change the remaining and sold)
    // and by onEditButtonClicked (to edit an item)

    if (row < 0 || row >= filteredItems.size()) // Guard
        return;

    filteredItems[row] = item;
    
    // Find the item in the main list
    for (int i = 0; i < items.size(); i++) {
        if (items[i].id == item.id) {
            items[i] = item;
            break;
        }
    }
    
    emit dataChanged(index(row, 0), index(row, columnCount() - 1)); //TODO: Is this even used????
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
