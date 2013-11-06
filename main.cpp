#include "shugoconsolewidget.h"
#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setStyle("Fusion");

    QTranslator translator;
    translator.load("ShugoConsole." + QLocale::system().name());
    app.installTranslator(&translator);

    /* Single application checking */

    QSharedMemory sharedMemory("ShugoConsoleIsBestConsole");
    if(!sharedMemory.create(1) || sharedMemory.error() == QSharedMemory::AlreadyExists)
    {
        QMessageBox::information(NULL, app.tr("ShugoConsole"), app.tr("An instance of ShugoConsole is already running"));
        return 1;
    }

    ShugoConsoleWidget w;
    w.hide();
    
    return app.exec();
}
