#include "widget.h"
#include "./ui_widget.h"
#include <QChart>
#include <QSplineSeries>
#include <QChartView>
#include <QVBoxLayout>
#include <QLegendMarker>
#include <QStack>
#include <math.h>

#define YELLOW QColor(255,255,0)
#define GREEN QColor(0,255,0)

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , m_chart(nullptr)
    , m_pointSize(0)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    init();
    hideLegendMaker();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::hideLegendMaker()
{
    for (auto maker : m_chart->legend()->markers() )
    {
        maker->setVisible(false);
    }
}

void Widget::init()
{
    QList<QList<QPointF>> points = itemDatas();
     // Create a QChart instance
    m_chart = new QChart();

    QPainterPath beginPath;
    QPainterPath loopPath;
    QPainterPath endPath;

    //begin
    QLineSeries *lineSeries = new QLineSeries();
    lineSeries->append(points.at(0));
    lineSeries->setColor(YELLOW);
    lineSeries->setName("Source");
    m_chart->addSeries(lineSeries);
    m_listLineSeries.append(lineSeries);

    for(auto p: points.at(0))
        beginPath.lineTo(p);

    //待会一个个弹出来
    QStack<QList<QPointF>> spLinesStak;
    for(QList<QPointF> p:m_spLines)
        spLinesStak.push(p);

    //end
    QLineSeries *lineSeriesEnd = new QLineSeries();
    lineSeriesEnd->append(points.at(points.size()-1));
    lineSeriesEnd->setColor(YELLOW);
    lineSeriesEnd->setName("Source");
    m_chart->addSeries(lineSeriesEnd);
    m_listLineSeries.append(lineSeriesEnd);

    //添加末尾的点
    for(auto p : points.at(points.size()-1))
        endPath.lineTo(p);

    //loop
    for(int i = 1;i<points.size()-1;i++)
    {
        if(i%3 == 1) //straight line
        {
            QLineSeries *lineSeries = new QLineSeries();
            lineSeries->setColor(YELLOW);
            lineSeries->setName("Source");
            lineSeries->append(points.at(i));
            m_chart->addSeries(lineSeries);
            m_listLineSeries.append(lineSeries);

            for(auto p: points.at(i))
                loopPath.lineTo(p);
        }
        else if ( i%3 == 2)
        {
            QSplineSeries *splineSeries = new QSplineSeries();
            splineSeries->append(points.at(i));
            splineSeries->setColor(YELLOW);
            splineSeries->setName("Source");
            m_chart->addSeries(splineSeries);
            m_listSplineSeries.append(splineSeries);

            QList<QPointF> spPoints = spLinesStak.pop();

            QPointF end,cur,control;
            for(int i=0; i< spPoints.count()-1;i++)
            {
                cur = spPoints.at(i);
                end = spPoints.at(i+1);
                //参考sin函数的波形，
                if(cur.y() > ua)
                    control =  QPointF(cur.x() + 1/fa/2,cur.y()-(sin(M_PI+M_PI_2)-sin(M_PI))*ur);
                else
                    control = QPointF(cur.x() + 1/fa/2, cur.y()+(sin(M_PI_2)*ur));

                loopPath.quadTo(control,end);
            }

        } else
        {
            QLineSeries *lineSeries = new QLineSeries();
            lineSeries->append(points.at(i));
            lineSeries->setColor(YELLOW);
            lineSeries->setName("Source");
            m_chart->addSeries(lineSeries);
            m_listLineSeries.append(lineSeries);
            for(auto p: points.at(i))
                loopPath.lineTo(p);

        }
    }

    //path to paint green , progress
    m_path.addPath(beginPath);
    m_path.addPath(loopPath);
    m_path.addPath(endPath);

    // Customize the m_chart
    m_chart->setTitle("QSplineSeries Example");
    m_chart->createDefaultAxes(); // Automatically create axes based on the series data

    // Customize the axes (optional)
    m_chart->axes(Qt::Horizontal).first()->setTitleText("X Axis");
    m_chart->axes(Qt::Vertical).first()->setTitleText("Y Axis");

    m_chart->setAnimationOptions(QChart::AllAnimations);
    // Create a QChartView instance to display the m_chart
    m_chartView = new QChartView(m_chart);

    m_chartView->setRenderHint(QPainter::Antialiasing); // Enable antialiasing for smooth curves

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
    m_timer.setInterval(stepTime*1000);

}

/*
void Widget::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(QPen(GREEN));
    painter.setViewport(m_chartView->geometry());
    painter.drawPath(m_path);
    return QWidget::paintEvent(e);
}

*/

void Widget::onTimeOut()
{

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

        // 余弦线
        /*
        double x = t + 0.01;
        while(x < t + t8)
        {
            m_spPoints.emplace_back(x, sin((2 * M_PI * fa * x + M_PI)) + ur + 1);
            x += (1/fa);
        }
        */

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

        m_spLines.append(spPoints);
    }

    //end
    QList<QPointF> endPoints ={QPointF(t += tend,uop)};
    allPoints.append(endPoints);

    for(auto _list : allPoints)
        m_pointSize += _list.size();

    qDebug()<<"all points num: "<< m_pointSize;

    return allPoints;
}
