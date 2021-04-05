#include "element.h"
#include "mainwindow.h"
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
#include <QSizePolicy>
#include <QFrame>
#include <QLineEdit>

int Element::OuterMargins = 10;
int Element::InnerMargins = 20;
Element::Element(unsigned int nContacts, QWidget* parent, const char* file, unsigned int sizeX_, unsigned int sizeY_)
    : QWidget(parent), sizeX(sizeX_), sizeY(sizeY_), contactsNum(nContacts)
{
    setMinimumSize(QSize(sizeX*OuterMargins, sizeY*OuterMargins));
    setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum));
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
                if((table[i][j] & (SubSceme | XC)) == (SubSceme | XC))
                    table[i][j] -= SubSceme | XC;
            }
        }
        inf.close();
    }
    else
        table = nullptr;
}
Element::Element(unsigned int nContacts, Element* parent, const char* file, unsigned int sizeX_, unsigned int sizeY_)
    : Element(nContacts, (QWidget*)parent, file, sizeX_, sizeY_)
{
    pen = parent->getPen();
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
const QPen& Element::getPen()
{
    return pen;
}
void Element::setSizeX(int val)
{
    sizeX = val;
    setMinimumWidth((2*val+1)*OuterMargins);
}
void Element::setSizeY(int val)
{
    sizeY = val;
    setMinimumHeight((2*val+1)*OuterMargins);
}
void Element::setPen(const QPen & val)
{
    pen = val;
}
void Element::setContact(unsigned int contact_number, unsigned int position)
{
    if (contact_number < contactsNum)
        contacts[contact_number] = position;
}
void Element::drawContacts(QPainter& painter)
{
    QPointF hStep(width()/(qreal)sizeX, 0),
            vStep(0, height()/(qreal)sizeY),
            hMargin(OuterMargins, 0),
            vMargin(0, OuterMargins),
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
    if(table != nullptr)
    {
        CustomGridLayout* layout = new CustomGridLayout(this);
        unsigned int i, j, props, k = 0;
        Multimetr* tmpM;
        Source* tmpS;
        Resistor* tmpR;
        Semiconductor* tmpSC;
        Subsceme* tmps;
        for(i = 1; i < sizeY-1; i++)
        {
            for(j = 1; j < sizeX-1; j++)
            {
                props = 0;
                switch (table[i][j] & (ElementMask | SubSceme)) {
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
                case SubSceme:
                    for(k = 1; !(table[i][j+k] & Right); k++) {}
                    props = k;
                    for(k = 1; !(table[i+k][j+props] & Bottom); k++) {}
                    tmps = new Subsceme("Печь", j, i, props, k, this);
                    //tmps->refreshRect();
                    //tmps->repaint();
                    break;
                }
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
        return SourceC | XC;
    case 'A':
        return MultimetrC | AmpermetrC | XC;
    case 'V':
        return MultimetrC | VoltmetrC | XC;
    case 'T':
        return MultimetrC | TermometrC | XC;
    case 'R':
        return ResistorC | XC;
    case 'S':
        return SemiconductorC | XC;
    case 'r':
        return Right | XC;
    case 't':
        return Top | XC;
    case 'l':
        return Left | XC;
    case 'b':
        return Bottom | XC;
    case 's':
        return SubSceme;
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
    setSizeConstraint(QLayout::SetMinimumSize);
    setVerticalSpacing(0);
    setHorizontalSpacing(0);
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
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    QPointF
            topP(width()/2.0, 0),
            leftP(0, height()/2.0),
            centerP(width()/2.0, height()/2.0),
            rightP(width(), height()/2.0),
            bottomP(width()/2.0, height()),
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
    display->setSegmentStyle(QLCDNumber::Flat);
    display->setAutoFillBackground(true);
    QPalette pal(display->palette());
    display->setFrameStyle(QFrame::Box);
    pal.setColor(display->backgroundRole(), QColor(Qt::lightGray));
    pal.setColor(QPalette::Light, QColor(Qt::darkGray));
    pal.setColor(QPalette::Dark, QColor(Qt::darkGray));
    display->setPalette(pal);

    QLabel *units = new QLabel(this),
            *head = new QLabel(this);
    switch(type)
    {
    case VoltmetrC:
        head->setText("<center>Вольтметр (U)</center>");
        units->setText("V");
        break;
    case AmpermetrC:
        head->setText("<center>Амперметр (I)</center>");
        units->setText("A");
        break;
    case TermometrC:
        head->setText("<center>Термометр (t)</center>");
        units->setText("°С");
        contactsNum = 0;
        display->display(0);
    }
    QCheckBox* recording = new QCheckBox("записывается", this);

    head->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    units->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    display->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    recording->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QVBoxLayout* vl = new QVBoxLayout(this);
    vl->setSizeConstraint(QLayout::SetMinimumSize);
    vl->setContentsMargins(InnerMargins, InnerMargins, InnerMargins, InnerMargins);
    QHBoxLayout* htl = new QHBoxLayout;
    htl->setSizeConstraint(QLayout::SetMinimumSize);
    htl->addWidget(display);
    htl->addWidget(units);
    vl->addWidget(head);
    vl->addLayout(htl);
    vl->addWidget(recording);
    setLayout(vl);

    QObject::connect(recording, SIGNAL(stateChanged(int)), this, SLOT(recordingState(int)));
    QObject::connect(this, SIGNAL(valueChanged(double)), display, SLOT(display(double)));
    recording->setCheckState(Qt::Unchecked);
}
void Multimetr::recordingState(int s)
{
    if(s == Qt::Unchecked)
    {
        record_state = false;
        emit recordingStateChanged(false);
    }
    else
    {
        record_state = true;
        emit recordingStateChanged(true);
    }
}
void Multimetr::setValue(double v)
{
    current_value = v;
    if(type == TermometrC)
        current_value -= MainWindow::ZeroCelsius;
    emit valueChanged(current_value);
}
void Multimetr::paintEvent(QPaintEvent *)
{
    QLCDNumber* display = findChild<QLCDNumber*>();
    display->setMaximumHeight(display->width()*2/7);
    QPainter painter(this);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::white);
    drawContacts(painter);
    painter.drawRect(OuterMargins, OuterMargins, width() - 2*OuterMargins, height() - 2*OuterMargins);
}

Source::Source(Element* parent)
    : Element(2, parent)
{
    Arrow* arr = new Arrow(Arrow::Up, this);
    QSlider* slider = new QSlider(Qt::Vertical, this);
    arr->setMinimumWidth(slider->width()/2);
    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    slider->setTickInterval(1);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setSizeConstraint(QLayout::SetMinimumSize);
    layout->setContentsMargins(InnerMargins, InnerMargins, InnerMargins, InnerMargins);
    layout->addWidget(arr);
    layout->addWidget(slider);
}
Source::Arrow::Arrow(int dir, QWidget* parent)
    :QWidget(parent), direction(dir)
{}
void Source::Arrow::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QPen pen;
    pen.setWidthF(3);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    QPointF
            topP(width()/3.0, height()/4.0),
            bottomP(width()/3.0, height()*3/4.0),
            vertical_margin(0, height()/6.0),
            horizontal_margin(width()/6.0, 0);
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
    painter.setFont(QFont("Times", 24, -1, true));
    painter.drawText(topP + 2*vertical_margin - 2*horizontal_margin, "I");
}
void Source::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::white);
    drawContacts(painter);
    painter.drawRect(OuterMargins, OuterMargins, width() - 2*OuterMargins, height() - 2*OuterMargins);
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
                        << "Алмаз"
                        << "Германий"
                        << "Кремний"
                        << "Селен"
                        << "Теллур"
                        << "Сульфид свинца"
                        << "Антимонид индия"
                        << "Арсенид галия");
    select->addItems(list);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(InnerMargins, InnerMargins, InnerMargins, InnerMargins);
    layout->addWidget(select);

    QHBoxLayout *hl1 = new QHBoxLayout;
    QLineEdit *field1 = new QLineEdit("1", this);
    QLabel *units1 = new QLabel("см", this);
    QLabel *variable1 = new QLabel("Длина:", this);
    hl1->addWidget(variable1);
    hl1->addWidget(field1);
    hl1->addWidget(units1);
    hl1->setSizeConstraint(QLayout::SetMinimumSize);
    QDoubleValidator *validator1 = new QDoubleValidator(field1);
    validator1->setRange(10e-8, 10e50, 8);
    field1->setValidator(validator1);
    QObject::connect(field1, SIGNAL(textEdited(const QString &)), this, SLOT(setLength(const QString &)));
    layout->addLayout(hl1);

    QHBoxLayout *hl2 = new QHBoxLayout;
    QLineEdit *field2 = new QLineEdit("1", this);
    QLabel *units2 = new QLabel("см", this);
    QLabel *variable2 = new QLabel("Площадь:", this);
    hl2->addWidget(variable2);
    hl2->addWidget(field2);
    hl2->addWidget(units2);
    hl2->setSizeConstraint(QLayout::SetMinimumSize);
    QDoubleValidator *validator2 = new QDoubleValidator(field2);
    validator2->setRange(10e-16, 10e100, 16);
    field2->setValidator(validator2);
    QObject::connect(field2, SIGNAL(textEdited(const QString &)), this, SLOT(setSquare(const QString &)));
    layout->addLayout(hl2);

    layout->addStretch();
    layout->setSizeConstraint(QLayout::SetMinimumSize);
}
void Semiconductor::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::white);
    painter.drawRect(OuterMargins, OuterMargins, width() - 2*OuterMargins, height() - 2*OuterMargins);
    painter.setPen(QPen(Qt::NoPen));
    painter.setPen(pen);
    drawContacts(painter);
    painter.setBrush(QBrush(QColor(Qt::darkGray), Qt::BDiagPattern));
    painter.drawRect(OuterMargins, OuterMargins, width() - 2*OuterMargins, height() - 2*OuterMargins);
}
void Semiconductor::setType(int type)
{
    emit typeChanged(type);
}
void Semiconductor::setLength(const QString &val)
{
    QLocale locale;
    double tmp = locale.toDouble(val);
    emit lengthChanged(tmp);
}
void Semiconductor::setSquare(const QString &val)
{
    QLocale locale;
    double tmp = locale.toDouble(val);
    emit squareChanged(tmp);
}

