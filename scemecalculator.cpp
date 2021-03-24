#include "scemecalculator.h"

ScemeCalculator::ScemeCalculator(double Troom_, double Isource_)
    : Troom(Troom_), Isource(Isource_)
{
    R = new Semiconductor(Troom, Isource);
    furance = new Furance(Troom);
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
        Troom = val;
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
        return Troom;
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
        V = R->getU();
        break;
    }
}

Semiconductor::Semiconductor(double T_, double I_, int type_, double S_, double L_)
    :type(type_), S(S_), L(L_), T(T_), I(I_)
{
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
    double C = constants(type, 'C'), betta = constants(type, 'b');
    U = L/C/S * exp(betta/T);
}

Furance::Furance(double T_, double I_)
    : T(T_), I(I_)
{}
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
    T = alpha * I * I;
}
double Furance::getI()
{
    return I;
}
double Furance::getT()
{
    return T;
}
