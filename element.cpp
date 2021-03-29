#include "element.h"
#include <qpainter.h>
#include <QLCDNumber>
#include <QLabel>
#include <QCheckBox>
#include <QBoxLayout>
#include <QSlider>
#include <QComboBox>
#include <fstream>
#include <QString>
#include <QList>
#include <QObject>

Element::Element(unsigned int nContacts, QWidget* parent, const char* file, unsigned int sizeX_, unsigned int sizeY_)
    : QWidget(parent), sizeX(sizeX_), sizeY(sizeY_), contactsNum(nContacts)
{
    if(nContacts)
    {
        contacts = new unsigned int[nContacts];
        for(unsigned int i = 0; i < nContacts; i++)
            contacts[i] = 0;
    }
    else
        contacts = nullptr;
    if(file != nullptr)
    {
        std::ifstream inf(file, std::ifstream::in);
        inf >> sizeY >> sizeX;
        unsigned int i, j;
        char first, second;
        table = new int*[sizeY];
        for(i = 0; i < sizeY; i++)
        {
            table[i] = new int[sizeX];
            for(j = 0; j < sizeX; j++)
            {
                inf >> first >> second;
                table[i][j] = parse(first) | parse(second);
            }
        }
        inf.close();
        genereteLayout();
    }
    else
        table = nullptr;
}
Element::~Element()
{
    if(contacts != nullptr)
        delete[] contacts;
    if(table != nullptr)
    {
        for(unsigned int i = 0; i < sizeY; i++)
            delete[] table[i];
        delete [] table;
    }
}
int Element::getSizeX()
{
    return sizeX;
}
int Element::getSizeY()
{
    return sizeY;
}
void Element::setSizeX(int val)
{
    sizeX = val;
}
void Element::setSizeY(int val)
{
    sizeY = val;
}
void Element::setContact(unsigned int contact_number, unsigned int position)
{
    if (contact_number < contactsNum)
        contacts[contact_number] = position;
}
void Element::drawContacts()
{
    QPainter painter(this);
    QPointF hStep(width()/sizeX, 0),
            vStep(0, height()/sizeY),
            hMargin(margins, 0),
            vMargin(0, margins),
            pos(0, 0);
    int c, j, flag;
    for(unsigned int i = 0; i < contactsNum; i++)
    {
        pos = QPointF(0, 0);
        c = contacts[i];
        flag = 1;
        for(j = 0; c > -1; j++)
        {
            if(j%2)
            {
                if(c < (int)sizeY)
                {
                    pos += (c + 0.5) * flag * vStep;
                    painter.drawLine(pos, pos - flag * hMargin);
                }
                pos += sizeY * (flag * vStep);
                c -= sizeY;
                flag = -flag;
            }
            else
            {
                if(c < (int)sizeX)
                {
                    pos += (c + 0.5) * flag * hStep;
                    painter.drawLine(pos, pos + flag * vMargin);
                }
                pos += sizeX * (flag * hStep);
                c -= sizeX;
            }
        }
    }
}
template<typename T>
void Element::setBorderPropereties(T* element, int** table, int i, int j)
{
    int contactPos = 0, contactNum = 0, elementType = table[i][j];
    while(!(table[i][j] & Right))
    {
        if(table[i-1][j++] == WireC)
            element->setContact(contactNum++, contactPos);
        contactPos++;
    }
    if(table[i-1][j] == WireC)
        element->setContact(contactNum++, contactPos);
    contactPos++;
    element->setSizeX(contactPos);
    while(!(table[i][j] & Bottom))
    {
        if(table[i++][j+1] == WireC)
            element->setContact(contactNum++, contactPos);
        contactPos++;
    }
    if(table[i][j+1] == WireC)
        element->setContact(contactNum++, contactPos);
    contactPos++;
    element->setSizeY(contactPos - element->getSizeX());
    while(!(table[i][j] & Left))
    {
        if(table[i+1][j--] == WireC)
            element->setContact(contactNum++, contactPos);
        contactPos++;
    }
    if(table[i+1][j] == WireC)
        element->setContact(contactNum++, contactPos);
    contactPos++;
    while(table[i][j] != elementType)
    {
        if(table[i--][j-1] == WireC)
            element->setContact(contactNum++, contactPos);
        contactPos++;
    }
    if(table[i][j-1] == WireC)
        element->setContact(contactNum++, contactPos);
    contactPos++;
}
void Element::genereteLayout()
{
    CustomGridLayout* layout = new CustomGridLayout(this);
    layout->setVerticalSpacing(0);
    layout->setHorizontalSpacing(0);
    unsigned int i, j, props;
    Multimetr* tmpM;
    Source* tmpS;
    Resistor* tmpR;
    Semiconductor* tmpSC;
    for(i = 1; i < sizeY-1; i++)
    {
        for(j = 1; j < sizeX-1; j++)
        {
            props = 0;
            switch (table[i][j] & ElementMask) {
            case WireC:
                if(table[i][j+1] & (ElementMask | XC))
                    props = props | Right;
                if(table[i][j-1] & (ElementMask | XC))
                    props = props | Left;
                if(table[i+1][j] & (ElementMask | XC))
                    props = props | Bottom;
                if(table[i-1][j] & (ElementMask | XC))
                    props = props | Top;
                layout->addElement(new Wire(props, this), i, j);
                break;
            case MultimetrC:
                tmpM = new Multimetr(table[i][j] & ElementTypeMask, this);
                switch (table[i][j] & ElementTypeMask) {
                case VoltmetrC:
                    tmpM->setName("voltmetr");
                    break;
                case AmpermetrC:
                    tmpM->setName("ampermetr");
                    break;
                case TermometrC:
                    tmpM->setName("termometr");
                    break;
                }
                setBorderPropereties<Multimetr>(tmpM, table, i, j);
                layout->addElement(tmpM, i, j);
                break;
            case SourceC:
                tmpS = new Source(this);
                tmpS->setName("source");
                setBorderPropereties<Source>(tmpS, table, i, j);
                layout->addElement(tmpS, i, j);
                break;
            case ResistorC:
                tmpR = new Resistor(this);
                tmpR->setName("resistor");
                setBorderPropereties<Resistor>(tmpR, table, i, j);
                layout->addElement(tmpR, i, j);
                break;
            case SemiconductorC:
                tmpSC = new Semiconductor(this);
                tmpSC->setName("semiconductor");
                setBorderPropereties<Semiconductor>(tmpSC, table, i, j);
                layout->addElement(tmpSC, i, j);
                break;
            }
        }
    }
}
int Element::parse(char c)
{
    switch (c) {
    case 'w':
        return WireC;
    case 'x':
        return XC;
    case 'I':
        return SourceC;
    case 'A':
        return MultimetrC | AmpermetrC;
    case 'V':
        return MultimetrC | VoltmetrC;
    case 'T':
        return MultimetrC | TermometrC;
    case 'R':
        return ResistorC;
    case 'S':
        return SemiconductorC;
    case 'r':
        return Right | XC;
    case 't':
        return Top | XC;
    case 'l':
        return Left | XC;
    case 'b':
        return Bottom | XC;
    default:
        return 0;
    }
}
void Element::setName(QString name)
{
    name += QString::number(
                                (parent()->findChildren<Element*>(
                                    QRegularExpression(name + "*")
                                    )
                                ).size()
                            );
    setObjectName(name);
}

