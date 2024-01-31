#include "treeitemmodel.h"

#include "treeitem.h"

#include <QDebug>
#include <map>
#include <QStandardItem>

using namespace TreeProxy;

struct TreeItemModel::Impl
{
    uint currentId {0};
    std::shared_ptr<TreeItem> headItem;

    std::shared_ptr<TreeItem> findItem(uint id, std::shared_ptr<TreeItem> currentItem = std::shared_ptr<TreeItem>());
    std::shared_ptr<TreeItem> findItem(const QModelIndex & index, std::shared_ptr<TreeItem> currentItem = std::shared_ptr<TreeItem>());

#ifdef TREEITEMMODEL_WORK_AS_PROXY
    QAbstractItemModel * sourceModel {nullptr};
    long currentSourceRowCount = 0;
    QVector<uint> idsToRemove; // List of ids to remove from source model

    // Source to proxy equ work
    std::map<QModelIndex, QModelIndex> stpEqus;
    QModelIndex source(const QModelIndex & proxyIndex);
    QModelIndex proxy(const QModelIndex & sourceIndex);
#endif // TREEITEMMODEL_WORK_AS_PROXY
};


TreeItemModel::TreeItemModel(QObject *parent) :
    QAbstractItemModel(parent),
    d {new Impl}
{
    d->headItem = std::make_shared<TreeItem>("ROOT");
    d->headItem->setId(0);
    d->headItem->setDataIndex(createIndex(-1, -1, (quintptr)0));

    beginInsertRows(d->headItem->dataIndex(), 0, 1);
    endInsertRows();

//    testSetupTreeModel();
}


TreeItemModel::~TreeItemModel()
{

}

// ------------------------------------------------------------------- //
// ------------------------------------------------------------------- //
// ----------------------- BASIC FUNCTIONAL -------------------------- //
// ------------------------------------------------------------------- //
// ------------------------------------------------------------------- //

QVariant TreeItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    Q_UNUSED(orientation) // No need, orientation must be horizontal
    return d->headItem->data(section);
}


QModelIndex TreeItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (row < 0)
        return QModelIndex();

    auto parentPos = d->findItem(parent);
    if (!parentPos.use_count())
        return QModelIndex();

    auto pChild = parentPos->childAt(row);
    if (pChild.use_count())
        return pChild->dataIndex(column);

    return QModelIndex();
}


QModelIndex TreeItemModel::parent(const QModelIndex &index) const
{
    auto pChild = d->findItem(index);
    if (!pChild.use_count())
        return QModelIndex();
    return pChild->parentIndex();
}


int TreeItemModel::rowCount(const QModelIndex &parent) const
{
    auto pParent = d->findItem(parent);
    if (!pParent.use_count())
        return 0;
    return pParent->childCount();
}


int TreeItemModel::columnCount(const QModelIndex &parent) const
{
    auto pParent = d->findItem(parent);
    if (!pParent.use_count())
        return 0;
    return pParent->columnCount();
}


QVariant TreeItemModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    auto pItem = d->findItem(index);
    if (!pItem.use_count())
        return 0;
    return pItem->data(index);
}


bool TreeItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::DisplayRole)
    {
        qDebug() << "[TreeItemModel] Set data called not for a display role, action skipped";
        return false;
    }
    auto pItem = d->findItem(index);

    if (!pItem.use_count())
        return false;

#ifdef TREEITEMMODEL_WORK_AS_PROXY
//    if (pItem->data(index.column()) == TREEITEMMODEL_EMPTY_FIELD_FILL)
//        replaceModelData();

//    QModelIndex itemEqu = d->sourceModel->parent(d->source(itemSourceIndex));
//    qDebug() << itemEqu;
//    itemEqu = d->sourceModel->index(itemEqu.row(), itemEqu.column() + 2, itemEqu);
//    qDebug() << itemEqu;

    if (!editItemInSource(value, index, role))
        return false;
#endif // TREEITEMMODEL_WORK_AS_PROXY

    pItem->setData(value, index.column());
    emit layoutChanged();
    return true;
}


std::shared_ptr<TreeItem> TreeItemModel::Impl::findItem(uint id, std::shared_ptr<TreeItem> currentItem)
{
    if (!currentItem.use_count())
        currentItem = headItem;

    if (currentItem->id() == id)
        return currentItem;

    std::shared_ptr<TreeItem> result;

    for (auto pItem : currentItem->childArray())
    {
        result = findItem(id, pItem.lock());
        if (result.use_count())
        {
            return result;
        }
    }
    return result;
}


