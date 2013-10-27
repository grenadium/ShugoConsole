#include "shugoconsolewidget.h"
#include <QApplication>
#include <QSharedMemory>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setStyle("Fusion");

    /* Single application checking */

    QSharedMemory sharedMemory("ShugoConsoleIsBestConsole");
    if(!sharedMemory.create(1) || sharedMemory.error() == QSharedMemory::AlreadyExists)
    {
        QMessageBox::information(NULL, "ShugoConsole", "An instance of ShugoConsole is already running");
        return 1;
    }

    ShugoConsoleWidget w;
    w.hide();
    
    return app.exec();
}