CustomGridLayout::CustomGridLayout(QWidget* parent)
    : QGridLayout(parent)
{
}
void CustomGridLayout::addElement(Element *element, int fromRow, int fromColumn, Qt::Alignment alignment)
{
    addWidget(element,
              fromRow,
              fromColumn,
              element->getSizeY(),
              element->getSizeX(),
              alignment);
}

Wire::Wire(int contype, Element* parent)
    : Element(0, parent), type(contype)
{
}
void Wire::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPointF
            topP(width()/2, 0),
            leftP(0, height()/2),
            centerP(width()/2, height()/2),
            rightP(width(), height()/2),
            bottomP(width()/2, height()),
            vertical_margin(0, node_radius);

    if((type & DirectionMask) == DirectionMask)
    {
        if (type & Connected)
        {
            painter.setBrush(QBrush(Qt::black));
            painter.drawEllipse(centerP, node_radius/2, node_radius/2);
            painter.drawLine(topP, bottomP);
            painter.drawLine(leftP, rightP);
        }
        else
        {
            painter.drawLine(topP, centerP - vertical_margin);
            painter.drawLine(bottomP, centerP + vertical_margin);
            painter.drawLine(leftP, rightP);
            painter.drawArc(
                        QRectF(
                            width()/2 - node_radius,
                            height()/2 - node_radius,
                            2*node_radius,
                            2*node_radius),
                        90*16,
                        180*16);
        }
    }
    else
    {
        int i = 0;
        if(type & Right)
        {
            painter.drawLine(rightP, centerP);
            i++;
        }
        if(type & Left)
        {
            painter.drawLine(leftP, centerP);
            i++;
        }
        if(type & Top)
        {
            painter.drawLine(topP, centerP);
            i++;
        }
        if(type & Bottom)
        {
            painter.drawLine(bottomP, centerP);
            i++;
        }
        if(i == 3)
        {
            painter.setBrush(QBrush(Qt::black));
            painter.drawEllipse(centerP, node_radius/2, node_radius/2);
        }
    }
}

