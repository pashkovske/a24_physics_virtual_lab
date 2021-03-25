#include "scemecalculator.h"

Constants::Constants()
{
    double betta;
    for (int i = 0; i < 8; i++)
    {
        betta = value[i][0] /2/k *e;
        value[i][0] = exp(betta / T_0) / value[i][1];
        value[i][1] = betta;
    }
}
double Constants::getConstant(int i, int j)
{
    return value[i][j];
}

ScemeCalculator::ScemeCalculator(double Isource_)
    : Isource(Isource_)
{
    R = new Semiconductor;
    furance = new Furance;
    refreshValue(Voltmetr);
    refreshValue(Ampermetr);
}
ScemeCalculator::~ScemeCalculator()
{
    delete R;
    delete furance;
}
void ScemeCalculator::setValue(int valtype, double val)
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
double ScemeCalculator::getValue(int valtype)
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
void ScemeCalculator::setSemiconductorType(int type_)
{
    R->setType(type_);
}
int ScemeCalculator::getSemiconductorType()
{
    return R->getType();
}
void ScemeCalculator::refreshValue(int valtype)
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

Semiconductor::Semiconductor(double T_, double I_, int type_, double S_, double L_)
    :type(type_), S(S_), L(L_), T(T_), I(I_)
{
    consts = new Constants;
}
Semiconductor::~Semiconductor()
{
    delete consts;
}
void Semiconductor::setType(int type_)
{
    type = type_;
}
void Semiconductor::setT(double val)
{
    T = val;
}
void Semiconductor::setI(double val)
{
    I = val;
}
void Semiconductor::setS(double val)
{
    S = val;
}
void Semiconductor::setL(double val)
{
    L = val;
}
int Semiconductor::getType()
{
    return type;
}
double Semiconductor::getI()
{
    return I;
}
double Semiconductor::getS()
{
    return S;
}
double Semiconductor::getL()
{
    return L;
}
double Semiconductor::getU()
{
    return U;
}
void Semiconductor::refreshU()
{
    double C_0 = consts->getConstant(type, C),
            betta = consts->getConstant(type, Betta);
    U = L/C_0/S * exp(betta/T)*I;
}

Furance::Furance(double Troom_, double T_, double I_)
    : Troom(Troom_), T(T_), I(I_)
{}
Furance::Furance(double Troom_)
    :Troom(Troom_)
{
    T = Troom;
    I = 0;
}
void Furance::setTroom(double val)
{
    Troom = val;
}
void Furance::setI(double val)
{
    I = val;
}
void Furance::setT(double val)
{
    T = val;
}
void Furance::refreshT()
{
    T = Troom + alpha * I * I;
}
double Furance::getI()
{
    return I;
}
double Furance::getT()
{
    return T;
}
double Furance::getTroom()
{
    return Troom;
}
