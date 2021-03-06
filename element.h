#pragma once
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPen>
#include <QSlider>
#include <QLCDNumber>
#include "mainwindow.h"

class Element : public QWidget
{
    int** table;
    template<typename T>
    void setBorderPropereties(T* element, int** table, int positionX, int positionY);
    int parse(char);
protected:
    unsigned int sizeX, sizeY;
    unsigned int* contacts;
    unsigned int contactsNum;
    QPen pen;
    static int
        OuterMargins,
        InnerMargins;

    void drawContacts(QPainter& painter);

public:
    Element(unsigned int number_of_contacts = 0,
            QWidget *parent = nullptr,
            const char* file_name = nullptr,
            unsigned int horizontal_size = 1,
            unsigned int vertical_size = 1);
    Element(unsigned int number_of_contacts = 0,
            Element *parent = nullptr,
            const char* file_name = nullptr,
            unsigned int horizontal_size = 1,
            unsigned int vertical_size = 1);
    ~Element();
    int getSizeX();
    int getSizeY();
    const QPen& getPen();
    void setSizeX(int);
    void setSizeY(int);
    void setPen(const QPen&);
    static const int
        Right =     0b1000000,
        Top =       0b10000000,
        Left =      0b100000000,
        Bottom =    0b1000000000,
        DirectionMask = Right | Top | Left | Bottom,

        ElementMask =           0b1111,
        ElementTypeMask =       0b110000,
        NumberMask =            0b1000000000000,
        WireC =                 0b0001,
        XC =                    0b10000000000,
        MultimetrC =            0b0011,
        SourceC =               0b0100,
        MainSourceC =           0b010000,
        FuranceSourceC =        0b100000,
        ResistorC =             0b0101,
        SemiconductorC =        0b0110,
        SubSceme =              0b100000000000,
        AmpermetrC =            0b110000,
        VoltmetrC =             0b010000,
        TermometrC =            0b100000;
    void setContact(unsigned contact_number, unsigned raw_position); // ?????????????? ???? 0 ???? ?????????????? ??????????????
    void setName(QString name);
    void genereteLayout();
};

class CustomGridLayout : public QGridLayout
{
public:
    CustomGridLayout(QWidget* parent = nullptr);
    void addElement(Element* element,
                    int fromRow,
                    int fromColumn,
                    Qt::Alignment alignment = Qt::Alignment());
};

class Wire : public Element
{
    int type;
    static const int node_radius = 7;

    void paintEvent(QPaintEvent*);

public:
    Wire(int connection_type, Element *parent = nullptr);
    static const int Connected = 0b10000;
};

class Multimetr : public Element
{
    Q_OBJECT
    bool record_state;
    double current_value;
    int type;
    QLCDNumber* display;

    static constexpr double relative_error = 0.02;
    void paintEvent(QPaintEvent*);

public:
    Multimetr(int tool_type, Element* parent = nullptr);
    bool isRecord();
    double value();
public slots:
    void recordingState(int);
    void setValue(double);
signals:
    void valueChanged(double);
    void recordingStateChanged(bool);
};

class Source : public Element
{
    Q_OBJECT
protected:
    int value;
    void drawBox(QPainter&);
    QSlider* slider;

public:
    Source(Element* parent = nullptr, int value = 0, QString name = "???????????????? ????????");
    int getValue();
    void setMin(int);
    void setMax(int);
public slots:
    virtual void setValue(int);
signals:
    void valueChanged(int);
};

class MainSource : public Source
{
    Q_OBJECT
    void paintEvent(QPaintEvent*);
    class Indicator : public QWidget
    {
        bool status;
        void paintEvent(QPaintEvent*);
        int indicator_radius;
    public:
        Indicator(QWidget *patent = nullptr, bool status = false);
        void setStatus(bool);
    };
    Indicator *indicator;
public:
    MainSource(Element* parent = nullptr, int value = 0);

public slots:
    void setValue(int);
    void setIndicatorStatus(bool);
};

class FuranceSource : public Source
{
    Q_OBJECT
    double Troom, max_dT;
    void paintEvent(QPaintEvent*);
    QLineEdit* line_edit_t;
    QString toString(int);
    int toInt(const QString&);
public:
    FuranceSource(Element* parent = nullptr,
                  int value = 0,
                  double room_tempirature = MainWindow::ZeroCelsius,
                  double max_diff_with_toom_temperature = 300);
public slots:
    //void setMin(double);
    void setValue(int);
    void setValue();
//signals:
//    void valueChanged(double);
};

class Semiconductor : public Element
{
    Q_OBJECT

    void paintEvent(QPaintEvent*);
public:
    Semiconductor(Element* parent = nullptr);
};

class Resistor : public Element
{
    void paintEvent(QPaintEvent*);

public:
    Resistor(Element* parent = nullptr);
};

class Subsceme : public Element
{
    Q_OBJECT

    QLabel *label;
    int X, Y;
    void paintEvent(QPaintEvent*);
    void refreshRect();

public:
    Subsceme(const QString &name,
             int x_position,
             int y_position,
             unsigned int width,
             unsigned int height,
             Element *parent = nullptr);
};
