#include "mainwindow.h"
#include "schemecalculator.h"
#include "element.h"
#include <QBoxLayout>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QLocale>
#include <QPushButton>
#include <QScrollArea>
#include <fstream>

int MainWindow::IFuranceMinInt = 0;
int MainWindow::IFuranceMaxInt = 100;
int MainWindow::IMainMinInt = 0;
int MainWindow::IMainMaxInt = 100;
double MainWindow::IFuranceMinDouble = 0;
double MainWindow::IFuranceMaxDouble= 5;
double MainWindow::IMainMinDouble = 0;
double MainWindow::IMainMaxDouble = 1;

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    Element* desk = new Element(0, this, "scheme.txt");

    QPen pen;
    pen.setWidthF(1.5);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    desk->setPen(pen);
    desk->genereteLayout();

    SchemeCalc* schemecalc = new SchemeCalc(this);
    Multimetr* tmpM;
    Source* tmpS;
    Semiconductor* tmpSC;
    Table* table = new Table(this);

    tmpS = desk->findChild<Source*>("source0");
    tmpS->setMin(IMainMinInt);
    tmpS->setMax(IMainMaxInt);
    QObject::connect(tmpS, SIGNAL(valueChanged(int)), schemecalc, SLOT(setMainSourceCurrent(int)));

    tmpS = desk->findChild<Source*>("source1");
    tmpS->setMin(IFuranceMinInt);
    tmpS->setMax(IFuranceMaxInt);
    QObject::connect(tmpS, SIGNAL(valueChanged(int)), schemecalc, SLOT(setFuranceCurrent(int)));

    tmpM = desk->findChild<Multimetr*>("voltmetr0");
    QObject::connect(schemecalc, SIGNAL(voltmetrStatusChanged(double)), tmpM, SLOT(setValue(double)));
    QObject::connect(schemecalc, SIGNAL(voltmetrStatusChanged(double)), table, SLOT(setU(double)));
    QObject::connect(tmpM, SIGNAL(recordingStateChanged(bool)), table, SLOT(setRecordStateU(bool)));


    tmpM = desk->findChild<Multimetr*>("ampermetr0");
    QObject::connect(schemecalc, SIGNAL(ampermetrStatusChanged(double)), tmpM, SLOT(setValue(double)));
    QObject::connect(schemecalc, SIGNAL(ampermetrStatusChanged(double)), table, SLOT(setI(double)));
    QObject::connect(tmpM, SIGNAL(recordingStateChanged(bool)), table, SLOT(setRecordStateI(bool)));

    tmpM = desk->findChild<Multimetr*>("termometr0");
    QObject::connect(schemecalc, SIGNAL(termometrStatusChanged(double)), tmpM, SLOT(setValue(double)));
    QObject::connect(schemecalc, SIGNAL(termometrStatusChanged(double)), table, SLOT(setT(double)));
    QObject::connect(tmpM, SIGNAL(recordingStateChanged(bool)), table, SLOT(setRecordStateT(bool)));

    tmpSC = desk->findChild<Semiconductor*>("semiconductor0");
    QObject::connect(tmpSC, SIGNAL(typeChanged(int)), schemecalc, SLOT(setSemiconductorType(int)));
    QObject::connect(tmpSC, SIGNAL(typeChanged(int)), schemecalc, SLOT(refreshAll()));
    QObject::connect(tmpSC, SIGNAL(lengthChanged(double)), schemecalc, SLOT(setSemiconductorLength(double)));
    QObject::connect(tmpSC, SIGNAL(squareChanged(double)), schemecalc, SLOT(setSemiconductorSquare(double)));

    Termometr* termometr = new Termometr(this);
    QObject::connect(termometr, SIGNAL(valueChanged(double)), schemecalc, SLOT(setRoomTemperature(double)));

    termometr->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    desk->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(termometr);
    layout->setAlignment(termometr, Qt::AlignLeft);
    layout->addWidget(desk);
    layout->addWidget(table);
}
void MainWindow:: setIFuranceMinInt(int val)
{
    IFuranceMinInt = val;
}
void MainWindow:: setIFuranceMaxInt(int val)
{
    IFuranceMaxInt = val;
}
void MainWindow:: setIMainMinInt(int val)
{
    IMainMinInt = val;
}
void MainWindow:: setIMainMaxInt(int val)
{
    IMainMaxInt = val;
}
void MainWindow:: setIFuranceMinDouble(double val)
{
    IFuranceMinDouble = val;
}
void MainWindow:: setIFuranceMaxDouble(double val)
{
    IFuranceMaxDouble = val;
}
void MainWindow:: setIMainMinDouble(double val)
{
    IMainMinDouble = val;
}
void MainWindow:: setIMainMaxDouble(double val)
{
    IMainMaxDouble = val;
}