Resistor::Resistor(Element* parent)
    : Element(2, parent)
{

}
void Resistor::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setPen(pen);
    painter.setRenderHint(QPainter::Antialiasing);
    drawContacts(painter);
    QPointF firstP(width()/2, OuterMargins),
            vStep(0, (height() - 2*OuterMargins)/8.0),
            hStep(width() - 2*OuterMargins, 0),
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

Subsceme::Subsceme(const QString &name, int x_, int y_, unsigned int width_, unsigned int height_, Element *parent)
    : Element(0, parent, nullptr, width_, height_), X(x_), Y(y_)
{
    label = new QLabel(name, this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->setContentsMargins(InnerMargins, InnerMargins, InnerMargins, InnerMargins);
    layout->setAlignment(label, Qt::AlignHCenter | Qt::AlignTop);
}
void Subsceme::refreshRect()
{
    int cx, cy, cwidth, cheight;
    cx = (parent()->findChild<Semiconductor*>("semiconductor0"))->x();
    cy = (parent()->findChild<Semiconductor*>("semiconductor0"))->y();
    cheight = (parent()->findChild<Semiconductor*>("semiconductor0"))->height();
    cwidth = (parent()->findChild<Semiconductor*>("semiconductor0"))->width();
    cwidth += (parent()->findChild<Multimetr*>("termometr0"))->width();
    cwidth += (parent()->findChild<Resistor*>("resistor0"))->width();
    cx -= OuterMargins;
    cy -= OuterMargins + 30;
    cwidth += 2*OuterMargins;
    cheight += 2*OuterMargins + 30;
    setGeometry(cx, cy, cwidth, cheight);
}
void Subsceme::paintEvent(QPaintEvent*)
{
    refreshRect();
    QPainter painter(this);
    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setWidthF(4);
    painter.setPen(pen);
    painter.drawRect(OuterMargins, OuterMargins, width() - 2*OuterMargins, height() - 2*OuterMargins);

}
