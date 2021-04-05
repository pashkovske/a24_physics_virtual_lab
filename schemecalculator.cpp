#include "schemecalculator.h"
#include "mainwindow.h"

Properties::Properties()
{
    double betta;
    for (int i = 0; i < 8; i++)
    {
        betta = value[i][0] /2/k *e;
        value[i][0] = exp(betta / T_0) / value[i][1];
        value[i][1] = betta;
    }
}
double Properties::getProperty(int i, int j)
{
    return value[i][j];
}

SchemeCalc::SchemeCalc(QObject* parent, double Isource_)
    : QObject(parent), Isource(Isource_)
{
    R = new SemiconductorCalc;
    furance = new FuranceCalc;
    refreshValue(Voltmetr);
    refreshValue(Ampermetr);
}
SchemeCalc::~SchemeCalc()
{
    delete R;
    delete furance;
}
void SchemeCalc::setValue(int valtype, double val)
{
    switch (valtype) {
    case Room_tempirature:
        furance->setTroom(val);
        break;
    case Furance_current:
        furance->setI(val);
        break;
    case Source_current:
        Isource = val;
        break;
    case Furance_tempirature:
        furance->setT(val);
        R->setT(val);
        break;
    case Semiconductor_length:
        R->setL(val);
        break;
    case Semiconductor_square:
        R->setS(val);
        break;
    case Semiconductor_current:
        R->setI(val);
        break;
    }
}
double SchemeCalc::getValue(int valtype)
{
    switch (valtype) {
    case Room_tempirature:
        return furance->getTroom();
    case Source_current:
        return Isource;
    case Furance_tempirature:
        return furance->getT();
    case Voltmetr:
        return V;
    case Ampermetr:
        return A;
    case Furance_current:
        return furance->getI();
    case Semiconductor_current:
        return R->getI();
    case Semiconductor_length:
        return R->getL();
    case Semiconductor_square:
        return R->getS();
    case Semiconductor_voltage:
        return R->getU();
    default:
        return 0;
    }
}
void SchemeCalc::setSemiconductorType(int type_)
{
    R->setType(type_);
}
int SchemeCalc::getSemiconductorType()
{
    return R->getType();
}
void SchemeCalc::refreshValue(int valtype)
{
    switch (valtype) {
    case Furance_tempirature:
        furance->refreshT();
        R->setT(furance->getT());
        break;
    case Ampermetr:
        A = Isource;
        R->setI(A);
        break;
    case Voltmetr:
        R->refreshU();
        V = R->getU();
        break;
    }
}
void SchemeCalc::refreshAll()
{
    refreshValue(Furance_tempirature);
    emit termometrStatusChanged(getValue(Furance_tempirature));
    refreshValue(Ampermetr);
    emit ampermetrStatusChanged(getValue(Ampermetr));
    refreshValue(Voltmetr);
    emit voltmetrStatusChanged(getValue(Voltmetr));
}
void SchemeCalc::setFuranceCurrent(int val)
{
    setValue(Furance_current, MainWindow::furanceIntToDouble(val));
    refreshAll();
}
void SchemeCalc::setMainSourceCurrent(int val)
{
    setValue(Source_current, MainWindow::mainIntToDouble(val));
    refreshAll();
}
void SchemeCalc::setRoomTemperature(double val)
{
    setValue(Room_tempirature, val);
    refreshAll();
}
void SchemeCalc::setSemiconductorLength(double val)
{
    setValue(Semiconductor_length, val);
    refreshAll();
}
void SchemeCalc::setSemiconductorSquare(double val)
{
    setValue(Semiconductor_square, val);
    refreshAll();
}

SemiconductorCalc::SemiconductorCalc(double T_, double I_, int type_, double S_, double L_)
    :type(type_), S(S_), L(L_), T(T_), I(I_)
{
    consts = new Properties;
}
SemiconductorCalc::~SemiconductorCalc()
{
    delete consts;
}
void SemiconductorCalc::setType(int type_)
{
    type = type_;
}
void SemiconductorCalc::setT(double val)
{
    T = val;
}
void SemiconductorCalc::setI(double val)
{
    I = val;
}
void SemiconductorCalc::setS(double val)
{
    S = val;
}
void SemiconductorCalc::setL(double val)
{
    L = val;
}
int SemiconductorCalc::getType()
{
    return type;
}
double SemiconductorCalc::getI()
{
    return I;
}
double SemiconductorCalc::getS()
{
    return S;
}
double SemiconductorCalc::getL()
{
    return L;
}
double SemiconductorCalc::getU()
{
    return U;
}
void SemiconductorCalc::refreshU()
{
    double C_0 = consts->getProperty(type, C),
            betta = consts->getProperty(type, Betta);
    U = L/C_0/S * exp(betta/T)*I;
}

FuranceCalc::FuranceCalc(double Troom_, double T_, double I_)
    : Troom(Troom_), T(T_), I(I_)
{}
FuranceCalc::FuranceCalc(double Troom_)
    :Troom(Troom_)
{
    T = Troom;
    I = 0;
}
void FuranceCalc::setTroom(double val)
{
    Troom = val;
}
void FuranceCalc::setI(double val)
{
    I = val;
}
void FuranceCalc::setT(double val)
{
    T = val;
}
void FuranceCalc::refreshT()
{
    T = Troom + alpha * I * I;
}
double FuranceCalc::getI()
{
    return I;
}
double FuranceCalc::getT()
{
    return T;
}
double FuranceCalc::getTroom()
{
    return Troom;
}
