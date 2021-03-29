#include "mainwindow.h"
#include "scemecalculator.h"
#include "element.h"
#include <QHBoxLayout>

int MainWindow::IFuranceMinInt = 0;
int MainWindow::IFuranceMaxInt = 100;
int MainWindow::IMainMinInt = 0;
int MainWindow::IMainMaxInt = 100;
double MainWindow::IFuranceMinDouble = 0;
double MainWindow::IFuranceMaxDouble= 5;
double MainWindow::IMainMinDouble = 0;
double MainWindow::IMainMaxDouble = 1;

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    Element* desk = new Element(0, this, "/home/geexander/sceme.txt");
    layout->addWidget(desk);
    ScemeCalc* scemecalc = new ScemeCalc(this);
    Multimetr* tmpM;
    Source* tmpS;
    Semiconductor* tmpSC;

    tmpS = desk->findChild<Source*>("source0");
    tmpS->setMin(IMainMinInt);
    tmpS->setMax(IMainMaxInt);
    QObject::connect(tmpS, SIGNAL(valueChanged(int)), scemecalc, SLOT(setMainSourceCurrent(int)));

    tmpS = desk->findChild<Source*>("source1");
    tmpS->setMin(IFuranceMinInt);
    tmpS->setMax(IFuranceMaxInt);
    QObject::connect(tmpS, SIGNAL(valueChanged(int)), scemecalc, SLOT(setFuranceCurrent(int)));

    tmpM = desk->findChild<Multimetr*>("voltmetr0");
    QObject::connect(scemecalc, SIGNAL(voltmetrStatusChanged(double)), tmpM, SLOT(setValue(double)));

    tmpM = desk->findChild<Multimetr*>("ampermetr0");
    QObject::connect(scemecalc, SIGNAL(ampermetrStatusChanged(double)), tmpM, SLOT(setValue(double)));

    tmpM = desk->findChild<Multimetr*>("termometr0");
    QObject::connect(scemecalc, SIGNAL(termometrStatusChanged(double)), tmpM, SLOT(setValue(double)));

    tmpSC = desk->findChild<Semiconductor*>("semiconductor0");
    QObject::connect(tmpSC, SIGNAL(typeChanged(int)), scemecalc, SLOT(setSemiconductorType(int)));
}
void MainWindow:: setIFuranceMinInt(int val)
{
    IFuranceMinInt = val;
}
void MainWindow:: setIFuranceMaxInt(int val)
{
    IFuranceMaxInt = val;
}
void MainWindow:: setIMainMinInt(int val)
{
    IMainMinInt = val;
}
void MainWindow:: setIMainMaxInt(int val)
{
    IMainMaxInt = val;
}
void MainWindow:: setIFuranceMinDouble(double val)
{
    IFuranceMinDouble = val;
}
void MainWindow:: setIFuranceMaxDouble(double val)
{
    IFuranceMaxDouble = val;
}
void MainWindow:: setIMainMinDouble(double val)
{
    IMainMinDouble = val;
}
void MainWindow:: setIMainMaxDouble(double val)
{
    IMainMaxDouble = val;
}

double MainWindow::furanceIntToDouble(int value)
{
    return value * (IFuranceMaxDouble - IFuranceMinDouble)/(IFuranceMaxInt - IFuranceMinInt);
}
double MainWindow::mainIntToDouble(int value)
{
    return value * (IMainMaxDouble - IMainMinDouble)/(IMainMaxInt - IMainMinInt);
}
int MainWindow::furanceDoubleToInt(double value)
{
    return value * (IFuranceMaxInt - IFuranceMinInt)/(IFuranceMaxDouble - IFuranceMinDouble);
}
int MainWindow::mainDoubleToInt(double value)
{
    return value * (IMainMaxInt - IMainMinInt)/(IMainMaxDouble - IMainMinDouble);
}
