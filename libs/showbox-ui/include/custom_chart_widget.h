#ifndef CUSTOM_CHART_WIDGET_H
#define CUSTOM_CHART_WIDGET_H

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLogValueAxis>
#include <QtCharts/QPieSlice>
#include <QGraphicsSimpleTextItem>
#include <QMouseEvent>
#include <QPair>
#include <QVector>

class CustomChartWidget : public QChartView
{
    Q_OBJECT

public:
    explicit CustomChartWidget(QWidget *parent = nullptr);
    ~CustomChartWidget() override;

    void setChartTitle(const QString &title);
    void addPoint(const QString &label, double value);
    void clearSeries();

    void setData(const QString &data);
    void appendData(const QString &data);
    void loadFromFile(const QString &filePath);
    void setAxis(const QString &config);
    void exportChart(const QString &path);
    const QVector<QPair<QString, double>> &dataPoints() const { return m_data; }

signals:
    void itemClicked(const QString &label, double value);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void onPieSeriesHovered(QPieSlice *slice, bool state);
    void handleMarkerClicked();
    void onSeriesHovered(const QPointF &point, bool state);

private:
    enum class Presentation { Pie, HorizontalBars, VerticalLine };
    void rebuildSeries();

    QChart *m_chart;
    QGraphicsSimpleTextItem *m_tooltip;
    QVector<QPair<QString, double>> m_data;
    Presentation m_presentation = Presentation::Pie;
};

#endif // CUSTOM_CHART_WIDGET_H
