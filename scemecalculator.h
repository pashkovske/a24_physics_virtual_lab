#pragma once
#include <cmath>

// Единицы измерения: см, Ом, В, А, эВ, К

class Constants
{
    double value[8][2] = {{6, 1e10},
                          {0.71, 43},
                          {1.1, 2.6e5},
                          {1.7, 3.1e8},
                          {0.36, 0.1},
                          {0.36, 0.2},
                          {0.18, 0.007},
                          {1.4, 150}};
    static constexpr double
            k = 1.38e-23,
            e = 1.6e-19,
            T_0 = 293;

public:
    Constants();
    double getConstant(int material_type, int constant_type);
};

class SemiconductorCalc
{
    int type;
    double S, L, T, I, U;
    Constants* consts;
    static const int
        C = 0,
        Betta = 1;

public:
    SemiconductorCalc(double tempireture = 293,
                  double current = 0,
                  int semicondacter_type = 1,
                  double square = 0.01,
                  double length = 1);
    ~SemiconductorCalc();
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

class FuranceCalc
{
    double Troom, T, I;
    static constexpr double alpha = 10.0;

public:
    FuranceCalc(double room_tempirature,
            double furance_tempirature,
            double current = 0);
    FuranceCalc(double room_tempirature = 293);
    void setTroom(double);
    void setT(double);
    void setI(double);
    void refreshT();
    double getT();
    double getTroom();
    double getI();
};

class ScemeCalc
{
    double Isource;
    double V, A;
    SemiconductorCalc* R;
    FuranceCalc* furance;

public:
    ScemeCalc(double source_current = 0);
    ~ScemeCalc();
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
