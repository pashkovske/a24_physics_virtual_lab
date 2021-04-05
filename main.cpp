#include "mainwindow.h"

#include <QtWidgets>
#include <QScrollArea>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow *w = new MainWindow;
    QScrollArea sa;
    sa.setWidgetResizable(true);
    sa.setWidget(w);
    sa.resize(1200, 600);
    sa.show();

    return app.exec();
}
