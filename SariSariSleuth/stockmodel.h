#ifndef STOCKMODEL_H
#define STOCKMODEL_H

#include <QAbstractTableModel>
#include <QVector>

struct StockItem {
    int id;
    QString productName;
    double price;
    int stock;
    int remaining;
    int sold;

    bool operator==(const StockItem &other) const {
        return id == other.id &&
               productName == other.productName &&
               price == other.price &&
               stock == other.stock &&
               remaining == other.remaining &&
               sold == other.sold;
    }
};

class StockModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit StockModel(QObject *parent = nullptr);

    // Required overrides for QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // Custom methods for data manipulation
    void addItem(const StockItem &item);
    void removeItem(int row);
    void updateItem(int row, const StockItem &item);
    StockItem getItem(int row) const;
    void filterItems(const QString &text);

private:
    QVector<StockItem> items;
    QVector<StockItem> filteredItems;
    QString currentFilter;
};

#endif // STOCKMODEL_H 