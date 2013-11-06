#include "shugoconsolewidget.h"
#include "ui_shugoconsolewidget.h"

#include <QMenu>
#include <QList>
#include "processutils.h"

#include <QSettings>

#include <QTranslator>


ShugoConsoleWidget::ShugoConsoleWidget(QWidget *parent) :
    QWidget(parent, Qt::Tool | Qt::WindowStaysOnTopHint),
    ui(new Ui::ShugoConsoleWidget)
{
    ui->setupUi(this);

    _showAction = new QAction(QString(tr("Show")),this);
    connect(_showAction,SIGNAL(triggered()),this,SLOT(show()));
    _exitAction = new QAction(QString(tr("Exit")),this);
    connect(_exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    QMenu *trayMenu = new QMenu(this);
    trayMenu->addAction(_showAction);
    trayMenu->addSeparator();
    trayMenu->addAction(_exitAction);

    _trayIcon = new QSystemTrayIcon(QIcon(":/ShugoConsole/shugo.png"),this);
    _trayIcon->setContextMenu(trayMenu);
    connect(_trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onTrayIconActivated(QSystemTrayIcon::ActivationReason)));

    _trayIcon->show();

    _processListModel = new ProcessListModel(this);
    ui->processesView->setModel(_processListModel);

#ifdef ENABLE_CONSOLE_ACTIVATION
    ui->consoleSettingsGroupBox->show();
    editors.append(new BoolComboVariableEditor(this, "con_disable_console", "EnableConsole", "EnableConsoleState", ui->consoleCheck, ui->consoleCombo, true));
#else
    ui->consoleSettingsGroupBox->hide();
#endif

    _editors.append(new DoubleSpinVariableEditor(this, "g_minFov", "EnableFOV", "FOV", ui->fovCheck, ui->fovSpinBox, 90.0, 60.0, 170.0));

    _editors.append(new BoolComboVariableEditor(this, "g_chatlog", "g_chatlog_monitor", "g_chatlog_value", ui->chatLogCheck, ui->chatLogCombo, true));
    _editors.append(new DoubleSpinVariableEditor(this, "g_camMax", "g_camMax_monitor", "g_camMax_value", ui->cammaxCheck, ui->cammaxSpinBox, 90.0, 60.0, 170.0));
    _editors.append(new BoolComboVariableEditor(this, "d3d9_TripleBuffering", "d3d9_TripleBuffering_monitor", "d3d9_TripleBuffering_value", ui->tripleBufferingCheck, ui->tripleBufferingCombo, true));
    _editors.append(new ValueComboVariableEditor(this, "r_Texture_Anisotropic_Level", "r_Texture_Anisotropic_Level_monitor", "r_Texture_Anisotropic_Level_value", ui->anisotropicCheck, ui->anisotropicCombo));

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ShugoConsole");

    foreach(VariableEditorBase* v, _editors)
    {
        connect(v, &VariableEditorBase::varEnable, this, &ShugoConsoleWidget::varEnable);
        connect(v, &VariableEditorBase::varDisable, this, &ShugoConsoleWidget::varDisable);
        v->loadSettings(settings);
        connect(v, &VariableEditorBase::settingsUpdated, this, &ShugoConsoleWidget::saveSettings);
        v->connectInterface();
    }

    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &ShugoConsoleWidget::onScanTimer);
    _timer->setInterval(1000);
    _timer->start();
}

ShugoConsoleWidget::~ShugoConsoleWidget()
{
    delete ui;
}

void ShugoConsoleWidget::saveSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ShugoConsole");

    foreach(VariableEditorBase* v, _editors)
    {
        v->saveSettings(settings);
    }
}

void ShugoConsoleWidget::varEnable(QString key, QVariant value)
{
    _consoleVars[key] = value;
    updateProcessesVars();
}

void ShugoConsoleWidget::varDisable(QString key)
{
    _consoleVars.remove(key);
    updateProcessesVars();
}

void ShugoConsoleWidget::updateProcessesVars()
{
    QList<AionProcessWorker*> worker_list = _workersHash.values();
    foreach(AionProcessWorker* worker, worker_list)
    {
        worker->updateVars(_consoleVars);
    }
}

void ShugoConsoleWidget::onStateUpdate(AionProcessWorker* worker, AionProcessWorker::State state)
{
    HANDLE pid = NULL;

    if(worker)
    {
        pid = worker->getPid();
    }

    _processListModel->setProcessState(pid, state);
}

void ShugoConsoleWidget::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick)
        show();
}

void ShugoConsoleWidget::onScanTimer()
{

    QList<HANDLE> sys_aion_process_list = findProcessHandleByImageName("aion.bin");
    QList<HANDLE> current_process_list = _workersHash.keys();

    foreach(HANDLE h, current_process_list)
    {
        if(!sys_aion_process_list.contains(h))
        {           
            QThread *thread = _workersHash.value(h)->thread();
            if(thread)
            {
                if(thread->isFinished())
                {
                    thread->deleteLater();
                }
                else
                {
                    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
                    thread->quit();
                }
            }
            _workersHash.value(h)->deleteLater();
            _workersHash.remove(h);
            _processListModel->removeProcess(h);
        }
    }

    foreach(HANDLE h, sys_aion_process_list)
    {
        if(!current_process_list.contains(h))
        {
            QThread* thread = new QThread(this);
            AionProcessWorker* worker = new AionProcessWorker(NULL, h);
            worker->moveToThread(thread);
            worker->updateVars(_consoleVars);
            _processListModel->addProcess(h);
            connect(worker,&AionProcessWorker::stateUpdate, this, &ShugoConsoleWidget::onStateUpdate, Qt::QueuedConnection);
            thread->start();
            QMetaObject::invokeMethod(worker, "init", Qt::QueuedConnection);
            _workersHash.insert(h, worker);
        }
    }
}
