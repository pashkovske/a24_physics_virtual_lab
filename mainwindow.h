#pragma once
#include <QWidget>

class MainWindow : public QWidget
{
    Q_OBJECT
    static double
        IMainMinDouble,
        IMainMaxDouble,
        IFuranceMinDouble,
        IFuranceMaxDouble;
    static int
        IMainMinInt,
        IMainMaxInt,
        IFuranceMinInt,
        IFuranceMaxInt;
public:
    MainWindow(QWidget *parent = nullptr);

    static void setIMainMinDouble(double);
    static void setIMainMaxDouble(double);
    static void setIFuranceMinDouble(double);
    static void setIFuranceMaxDouble(double);
    static void setIMainMinInt(int);
    static void setIMainMaxInt(int);
    static void setIFuranceMinInt(int);
    static void setIFuranceMaxInt(int);

    static double furanceIntToDouble(int value);
    static double mainIntToDouble(int value);
    static int furanceDoubleToInt(double value);
    static int mainDoubleToInt(double value);
    static const int
        Diamond = 0,
        Ge = 1,
        Si = 2,
        Se = 3,
        Te = 4,
        PbS = 5,
        InSb = 6,
        GaAs = 7;
};

