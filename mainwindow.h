#pragma once
#include <QWidget>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
};

class Element : public QWidget
{
protected:
    int sizeX, sizeY;
    static const int margins = 20;

public:
    Element(Element *parent = nullptr);
    int getSizeX();
    int getSizeY();
};

class Wire : public Element
{
    int type;
    static const int node_radius = 7;

public:
    Wire(int connection_type, Element *parent = nullptr);
    static const int
        Right = 0b1,
        Top = 0b10,
        Left = 0b100,
        Bottom = 0b1000,
        Connected = 0b10000;
    void paintEvent(QPaintEvent*);
};

class Multimetr : public Element
{
    Q_OBJECT
    bool record_state;
    double current_value;
    int type;
    int contacts;

public:
    Multimetr(int tool_type, int orintation, Element* parent = nullptr);
    static const int
        Voltmetr = 1,
        Ampermetr = 2,
        Vertical = 1,
        Horizontal = 2;
    bool isRecord();
    double value();
    void setValue(double);
    void paintEvent(QPaintEvent*);
public slots:
    void recordingState(int);
signals:
    void valueChanged(double);
};
