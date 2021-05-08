#include "element.h"
#include "mainwindow.h"
#include <qpainter.h>
#include <QLabel>
#include <QCheckBox>
#include <QBoxLayout>
#include <fstream>
#include <QString>
#include <QList>
#include <QObject>
#include <QSizePolicy>
#include <QFrame>
#include <QLineEdit>
#include <ctime>
#include <random>
#include <QRegularExpression>
#include <QDoubleValidator>

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
        MainSource* tmpMS;
        FuranceSource *tmpFS;
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
                    switch (table[i][j] & ElementTypeMask) {
                    case MainSourceC:
                        tmpMS = new MainSource(this);
                        tmpMS->setName("mainsource");
                        setBorderPropereties<Source>(tmpMS, table, i, j);
                        layout->addElement(tmpMS, i, j);
                        break;
                    case FuranceSourceC:
                        tmpFS = new FuranceSource(this);
                        tmpFS->setName("furancesource");
                        setBorderPropereties<Source>(tmpFS, table, i, j);
                        layout->addElement(tmpFS, i, j);
                        break;
                    }
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
    case 'f':
        return FuranceSourceC;
    case 'm':
        return MainSourceC;
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

Wire::Wire(int contype, Element *parent)
    : Element(0, parent), type(contype)
{}
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
    display = new QLCDNumber(6, this);
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
        units->setText("В");
        break;
    case AmpermetrC:
        head->setText("<center>Амперметр (I)</center>");
        units->setText("А");
        break;
    case TermometrC:
        head->setText("<center>Термометр (t)</center>");
        units->setText("°С");
        contactsNum = 0;
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
    //QObject::connect(this, SIGNAL(valueChanged(double)), display, SLOT(display(double)));
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
    current_value *= 1 - relative_error + 2 * relative_error * std::rand()/RAND_MAX;
    current_value = MainWindow::round(current_value, 3);
    display->display(current_value);
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

Source::Source(Element* parent, int value_, QString name)
    : Element(2, parent), value(value_)
{
    slider = new QSlider(Qt::Vertical, this);
    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
    slider->setValue(value_);
    slider->setTickInterval(1);

    QHBoxLayout *layout_horizontal = new QHBoxLayout(this);
    layout_horizontal->setSizeConstraint(QLayout::SetMinimumSize);
    layout_horizontal->setContentsMargins(InnerMargins, InnerMargins, InnerMargins, InnerMargins);
    QVBoxLayout *layout_vertical = new QVBoxLayout;
    layout_vertical->setSizeConstraint(QLayout::SetMinimumSize);
    layout_vertical->addWidget(new QLabel(name, this), 0, Qt::AlignTop | Qt::AlignHCenter);
    layout_horizontal->addLayout(layout_vertical);
    layout_horizontal->addWidget(slider);
}
void Source::drawBox(QPainter &painter)
{
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
    value = val;
    emit valueChanged(val);
}
int Source::getValue()
{
    return value;
}

MainSource::MainSource(Element* parent, int value_)
    :Source(parent, value_)
{
    indicator = new Indicator(this);
    ((QVBoxLayout*)layout()->itemAt(0))->addWidget(indicator, 0, Qt::AlignBottom | Qt::AlignLeft);
}
MainSource::Indicator::Indicator(QWidget *parent, bool status_)
    :QWidget(parent), status(status_)
{
    indicator_radius = 10;
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(new QLabel("Максимальное\nнапряжение", this));
    layout->addSpacing(3*indicator_radius);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}
void MainSource::Indicator::paintEvent(QPaintEvent*)
{
    int right_margin;
    right_margin = contentsMargins().right();
    QColor color;
    Source *source = (Source*)parent();
    if(source->getValue() == 0)
        color = QColor(Qt::gray);
    else
        if(status)
            color = QColor(Qt::red);
        else
            color = QColor(Qt::green);
    QPainter painter(this);
    painter.setPen(source->getPen());
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(color);
    painter.drawEllipse(
                QPoint(width() - right_margin - 1.5*indicator_radius, height()/2),
                indicator_radius,
                indicator_radius);
}
void MainSource::Indicator::setStatus(bool val)
{
    status = val;
}
void MainSource::setIndicatorStatus(bool val)
{
    indicator->setStatus(val);
    indicator->update();
}
void MainSource::setValue(int val)
{
    value = val;
    emit valueChanged(val);
    indicator->update();
}
void MainSource::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    drawBox(painter);
}

FuranceSource::FuranceSource(Element* parent, int value_, double Troom_, double maxdt_)
    :Source(parent, value_, "Нагрев печи"), Troom(Troom_), max_dT(maxdt_)
{
    line_edit_t = new QLineEdit("0", this);
    QDoubleValidator *validator_t = new QDoubleValidator(line_edit_t);
    validator_t->setRange(0, MainWindow::getFuranceMaxDeltaT(), 1);
    QObject::connect(line_edit_t, SIGNAL(editingFinished()), this, SLOT(setValue()));

    QHBoxLayout *line_edit_layout = new QHBoxLayout;
    line_edit_layout->addWidget(new QLabel("Нагрев на", this));
    line_edit_layout->addWidget(line_edit_t);
    line_edit_layout->addWidget(new QLabel("°С", this));
    ((QVBoxLayout*)layout()->itemAt(0))->addLayout(line_edit_layout);
    ((QVBoxLayout*)layout()->itemAt(0))->addStretch(1);
}
QString FuranceSource::toString(int val)
{
    return QLocale().toString(
                    MainWindow::round(
                        MainWindow::TfuranceIntToDouble(val), 1
                    )
    );
}
int FuranceSource::toInt(const QString& val)
{
   return MainWindow::TfuranceDoubleToInt(
                QLocale().toDouble(
                    val
                )
    );
}
void FuranceSource::setValue(int val)
{
    value = val;
    line_edit_t->setText(toString(val));
    emit valueChanged(val);
}
void FuranceSource::setValue()
{
    int int_val = toInt(line_edit_t->text());
    slider->setValue(int_val);
    line_edit_t->setText(toString(int_val));
}
void FuranceSource::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    drawBox(painter);
}

Semiconductor::Semiconductor(Element *parent)
    : Element(2, parent)
{}
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

Resistor::Resistor(Element* parent)
    : Element(2, parent)
{}
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
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setWidthF(4);
    painter.setPen(pen);
    painter.drawRect(OuterMargins, OuterMargins, width() - 2*OuterMargins, height() - 2*OuterMargins);

}