std::shared_ptr<TreeItem> TreeItemModel::Impl::findItem(const QModelIndex &index, std::shared_ptr<TreeItem> currentItem)
{
    if (!currentItem.use_count())
        currentItem = headItem;

    if (currentItem->dataIndex().internalId() == index.internalId())
        return currentItem;

    std::shared_ptr<TreeItem> result;

    for (auto pItem : currentItem->childArray())
    {
        result = findItem(index, pItem.lock());
        if (result.use_count())
            return result;
    }
    return result;
}


bool TreeItemModel::addItem(const QVariant &data, const QModelIndex &parent)
{
    QVector<QVariant> inputData;
    inputData.push_back(data);
    return addItem(inputData, parent);
}


bool TreeItemModel::addItem(const QVector<QVariant> &data, const QModelIndex &parent)
{
#ifdef TREEITEMMODEL_WORK_AS_PROXY
    return addItemAsProxy(data, parent); // Redirection to proxy mode function
#endif // TREEITEMMODEL_WORK_AS_PROXY

    auto pParent = d->findItem(parent);
    if (!pParent.use_count())
        return false;

    return addItem(data, d->currentId + 1, pParent->id()); // ID will be updated automatically right here
}

bool TreeItemModel::removeItem(const QModelIndex &index)
{
    auto pParent = d->findItem(index);
    if (!pParent.use_count())
        return false;

    removeItem(pParent);
    return true;
}


bool TreeItemModel::addItem(const QVector<QVariant> &data, const uint id, const uint pid)
{
    if (d->findItem(id).use_count())
    {
        qDebug() << "[TreeItemModel] Found item with equal ID:" << id << "- ITEM NOT ADDED";
        return false;
    }

    std::weak_ptr<TreeItem> pParent_w = d->findItem(pid);

    if (pParent_w.use_count())
    {
        std::shared_ptr<TreeItem> pParent = pParent_w.lock();

        const int row = pParent->childCount();
        pParent->childAdd(data);

        std::shared_ptr<TreeItem> pItem = pParent->childAt( row );
        pItem->setId(id);

        const uint id = pItem->id();

        beginInsertRows(pParent->dataIndex(), row, row + 1);
        pItem->setDataIndex(createIndex( row, 0, (quintptr)id ));

        beginInsertColumns(pItem->dataIndex(), 0, data.size() - 1);
        pItem->setColumnCount(data.size());

        for (int i = 1; i < data.size(); i++)
            pItem->setDataIndex(createIndex( row, i, (quintptr)id ), i);
        pItem->setData(data);

        if (id > d->currentId) // Remember ID as max id in model
            d->currentId = id;

        endInsertColumns();
        endInsertRows();

        emit layoutChanged();
        return true;
    }
    return false;
}


void TreeItemModel::removeItem(std::shared_ptr<TreeItem> pItem)
{
#ifdef TREEITEMMODEL_WORK_AS_PROXY
    d->idsToRemove.clear();
#endif // TREEITEMMODEL_WORK_AS_PROXY

    if (pItem->childCount())
        clearChildren(pItem);

#ifdef TREEITEMMODEL_WORK_AS_PROXY
    removeItemsFromSource();
#endif // TREEITEMMODEL_WORK_AS_PROXY

    std::weak_ptr<TreeItem> pParent_w = d->findItem(pItem->parentIndex());

    if (pParent_w.use_count())
    {
        std::shared_ptr<TreeItem> pParent = pParent_w.lock();

        beginRemoveRows(pParent->dataIndex(), 0, 1);
        beginRemoveColumns(pParent->dataIndex(), 0, pItem->columnCount());
        pParent->childRemove(pItem->dataIndex());
        for (int i = 0; i < pItem->columnCount(); i++)
            pItem->setDataIndex(QModelIndex(), i);
        endRemoveColumns();
        endRemoveRows();

        const int childCount = pParent->childCount();
        for (int c = 0; c < childCount; c++)
        {
            if (c != pParent->childAt(c)->dataIndex().row())
            {
                beginInsertRows(pParent->dataIndex(), c, c + 1);
                pParent->childAt(c)->setDataIndex(createIndex(c, 0, (quintptr)pParent->childAt(c)->id() ), 0);
                if (d->headItem->columnCount())
                {
                    beginInsertColumns(pParent->childAt(c)->dataIndex(), 1, d->headItem->columnCount() - 1);
                    for (int col = 1; col < d->headItem->columnCount(); col++)
                        pParent->childAt(c)->setDataIndex(createIndex(c, col, (quintptr)pParent->childAt(c)->id() ), col);
                    endInsertColumns();
                }
                endInsertRows();
            }
        }
        emit layoutChanged();
    }
    else
        qDebug() << "[TreeItemModel] Not found parent with such index:" << pItem->parentIndex();
}


