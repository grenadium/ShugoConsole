#include "wintaskscheduler.h"

#include <QProcess>

bool WinTaskScheduler::isTaskInstalled(const QString &taskName)
{
    return 0 == QProcess::execute("schtasks", {"/query", "/tn", taskName});
}

bool WinTaskScheduler::deleteTask(const QString &taskName)
{
    return 0 == QProcess::execute("schtasks", {"/delete", "/f", "/tn", taskName});
}

bool WinTaskScheduler::createStartupTask(const QString &taskName, const QString &taskPath)
{
    return 0 == QProcess::execute("schtasks", {"/create", "/f", "/sc", "onlogon", "/rl", "highest", "/tn", taskName, "/tr", taskPath });
}
