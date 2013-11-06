#include <QCoreApplication>
#include <QThread>
#include <QHashIterator>

#include "win64utils.h"
#include "aionprocessworker.h"
#include "remotememorylookup.h"

AionProcessWorker::AionProcessWorker(QObject *parent, void* _process_id) :
    QObject(parent), process_id(_process_id), hProcess(NULL), timer(NULL)
{

}

AionProcessWorker::~AionProcessWorker()
{
    resetProcessHandles();
}

void AionProcessWorker::updateVars(const QHash<QString, QVariant>& updateVariables)
{
    QMutexLocker lock(&varsLock);

    QList<QString> update_keys = updateVariables.keys();
    QList<QString> worker_keys = consoleVariables.keys();

    foreach(QString s, worker_keys)
    {
        if(!update_keys.contains(s))
            consoleVariables.remove(s);
    }

    foreach(QString s, update_keys)
    {
        if(consoleVariables.contains(s))
        {
            ConsoleVariable cv;
            cv.cvar = consoleVariables[s].cvar;
            cv.wantedValue = updateVariables[s];
            consoleVariables[s] = cv;
        }
        else
        {
            ConsoleVariable cv;
            cv.wantedValue = updateVariables[s];
            consoleVariables.insert(s, cv);
        }
    }
}

void AionProcessWorker::periodicMemoryScan()
{
    if(!hProcess)
    {
        hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, FALSE, (DWORD)process_id);
        emit stateUpdate(this, INITIALIZING);
    }

    if(hProcess)
    {
        QMutexLocker lock(&varsLock);

        QList<QString> keys = consoleVariables.keys();
        foreach(const QString& key, keys)
        {
            CryCVar cvar = consoleVariables[key].cvar;
            if(!cvar.valid())
            {
                bool processIs64 = IsProcess64(hProcess);

                if(processIs64 && sizeof(void *) != 8)
                {
                    emit stateUpdate(this, E_PROCESS_IS_64);
                    QMetaObject::invokeMethod(thread(),"quit",Qt::QueuedConnection);
                    break;
                }

                CVarMemoryPattern pattern(processIs64 ? CVAR_MEM_NAME_64 - 1 : CVAR_MEM_NAME_32 - 1, key.toLatin1());
                RemoteMemoryLookup remote_lookup;
                emit stateUpdate(this, SCANNING_MEMORY);
                uint8_t* address_of_cvar = remote_lookup.addressOfPattern(hProcess, pattern);

                if(address_of_cvar)
                {
                    consoleVariables[key].cvar = cvar = CryCVar(hProcess, (char*)address_of_cvar, processIs64);
                }
            }

            if(cvar.valid())
            {
                bool read_ok = true;
                bool write_ok = true;

                QVariant value = consoleVariables[key].wantedValue;

                switch(value.type())
                {
                case QMetaType::Double:
                case QMetaType::Float:
                    {
                        float f = value.toFloat();
                        float v = cvar.floatValue(&read_ok);
                        if(f != v)
                        {
                            write_ok = cvar.set(f);
                        }
                    }
                    break;
                case QMetaType::Int:
                case QMetaType::Long:
                    {
                        int i = value.toInt();
                        int v = cvar.intValue(&read_ok);
                        if(i != v)
                        {
                            write_ok = cvar.set(i);
                        }
                    }
                    break;
                case QMetaType::QString:
                default:
                    {
                        QString s = value.toString();
                        write_ok = cvar.set(s.toLatin1().constData());
                    }
                    break;
                }

                if(!read_ok)
                {
                    emit stateUpdate(this, E_READ_ERROR);
                    resetProcessHandles();
                    break;
                }
                else if(!write_ok)
                {
                    emit stateUpdate(this, E_WRITE_ERROR);
                    resetProcessHandles();
                    break;
                }
            }
        }

        emit stateUpdate(this, WAITING);
    }
    else
    {
        emit stateUpdate(this, E_OPEN_PROCESS_FAILED);
        QMetaObject::invokeMethod(thread(),"quit",Qt::QueuedConnection);
    }
}

void AionProcessWorker::resetProcessHandles()
{
    if(hProcess)
    {
        CloseHandle(hProcess);
        hProcess = NULL;
    }

    QMutableHashIterator<QString, ConsoleVariable> i(consoleVariables);
    while (i.hasNext())
    {
         i.next();
         i.value().cvar = CryCVar();
    }
}

void AionProcessWorker::init()
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &AionProcessWorker::checkTimer);
    timer->setInterval(1000);
    timer->start();
}

void AionProcessWorker::checkTimer()
{
    periodicMemoryScan();
}
