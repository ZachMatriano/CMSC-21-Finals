#ifndef ITEMSELECTIONDIALOG_H
#define ITEMSELECTIONDIALOG_H

#include <QDialog>
#include <QTableView>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include "stockmodel.h"

class ItemSelectionDialog : public QDialog {
    Q_OBJECT

    private:
        QTableView *tableView;
        QSpinBox *quantitySpinBox;
        QPushButton *acceptButton;
        QPushButton *cancelButton;
        QLineEdit *searchBar;
        QSortFilterProxyModel *proxyModel;
        StockModel *stockModel;
        StockItem selectedItem;

    private slots:
        void onItemSelected(const QItemSelection &selected, const QItemSelection &deselected);
        void onAcceptClicked();
        void onCancelClicked();
        void onSearchTextChanged(const QString &text);

    public:
        explicit ItemSelectionDialog(StockModel *model, QWidget *parent = nullptr);
        ~ItemSelectionDialog();
        StockItem getSelectedItem() const { return selectedItem; }
        int getQuantity() const { return quantitySpinBox->value(); }
};

#endif