void TreeItemModel::clearChildren(std::shared_ptr<TreeItem> pItem)
{
    std::shared_ptr<TreeItem> pChild;
    const int childCount = pItem->childCount();

    for (int i = 0; i < childCount; i++)
    {
        pChild = pItem->childAt(i);
        clearChildren(pChild);
    }
    beginRemoveRows(pItem->dataIndex(), 0, pItem->childCount());
    beginRemoveColumns(pItem->dataIndex(), 0, pItem->columnCount());

#ifdef TREEITEMMODEL_WORK_AS_PROXY
    d->idsToRemove.push_back(pItem->id());
#endif // TREEITEMMODEL_WORK_AS_PROXY

    pItem->childClear();
    endRemoveColumns();
    endRemoveRows();
}

// ------------------------------------------------------------------- //
// ------------------------------------------------------------------- //
// ----------------------- PROXY FUNCTIONAL -------------------------- //
// ------------------------------------------------------------------- //
// ------------------------------------------------------------------- //
#ifdef TREEITEMMODEL_WORK_AS_PROXY
void TreeItemModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    d->sourceModel = sourceModel;
    replaceModelData();
    qDebug() << "[TreeItemModel: Proxy mode] Source model changed";
}


QModelIndex TreeItemModel::Impl::source(const QModelIndex &proxyIndex)
{
    for (const std::pair<QModelIndex, QModelIndex> & p : stpEqus)
    {
        if (p.second == proxyIndex)
            return p.first;
    }
    qDebug() << "[TreeItemModel: Proxy mode] Source index not found for index:" << proxyIndex;
    return QModelIndex();
}


QModelIndex TreeItemModel::Impl::proxy(const QModelIndex &sourceIndex)
{
    for (const std::pair<QModelIndex, QModelIndex> & p : stpEqus)
    {
        if (p.first == sourceIndex)
            return p.second;
    }
    qDebug() << "[TreeItemModel: Proxy mode] Proxy index not found for index:" << sourceIndex;
    return QModelIndex();
}


bool TreeItemModel::addItemFromSource(QVector<QVariant> &data, QVector<QModelIndex> &sourceIndexes)
{
    uint itemId = data[TreeItemModel_ColumnNumbers::ID_COLUMN_NUMBER].toInt(),
         itemPid = data[TreeItemModel_ColumnNumbers::PID_COLUMN_NUMBER].toInt()
    ;
    data.remove(0, 2);
    sourceIndexes.remove(0, 2);

    bool result = addItem(data, itemId, itemPid);

    if (result)
    {
        auto pItem = d->findItem(itemId);

        for (int i = 0; i < pItem->columnCount(); i++)
            d->stpEqus[sourceIndexes[i]] = pItem->dataIndex(i);
    }

    return result;
}


bool TreeItemModel::addItemAsProxy(QVector<QVariant> data, const QModelIndex &parentIndex)
{
    auto pParent = d->findItem(parentIndex);
    if (!pParent.use_count())
        return false;

    for (int i = data.size(); i < d->headItem->columnCount(); i++)
        data.push_back(TREEITEMMODEL_EMPTY_FIELD_FILL);

    if (!addItemToSource(data, d->currentId + 1, pParent->id()))
    {
        qDebug() << "[TreeItemModel: Proxy mode] Error adding element to source model";
        return false;
    }
    replaceModelData();
    return true;
}


// ------------------------------------------------------------------- //
// ------------------------ SOURCE MODEL WORK ------------------------ //
// ------------------------------------------------------------------- //
bool TreeItemModel::addItemToSource(const QVector<QVariant> &data, uint id, uint pid)
{
    d->sourceModel->insertRow(d->sourceModel->rowCount() - 1);

    d->sourceModel->setData( d->sourceModel->index(d->sourceModel->rowCount() - 2, 0), id );
    d->sourceModel->setData( d->sourceModel->index(d->sourceModel->rowCount() - 2, 1), pid );

    for (int i = 0; i < data.size(); i++)
        d->sourceModel->setData( d->sourceModel->index(d->sourceModel->rowCount() - 2, i + 2), data[i] );

    return true;
}
bool TreeItemModel::editItemInSource(const QVariant &newData, const QModelIndex &itemSourceIndex, const int role)
{
    return d->sourceModel->setData(d->source(itemSourceIndex), newData, role);
}

