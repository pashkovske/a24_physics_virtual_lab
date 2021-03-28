#include "mainwindow.h"
#include "scemecalculator.h"
#include <iostream>
#include <fstream>

#include <QtWidgets>

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    int
            w = Element::WireC,
            x = Element::XC,
            M = Element::MultimetrC,
            I = Element::SourceC,
            R = Element::ResistorC,
            D = Element::SemiconductorC,
            V = Element::VoltmetrC,
            A = Element::AmpermetrC;
    int a = M | A,
            v = M | V,
            r = Element::Right | x,
            l = Element::Left | x,
            b = Element::Bottom | x;
    int layoutTable[10][12] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, a, x, r, 0, 0, 0, 0},
        {0, 0, w, w, w, x, x, x, w, w, w, 0},
        {0, 0, w, 0, 0, l, x, b, 0, w, 0, 0},
        {0, I, x, r, 0, 0, 0, 0, v, x, r, 0},
        {0, x, x, x, 0, 0, 0, 0, x, x, x, 0},
        {0, l, x, b, 0, 0, 0, 0, l, x, b, 0},
        {0, 0, w, 0, 0, 0, 0, 0, 0, w, 0, 0},
        {0, 0, w, w, w, w, w, w, w, w, w, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
    int** table = new int*[10];
    int j;
    for(int i = 0; i < 10; i++)
    {
        table[i] = new int[12];
        for(j = 0; j < 12; j++)
            table[i][j] = layoutTable[i][j];
    }
    Element desk(10, 8, 0, nullptr, table);

    desk.resize(800, 600);
    desk.show();

    for(int i = 0; i < 10; i++)
        delete[] table[i];
    delete[] table;
    return app.exec();
}
