#pragma once
#include <QWidget>
#include <QFrame>
#include <list>
#include <string>

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

    static constexpr double ZeroCelsius = 273.15;

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

class Termometr : public QFrame
{
    Q_OBJECT
    double T;

public:
    Termometr(QWidget *parent = nullptr, double temperature = 273.15);
public slots:
    void setT(const QString &);
signals:
    void valueChanged(double);
};

class Table : public QWidget
{
    Q_OBJECT
    class Column : public QFrame
    {
    public:
        Column(QWidget *parent = nullptr,
               const QString& temperature = "",
               const QString& current = "",
               const QString& voltage = "");
        std::string getT();
        std::string getI();
        std::string getU();
    };

    std::list<Column*> data;
    double T, I, U;
    bool recordT, recordI, recordU;
    static QString toString(double);

public:
    Table(QWidget *parent = nullptr);
    ~Table();
public slots:
    void addColumn();
    void removeColumn(unsigned int column_number);
    void setT(double);
    void setI(double);
    void setU(double);
    void setRecordStateT(bool state);
    void setRecordStateI(bool state);
    void setRecordStateU(bool state);
    void saveIntoFile();
};
