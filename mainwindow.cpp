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
#include <QComboBox>
#include <cmath>
#include <QPainter>
#include <QtCharts>

int MainWindow::IFuranceMinInt = 0;
int MainWindow:: IFuranceMaxInt = 100;
int MainWindow:: IMainMinInt = 0;
int MainWindow:: IMainMaxInt = 100;
double MainWindow:: IFuranceMinDouble = 0;
double MainWindow:: IFuranceMaxDouble= 5;
double MainWindow:: FuranceMaxDeltaT = 300;
double MainWindow:: IMainMinDouble = 0;
double MainWindow:: IMainMaxDouble = 1;

MainWindow:: MainWindow(QWidget *parent)
    : QWidget(parent)
{
    Element* desk = new Element(0, this, "scheme.txt");

    QPen pen;
    pen.setWidthF(1.5);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    desk->setPen(pen);
    desk->genereteLayout();

    SchemeCalc* schemecalc = new SchemeCalc(this);
    Multimetr* tmpM;
    MainSource* tmpMS;
    FuranceSource *tmpFS;
    SemiconductorProps* tmpSCP;
    Table* table = new Table(this);

    tmpMS = desk->findChild<MainSource*>("mainsource0");
    tmpMS->setMin(IMainMinInt);
    tmpMS->setMax(IMainMaxInt);
    QObject::connect(tmpMS, SIGNAL(valueChanged(int)), schemecalc, SLOT(setMainSourceCurrent(int)));
    QObject::connect(schemecalc, SIGNAL(voltageIsMaximum(bool)), tmpMS, SLOT(setIndicatorStatus(bool)));

    tmpFS = desk->findChild<FuranceSource*>("furancesource0");
    tmpFS->setMin(IFuranceMinInt);
    tmpFS->setMax(IFuranceMaxInt);
    QObject::connect(tmpFS, SIGNAL(valueChanged(int)), schemecalc, SLOT(setFuranceT(int)));

    tmpM = desk->findChild<Multimetr*>("voltmetr0");
    QObject::connect(schemecalc, SIGNAL(voltmetrStatusChanged(double)), tmpM, SLOT(setValue(double)));
    QObject::connect(tmpM, SIGNAL(valueChanged(double)), table, SLOT(setU(double)));
    QObject::connect(tmpM, SIGNAL(recordingStateChanged(bool)), table, SLOT(setRecordStateU(bool)));


    tmpM = desk->findChild<Multimetr*>("ampermetr0");
    QObject::connect(schemecalc, SIGNAL(ampermetrStatusChanged(double)), tmpM, SLOT(setValue(double)));
    QObject::connect(tmpM, SIGNAL(valueChanged(double)), table, SLOT(setI(double)));
    QObject::connect(tmpM, SIGNAL(recordingStateChanged(bool)), table, SLOT(setRecordStateI(bool)));

    tmpM = desk->findChild<Multimetr*>("termometr0");
    QObject::connect(schemecalc, SIGNAL(termometrStatusChanged(double)), tmpM, SLOT(setValue(double)));
    QObject::connect(tmpM, SIGNAL(valueChanged(double)), table, SLOT(setT(double)));
    QObject::connect(tmpM, SIGNAL(recordingStateChanged(bool)), table, SLOT(setRecordStateT(bool)));

    tmpSCP = new SemiconductorProps(this);
    QObject::connect(tmpSCP, SIGNAL(typeChanged(int)), schemecalc, SLOT(setSemiconductorType(int)));
    QObject::connect(tmpSCP, SIGNAL(typeChanged(int)), schemecalc, SLOT(refreshAll()));
    QObject::connect(tmpSCP, SIGNAL(lengthChanged(double)), schemecalc, SLOT(setSemiconductorLength(double)));
    QObject::connect(tmpSCP, SIGNAL(squareChanged(double)), schemecalc, SLOT(setSemiconductorSquare(double)));

    Termometr* termometr = new Termometr(this);
    QObject::connect(termometr, SIGNAL(valueChanged(double)), schemecalc, SLOT(setRoomTemperature(double)));

    desk->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout* layout = new QVBoxLayout(this);
    QHBoxLayout* layout_top_string = new QHBoxLayout;
    layout_top_string->addWidget(tmpSCP);
    layout_top_string->addWidget(termometr);
    layout_top_string->addStretch(1);
    layout_top_string->setSpacing(15);
    layout->addLayout(layout_top_string);
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
void MainWindow:: setFuranceMaxDeltaT(double val)
{
    FuranceMaxDeltaT = val;
}
double MainWindow:: getFuranceMaxDeltaT()
{
    return FuranceMaxDeltaT;
}

double MainWindow:: IfuranceIntToDouble(int value)
{
    return value * (IFuranceMaxDouble - IFuranceMinDouble)/(IFuranceMaxInt - IFuranceMinInt);
}
double MainWindow:: TfuranceIntToDouble(int value)
{
    return value * FuranceMaxDeltaT/(IFuranceMaxInt - IFuranceMinInt);
}
double MainWindow:: mainIntToDouble(int value)
{
    return value * (IMainMaxDouble - IMainMinDouble)/(IMainMaxInt - IMainMinInt);
}
int MainWindow:: IfuranceDoubleToInt(double value)
{
    return value * (IFuranceMaxInt - IFuranceMinInt)/(IFuranceMaxDouble - IFuranceMinDouble);
}
int MainWindow:: TfuranceDoubleToInt(double value)
{
    return value * (IFuranceMaxInt - IFuranceMinInt)/FuranceMaxDeltaT;
}
int MainWindow:: mainDoubleToInt(double value)
{
    return value * (IMainMaxInt - IMainMinInt)/(IMainMaxDouble - IMainMinDouble);
}

double MainWindow:: round(double value, int digits)
{
    return floor(value * pow(10, digits) + 0.5) * pow(10, -digits);
}
double MainWindow:: roundSignificantDigits(double value, int digits)
{
    double exp = floor(log10(value));
    return floor(value * pow(10, digits - exp) + 0.5) * pow(10, -digits -exp);
}

Props::Props(QWidget *parent)
    :QFrame(parent)
{
    setFrameStyle(QFrame::Box | QFrame::Raised);
    setLineWidth(2);
    setMidLineWidth(2);
    QPalette pal = palette();
    pal.setBrush(backgroundRole(), QBrush(Qt::white));
    setPalette(pal);
    setAutoFillBackground(true);
}

Termometr:: Termometr(QWidget *parent, double T_)
    : Props(parent), T(T_)
{
    QVBoxLayout *vl = new QVBoxLayout(this);
    vl->addWidget(new  QLabel("?????????????????? ??????????????????????", this));

    QLocale locale;
    line_edit = new QLineEdit(QLocale().toString(T - MainWindow::ZeroCelsius), this);

    QHBoxLayout *hl = new QHBoxLayout;
    hl->addWidget(line_edit);
    hl->addWidget(new QLabel("????", this));
    vl->addLayout(hl);
    hl->setSizeConstraint(QLayout::SetMinimumSize);
    vl->setSizeConstraint(QLayout::SetMinimumSize);

    QDoubleValidator *validator = new QDoubleValidator(line_edit);
    validator->setRange(-273.15, 120, 1);
    line_edit->setValidator(validator);
    QObject::connect(line_edit, SIGNAL(editingFinished()), this, SLOT(setT()));

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}
void Termometr:: setT()
{
    T = QLocale().toDouble(line_edit->displayText()) + MainWindow::ZeroCelsius;
    emit valueChanged(T);
}

SemiconductorProps:: SemiconductorProps(QWidget *parent, double L_, double S_)
    :Props(parent), L(L_), S(S_)
{
    QComboBox* select = new QComboBox(this);
    QObject::connect(select, SIGNAL(currentIndexChanged(int)), this, SLOT(setType(int)));
    select->addItems(QStringList()
                     << "??????????"
                     << "????????????????"
                     << "??????????????"
                     << "??????????"
                     << "????????????"
                     << "?????????????? ????????????"
                     << "?????????????????? ??????????"
                     << "?????????????? ??????????");
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("???????????????? ????????????????????????????", this), 0, Qt::AlignHCenter);
    layout->addWidget(select);

    QHBoxLayout *hl1 = new QHBoxLayout;
    hl1->addWidget(new QLabel("??????????:", this));
    line_edit_L = new QLineEdit(QLocale().toString(L), this);
    hl1->addWidget(line_edit_L);
    hl1->addWidget(new QLabel("????", this));
    hl1->setSizeConstraint(QLayout::SetMinimumSize);
    QDoubleValidator *validator1 = new QDoubleValidator(line_edit_L);
    validator1->setRange(1e-7, 10000, 8);
    line_edit_L->setValidator(validator1);
    QObject::connect(line_edit_L, SIGNAL(editingFinished()), this, SLOT(setLength()));
    layout->addLayout(hl1);

    QHBoxLayout *hl2 = new QHBoxLayout;
    hl2->addWidget(new QLabel("??????????????:", this));
    line_edit_S = new QLineEdit(QLocale().toString(S), this);
    hl2->addWidget(line_edit_S);
    hl2->addWidget(new QLabel("??????", this));
    hl2->setSizeConstraint(QLayout::SetMinimumSize);
    QDoubleValidator *validator2 = new QDoubleValidator(line_edit_S);
    validator2->setRange(1e-7, 10000, 8);
    line_edit_S->setValidator(validator2);
    QObject::connect(line_edit_S, SIGNAL(editingFinished()), this, SLOT(setSquare()));
    layout->addLayout(hl2);

    layout->addStretch();
    layout->setSizeConstraint(QLayout::SetMinimumSize);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}
