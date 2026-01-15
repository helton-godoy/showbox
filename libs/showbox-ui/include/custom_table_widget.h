#ifndef CUSTOM_TABLE_WIDGET_H
#define CUSTOM_TABLE_WIDGET_H

#include <QTableWidget>
#include <QHeaderView>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QDebug>

class CustomTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CustomTableWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        m_layout = new QVBoxLayout(this);
        m_layout->setContentsMargins(0, 0, 0, 0);
        m_layout->setSpacing(2);

        m_searchBar = new QLineEdit(this);
        m_searchBar->setPlaceholderText(tr("Search..."));
        m_searchBar->setClearButtonEnabled(true);
        m_searchBar->setVisible(false);

        m_table = new QTableWidget(this);
        m_table->horizontalHeader()->setStretchLastSection(true);
        m_table->setAlternatingRowColors(true);
        m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_table->setSortingEnabled(true);
        
        m_table->setMinimumSize(400, 200);

        m_layout->addWidget(m_searchBar);
        m_layout->addWidget(m_table);
        
        setLayout(m_layout);
        setMinimumSize(400, 250); // Ensure container is large enough

        connect(m_searchBar, &QLineEdit::textChanged, this, &CustomTableWidget::filterTable);
        
        // Relay table signals
        connect(m_table, &QTableWidget::cellChanged, this, [this](int row, int col) {
            emit cellEdited(row, col, m_table->item(row, col)->text());
        });
        
        connect(m_table, &QTableWidget::itemSelectionChanged, this, [this]() {
            QList<QTableWidgetItem *> selected = m_table->selectedItems();
            if (!selected.isEmpty()) {
                emit rowSelected(selected.first()->row());
            }
        });
    }

    QTableWidget* table() const { return m_table; }
    
    void setObjectName(const QString &name) { 
        QWidget::setObjectName(name);
    }

    void setSearchVisible(bool visible) { 
        m_searchBar->setVisible(visible); 
    }

    void loadFromFile(const QString &fileName) {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) return;

        QByteArray data = file.readAll();
        if (fileName.endsWith(".json", Qt::CaseInsensitive)) {
            loadFromJson(data);
        } else {
            loadFromCsv(data);
        }
    }

    void loadFromCsv(const QByteArray &data) {
        QString content = QString::fromUtf8(data);
        QStringList lines = content.split('\n', Qt::SkipEmptyParts);
        
        for (const QString &line : lines) {
            if (!line.trimmed().isEmpty())
                addRowData(line.split(';'));
        }
    }

    void loadFromJson(const QByteArray &data) {
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (!doc.isArray()) return;

        QJsonArray root = doc.array();
        for (const QJsonValue &val : root) {
            QStringList row;
            if (val.isArray()) {
                for (const QJsonValue &cell : val.toArray()) {
                    row << cell.toString();
                }
            } else if (val.isObject()) {
                QJsonObject obj = val.toObject();
                for (const QString &key : obj.keys()) {
                    row << obj.value(key).toString();
                }
            }
            if (!row.isEmpty()) addRowData(row);
        }
    }

    void addRowData(const QStringList &data) {
        int row = m_table->rowCount();
        m_table->setSortingEnabled(false);
        m_table->insertRow(row);
        
        if (m_table->columnCount() < data.size()) {
            m_table->setColumnCount(data.size());
        }

        for (int i = 0; i < data.size(); ++i) {
            m_table->setItem(row, i, new QTableWidgetItem(data[i].trimmed()));
        }
        m_table->setSortingEnabled(true);
    }

    void setActivateFlag(bool flag) { activateFlag = flag; }
    bool getActivateFlag() const { return activateFlag; }

signals:
    void cellEdited(int row, int col, const QString &text);
    void rowSelected(int row);

private slots:
    void filterTable(const QString &text) {
        for (int i = 0; i < m_table->rowCount(); ++i) {
            bool match = false;
            for (int j = 0; j < m_table->columnCount(); ++j) {
                QTableWidgetItem *item = m_table->item(i, j);
                if (item && item->text().contains(text, Qt::CaseInsensitive)) {
                    match = true;
                    break;
                }
            }
            m_table->setRowHidden(i, !match);
        }
    }

protected:
    void focusInEvent(QFocusEvent *event) override {
        m_table->setFocus();
        QWidget::focusInEvent(event);
    }

private:
    QVBoxLayout *m_layout;
    QLineEdit *m_searchBar;
    QTableWidget *m_table;
    bool activateFlag = false;
};

#endif // CUSTOM_TABLE_WIDGET_H