double MainWindow::furanceIntToDouble(int value)
{
    return value * (IFuranceMaxDouble - IFuranceMinDouble)/(IFuranceMaxInt - IFuranceMinInt);
}
double MainWindow::mainIntToDouble(int value)
{
    return value * (IMainMaxDouble - IMainMinDouble)/(IMainMaxInt - IMainMinInt);
}
int MainWindow::furanceDoubleToInt(double value)
{
    return value * (IFuranceMaxInt - IFuranceMinInt)/(IFuranceMaxDouble - IFuranceMinDouble);
}
int MainWindow::mainDoubleToInt(double value)
{
    return value * (IMainMaxInt - IMainMinInt)/(IMainMaxDouble - IMainMinDouble);
}

Termometr::Termometr(QWidget *parent, double T_)
    : QFrame(parent), T(T_)
{
    QVBoxLayout *vl = new QVBoxLayout(this);
    QLabel *label = new  QLabel("Комнатная температура", this);
    vl->addWidget(label);
    QHBoxLayout *hl = new QHBoxLayout;
    QLineEdit *field = new QLineEdit("0", this);
    QLabel *units = new QLabel("°С", this);
    hl->addWidget(field);
    hl->addWidget(units);
    vl->addLayout(hl);
    hl->setSizeConstraint(QLayout::SetMinimumSize);
    vl->setSizeConstraint(QLayout::SetMinimumSize);

    QDoubleValidator *validator = new QDoubleValidator(field);
    validator->setRange(-273.15, 10000, 2);
    field->setValidator(validator);
    QObject::connect(field, SIGNAL(textEdited(const QString &)), this, SLOT(setT(const QString &)));
    setFrameStyle(QFrame::Box | QFrame::Raised);
    setLineWidth(2);
    setMidLineWidth(2);
    QPalette pal = palette();
    pal.setBrush(backgroundRole(), QBrush(Qt::white));
    setPalette(pal);
    setAutoFillBackground(true);
}
void Termometr::setT(const QString &val)
{
    QLocale locale;
    T = locale.toDouble(val) + MainWindow::ZeroCelsius;
    if(T > 0)
        emit valueChanged(T);
}

