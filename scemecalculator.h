#pragma once

#include <cmath>
// Единицы измерения: см, Ом, В, А, эВ, К

class Semiconductor
{
    int type;
    double S, L, T, I, U;
    static constexpr double properities[8][2] = {{6, 1e10},
                                                {0.71, 43},
                                                {1.1, 2.6e5},
                                                {1.7, 3.1e8},
                                                {0.36, 0.1},
                                                {0.36, 0.2},
                                                {0.18, 0.007},
                                                {1.4, 150}};
    static constexpr double constants(int i, char j)
    {
        const double
                k = 1.38e-23,
                e = 1.6e-19,
                T_0 = 293;
        switch (j) {
        case 'C':
            return exp(properities[i][0]/2/k/T_0*e)/properities[i][1];
            break;
        default:
            return properities[i][0]/2/k*e;
        }
    }
public:
    Semiconductor(double tempireture = 293,
                  double current = 0,
                  int semicondacter_type = 0,
                  double square = 0.01,
                  double length = 1);
    void setType(int);
    static const int
        Diamond = 0,
        Ge = 1,
        Si = 2,
        Se = 3,
        Te = 4,
        PbS = 5,
        InSb = 6,
        GaAs = 7;
    void setS(double);
    void setL(double);
    void setT(double);
    void setI(double);
    void refreshU();
    int getType();
    double getU();
    double getS();
    double getL();
    double getI();
};

class Furance
{
    double T, I;
    static constexpr double alpha = 10.0;

public:
    Furance(double tempirature = 293, double current = 0);
    void setT(double);
    void setI(double);
    void refreshT();
    double getT();
    double getI();
};

class ScemeCalculator
{
    double Troom,
        Isource;
    double V, A;
    Semiconductor* R;
    Furance* furance;

public:
    ScemeCalculator(double room_tempirature = 293, double source_current = 0);
    ~ScemeCalculator();
    void setValue(int value_type, double);
    double getValue(int value_type);
    void refreshValue(int value_type);
    static const int
        Room_tempirature = 1,
        Furance_current = 2,
        Source_current = 3,
        Furance_tempirature = 4,
        Voltmetr = 5,
        Ampermetr = 6,
        Semiconductor_length = 7,
        Semiconductor_square = 8,
        Semiconductor_current = 9,
        Semiconductor_voltage = 10;
    void setSemiconductorType(int);
    int getSemiconductorType();
};
