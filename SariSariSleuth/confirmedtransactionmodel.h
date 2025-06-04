#ifndef CONFIRMEDTRANSACTIONMODEL_H
#define CONFIRMEDTRANSACTIONMODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QDateTime>
#include "stockmodel.h"

struct ConfirmedTransaction {
    int transactionId;
    StockItem item;
    int quantity;
    QDateTime timestamp;

    bool operator==(const ConfirmedTransaction &other) const {
        return transactionId == other.transactionId &&
               item == other.item &&
               quantity == other.quantity &&
               timestamp == other.timestamp;
    }
};

class ConfirmedTransactionModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit ConfirmedTransactionModel(QObject *parent = nullptr);

    // Required overrides for QAbstractTableModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Custom methods for data manipulation
    void addTransaction(const StockItem &item, int quantity);
    ConfirmedTransaction getTransaction(int row) const;
    void clearTransactions();

private:
    QVector<ConfirmedTransaction> transactions;
    int nextTransactionId;
};

#endif // CONFIRMEDTRANSACTIONMODEL_H 