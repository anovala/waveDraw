#include "widget.h"
#include "./ui_widget.h"
#include <QChart>
#include <QSplineSeries>
#include <QChartView>
#include <QVBoxLayout>
#include <QLegendMarker>
#include <QStack>

#define YELLOW QColor(255,255,0)
#define GREEN QColor(0,255,0)
#define RED QColor(255,0,0)

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , m_chart(nullptr)
    , m_pointSize(0)
    , m_xAxis()
    , m_yAxis()
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    init();
    resetIter();
    configTimer();
    configLegendMaker();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::configLegendMaker()
{
    for(auto *pLegendMaker:m_chart->legend()->markers())
    {
        if(pLegendMaker->series() == m_processSeries.at(0)\
            || pLegendMaker->series() == m_staticSeries.at(0))
            pLegendMaker->setVisible(true);
        else
            pLegendMaker->setVisible(false);
    }

}

void Widget::init()
{
     // Create a QChart instance
    m_chart = new QChart();

    // Create a QChartView instance to display the m_chart
    m_chartView = new QChartView(m_chart);

    m_chartView->setRenderHint(QPainter::Antialiasing); // Enable antialiasing for smooth curves
    m_chartView->repaint();

    //axis setting
    m_xAxis = new QValueAxis(m_chart);
    m_yAxis = new QValueAxis(m_chart);

    m_yAxis->setMax(11);
    m_yAxis->setMin(3);
    m_xAxis->setMin(0);
    m_xAxis->setMax(200);
    m_xAxis->setTickCount(11);
    m_xAxis->setTickCount(11);
    m_chart->addAxis(m_xAxis,Qt::AlignBottom);
    m_chart->addAxis(m_yAxis,Qt::AlignLeft);

    QList<QList<QPointF>> points = itemDatas();

    //begin
    addWaveSeries(LINE,YELLOW,"Soruce")->append(points.at(0));

    //loop
    for(int i = 1;i<points.size()-1;i++)
    {
        if(i%3 == 1) //straight line
        {
            addWaveSeries(LINE,YELLOW,"Soruce")->append(points.at(i));
        }
        else if ( i%3 == 2)
        {
            addWaveSeries(SPLINE,YELLOW,"Source")->append(points.at(i));
        } else
        {
            addWaveSeries(LINE,YELLOW,"Soruce")->append(points.at(i));
        }
    }

    //end
    addWaveSeries(LINE,YELLOW,"Soruce")->append(points.at(points.size()-1));

    rTimeAddProcessSeries();
     // Customize the m_chart
    m_chart->setTitle("QSplineSeries Example");

    // Customize the axes (optional)
    m_xAxis->setTitleText("X Axis");
    m_yAxis->setTitleText("Y Axis");

    m_chart->setAnimationOptions(QChart::AllAnimations);

    // Set up a layout (e.g., QVBoxLayout) in the widget
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Add the QChartView to the layout
    layout->addWidget(m_chartView);

    // Set the layout as the layout of the QWidget
    setLayout(layout);

}

void Widget::configTimer()
{
    double stepTime = m_time / m_pointSize;
    qDebug()<<"time interval ms: "<<stepTime;
    m_timer.setInterval(stepTime*1000);
    connect(&m_timer,&QTimer::timeout,this,&Widget::onTimeOut);
    m_timer.start();
}

void Widget::resetIter()
{
    m_currentLineIter = m_processPoints.begin();
    m_currentPointIndex = 0;
}

QXYSeries* Widget::addWaveSeries(lineType type ,QColor color,QString SeriesName,bool b)
{
    if(type == LINE)
    {
        QLineSeries *lineSeries;
        lineSeries = new QLineSeries;
        lineSeries->setColor(color);
        lineSeries->setName(SeriesName);
        m_chart->addSeries(lineSeries);
        lineSeries->attachAxis(m_xAxis);
        lineSeries->attachAxis(m_yAxis);
        if(b)
            m_processSeries.append(lineSeries);
        else
            m_staticSeries.append(lineSeries);
        return lineSeries;
    }else{
        QSplineSeries *spSeries;
        spSeries = new QSplineSeries;
        spSeries->setColor(color);
        spSeries->setName(SeriesName);
        m_chart->addSeries(spSeries);
        spSeries->attachAxis(m_xAxis);
        spSeries->attachAxis(m_yAxis);
        if(b)
            m_processSeries.append(spSeries);
        else
            m_staticSeries.append(spSeries);
        return spSeries;
    }
}

