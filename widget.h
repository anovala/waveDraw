#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLineSeries>
#include <QSplineSeries>
#include <QTimer>
#include <QPainterPath>
#include <QPainter>

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

protected:
    void paintEvent(QPaintEvent *e) override;
private:

    QChart *m_chart;
    QChartView *m_chartView;
    // for show
    QList<QLineSeries*> m_listLineSeries;
    QList<QSplineSeries*> m_listSplineSeries;

    double m_time;
    int m_pointSize;
    QTimer m_timer;
    // for progress
    QPainterPath m_path;

    //helper member
    QList<QList<QPointF>> m_spLines;
    double ua;
    double ur;
    double fa;
private slots:
    void onTimeOut();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
