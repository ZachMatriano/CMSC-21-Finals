#ifndef TRANSACTIONMODEL_H
#define TRANSACTIONMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QDateTime>
#include "stockmodel.h"

struct Transaction {
    int transactionId;
    StockItem item;
    int quantity;
    QDateTime timestamp;

    bool operator==(const Transaction &other) const {
        return transactionId == other.transactionId &&
               item == other.item &&
               quantity == other.quantity &&
               timestamp == other.timestamp;
    }
};

class TransactionModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit TransactionModel(QObject *parent = nullptr);

    // Required overrides for QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Custom methods for data manipulation
    void addTransaction(const StockItem &item, int quantity);
    Transaction getTransaction(int row) const;
    void removeTransaction(int row);
    void clearTransactions();

private:
    QVector<Transaction> transactions;
    int nextTransactionId;
};

#endif // TRANSACTIONMODEL_H 