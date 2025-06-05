#ifndef STOCKMODEL_H
#define STOCKMODEL_H

#include <QAbstractTableModel>
#include <QVector>

struct StockItem { // Our Stock Item structure
    int id; // Used to display on the StockModel
    QString productName;
    double price;
    int stock;
    int remaining; // Technically redundant but its more security that the data is performing the correct way
    int sold;

    bool operator==(const StockItem &other) const { // For easy comparison
        return id == other.id &&
               productName == other.productName &&
               price == other.price &&
               stock == other.stock &&
               remaining == other.remaining &&
               sold == other.sold;
    }
};

class StockModel : public QAbstractTableModel {
    Q_OBJECT
    private:
        QVector<StockItem> items; // The whole stock
        QVector<StockItem> filteredItems; // The filtered stock, our actual working copy of the items
        QString currentFilter; // The filter

    public:
        explicit StockModel(QObject *parent = nullptr); // Constructor

    // Required overrides for QAbstractTableModel: see in the documentation
        int rowCount(const QModelIndex &parent = QModelIndex()) const override; // const = nothing should change in the object
        int columnCount(const QModelIndex &parent = QModelIndex()) const override; // override = marker that it overrides a virtual function
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
};

#endif
