#include "ntdll.h"
#include "mainwindow.h"
#include "qtsingleapplication.h"
#include <QtGui/QMessageBox>

int main(int argc, char *argv[])
{
	if(!NtDll::load())
	{
		QMessageBox::critical(NULL,"Error","Could not load ntdll.dll");
		return 1;
	}

	QtSingleApplication a(argc, argv);

	if(a.isRunning())
	{
		QMessageBox::information(NULL,"ShugoConsole","An instance of ShugoToolbox is already running");
		return 0;
	}

	MainWindow w;
	return a.exec();
}
