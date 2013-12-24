#ifndef PROCESSUTILS_H
#define PROCESSUTILS_H

#include <QList>
#include <QString>

#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

QList<HANDLE> findProcessHandleByImageName(const QString& process_names);

#endif // PROCESSUTILS_H
