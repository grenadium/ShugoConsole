#ifndef PROCESSLISTMODEL_H
#define PROCESSLISTMODEL_H

#include "aionprocessworker.h"

#include <QAbstractListModel>
#include <QMutableListIterator>

class ProcessListModel : public QAbstractListModel
{
     Q_OBJECT
public:
    ProcessListModel(QObject * parent = 0);

    void addProcess(HANDLE h);
    void removeProcess(HANDLE h);
    void setProcessState(HANDLE h, AionProcessWorker::State state);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent) const;

private:
    struct ProcessData
    {
        ProcessData(HANDLE _handle = NULL, AionProcessWorker::State _state = AionProcessWorker::INITIALIZING)
            : handle(_handle), state(_state) {}
        HANDLE handle;
        AionProcessWorker::State state;
    };

    typedef QList<ProcessData> Data;

    Data _data;
};


#endif // PROCESSLISTMODEL_H
