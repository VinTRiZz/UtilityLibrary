#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVariant>
#include <QVector>
#include <memory>
#include <QModelIndex>

namespace TreeProxy
{

class TreeItem
{
public:
    TreeItem(const QVariant & data);
    TreeItem(const QVector<QVariant> & data);
    ~TreeItem();

    uint id() const;
    void setId(uint newId);

    void setColumnCount(const int count);
    int columnCount() const;
    void appendColumn(const QVariant & data);
    void insertColumn(const QVariant & data, const int beforeColumn);

    void setData(const QVariant & data, const int column = 0);
    void setData(const QVector<QVariant> & data);
    void setDataIndex(const QModelIndex & index, const int column = 0);
    QVariant data(const int column = 0) const;
    QVariant data(const QModelIndex & index) const;
    QModelIndex dataIndex(const int column = 0);

    QModelIndex parentIndex() const;

    void childAdd(const QVariant & data);
    void childAdd(const QVector<QVariant> & data);
    void childRemove(const int index);
    void childRemove(const QModelIndex & childIndex);
    void childClear();

    std::shared_ptr<TreeItem> child(const uint id);
    std::shared_ptr<TreeItem> child(const QVariant & data);
    std::shared_ptr<TreeItem> childAt(const int index);
    std::shared_ptr<TreeItem> childAt(const QModelIndex & index);
    QVector<std::weak_ptr<TreeItem>> childArray();
    int childCount() const;

private:
    TreeItem * pParent {nullptr};
    QVector< std::shared_ptr<TreeItem> > children;

    QVector<QVariant> itemData;
    QVector<QModelIndex> columnIndexes;

    uint itemId {0};
    void setParent(TreeItem * parent);
    TreeItem * parent();
};

}

#endif // TREEITEM_H
