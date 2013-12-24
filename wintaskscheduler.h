#ifndef WINTASKSCHEDULER_H
#define WINTASKSCHEDULER_H

#include <QString>

class WinTaskScheduler
{
public:
    static bool isTaskInstalled(const QString& taskName);
    static bool deleteTask(const QString& taskName);
    static bool createStartupTask(const QString& taskName, const QString& taskPath);
};

#endif // WINTASKSCHEDULER_H
