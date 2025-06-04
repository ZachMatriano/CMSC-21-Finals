#include "stockmodel.h"
#include <QDebug>

StockModel::StockModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int StockModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return filteredItems.size();
}

int StockModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 6; // id, productName, price, stock, remaining, sold
}

QVariant StockModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= filteredItems.size() || index.column() >= 6)
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        const StockItem &item = filteredItems[index.row()];
        switch (index.column()) {
            case 0: return item.id;
            case 1: return item.productName;
            case 2: return QString::number(item.price, 'f', 2);
            case 3: return item.stock;
            case 4: return item.remaining;
            case 5: return item.sold;
            default: return QVariant();
        }
    }
    return QVariant();
}

QVariant StockModel::headerData(int section, Qt::Orientation orientation, int role) const
{
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

bool StockModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || role != Qt::EditRole)
        return false;

    if (index.row() >= filteredItems.size() || index.column() >= 6)
        return false;

    StockItem &item = filteredItems[index.row()];
    bool success = false;

    switch (index.column()) {
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

Qt::ItemFlags StockModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void StockModel::addItem(const StockItem &item)
{
    beginInsertRows(QModelIndex(), items.size(), items.size());
    items.append(item);
    if (currentFilter.isEmpty() || item.productName.contains(currentFilter, Qt::CaseInsensitive)) {
        filteredItems.append(item);
    }
    endInsertRows();
}

void StockModel::removeItem(int row)
{
    if (row < 0 || row >= filteredItems.size())
        return;

    beginRemoveRows(QModelIndex(), row, row);
    StockItem item = filteredItems[row];
    filteredItems.removeAt(row);
    items.removeOne(item);
    endRemoveRows();
}

void StockModel::updateItem(int row, const StockItem &item)
{
    if (row < 0 || row >= filteredItems.size())
        return;

    filteredItems[row] = item;
    int mainIndex = items.indexOf(filteredItems[row]);
    if (mainIndex != -1) {
        items[mainIndex] = item;
    }
    emit dataChanged(index(row, 0), index(row, columnCount() - 1));
}

StockItem StockModel::getItem(int row) const
{
    if (row >= 0 && row < filteredItems.size())
        return filteredItems[row];
    return StockItem();
}

void StockModel::filterItems(const QString &text)
{
    beginResetModel();
    currentFilter = text;
    filteredItems.clear();
    
    if (text.isEmpty()) {
        filteredItems = items;
    } else {
        for (const StockItem &item : items) {
            if (item.productName.contains(text, Qt::CaseInsensitive)) {
                filteredItems.append(item);
            }
        }
    }
    endResetModel();
} 