#pragma once
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPen>

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
        ResistorC =             0b0101,
        SemiconductorC =        0b0110,
        SubSceme =              0b100000000000,
        AmpermetrC =            0b110000,
        VoltmetrC =             0b010000,
        TermometrC =            0b100000;
    void setContact(unsigned contact_number, unsigned raw_position); // позиция от 0 по часовой стрелке
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
    int contacts;

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

    void paintEvent(QPaintEvent*);

public:
    Source(Element* parent = nullptr);
    class Arrow : public QWidget
    {
        int direction;
        void paintEvent(QPaintEvent*);

    public:
        Arrow(int arrow_direction = 0, QWidget* parent = nullptr);
        static const int
            Up = 0,
            Down = 1;
    };
    void setMin(int);
    void setMax(int);
public slots:
    void setValue(int);
signals:
    void valueChanged(int);
};

class Semiconductor : public Element
{
    Q_OBJECT

    void paintEvent(QPaintEvent*);
public:
    Semiconductor(Element* parent = nullptr);
public slots:
    void setType(int);
signals:
    void typeChanged(int);
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
    void paintEvent(QPaintEvent*);
    int X, Y;

public:
    Subsceme(const QString &name,
             int x_position,
             int y_position,
             unsigned int width,
             unsigned int height,
             Element *parent = nullptr);
public slots:
    void refreshRect();
};
