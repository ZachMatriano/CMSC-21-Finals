#include "analyticsmodel.h"
#include <QDateTime>
#include <algorithm>

AnalyticsModel::AnalyticsModel(QObject *parent)
    : QAbstractListModel(parent)
    , currentPeriod(TimePeriod::LastWeek)
{
    // Initial data load will be triggered by MainWindow after all models are initialized
}

int AnalyticsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return analyticsData.size();
}

QVariant AnalyticsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= analyticsData.size())
        return QVariant();

    if (role == Qt::DisplayRole) {
        const ProductAnalytics &analytics = analyticsData[index.row()];
        return QString("%1 - Sold: %2 (Rate: %3/day)")
            .arg(analytics.productName)
            .arg(analytics.totalSold)
            .arg(QString::number(analytics.salesRate, 'f', 2));
    }
    return QVariant();
}

void AnalyticsModel::setTimePeriod(TimePeriod period)
{
    if (currentPeriod != period) {
        currentPeriod = period;
        // Trigger update of the view
        beginResetModel();
        endResetModel();
    }
}

void AnalyticsModel::updateAnalytics(const QVector<ConfirmedTransaction>& transactions, TimePeriod period)
{
    beginResetModel();
    currentPeriod = period;
    calculateAnalytics(transactions);
    endResetModel();
}

void AnalyticsModel::calculateAnalytics(const QVector<ConfirmedTransaction>& transactions)
{
    QDateTime now = QDateTime::currentDateTime();
    QDateTime cutoff;
    
    // Set cutoff date based on current period
    switch (currentPeriod) {
        case TimePeriod::LastWeek:
            cutoff = now.addDays(-7);
            break;
        case TimePeriod::LastMonth:
            cutoff = now.addDays(-30);
            break;
        case TimePeriod::LastYear:
            cutoff = now.addDays(-365);
            break;
    }

    // Map to store product analytics
    QMap<QString, ProductAnalytics> productMap;

    // Process transactions within the time period
    for (const ConfirmedTransaction &transaction : transactions) {
        if (transaction.timestamp >= cutoff) {
            QString productName = transaction.item.productName;
            
            if (!productMap.contains(productName)) {
                ProductAnalytics analytics;
                analytics.productName = productName;
                analytics.totalSold = 0;
                analytics.timePeriodDays = (currentPeriod == TimePeriod::LastWeek) ? 7 :
                                         (currentPeriod == TimePeriod::LastMonth) ? 30 : 365;
                productMap[productName] = analytics;
            }
            
            productMap[productName].totalSold += transaction.quantity;
        }
    }

    // Calculate sales rates and convert to vector
    analyticsData.clear();
    for (auto it = productMap.begin(); it != productMap.end(); ++it) {
        ProductAnalytics &analytics = it.value();
        analytics.salesRate = static_cast<double>(analytics.totalSold) / analytics.timePeriodDays;
        analyticsData.append(analytics);
    }

    // Sort by sales rate (highest first)
    std::sort(analyticsData.begin(), analyticsData.end(),
              [](const ProductAnalytics &a, const ProductAnalytics &b) {
                  return a.salesRate > b.salesRate;
              });
} 