Multimetr::Multimetr(int tool_type, Element* parent)
    : Element(2, parent), type(tool_type)
{
    QLCDNumber* display = new QLCDNumber(6, this);
    display->setMode(QLCDNumber::Dec);
    display->setSmallDecimalPoint(true);

    QLabel* units = new QLabel(this);
    switch(type)
    {
    case VoltmetrC:
        units->setText("V");
        break;
    case AmpermetrC:
        units->setText("A");
        break;
    case TermometrC:
        units->setText("K");
        contactsNum = 0;
    }
    QCheckBox* recording = new QCheckBox("is recording", this);

    QVBoxLayout* vl = new QVBoxLayout(this);
    vl->setContentsMargins(1.5*margins, 1.5*margins, 1.5*margins, 1.5*margins);
    QHBoxLayout* htl = new QHBoxLayout(this);
    htl->addWidget(display);
    htl->addWidget(units);
    vl->addLayout(htl);
    vl->addWidget(recording);
    setLayout(vl);

    QObject::connect(recording, SIGNAL(stateChanged(int)), this, SLOT(recordingState(int)));
    QObject::connect(this, SIGNAL(valueChanged(double)), display, SLOT(display(double)));
    recording->setCheckState(Qt::Unchecked);
    setValue(0.1);
}
void Multimetr::recordingState(int s)
{
    if(s == Qt::Unchecked)
        record_state = false;
    else
        record_state = true;
}
void Multimetr::setValue(double v)
{
    current_value = v;
    emit valueChanged(v);
}
void Multimetr::paintEvent(QPaintEvent *)
{
    drawContacts();
    QPainter painter(this);
    painter.drawRect(margins, margins, width() - 2*margins, height() - 2*margins);
}

Source::Source(Element* parent)
    : Element(2, parent)
{
    QLabel* label = new QLabel("I", this);
    Arrow* arr = new Arrow(Arrow::Up, this);
    QSlider* slider = new QSlider(Qt::Vertical, this);
    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    slider->setTickInterval(1);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(1.5*margins, 1.5*margins, 1.5*margins, 1.5*margins);
    layout->addWidget(label);
    layout->addWidget(arr);
    layout->addWidget(slider);
}
Source::Arrow::Arrow(int dir, QWidget* parent)
    :QWidget(parent), direction(dir)
{}
void Source::Arrow::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QPointF
            topP(width()/2, height()/4),
            bottomP(width()/2, height()*3/4),
            vertical_margin(0, height()/6),
            horizontal_margin(width()/4, 0);
    painter.drawLine(topP, bottomP);
    switch (direction) {
    case Up:
        painter.drawLine(topP, topP + vertical_margin + horizontal_margin);
        painter.drawLine(topP, topP + vertical_margin - horizontal_margin);
        break;
    case Down:
        painter.drawLine(bottomP, bottomP - vertical_margin + horizontal_margin);
        painter.drawLine(bottomP, bottomP - vertical_margin - horizontal_margin);
        break;
    }
}
void Source::paintEvent(QPaintEvent *)
{
    drawContacts();
    QPainter painter(this);
    painter.drawRect(margins, margins, width() - 2*margins, height() - 2*margins);
}
void Source::setMin(int val)
{
    findChild<QSlider*>()->setMinimum(val);
}
void Source::setMax(int val)
{
    findChild<QSlider*>()->setMaximum(val);
}
void Source::setValue(int val)
{
    emit valueChanged(val);
}

Semiconductor::Semiconductor(Element* parent)
    : Element(2, parent)
{
    QComboBox* select = new QComboBox(this);
    QObject::connect(select, SIGNAL(currentIndexChanged(int)), this, SLOT(setType(int)));
    QStringList list = (QStringList()
                        << "Diamond"
                        << "Ge"
                        << "Si"
                        << "Se"
                        << "Te"
                        << "PbS"
                        << "InSb"
                        << "GaAs");
    select->addItems(list);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(1.5*margins, 1.5*margins, 1.5*margins, 1.5*margins);
    layout->addWidget(select);
    layout->addStretch();
}
void Semiconductor::paintEvent(QPaintEvent*)
{
    drawContacts();
    QPainter painter(this);
    painter.drawRect(margins, margins, width() - 2*margins, height() - 2*margins);
}
void Semiconductor::setType(int type)
{
    emit typeChanged(type);
}

Resistor::Resistor(Element* parent)
    : Element(2, parent)
{
}
void Resistor::paintEvent(QPaintEvent*)
{
    drawContacts();
    QPainter painter(this);
    QPointF firstP(width()/2, margins),
            vStep(0, (height() - 2*margins)/8),
            hStep(width() - 2*margins, 0),
            secondP = firstP + vStep/2 + hStep/2;
    painter.drawLine(firstP, secondP);
    int sign = -1;
    for(int i = 0; i < 7; i++)
    {
        firstP = secondP;
        secondP += sign* hStep + vStep;
        sign *= -1;
        painter.drawLine(firstP, secondP);
    }
    firstP = secondP + sign* hStep/2 + vStep/2;
    painter.drawLine(firstP, secondP);
}

