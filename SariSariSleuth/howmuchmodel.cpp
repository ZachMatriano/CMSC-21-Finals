#include "howmuchmodel.h"
#include <QFont>
#include <cmath>

HowMuchModel::HowMuchModel(QObject *parent)
    : QAbstractListModel(parent)
    , daysToStock(7)
{
}

int HowMuchModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;
    return recommendations.size();
}

QVariant HowMuchModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= recommendations.size())
        return QVariant();

    const StockRecommendation &recommendation = recommendations[index.row()];

    if (role == Qt::DisplayRole) {
        return QString("%1 - Stock: %2").arg(recommendation.productName).arg(QString::number(recommendation.amountToStock, 'f', 0));
    }
    else if (role == Qt::FontRole && recommendation.isOutOfStock) {
        QFont font;
        font.setBold(true);
        return font;
    }

    return QVariant();
}

void HowMuchModel::updateRecommendations(const QVector<ProductAnalytics>& analytics, int daysToStock, const StockModel* stockModel) {

    // Called by onDaysToStockChanged and onTimePeriodChanged
    beginResetModel();
    recommendations.clear();

    for (const ProductAnalytics &analytic : analytics) {
        StockRecommendation recommendation;
        recommendation.productName = analytic.productName;
        recommendation.amountToStock = 1.65 * analytic.salesRate * std::sqrt(daysToStock);
        
        // Check if item is out of stock
        recommendation.isOutOfStock = false;
        for (int i = 0; i < stockModel->rowCount(); ++i) {
            StockItem item = stockModel->getItem(i);
            if (item.productName == analytic.productName && item.remaining == 0) {
                recommendation.isOutOfStock = true;
                break;
            }
        }
        
        recommendations.append(recommendation);
    }

    // Sort: out of stock items first, then by amount to stock
    std::sort(recommendations.begin(), recommendations.end(),
              [](const StockRecommendation &a, const StockRecommendation &b) {
                  if (a.isOutOfStock != b.isOutOfStock)
                      return a.isOutOfStock;
                  return a.amountToStock > b.amountToStock;
              });

    endResetModel();
} 
