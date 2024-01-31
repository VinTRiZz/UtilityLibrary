#ifndef TREEITEMMODEL_H
#define TREEITEMMODEL_H

#define TREEITEMMODEL_WORK_AS_PROXY

#include <QAbstractProxyModel>
#include <memory>

namespace TreeProxy
{

const QString TREEITEMMODEL_EMPTY_FIELD_FILL {"[NULL]"};

class TreeItem;

#ifdef TREEITEMMODEL_WORK_AS_PROXY

enum TreeItemModel_ColumnNumbers
{
    ID_COLUMN_NUMBER  = 0,
    PID_COLUMN_NUMBER = 1
};
#endif // TREEITEMMODEL_WORK_AS_PROXY

class TreeItemModel : public QAbstractItemModel
{
public:
    explicit TreeItemModel(QObject *parent = nullptr);
    ~TreeItemModel();

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;

    bool addItem(const QVariant & data, const QModelIndex & parent = QModelIndex());
    bool addItem(const QVector<QVariant> & data, const QModelIndex & parent = QModelIndex());
    bool removeItem(const QModelIndex & index);

#ifdef TREEITEMMODEL_WORK_AS_PROXY
    void setSourceModel(QAbstractItemModel *sourceModel);

    // Replaces internal data with data from source model
    void replaceModelData();
#endif // TREEITEMMODEL_WORK_AS_PROXY

private:
    struct Impl;
    std::unique_ptr<Impl> d;

    bool addItem(const QVector<QVariant> & data, const uint id = 0, const uint pid = 0);
    void removeItem(std::shared_ptr<TreeItem> pItem);
    void clearChildren(std::shared_ptr<TreeItem> pItem);

#ifdef TREEITEMMODEL_WORK_AS_PROXY
    // Source interact
    bool addItemFromSource(QVector<QVariant> &data, QVector<QModelIndex> &sourceIndexes);
    bool addItemAsProxy(QVector<QVariant> data, const QModelIndex &parentIndex);

    // Sync data with source
    bool addItemToSource(const QVector<QVariant> & data, uint id, uint pid);
    bool editItemInSource(const QVariant & newData, const QModelIndex & itemSourceIndex, const int role);
    bool removeItemFromSource(const QModelIndex & nameSourceIndex);
    bool removeItemsFromSource();

    void searchForNewRowsInModel(); // Used only after item added to get changes
#endif // TREEITEMMODEL_WORK_AS_PROXY

    // Etc, can be used for unit-test
    void testSetupTreeModel();
};

}

#endif // TREEITEMMODEL_H
