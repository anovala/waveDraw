#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLineSeries>
#include <QSplineSeries>
#include <QTimer>
#include <QPainterPath>
#include <QPainter>
#include <QValueAxis>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE


class QChart;
class QChartView;

struct ItemDatas
{
    int panel;
    double step;
    double time;
    QList<QPointF> durationPoints;
    QList<QPointF> timePoints;
    std::pair<QString, QString> traceName;

    void updatePoints(double t, const QList<QPointF>& dPoints,
                      const QList<QPointF>& tPoints)
    {
        this->time = t;
        this->durationPoints = std::move(dPoints);
        this->timePoints = std::move(tPoints);
    }

    bool isEmpty() const { return durationPoints.isEmpty() || timePoints.isEmpty();}
};

enum lineType {
    LINE,
    SPLINE
};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void hideLegendMaker();
private:
    QList<QList<QPointF>> itemDatas();
    void init();
    void configTimer();

private:

    QChart *m_chart;
    QChartView *m_chartView;
    // for show
    double m_time;
    int m_pointSize;
    QPainterPath m_path;
    QValueAxis *m_xAxis;
    QValueAxis *m_yAxis;

    //helper member
    //QList<QList<QPointF>> m_spLines;
    double ua;
    double ur;
    double fa;

    //var for draw process
    QList<std::pair<lineType,QList<QPointF>>>::Iterator m_currentLineIter;
    QList<std::pair<lineType,QList<QPointF>>> m_processPoints;
    QList<lineType> m_tempProcessSeries;
    int m_currentPointIndex;
    QTimer m_timer;
    QList<QXYSeries*> m_processSeries;

    void resetIter();
    QXYSeries* addWaveSeries(lineType type , QColor color, QString,bool bProcess = false);

    //在添加完黄线后，再添加红线，先创建个temp记录
    void addProcessTempSeries(lineType type);
    void rTimeAddProcessSeries();

private slots:
    void onTimeOut();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
