#include "mainwindow.h"

#include <QtWidgets>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;

    //desk.findChild("voltmetr0");
    w.resize(800, 600);
    w.show();

    return app.exec();
}
