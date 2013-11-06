#include "processlistmodel.h"
#include <QTranslator>

ProcessListModel::ProcessListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

void ProcessListModel::addProcess(HANDLE h)
{
    _data.append(ProcessData(h));
    beginInsertRows(QModelIndex(), _data.size()-1, _data.size()-1);
    endInsertRows();
}

void ProcessListModel::removeProcess(HANDLE h)
{
    int row = 0;
    QMutableListIterator<ProcessData> i(_data);
    while(i.hasNext())
    {
        ProcessData& pd = i.next();
        if(h == pd.handle)
        {
            _data.removeAt(row);
            beginRemoveRows(QModelIndex(), row, row);
            endRemoveRows();
            break;
        }
        row ++;
    }
}

void ProcessListModel::setProcessState(HANDLE h, AionProcessWorker::State state)
{
    int row = 0;
    QMutableListIterator<ProcessData> i(_data);
    while(i.hasNext())
    {
        ProcessData& pd = i.next();
        if(h == pd.handle)
        {
            pd.state = state;
            QModelIndex index = createIndex(row, 0);
            emit dataChanged(index, index);
            break;
        }
        row ++;
    }
}

QVariant ProcessListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= _data.size()) return QVariant();

    if(role == Qt::DisplayRole)
    {
        QString msg;

        ProcessData pd = _data[index.row()];
        switch(pd.state)
        {
        case AionProcessWorker::WAITING:
            msg = tr("OK");
            break;
        case AionProcessWorker::INITIALIZING:
            msg = tr("Initializing...");
            break;
        case AionProcessWorker::SCANNING_MEMORY:
            msg = tr("Scanning memory...");
            break;
        case AionProcessWorker::E_PROCESS_IS_64:
            msg = tr("Process is 64 bit, cannot open it from a 32-bit ShugoConsole");
            break;
        case AionProcessWorker::E_OPEN_PROCESS_FAILED:
            msg = tr("Cannot open process. You should run ShugoConsole as an administrator.");
            break;
        case AionProcessWorker::E_READ_ERROR:
            msg = tr("Process read error.");
            break;
        case AionProcessWorker::E_WRITE_ERROR:
            msg = tr("Process write error");
            break;
        }

        msg = QString(tr("Process %1: %2")).arg((qulonglong)pd.handle).arg(msg);

        return QVariant(msg);
    }
    else
    {
        return QVariant();
    }
}

int ProcessListModel::rowCount(const QModelIndex &parent) const
{
     Q_UNUSED(parent)
     return _data.size();
}
