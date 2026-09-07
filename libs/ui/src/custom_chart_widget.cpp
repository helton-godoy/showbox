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
#include <utility>

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

void CustomChartWidget::setChartTitle(const QString &title)
{
    m_chart->setTitle(title);
}

void CustomChartWidget::clearSeries()
{
    m_data.clear();
    m_chart->removeAllSeries();
    for (QAbstractAxis *axis : m_chart->axes()) {
        m_chart->removeAxis(axis);
        delete axis;
    }
}

void CustomChartWidget::addPoint(const QString &label, double value)
{
    m_data.append(qMakePair(label, value));
    rebuildSeries();
}

void CustomChartWidget::setData(const QString &data)
{
    m_data.clear();
    appendData(data);
}

void CustomChartWidget::appendData(const QString &data)
{
    for (const QString &pair : data.split(';', Qt::SkipEmptyParts)) {
        const qsizetype separator = pair.lastIndexOf(':');
        bool valid = false;
        const double value = pair.mid(separator + 1).toDouble(&valid);
        if (separator > 0 && valid)
            m_data.append(qMakePair(pair.left(separator), value));
    }
    rebuildSeries();
}

void CustomChartWidget::rebuildSeries()
{
    m_chart->removeAllSeries();
    for (QAbstractAxis *axis : m_chart->axes()) {
        m_chart->removeAxis(axis);
        delete axis;
    }
    if (m_data.isEmpty()) return;

    if (m_presentation == Presentation::Pie) {
        auto *series = new QPieSeries();
        for (const auto &entry : std::as_const(m_data))
            series->append(entry.first, entry.second);
        connect(series, &QPieSeries::hovered, this,
                &CustomChartWidget::onPieSeriesHovered);
        connect(series, &QPieSeries::clicked, this, [this](QPieSlice *slice) {
            emit itemClicked(slice->label(), slice->value());
        });
        m_chart->addSeries(series);
        const auto markers = m_chart->legend()->markers(series);
        for (QLegendMarker *marker : markers)
            connect(marker, &QLegendMarker::clicked, this,
                    &CustomChartWidget::handleMarkerClicked);
        return;
    }

    QStringList categories;
    qreal maximum = 0;
    for (const auto &entry : std::as_const(m_data)) {
        categories.append(entry.first);
        maximum = qMax(maximum, entry.second);
    }
    auto *valueAxis = new QValueAxis();
    valueAxis->setRange(0, maximum > 0 ? maximum * 1.1 : 1);
    auto *categoryAxis = new QBarCategoryAxis();
    categoryAxis->append(categories);

    if (m_presentation == Presentation::HorizontalBars) {
        auto *set = new QBarSet(tr("Value"));
        for (const auto &entry : std::as_const(m_data)) *set << entry.second;
        auto *series = new QHorizontalBarSeries();
        series->append(set);
        connect(series, &QHorizontalBarSeries::clicked, this,
                [this](int index, QBarSet *) {
                    if (index >= 0 && index < m_data.size())
                        emit itemClicked(m_data[index].first,
                                         m_data[index].second);
                });
        m_chart->addSeries(series);
        m_chart->addAxis(valueAxis, Qt::AlignBottom);
        m_chart->addAxis(categoryAxis, Qt::AlignLeft);
        series->attachAxis(valueAxis);
        series->attachAxis(categoryAxis);
    } else {
        auto *series = new QLineSeries();
        for (qsizetype index = 0; index < m_data.size(); ++index)
            series->append(index, m_data[index].second);
        connect(series, &QLineSeries::clicked, this, [this](const QPointF &point) {
            const int index = qRound(point.x());
            if (index >= 0 && index < m_data.size())
                emit itemClicked(m_data[index].first, m_data[index].second);
        });
        m_chart->addSeries(series);
        m_chart->addAxis(categoryAxis, Qt::AlignBottom);
        m_chart->addAxis(valueAxis, Qt::AlignLeft);
        series->attachAxis(categoryAxis);
        series->attachAxis(valueAxis);
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
    if (type == "horizontal") {
        m_presentation = Presentation::HorizontalBars;
        rebuildSeries();
        return;
    }
    if (type == "vertical") {
        m_presentation = Presentation::VerticalLine;
        rebuildSeries();
        return;
    }

    m_presentation = Presentation::VerticalLine;
    rebuildSeries();
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

    // Replace the generated value axis for the legacy linear/log grammar.
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
