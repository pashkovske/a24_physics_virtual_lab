#pragma once
#include <cmath>
#include <QObject>

// Единицы измерения: см, Ом, В, А, эВ, К

class Properties
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
    Properties();
    double getProperty(int material_type, int property_type);
};

class SemiconductorCalc
{
    int type;
    double S, L, T, I, U;
    Properties* consts;
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

class ScemeCalc : public QObject
{
    Q_OBJECT

    double Isource;
    double V, A;
    SemiconductorCalc* R;
    FuranceCalc* furance;

public:
    ScemeCalc(QObject* parent = nullptr, double source_current = 0);
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
    int getSemiconductorType();
public slots:
    void setSemiconductorType(int);
    void setFuranceCurrent(int);
    void setMainSourceCurrent(int);
signals:
    void termometrStatusChanged(double);
    void voltmetrStatusChanged(double);
    void ampermetrStatusChanged(double);
};
