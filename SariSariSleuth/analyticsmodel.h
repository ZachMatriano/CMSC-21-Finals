#ifndef ANALYTICSMODEL_H
#define ANALYTICSMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include "confirmedtransactionmodel.h"

enum class TimePeriod {
    LastWeek,
    LastMonth,
    LastYear
};

struct ProductAnalytics {
    QString productName;
    int totalSold;
    int timePeriodDays;
    double salesRate;  // totalSold / timePeriodDays
};

class AnalyticsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit AnalyticsModel(QObject *parent = nullptr);

    // Required overrides for QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Custom methods
    void updateAnalytics(const QVector<ConfirmedTransaction>& transactions, TimePeriod period);
    void setTimePeriod(TimePeriod period);

private:
    QVector<ProductAnalytics> analyticsData;
    TimePeriod currentPeriod;
    void calculateAnalytics(const QVector<ConfirmedTransaction>& transactions);
};

#endif // ANALYTICSMODEL_H 