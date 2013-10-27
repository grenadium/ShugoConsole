#include "shugoconsolewidget.h"
#include "ui_shugoconsolewidget.h"

#include <QMenu>
#include <QList>
#include "processutils.h"

#include <QSettings>

ShugoConsoleWidget::ShugoConsoleWidget(QWidget *parent) :
    QWidget(parent, Qt::Tool | Qt::WindowStaysOnTopHint),
    ui(new Ui::ShugoConsoleWidget)
{
    ui->setupUi(this);

    loadConfig();

    showAction = new QAction(QString("Show"),this);
    connect(showAction,SIGNAL(triggered()),this,SLOT(show()));
    exitAction = new QAction(QString("Exit"),this);
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    QMenu *trayMenu = new QMenu(this);
    trayMenu->addAction(showAction);
    trayMenu->addSeparator();
    trayMenu->addAction(exitAction);

    trayIcon = new QSystemTrayIcon(QIcon(":/ShugoConsole/shugo.png"),this);
    trayIcon->setContextMenu(trayMenu);
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onTrayIconActivated(QSystemTrayIcon::ActivationReason)));

    trayIcon->show();

    processListModel = new ProcessListModel(this);
    ui->processesView->setModel(processListModel);

    connect(ui->consoleCheck, &QCheckBox::stateChanged, this, &ShugoConsoleWidget::onConsoleCheckStateChanged);
    connect(ui->fovCheck, &QCheckBox::stateChanged, this, &ShugoConsoleWidget::onFovCheckStateChanged);
    connect(ui->fovSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onFovSpinBoxValueChanged(double)));

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ShugoConsoleWidget::onScanTimer);
    timer->setInterval(1000);
    timer->start();
}

ShugoConsoleWidget::~ShugoConsoleWidget()
{
    delete ui;
}

void ShugoConsoleWidget::loadConfig()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ShugoConsole");

    QVariant vEnableConsole = settings.value("EnableConsole", QVariant(false));
    if(vEnableConsole.toBool())
    {
        ui->consoleCheck->setChecked(true);
        setVarValue("con_disable_console", 0);
    }

    QVariant vFOV = settings.value("FOV", "90.0");
    double FOV = vFOV.toDouble();
    if(FOV < 60.0 || FOV > 170.0)
        FOV = 90.0;

    ui->fovSpinBox->setValue(FOV);

    QVariant vEnableFOV = settings.value("EnableFOV", QVariant(false));
    if(vEnableFOV.toBool())
    {
        ui->fovCheck->setChecked(true);
        setVarValue("g_minFov", FOV);
    }
}

void ShugoConsoleWidget::saveConfig()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "ShugoConsole");

    settings.setValue("EnableConsole", ui->consoleCheck->isChecked());
    settings.setValue("EnableFOV", ui->fovCheck->isChecked());
    settings.setValue("FOV", ui->fovSpinBox->value());
}

void ShugoConsoleWidget::setVarValue(const QString & key, const QVariant & value)
{
    consoleVars[key] = value;
    updateProcessesVars();
}

void ShugoConsoleWidget::removeVar(const QString & key)
{
    consoleVars.remove(key);
    updateProcessesVars();
}

void ShugoConsoleWidget::updateProcessesVars()
{
    QList<AionProcessWorker*> worker_list = workersHash.values();
    foreach(AionProcessWorker* worker, worker_list)
    {
        worker->updateVars(consoleVars);
    }
}

void ShugoConsoleWidget::onStateUpdate(AionProcessWorker* worker, AionProcessWorker::State state)
{
    HANDLE pid = NULL;

    if(worker)
    {
        pid = worker->getPid();
    }

    processListModel->setProcessState(pid, state);
}

void ShugoConsoleWidget::onFovSpinBoxValueChanged(double value)
{
    if(ui->fovCheck->isChecked())
    {
        setVarValue("g_minFov", value);
    }

    saveConfig();
}

void ShugoConsoleWidget::onFovCheckStateChanged(int checked)
{
    if(checked)
    {
        setVarValue("g_minFov", ui->fovSpinBox->value());
    }
    else
    {
        removeVar("g_minFov");
    }

    saveConfig();
}

void ShugoConsoleWidget::onConsoleCheckStateChanged(int checked)
{
    if(checked)
    {
        setVarValue("con_disable_console", 0);
    }
    else
    {
        removeVar("con_disable_console");
    }

    saveConfig();
}

void ShugoConsoleWidget::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick)
        show();
}

void ShugoConsoleWidget::onScanTimer()
{

    QList<HANDLE> sys_aion_process_list = findProcessHandleByImageName("aion.bin");
    QList<HANDLE> current_process_list = workersHash.keys();

    foreach(HANDLE h, current_process_list)
    {
        if(!sys_aion_process_list.contains(h))
        {           
            QThread *thread = workersHash.value(h)->thread();
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
            workersHash.value(h)->deleteLater();
            workersHash.remove(h);
            processListModel->removeProcess(h);
        }
    }

    foreach(HANDLE h, sys_aion_process_list)
    {
        if(!current_process_list.contains(h))
        {
            QThread* thread = new QThread(this);
            AionProcessWorker* worker = new AionProcessWorker(NULL, h);
            worker->moveToThread(thread);
            worker->updateVars(consoleVars);
            processListModel->addProcess(h);
            connect(worker,&AionProcessWorker::stateUpdate, this, &ShugoConsoleWidget::onStateUpdate, Qt::QueuedConnection);
            thread->start();
            QMetaObject::invokeMethod(worker, "init", Qt::QueuedConnection);
            workersHash.insert(h, worker);
        }
    }
}