void SemiconductorProps::setType(int type)
{
    emit typeChanged(type);
}
void SemiconductorProps::setLength()
{
    L = QLocale().toDouble(line_edit_L->displayText());
    emit lengthChanged(L);
}
void SemiconductorProps::setSquare()
{
    QLocale locale;
    S = QLocale().toDouble(line_edit_S->displayText());
    emit squareChanged(S);
}

Table:: Table(QWidget *parent)
    :QWidget(parent)
{
    QPushButton *record = new QPushButton("????????????????", this);
    QPushButton *graph = new QPushButton("????????????", this);
    QPushButton *save = new QPushButton("??????????????????", this);
    QObject::connect(graph, SIGNAL(clicked()), this, SLOT(drawGraph()));
    QObject::connect(record, SIGNAL(clicked()), this, SLOT(addColumn()));
    QObject::connect(save, SIGNAL(clicked()), this, SLOT(saveIntoFile()));
    record->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    save->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    graph->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *layout = new QHBoxLayout(this);
    QVBoxLayout *buttons = new QVBoxLayout;
    buttons->addWidget(record);
    buttons->addWidget(save);
    buttons->addWidget(graph);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addLayout(buttons);
    layout->addWidget(scrollArea);
    layout->setSizeConstraint(QLayout::SetMinimumSize);

    QWidget *scrollAreaContent = new QWidget(scrollArea);
    scrollAreaContent->setObjectName("scrollareacontent0");
    scrollAreaContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QHBoxLayout *scrollAreaLayout = new QHBoxLayout(scrollAreaContent);
    Column *header = new Column(scrollAreaContent, "t, ??C", "I, ??", "U, ??", "R, ????");
    data.push_back(header);
    scrollArea->setMinimumHeight(header->height() + 70);
    scrollAreaLayout->addWidget(header);
    scrollAreaLayout->addStretch();
    scrollAreaLayout->setContentsMargins(0, 0, 0, 0);
    scrollAreaLayout->setSpacing(0);
    scrollArea->setWidget(scrollAreaContent);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    setRecordStateT(false);
    setRecordStateI(false);
    setRecordStateU(false);
}
Table:: ~Table()
{
    while(!data.empty())
        removeColumn(-1);
}
void Table:: addColumn()
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
void Table:: removeColumn(unsigned int n)
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
void Table:: setT(double val)
{
    T = val;
}
void Table:: setI(double val)
{
    I = val;
}
void Table:: setU(double val)
{
    U = val;
}
void Table:: setRecordStateT(bool state)
{
    recordT = state;
}
void Table:: setRecordStateI(bool state)
{
    recordI = state;
}
void Table:: setRecordStateU(bool state)
{
    recordU = state;
}
void Table:: saveIntoFile()
{
    std::ofstream out("output.txt");
    for(auto it = data.begin(); it != data.end(); ++it)
    {
        out << (*it)->getT().toStdString() << "\t\t"
            << (*it)->getI().toStdString() << "\t\t"
            << (*it)->getU().toStdString() << "\t\t"
            << (*it)->getR().toStdString() << "\n";
    }
}
void Table:: drawGraph()
{
    std::list<std::pair<double, double>> func_data;
    auto it = data.begin();
    for(++it; it != data.end(); ++it)
    {
        if(((*it)->getR() != "") && ((*it)->getT() != ""))
        {
            func_data.push_back(
                        std::pair<double, double>(
                            QLocale().toDouble((*it)->getT()),
                            QLocale().toDouble((*it)->getR())
                            )
                        );
        }
    };
    if(func_data.size() > 4)
    {
        Graph *graph = new Graph(func_data, "t, ??C", "R, ????");
        graph->show();
        graph->exec();
        delete graph;
    }
}
Table::Column:: Column(QWidget *parent, const QString& T, const QString& I, const QString& U, const QString& R)
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
QString Table:: Column:: getT()
{
        QLabel *lbl = findChild<QLabel*>("lblT");
        return lbl->text();
}
QString Table:: Column:: getI()
{
        QLabel *lbl = findChild<QLabel*>("lblI");
        return lbl->text();
}
QString Table:: Column:: getU()
{
        QLabel *lbl = findChild<QLabel*>("lblU");
        return lbl->text();
}
QString Table:: Column:: getR()
{
        QLabel *lbl = findChild<QLabel*>("lblR");
        return lbl->text();
}