void Widget::rTimeAddProcessSeries()
{
    for(auto&& [type, ponits] : m_processPoints)
        addWaveSeries(type,GREEN,"Run",true);
}

//每次画一个点
void Widget::onTimeOut()
{
    //static int count;
    //qDebug()<<"call "<<count++<<" times";

    if(m_currentLineIter == m_processPoints.end())
    {
        m_timer.stop();
        return;
    }

    lineType type = m_currentLineIter->first;
    QList<QPointF> &points = m_currentLineIter->second;

    int offset = 0;
    auto beg = m_processPoints.begin();
    offset = m_currentLineIter - beg;

    if(m_currentPointIndex < points.size())
    {
        QPointF point = points.at(m_currentPointIndex);
        if(type == LINE)
        {
            QLineSeries *lineSeries = dynamic_cast<QLineSeries*> \
                (m_processSeries.at(offset));
            lineSeries->append(point);
        }else //SPLINE
        {
            QSplineSeries *spSeries = dynamic_cast<QSplineSeries*>\
                (m_processSeries.at(offset));
            spSeries->append(point);
        }
        m_currentPointIndex++;
    }

    //next line
    if(m_currentPointIndex >= points.size())
    {
        m_currentPointIndex = 0;
        m_currentLineIter++;
    }

    if(m_currentLineIter == m_processPoints.end())
    {
        m_timer.stop();
        return;
    }

    return;

}

QList<QList<QPointF>> Widget::itemDatas()
{
    double uop = 11;
    double ut = 4.5;
    double us = 4.5;
    ua = 6.5;
    ur = 2;
    double trise = 0.5;
    double tfall = 0.5;
    double t4 = 2.0;
    double t5 = 1.5;
    double t6 = 2.0;
    double t7 = 0.5;
    double t8 = 10.0;
    fa = 2.0;
    double tbegin = 0.5;
    double tend = 0.5;
    int n = 10;
    double tbreak = 2.0;

    double t = 0;
    QList<QList<QPointF>> allPoints;

    //begin
    QList<QPointF> beginPoints = {
        QPointF(t,uop),
        QPointF(t += tbegin, uop)
    };

    allPoints.append(beginPoints);
    m_processPoints.append(std::make_pair(LINE,beginPoints));

        //cycles
    for(int i= 0; i<n;i++){

        QList<QPointF> linePoints1;
        QList<QPointF> spPoints;
        QList<QPointF> linePoints2;

        linePoints1.emplaceBack(QPointF(t,uop)); // for the last cycles
        linePoints1.emplaceBack(QPointF(t += tfall, ut));
        linePoints1.emplaceBack(QPointF(t += t4, ut));
        linePoints1.emplaceBack(QPointF(t += t5, us));
        linePoints1.emplaceBack(QPointF(t += t6, us));
        linePoints1.emplaceBack(QPointF(t += t7,ua));

        // x y axis for spline
        double x = t;
        bool b = true;
        while(x < t + t8)
        {
            if(b)
                spPoints.emplaceBack(x,ua);
            else
            {
                spPoints.emplaceBack(x,ua+ur);
            }
            b = !b;
            x += 1/fa;
        }

        t += t8;
        linePoints2.emplace_back(t-1/fa,ua+ur);  // continues with the end of curves
        linePoints2.emplaceBack(t += trise, uop);
        linePoints2.emplaceBack(t += tbreak,uop); // break time

        allPoints.append(linePoints1);
        allPoints.append(spPoints);
        allPoints.append(linePoints2);
        m_processPoints.append(std::make_pair(LINE,linePoints1));
        m_processPoints.append(std::make_pair(SPLINE,spPoints));
        m_processPoints.append(std::make_pair(LINE,linePoints2));

        /*
        m_spLines.append(spPoints);
        */
    }

    //end
    QList<QPointF> endPoints ={QPointF(t += tend,uop)};
    allPoints.append(endPoints);
    m_processPoints.append(std::make_pair(LINE,endPoints));
    for(auto _list : allPoints)
        m_pointSize += _list.size();

    m_time = t;
    qDebug()<< "total time = "<<m_time;
    qDebug()<<"all points num: "<< m_pointSize;

    return allPoints;
}
