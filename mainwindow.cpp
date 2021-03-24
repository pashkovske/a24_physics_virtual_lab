#include "mainwindow.h"
#include <qpainter.h>
#include <QLCDNumber>
#include <QLabel>
#include <QCheckBox>
#include <QBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
}

Element::Element(Element* parent)
    : QWidget(parent)
{
}
int Element::getSizeX()
{
    return sizeX;
}
int Element::getSizeY()
{
    return sizeY;
}

Wire::Wire(int contype, Element* parent)
    : Element(parent), wtype(contype)
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

    if((wtype & (Right | Top | Left | Bottom)) == (Right | Top | Left | Bottom))
    {
        if (wtype & Connected)
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
        if(wtype & Right)
        {
            painter.drawLine(rightP, centerP);
            i++;
        }
        if(wtype & Left)
        {
            painter.drawLine(leftP, centerP);
            i++;
        }
        if(wtype & Top)
        {
            painter.drawLine(topP, centerP);
            i++;
        }
        if(wtype & Bottom)
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

Multimetr::Multimetr(int tool_type, int orintation, Element* parent)
    : Element(parent), mtype(tool_type), contacts(orintation)
{
    sizeX = 3;
    sizeY = 3;

    QLCDNumber* display = new QLCDNumber(4, this);
    display->setMode(QLCDNumber::Dec);
    display->setSmallDecimalPoint(true);
    QLabel* units = new QLabel(this);
    switch(mtype)
    {
    case Voltmetr:
        units->setText("V");
        break;
    case Ampermetr:
        units->setText("A");
        break;
    }
    QCheckBox* recording = new QCheckBox("is recording", this);

    QVBoxLayout* vl = new QVBoxLayout;
    vl->setContentsMargins(1.5*margins, 1.5*margins, 1.5*margins, 1.5*margins);
    QHBoxLayout* htl = new QHBoxLayout;
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
    QPainter painter(this);
    painter.drawRect(margins, margins, width() - 2*margins, height() - 2*margins);
    if(contacts & Vertical)
    {
        painter.drawLine(width()/2, 0, width()/2, margins);
        painter.drawLine(width()/2, height() - margins, width()/2, height());
    }
    if(contacts & Horizontal)
    {
        painter.drawLine(0, height()/2, margins, height()/2);
        painter.drawLine(width(), height()/2, width() - margins, height()/2);
    }
}
