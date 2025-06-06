#ifndef HOWMUCHMODEL_H
#define HOWMUCHMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include "stockmodel.h"
#include "analyticsmodel.h"

struct StockRecommendation {
    QString productName;
    double amountToStock;
    bool isOutOfStock;
};

class HowMuchModel : public QAbstractListModel {
    Q_OBJECT

    private:
        QVector<StockRecommendation> recommendations;
        int daysToStock;

    public:
        explicit HowMuchModel(QObject *parent = nullptr);

    // Required overrides for QAbstractListModel
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Custom methods
        void updateRecommendations(const QVector<ProductAnalytics>& analytics, int daysToStock, const StockModel* stockModel);
        void setDaysToStock(int days);
};

#endif // HOWMUCHMODEL_H 
