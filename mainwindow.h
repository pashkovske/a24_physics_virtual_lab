#pragma once
#include <QWidget>
#include <QMainWindow>
#include <QFrame>
#include <list>
#include <string>
#include <QLineEdit>
#include <QDialog>
#include <QLabel>


class MainWindow : public QWidget
{
    Q_OBJECT
    static double
        IMainMinDouble,
        IMainMaxDouble,
        IFuranceMinDouble,
        IFuranceMaxDouble,
        FuranceMaxDeltaT;
    static int
        IMainMinInt,
        IMainMaxInt,
        IFuranceMinInt,
        IFuranceMaxInt;
public:
    MainWindow(QWidget *parent = nullptr);

    static constexpr double ZeroCelsius = 273.15;
    static constexpr double ZeroValue = 10e-20;

    static void setIMainMinDouble(double);
    static void setIMainMaxDouble(double);
    static void setIFuranceMinDouble(double);
    static void setIFuranceMaxDouble(double);
    static void setIMainMinInt(int);
    static void setIMainMaxInt(int);
    static void setIFuranceMinInt(int);
    static void setIFuranceMaxInt(int);
    static void setFuranceMaxDeltaT(double);
    static double getFuranceMaxDeltaT();

    static double IfuranceIntToDouble(int value);
    static double TfuranceIntToDouble(int value);
    static double mainIntToDouble(int value);
    static int IfuranceDoubleToInt(double value);
    static int TfuranceDoubleToInt(double value);
    static int mainDoubleToInt(double value);

    static double round(double value, int digits);
    static double roundSignificantDigits(double value, int digits);

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

class Table : public QWidget
{
    Q_OBJECT

    class Column : public QFrame
    {
    public:
        Column(QWidget *parent = nullptr,
               const QString& temperature = "",
               const QString& current = "",
               const QString& voltage = "",
               const QString& resistance = "");
        Column(QWidget *parent,
               double temperature,
               double current,
               double voltage,
               double resistance);
        QString getT();
        QString getI();
        QString getU();
        QString getR();
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
    void drawGraph();
};

class Props : public QFrame
{
public:
    Props(QWidget *parent = nullptr);
};

class SemiconductorProps : public Props
{
    Q_OBJECT
    double L, S;
    QLineEdit *line_edit_L;
    QLineEdit *line_edit_S;

public:
    SemiconductorProps(QWidget *parent = nullptr,
                       double length = 1,
                       double square = 1);
public slots:
    void setType(int);
    void setLength();
    void setSquare();
signals:
    void typeChanged(int);
    void lengthChanged(double);
    void squareChanged(double);
};

class Termometr : public Props
{
    Q_OBJECT
    double T;
    QLineEdit *line_edit;

public:
    Termometr(QWidget *parent = nullptr,
              double temperature = MainWindow::ZeroCelsius);
public slots:
    void setT();
signals:
    void valueChanged(double);
};

class Graph : public QDialog
{
    Q_OBJECT

public:
    Graph(std::list<std::pair<double, double>>& data,
          const QString& title_x = "x",
          const QString& title_y = "y",
          QDialog *parent = nullptr);
};
