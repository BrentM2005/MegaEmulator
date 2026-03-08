#include <QApplication>
#include <QDesktopServices>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    app.setApplicationName("Mega Emulator");
    app.setOrganizationName("Brent Inc");

    app.setWindowIcon(QIcon(":/icons/MegaEmulator.ico"));

    MainWindow window;
    window.show();

    return app.exec();
}
