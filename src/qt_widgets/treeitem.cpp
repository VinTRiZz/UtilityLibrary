#include "treeitem.h"

#include <QDebug>

using namespace TreeProxy;

TreeItem::TreeItem(const QVariant &data)
{
    itemData.push_back(data);
    columnIndexes.resize(1);
}

TreeItem::TreeItem(const QVector<QVariant> &data)
{
    itemData = data;
    columnIndexes.resize(data.size());
}

TreeItem::~TreeItem()
{

}

uint TreeItem::id() const
{
    return itemId;
}

void TreeItem::setId(uint newId)
{
    itemId = newId;
}

void TreeItem::setColumnCount(const int count)
{
    itemData.resize(count);
}

int TreeItem::columnCount() const
{
    return itemData.size();
}

void TreeItem::insertColumn(const QVariant &data, const int beforeColumn)
{
    itemData.insert(beforeColumn, data);
}

void TreeItem::appendColumn(const QVariant &data)
{
    itemData.append(data);
}

void TreeItem::setData(const QVariant &data, const int column)
{
    if ((column < 0) || (column >= itemData.size()))
        return;

    itemData[column] = data;
}

void TreeItem::setData(const QVector<QVariant> &data)
{
    if (data.size() != itemData.size())
        return;

    itemData = data;
}

void TreeItem::setDataIndex(const QModelIndex &index, const int column)
{
    if ((column < 0) || (column >= columnIndexes.size()))
        return;

    columnIndexes[column] = index;
}

QVariant TreeItem::data(const int column) const
{
    if ((column < 0) || (column >= itemData.size()))
        return QVariant();

    return itemData[column];
}

QVariant TreeItem::data(const QModelIndex &index) const
{
    if (columnIndexes.contains(index))
        return itemData[ columnIndexes.indexOf(index) ];
    return QVariant();
}

QModelIndex TreeItem::dataIndex(const int column)
{
    if ((column < 0) || (column >= columnIndexes.size()))
        return QModelIndex();

    return columnIndexes[column];
}

QModelIndex TreeItem::parentIndex() const
{
    if (pParent == nullptr)
        return QModelIndex();

    return pParent->dataIndex(0);
}

void TreeItem::childAdd(const QVariant &data)
{
    children.push_back( std::make_shared<TreeItem>(data) );
    children.last()->setParent(this);
}

void TreeItem::childAdd(const QVector<QVariant> &data)
{
    children.push_back( std::make_shared<TreeItem>(data) );
    children.last()->setParent(this);
}

void TreeItem::childRemove(const int index)
{
    if ((index < 0) || (index >= children.size()))
        return;
    children.removeAt(index);
}

void TreeItem::childRemove(const QModelIndex &childIndex)
{
    int index = 0;
    for (auto pChild : children)
    {
        if (pChild->dataIndex() == childIndex)
        {
            childRemove(index);
            break;
        }
        index++;
    }
}

void TreeItem::childClear()
{
    for (auto & pChild : children)
        pChild.reset();
    children.clear();
}

std::shared_ptr<TreeItem> TreeItem::childAt(const int index)
{
    if ((index < 0) || (index >= children.size()))
        return std::shared_ptr<TreeItem>();

    return children.at(index);
}

std::shared_ptr<TreeItem> TreeItem::childAt(const QModelIndex &index)
{
    for (auto pChild : children)
    {
        if (pChild->dataIndex() == index)
        {
            return pChild;
        }
    }
    return std::shared_ptr<TreeItem>();
}

QVector<std::weak_ptr<TreeItem>> TreeItem::childArray()
{
    QVector<std::weak_ptr<TreeItem>> result;
    for (auto pChild : children)
    {
        result.push_back(pChild);
    }
    return result;
}

int TreeItem::childCount() const
{
    return children.count();
}

void TreeItem::setParent(TreeItem * parent)
{
    pParent = parent;
}

TreeItem * TreeItem::parent()
{
    return pParent;
}
