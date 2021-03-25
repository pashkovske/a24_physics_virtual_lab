#include "mainwindow.h"
#include "scemecalculator.h"
#include <iostream>
#include <fstream>

#include <QtWidgets>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Element w;

    Wire* w1 = new Wire(Wire::Bottom | Wire::Right, &w);
    Wire* w2 = new Wire(Wire::Left | Wire::Right, &w);
    Wire* w3 = new Wire(Wire::Left | Wire::Right, &w);
    Wire* w4 = new Wire(Wire::Left | Wire::Right, &w);
    Wire* w5 = new Wire(Wire::Bottom | Wire::Left | Wire::Right, &w);
    Wire* w6 = new Wire(Wire::Bottom | Wire::Top, &w);
    Multimetr* volt = new Multimetr(Multimetr::Voltmetr, Multimetr::Horizontal, &w);
    Multimetr* amp = new Multimetr(Multimetr::Ampermetr, Multimetr::Vertical, &w);

    QGridLayout* sceme = new QGridLayout(&w);
    sceme->setVerticalSpacing(0);
    sceme->setHorizontalSpacing(0);
    sceme->addWidget(volt, 0, 4, 3, 3);
    sceme->addWidget(amp, 3, 7, 3, 3);
    sceme->addWidget(w1, 1, 1);
    sceme->addWidget(w2, 1, 2);
    sceme->addWidget(w3, 1, 3);
    sceme->addWidget(w4, 1, 7);
    sceme->addWidget(w5, 1, 8);
    sceme->addWidget(w6, 2, 8);

    w.setLayout(sceme);
    w.resize(800, 600);
    w.show();
    return a.exec();
}
