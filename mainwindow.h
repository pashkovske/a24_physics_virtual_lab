#pragma once
#include <QWidget>
#include <QGridLayout>

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
};

class Element : public QWidget
{
protected:
    unsigned int sizeX, sizeY;
    unsigned int* contacts;
    unsigned int contactsNum;
    static const int margins = 20;

    template<typename T>
    void setBorderPropereties(T* element, int** table, int positionX, int positionY);
    void drawContacts();

public:
    Element(unsigned int horizontal_size = 1,
            unsigned int vertical_size = 1,
            unsigned int number_of_contacts = 0,
            Element *parent = nullptr,
            int** layout_table = nullptr);
    ~Element();
    int getSizeX();
    int getSizeY();
    void setSizeX(int);
    void setSizeY(int);
    static const int
        Right =     0b1000000,
        Top =       0b10000000,
        Left =      0b100000000,
        Bottom =    0b1000000000,
        DirectionMask = Right | Top | Left | Bottom,

        ElementMask =          0b1111,
        ElementTypeMask =      0b110000,
        WireC =             0b0001,
        XC =                0b0010,
        MultimetrC =        0b0011,
        SourceC =           0b0100,
        ResistorC =         0b0101,
        SemiconductorC =    0b0110,
        AmpermetrC =        0b110000,
        VoltmetrC =         0b010000,
        TermometrC =        0b100000;
    void setContact(unsigned contact_number, unsigned raw_position); // позиция от 0 по часовой стрелке
    void genereteLayout(int** table);
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
    int contacts;

    void paintEvent(QPaintEvent*);

public:
    Multimetr(int tool_type, Element* parent = nullptr);
    bool isRecord();
    double value();
    void setValue(double);
public slots:
    void recordingState(int);
signals:
    void valueChanged(double);
};

class Source : public Element
{
    Q_OBJECT
    double I, range;

    void paintEvent(QPaintEvent*);

public:
    Source(double current_value = 0, Element* parent = nullptr);
    void setValue(double);
    class Arrow : public QWidget
    {
        int direction;

    public:
        Arrow(int arrow_direction = 0, QWidget* parent = nullptr);
        static const int
            Up = 0,
            Down = 1;
        void paintEvent(QPaintEvent*);
    };
};

class Semiconductor : public Element
{
    void paintEvent(QPaintEvent*);

public:
    Semiconductor(Element* parent = nullptr);
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

class Resistor : public Element
{
    void paintEvent(QPaintEvent*);

public:
    Resistor(Element* parent = nullptr);
};
/*
class Furance : public Element
{

};*/