Graph:: Graph(std::list<std::pair<double, double>>& func_data, const QString& title_x, const QString& title_y, QDialog * parent)
    :QDialog(parent)
{
    QChart *chart = new QChart;
    chart->setParent(this);
    QScatterSeries *data = new QScatterSeries(this);
    data->setColor(QColor(Qt::black));
    data->setMarkerSize(8);
    for(auto it = func_data.begin(); it != func_data.end(); ++it)
        data->append((*it).first, (*it).second);
    chart->addSeries(data);
    chart->createDefaultAxes();
    chart->setTitle("???????????? R(t)");
    chart->setLocalizeNumbers(true);
    chart->legend()->hide();
    QValueAxis *x = (QValueAxis*)chart->axes(Qt::Horizontal).first();
    x->setMax(1.05* x->max() - 0.05* x->min());
    x->setMin(1.05* x->min() - 0.05* x->max());
    x->setTickCount(8);
    x->setMinorTickCount(4);
    x->setTitleText(title_x);
    QValueAxis *y = (QValueAxis*)chart->axes(Qt::Vertical).first();
    y->setMax(1.05* y->max() - 0.05* y->min());
    y->setMin(1.05* y->min() - 0.05* y->max());
    y->setTickCount(8);
    y->setMinorTickCount(4);
    y->setTitleText(title_y);

    QChartView *plot = new QChartView(chart, this);
    plot->setMinimumSize(QSize(600, 600));

    QPushButton *close = new QPushButton("??????????????", this);
    close->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QObject::connect(close, SIGNAL(clicked()), this, SLOT(close()));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(plot);
    layout->addWidget(close);
    layout->setSizeConstraint(QLayout::SetMinimumSize);
    layout->setAlignment(close, Qt::AlignRight);
}