bool TreeItemModel::removeItemFromSource(const QModelIndex &nameSourceIndex)
{
    if (!nameSourceIndex.isValid())
        return false;

    bool result = d->sourceModel->removeRow(nameSourceIndex.row());
    if (nameSourceIndex.row() != d->sourceModel->rowCount())
        replaceModelData(); // Update indexes after deleting one
    return result;
}

bool TreeItemModel::removeItemsFromSource()
{
    auto needToRemove = [this](uint itemId){
        for (uint id : d->idsToRemove)
        {
            if (id == itemId)
                return true;
        }
        return false;
    };

    QModelIndex tempIndex;
    int row = 0;
    uint id = 0;
    uint currentLoop = 0;
    bool result = false;

    while (d->idsToRemove.size())
    {
        tempIndex = d->sourceModel->index(row, TreeItemModel_ColumnNumbers::ID_COLUMN_NUMBER);
        row++;

        id = d->sourceModel->data(tempIndex).toInt();
        if (needToRemove(id))
        {
            result = removeItemFromSource(tempIndex);
            if (!result)
                return false;
            d->idsToRemove.removeOne(id);
            row = 0;
        }

        // TODO: Remove, protection
        currentLoop++;
        if (currentLoop > 10000)
            return false;
    }
    return true;
}

// ------------------------------------------------------------------- //
// --------------------- UPDATE PROXY MODEL DATA --------------------- //
// ------------------------------------------------------------------- //
void TreeItemModel::replaceModelData()
{
    if (d->stpEqus.size())
    {
        d->currentId = 1;
        beginRemoveColumns(d->headItem->dataIndex(), 0, d->headItem->columnCount());
        beginRemoveRows(d->headItem->dataIndex(), 0, d->headItem->childCount());
        d->stpEqus.clear();
        d->headItem->childClear();
        endRemoveColumns();
        endRemoveRows();
    }

    // Get header data
    QVector<QVariant> dataBuffer;
    d->headItem->setColumnCount(d->sourceModel->columnCount() - 2);
    for (int i = 0; i < d->sourceModel->columnCount() - 2; i++)
        dataBuffer.push_back( d->sourceModel->headerData(i + 2, Qt::Horizontal, Qt::DisplayRole) );
    d->headItem->setData(dataBuffer);

    // Get items data
    QVector<QModelIndex> indexEqus;
    d->currentSourceRowCount = d->sourceModel->rowCount();
    for (int i = 0; i < d->currentSourceRowCount; i++)
    {
        indexEqus.clear();
        dataBuffer.clear();
        for (int j = 0; j < d->sourceModel->columnCount(); j++)
        {
            indexEqus.push_back(d->sourceModel->index(i, j));
            dataBuffer.push_back(d->sourceModel->data(indexEqus.last()));
        }

        addItemFromSource(dataBuffer, indexEqus);
    }
}
#endif // TREEITEMMODEL_WORK_AS_PROXY



// ------------------------------------------------------------------- //
// --------------------------- ETC (TESTS) --------------------------- //
// ------------------------------------------------------------------- //
void TreeItemModel::testSetupTreeModel()
{
    qDebug() << "\033[36m[TEST MODEL SETUP STARTED]\033[0m";

    d->headItem->setData("Col 1", 0);
    d->headItem->appendColumn("Col 2");
    d->headItem->appendColumn("Col 3");
    d->headItem->appendColumn("Col 4");

    QVector<QVariant> testData;

    for (int i = 0; i < d->headItem->columnCount(); i++)
    {
        testData.push_back(QVariant(i));
    }

    uint nextId = 1;
    for (int i = 0; i < 3; i++)
    {
        testData[0] = QString("Its head %1").arg(i);
        addItem(testData, nextId++);
    }

    for (int j = 0; j < 2; j++)
    {
        testData[0] = QString("Its child %1 of head 1").arg(QString::number(j));
        addItem(testData, nextId++, 1);
    }

    for (int k = 0; k < 3; k++)
    {
        testData[0] = QString("Its child %1 of child 2 of head 1").arg(QString::number(k));
        addItem(testData, nextId++, 4);
    }

    qDebug() << "\033[36m[TEST MODEL SETUP COMPLETE]\033[0m";
}