Table::Table(QWidget *parent)
    :QWidget(parent)
{
    QPushButton *record = new QPushButton("Записать", this);
    QPushButton *save = new QPushButton("Сохранить", this);
    QObject::connect(record, SIGNAL(clicked()), this, SLOT(addColumn()));
    QObject::connect(save, SIGNAL(clicked()), this, SLOT(saveIntoFile()));
    record->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    save->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *layout = new QHBoxLayout(this);
    QVBoxLayout *buttons = new QVBoxLayout;
    buttons->addWidget(record);
    buttons->addWidget(save);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addLayout(buttons);
    layout->addWidget(scrollArea);
    layout->setSizeConstraint(QLayout::SetMinimumSize);

    QWidget *scrollAreaContent = new QWidget(scrollArea);
    scrollAreaContent->setObjectName("scrollareacontent0");
    scrollAreaContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *scrollAreaLayout = new QHBoxLayout(scrollAreaContent);
    Column *header = new Column(scrollAreaContent, "t, °C", "I, А", "U, В", "R, Ом");
    data.push_back(header);
    scrollArea->setMinimumHeight(header->height() + 70);
    scrollAreaLayout->addWidget(header);
    scrollAreaLayout->addStretch();
    scrollAreaLayout->setContentsMargins(0, 0, 0, 0);
    scrollAreaLayout->setSpacing(0);
    scrollArea->setWidget(scrollAreaContent);

    setRecordStateT(false);
    setRecordStateI(false);
    setRecordStateU(false);


}
Table::~Table()
{
    while(!data.empty())
        removeColumn(-1);
}
void Table::addColumn()
{
    if(recordT || recordI || recordU || (I > MainWindow::ZeroValue))
    {
        QWidget *sac = findChild<QWidget*>("scrollareacontent0");
        QString strT = "", strI = "", strU = "", strR = "";
        QLocale locale;
        if(recordT)
        {
            if((T < MainWindow::ZeroValue) && (-T < MainWindow::ZeroValue))
                strT = "0";
            else
                strT = locale.toString(T);
        }
        if(recordI)
        {
            if(I < MainWindow::ZeroValue)
                strI = "0";
            else
                strI = locale.toString(I);
        }
        if(recordU)
        {
            if(U < MainWindow::ZeroValue)
                strU = "0";
            else
                strU = locale.toString(U);
        }
        if(I > MainWindow::ZeroValue)
            strR = locale.toString(U/I);
        Column *column = new Column(sac, strT, strI, strU, strR);
        QHBoxLayout *layout = (QHBoxLayout*) sac->layout();
        layout->insertWidget(layout->count() - 1, column);
        data.push_back(column);
    }
}
void Table::removeColumn(unsigned int n)
{
    ++n;
    if(n < data.size())
    {
        auto it = data.begin();
        for(unsigned int i = 0; i < n; ++i)
            ++it;
        delete *it;
        data.erase(it);
    }
}
void Table::setT(double val)
{
    T = val - MainWindow::ZeroCelsius;
}
void Table::setI(double val)
{
    I = val;
}
void Table::setU(double val)
{
    U = val;
}
void Table::setRecordStateT(bool state)
{
    recordT = state;
}
void Table::setRecordStateI(bool state)
{
    recordI = state;
}
void Table::setRecordStateU(bool state)
{
    recordU = state;
}
void Table::saveIntoFile()
{
    std::ofstream out("output.txt");
    for(auto it = data.begin(); it != data.end(); ++it)
    {
        out << (*it)->getT() << "\t\t" << (*it)->getI() << "\t\t" << (*it)->getU() << "\t\t" << (*it)->getR() << "\n";
    }
}
Table::Column::Column(QWidget *parent, const QString& T, const QString& I, const QString& U, const QString& R)
    : QFrame(parent)
{
    QLabel *lblT, *lblI, *lblU, *lblR;
    lblT = new QLabel(T, this);
    lblT->setObjectName("lblT");
    lblT->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lblT->setFrameStyle(QFrame::Box | QFrame::Plain);
    lblT->setMidLineWidth(0);
    lblT->setLineWidth(1);

    lblI = new QLabel(I, this);
    lblI->setObjectName("lblI");
    lblI->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lblI->setFrameStyle(QFrame::Box | QFrame::Plain);
    lblI->setMidLineWidth(0);
    lblI->setLineWidth(1);

    lblU = new QLabel(U, this);
    lblU->setObjectName("lblU");
    lblU->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lblU->setFrameStyle(QFrame::Box | QFrame::Plain);
    lblU->setMidLineWidth(0);
    lblU->setLineWidth(1);

    lblR = new QLabel(R, this);
    lblR->setObjectName("lblR");
    lblR->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    lblR->setFrameStyle(QFrame::Box | QFrame::Plain);
    lblR->setMidLineWidth(0);
    lblR->setLineWidth(1);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(lblT);
    layout->addWidget(lblI);
    layout->addWidget(lblU);
    layout->addWidget(lblR);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}
std::string Table::Column::getT()
{
        QLabel *lbl = findChild<QLabel*>("lblT");
        return lbl->text().toStdString();
}
std::string Table::Column::getI()
{
        QLabel *lbl = findChild<QLabel*>("lblI");
        return lbl->text().toStdString();
}
std::string Table::Column::getU()
{
        QLabel *lbl = findChild<QLabel*>("lblU");
        return lbl->text().toStdString();
}
std::string Table::Column::getR()
{
        QLabel *lbl = findChild<QLabel*>("lblR");
        return lbl->text().toStdString();
}
