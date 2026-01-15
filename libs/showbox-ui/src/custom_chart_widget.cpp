#include "custom_chart_widget.h"
#include <QtCharts>

#include <QLogValueAxis>
#include <QValueAxis>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QLegendMarker>
#include <QtCharts/QPieLegendMarker>
#include <QFile>
#include <QTextStream>
#include <QCursor>
#include <QGraphicsSimpleTextItem>
#include <QPixmap>

CustomChartWidget::CustomChartWidget(QWidget *parent)
    : QChartView(parent)
{
    m_chart = new QChart();
    setChart(m_chart);
    setRenderHint(QPainter::Antialiasing);
    setRubberBand(QChartView::RectangleRubberBand);
    m_chart->legend()->setVisible(true);

    m_tooltip = new QGraphicsSimpleTextItem(m_chart);
    m_tooltip->setBrush(Qt::black);
    m_tooltip->setZValue(11);
    m_tooltip->hide();
}

CustomChartWidget::~CustomChartWidget()
{
}

void CustomChartWidget::setData(const QString &data)
{
    m_chart->removeAllSeries();
    appendData(data);
}

void CustomChartWidget::appendData(const QString &data)
{
    QPieSeries *series = nullptr;
    if (m_chart->series().isEmpty()) {
        series = new QPieSeries();
        connect(series, &QPieSeries::hovered, this, &CustomChartWidget::onPieSeriesHovered);
        connect(series, &QPieSeries::clicked, this, [this](QPieSlice *slice) {
            emit itemClicked(slice->label());
        });
        m_chart->addSeries(series);
    } else {
        series = qobject_cast<QPieSeries *>(m_chart->series().first());
    }

    if (!series) return;

    // Simple parsing for now: label:value;label:value
    QStringList pairs = data.split(';');
    
    for (const QString &pair : pairs) {
        QStringList parts = pair.split(':');
        if (parts.size() == 2) {
            series->append(parts[0], parts[1].toDouble());
        }
    }

    const auto markers = m_chart->legend()->markers(series);
    for (QLegendMarker *marker : markers) {
        disconnect(marker, &QLegendMarker::clicked, this, &CustomChartWidget::handleMarkerClicked);
        connect(marker, &QLegendMarker::clicked, this, &CustomChartWidget::handleMarkerClicked);
    }
}

void CustomChartWidget::onPieSeriesHovered(QPieSlice *slice, bool state)
{
    if (state) {
        m_tooltip->setText(QString("%1: %2").arg(slice->label()).arg(slice->value()));
        QPointF pScene = mapToScene(mapFromGlobal(QCursor::pos()));
        m_tooltip->setPos(pScene + QPointF(10, -10));
        m_tooltip->show();
        slice->setExploded(true);
    } else {
        m_tooltip->hide();
        slice->setExploded(false);
    }
}

void CustomChartWidget::handleMarkerClicked()
{
    auto *marker = qobject_cast<QLegendMarker *>(sender());
    if (!marker) return;

    if (marker->type() == QLegendMarker::LegendMarkerTypePie) {
        auto *pieMarker = qobject_cast<QPieLegendMarker *>(marker);
        if (pieMarker) {
            QPieSlice *slice = pieMarker->slice();
            slice->setExploded(!slice->isExploded());
        }
    }
}

void CustomChartWidget::onSeriesHovered(const QPointF &point, bool state)
{
    if (state) {
        m_tooltip->setText(QString("X: %1, Y: %2").arg(point.x()).arg(point.y()));
        QPointF p = m_chart->mapToPosition(point);
        m_tooltip->setPos(p + QPointF(10, -10));
        m_tooltip->show();
    } else {
        m_tooltip->hide();
    }
}

void CustomChartWidget::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        setData(in.readAll());
    }
}

void CustomChartWidget::setAxis(const QString &config)
{
    QStringList parts = config.split(' ', Qt::SkipEmptyParts);
    if (parts.isEmpty()) return;

    QString type = parts[0].toLower();
    qreal min = 0;
    qreal max = 10;
    if (parts.size() > 1) min = parts[1].toDouble();
    if (parts.size() > 2) max = parts[2].toDouble();

    QAbstractAxis *axisY = nullptr;

    if (type == "log") {
        auto *logAxis = new QLogValueAxis();
        logAxis->setBase(10);
        logAxis->setMin(min > 0 ? min : 1);
        logAxis->setMax(max);
        axisY = logAxis;
    } else {
        auto *valueAxis = new QValueAxis();
        valueAxis->setMin(min);
        valueAxis->setMax(max);
        axisY = valueAxis;
    }

    // Remove existing Y axes
    auto axes = m_chart->axes(Qt::Vertical);
    for (auto *ax : axes) {
        m_chart->removeAxis(ax);
        delete ax;
    }

    m_chart->addAxis(axisY, Qt::AlignLeft);
    
    // Attach to existing series
    for (auto *series : m_chart->series()) {
        series->attachAxis(axisY);
    }
}

void CustomChartWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        m_chart->zoomReset();
        event->accept();
    } else {
        QChartView::mousePressEvent(event);
    }
}

void CustomChartWidget::exportChart(const QString &path)
{
    if (path.isEmpty()) return;
    
    QPixmap pixmap = grab();
    pixmap.save(path);
}
