#ifndef AIONPROCESS_H
#define AIONPROCESS_H

#include <QObject>
#include <QTimer>
#include <QList>
#include <QString>
#include <QVariant>
#include <QMutex>

#include "crycvar.h"

class AionProcessWorker : public QObject
{
    Q_OBJECT
public:
    explicit AionProcessWorker(QObject *parent, HANDLE _process_id);
    virtual ~AionProcessWorker();

    enum State
    {
        WAITING,
        INITIALIZING,
        SCANNING_MEMORY,

        E_PROCESS_IS_64,
        E_OPEN_PROCESS_FAILED,
        E_READ_ERROR,
        E_WRITE_ERROR
    };

    HANDLE getPid() const { return process_id; }
    void updateVars(const QHash<QString, QVariant>& updateVariables);

signals:
    void stateUpdate(AionProcessWorker*, AionProcessWorker::State);
    
public slots:
    void init();
    void checkTimer();

private:
    void periodicMemoryScan();
    void resetProcessHandles();

    HANDLE process_id;
    HANDLE hProcess;

    QTimer* timer;

    struct ConsoleVariable
    {
        CryCVar cvar;
        QVariant wantedValue;
    };

    QHash<QString, ConsoleVariable> consoleVariables;

    QMutex varsLock;
};

Q_DECLARE_METATYPE(AionProcessWorker*)
Q_DECLARE_METATYPE(AionProcessWorker::State)

#endif // AIONPROCESS_H
