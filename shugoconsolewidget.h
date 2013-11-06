#ifndef SHUGOCONSOLEWIDGET_H
#define SHUGOCONSOLEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QThread>
#include <QHash>

#include "aionprocessworker.h"
#include "processlistmodel.h"
#include "variableeditor.h"

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
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason);

    void varEnable(QString, QVariant);
    void varDisable(QString);

    void saveSettings();
private:

    void updateProcessesVars();

    Ui::ShugoConsoleWidget *ui;

    QTimer* _timer;

    QAction *_showAction;
    QAction *_exitAction;

    QSystemTrayIcon* _trayIcon;

    ProcessListModel* _processListModel;

    QHash<HANDLE, AionProcessWorker*> _workersHash;
    QHash<QString,QVariant> _consoleVars;

    QList<VariableEditorBase*> _editors;

};

#endif // SHUGOCONSOLEWIDGET_H
