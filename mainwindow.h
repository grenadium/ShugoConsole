#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QSystemTrayIcon>

#include <QTimer>
#include <QSet>
#include <QThread>

#include "ntdll.h"

class MainWindow : public QWidget
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWindow();

protected:
	virtual void closeEvent (QCloseEvent *);
	
private slots:
	void enableToggled(bool);
	void scantimer();

private:
	bool workerEnabled;

	QTextEdit* textEdit;

	QAction *enableAction;
	QAction *showAction;
	QAction *exitAction;

	QSystemTrayIcon* trayIcon;

	QTimer *timer;

	SystemInformationQuery siquery;

	QSet<HANDLE> processes_done;
};

#endif // MAINWINDOW_H
