#ifndef SHUGOCONSOLEWIDGET_H
#define SHUGOCONSOLEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QThread>
#include <QHash>

#include "aionprocessworker.h"
#include "processlistmodel.h"

namespace Ui {
class ShugoConsoleWidget;
}

class ShugoConsoleWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ShugoConsoleWidget(QWidget *parent = 0);
    ~ShugoConsoleWidget();

public slots:
    void onScanTimer();
    void onStateUpdate(AionProcessWorker*, AionProcessWorker::State);

    void onFovSpinBoxValueChanged(double);
    void onFovCheckStateChanged(int);
    void onConsoleCheckStateChanged(int);

    void onTrayIconActivated(QSystemTrayIcon::ActivationReason);
    
private:
    void loadConfig();
    void saveConfig();

    void setVarValue(const QString&, const QVariant&);
    void removeVar(const QString&);
    void updateProcessesVars();

    Ui::ShugoConsoleWidget *ui;

    QTimer* timer;

    QAction *showAction;
    QAction *exitAction;

    QSystemTrayIcon* trayIcon;

    ProcessListModel* processListModel;

    QHash<HANDLE, AionProcessWorker*> workersHash;
    QHash<QString,QVariant> consoleVars;

};

#endif // SHUGOCONSOLEWIDGET_H
