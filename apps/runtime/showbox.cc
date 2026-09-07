/*
 * GUI widgets for shell scripts - showbox version 1.0
 *
 * Copyright (C) 2015-2016, 2020 Andriy Martynets <andy.martynets@gmail.com>
 *------------------------------------------------------------------------------
 * This file is part of showbox.
 *
 * Showbox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Showbox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with showbox. If not, see http://www.gnu.org/licenses/.
 *------------------------------------------------------------------------------
 */

#include "showbox.h"
#include "icon_helper.h"
#include "custom_table_widget.h"
#include "custom_chart_widget.h"
#include <iostream>

using namespace DialogCommandTokens;

static const char *aboutLabel = "_dbabout_";

/*******************************************************************************
 *  ShowBox constructor
 ******************************************************************************/
ShowBox::ShowBox(const char *title, const char *about, bool resizable,
                      FILE *out):
    defaultPushButton(nullptr),
    currentLayout(new QVBoxLayout),
    currentIndex(0),
    groupLayout(nullptr),
    currentView(nullptr),
    currentListWidget(nullptr),
    currentTabsWidget(nullptr),
    output(out),
    empty(true)
{

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(9, 9, 9, 9);
    mainLayout->setSpacing(6);
    QHBoxLayout *hl = new QHBoxLayout;
    hl->setContentsMargins(0, 0, 0, 0);
    hl->setSpacing(6);

    setLayout(mainLayout);
    mainLayout->addLayout(hl);
    hl->addLayout(currentLayout);

    if (!resizable)
        mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    else
        setSizeGripEnabled(true);

    mainLayout->setAlignment(LAYOUTS_ALIGNMENT);
    currentLayout->setContentsMargins(0, 0, 0, 0);
    currentLayout->setSpacing(6);
    currentLayout->setAlignment(LAYOUTS_ALIGNMENT);

    pages.append(this);

    setWindowTitle(title);
    if (about) {
        QPushButton *pb = new QPushButton(tr("About"));
        pb->setObjectName(QString(aboutLabel));
        connect(pb, SIGNAL(clicked()), this, SLOT(pushButtonClicked()));

        QHBoxLayout *hl = new QHBoxLayout();
        hl->addStretch(1);
        hl->addWidget(pb);
        currentLayout->addLayout(hl);
        currentIndex++;
    }
}

void ShowBox::setThemeManager(ThemeManager *manager)
{
    m_themeManager = manager;
    if (m_themeManager) {
        connect(m_themeManager, &ThemeManager::themeChanged, this, &ShowBox::applyTheme);
        applyTheme(m_themeManager->currentTheme());
    }
}

void ShowBox::applyTheme(ThemeManager::ThemeMode mode)
{
    if (mode == ThemeManager::Unknown) return;

    QPalette pal = palette();
    QString baseStyle;

    if (mode == ThemeManager::Dark) {
        pal.setColor(QPalette::Window, QColor(45, 45, 45));
        pal.setColor(QPalette::WindowText, Qt::white);
        pal.setColor(QPalette::Base, QColor(30, 30, 30));
        pal.setColor(QPalette::AlternateBase, QColor(45, 45, 45));
        pal.setColor(QPalette::ToolTipBase, Qt::white);
        pal.setColor(QPalette::ToolTipText, Qt::white);
        pal.setColor(QPalette::Text, Qt::white);
        pal.setColor(QPalette::Button, QColor(53, 53, 53));
        pal.setColor(QPalette::ButtonText, Qt::white);
        pal.setColor(QPalette::BrightText, Qt::red);
        pal.setColor(QPalette::Link, QColor(42, 130, 218));
        pal.setColor(QPalette::Highlight, QColor(42, 130, 218));
        pal.setColor(QPalette::HighlightedText, Qt::black);
        
        baseStyle = "QGroupBox { border: 1px solid #444; border-radius: 10px; margin-top: 1.5ex; padding: 10px; }"
                    "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; color: #888; }"
                    "QFrame { border: 1px solid #444; border-radius: 10px; border-style: solid; }"
                    "QFrame[frameShape=\"4\"], QFrame[frameShape=\"5\"] { border: none; height: 1px; background: #444; margin: 5px 0; }"
                    "QFrame[flat=\"true\"], QFrame[frameShadow=\"16\"] { border: none; background: transparent; }";
    } else {
        pal = QApplication::style()->standardPalette();
        baseStyle = "QGroupBox { border: 1px solid #ddd; border-radius: 10px; margin-top: 1.5ex; padding: 10px; }"
                    "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; color: #999; }"
                    "QFrame { border: 1px solid #ddd; border-radius: 10px; border-style: solid; }"
                    "QFrame[frameShape=\"4\"], QFrame[frameShape=\"5\"] { border: none; height: 1px; background: #ddd; margin: 5px 0; }"
                    "QFrame[flat=\"true\"], QFrame[frameShadow=\"16\"] { border: none; background: transparent; }";
    }
    
    setPalette(pal);
    setStyleSheet(baseStyle);
}

void ShowBox::addPushButton(const char *title, const char *name, bool apply,
                             bool exit, bool def)
{
    QPushButton *pb = new QPushButton(title);

    pb->setObjectName(QString(name));

    if (groupLayout)
        groupLayout->insertWidget(groupIndex++, pb);
    else
        currentLayout->insertWidget(currentIndex++, pb);

    connect(pb, SIGNAL(clicked()), this, SLOT(pushButtonClicked()));
    connect(pb, SIGNAL(toggled(bool)), this, SLOT(pushButtonToggled(bool)));
    if (apply) {
        connect(pb, SIGNAL(clicked()), this, SLOT(report()));
        if (exit)
            connect(pb, SIGNAL(clicked()), this, SLOT(accept()));
    } else {
        if (exit)
            connect(pb, SIGNAL(clicked()), this, SLOT(reject()));
    }

    if (def) {
        pb->setDefault(true);
        defaultPushButton = pb;
    }

    updateTabsOrder();
}

void ShowBox::addCheckBox(const char *title, const char *name, bool checked)
{
    QCheckBox *cb = new QCheckBox(title);

    cb->setObjectName(QString(name));
    cb->setChecked(checked);

    if (groupLayout)
        groupLayout->insertWidget(groupIndex++, cb);
    else
        currentLayout->insertWidget(currentIndex++, cb);

    updateTabsOrder();
}

void ShowBox::addRadioButton(const char *title, const char *name,
                               bool checked)
{
    QRadioButton *rb = new QRadioButton(title);

    rb->setObjectName(QString(name));
    rb->setChecked(checked);

    if (groupLayout)
        groupLayout->insertWidget(groupIndex++, rb);
    else
        currentLayout->insertWidget(currentIndex++, rb);

    updateTabsOrder();
}

void ShowBox::addLabel(const char *title, const char *name,
                         enum ContentType content)
{
    QLabel *lb = new QLabel;

    lb->setObjectName(QString(name));

    if (groupLayout)
        groupLayout->insertWidget(groupIndex++, lb);
    else
        currentLayout->insertWidget(currentIndex++, lb);

    sanitizeLabel(lb, content);

    switch (content) {
    case PixmapContent:
        lb->setPixmap(QPixmap(title)); // QLabel copies QPixmap object
        break;
    case MovieContent: {
        QMovie *mv = new QMovie(title);

        lb->setMovie(mv); // QLabel stores pointer to QMovie object
        mv->setParent(lb);
        mv->start();
        break;
    }
    default:
        lb->setText(title); // QLabel copies QString object
        break;
    }
}

void ShowBox::addGroupBox(const char *title, const char *name, bool vertical,
                            bool checkable, bool checked)
{
    QGroupBox *gb = new QGroupBox(title);

    gb->setObjectName(QString(name));
    gb->setCheckable(checkable);
    gb->setChecked(checked);

    groupLayout = (vertical ? (QBoxLayout *)new QVBoxLayout
                  : (QBoxLayout *)new QHBoxLayout);
    groupIndex = 0;
    gb->setLayout(groupLayout);
    groupLayout->setContentsMargins(9, 9, 9, 9);
    groupLayout->setSpacing(6);
    groupLayout->setAlignment(LAYOUTS_ALIGNMENT);
    currentLayout->insertWidget(currentIndex++, gb);

    updateTabsOrder();
}

void ShowBox::addFrame(const char *name, bool vertical, unsigned int style)
{
    QFrame *frame = new QFrame;

    frame->setObjectName(QString(name));

    // Style is a DialogCommandTokens::Control value
    style &= PropertyMask;
    // Reset this bit instead of masking another 7 bits
    style &= ~PropertyVertical;

    groupLayout = vertical ? (QBoxLayout *)new QVBoxLayout
                  : (QBoxLayout *)new QHBoxLayout;
    groupIndex = 0;
    frame->setLayout(groupLayout);
    groupLayout->setContentsMargins(9, 9, 9, 9);
    groupLayout->setSpacing(6);
    groupLayout->setAlignment(LAYOUTS_ALIGNMENT);
    currentLayout->insertWidget(currentIndex++, frame);

    updateTabsOrder();
}

void ShowBox::addTextBox(const char *title, const char *name,
                           const char *text, const char *placeholder,
                           bool password)
{
    QHBoxLayout *box = new QHBoxLayout;
    QLabel *label = new QLabel(title);
    QLineEdit *edit = new QLineEdit(text);

    edit->setPlaceholderText(placeholder);
    edit->setEchoMode(password ? QLineEdit::Password : QLineEdit::Normal);

    label->setObjectName(name);
    label->setBuddy(edit);
    label->setFocusProxy(edit);

    box->addWidget(label);
    box->addWidget(edit);

    if (groupLayout)
        ((QBoxLayout *)groupLayout)->insertLayout(groupIndex++, box);
    else
        ((QBoxLayout *)currentLayout)->insertLayout(currentIndex++, box);

    updateTabsOrder();
}

void ShowBox::addListBox(const char *title, const char *name, bool activation,
                           bool selection)
{
    QVBoxLayout *box = new QVBoxLayout;
    QLabel *label = new QLabel(title);
    ListBox *list = new ListBox;

    label->setObjectName(QString(name));
    label->setBuddy(list);
    label->setFocusProxy(list);

    box->addWidget(label);
    box->addWidget(list);

    currentListWidget = currentView = list;
    viewIndex = 0;

    if (groupLayout)
        groupLayout->insertLayout(groupIndex++, box);
    else
        currentLayout->insertLayout(currentIndex++, box);

    if (activation) {
        connect(list, SIGNAL(activated(const QModelIndex &)), this,
                SLOT(listBoxItemActivated(const QModelIndex &)));
        list->setActivateFlag(true);
    }

    if (selection) {
        connect(list,
            SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
            this, SLOT(listBoxItemSelected(QListWidgetItem *)));
    }

    updateTabsOrder();
}

void ShowBox::addComboBox(const char *title, const char *name, bool editable,
                            bool selection)
{
    QHBoxLayout *box = new QHBoxLayout;
    QLabel *label = new QLabel(title);
    QComboBox *list = new QComboBox;

    label->setObjectName(QString(name));
    label->setBuddy(list);
    label->setFocusProxy(list);

    list->setEditable(editable);

    // Prevent insertions as there is no way to report them. Instead an apply
    // pushbutton must be used to retrieve and process current values.
    list->setInsertPolicy(QComboBox::NoInsert);

    box->addWidget(label);
    box->addWidget(list);

    currentView = list->view();
    currentListWidget = list;
    viewIndex = 0;

    if (groupLayout)
        groupLayout->insertLayout(groupIndex++, box);
    else
        currentLayout->insertLayout(currentIndex++, box);

    // Qt::QueuedConnection is used for this type widget to fix the bug when
    // the first added item made current and is reported as an emty one
    if (selection) {
        connect(list, SIGNAL(currentIndexChanged(int)), this,
                SLOT(comboBoxItemSelected(int)), Qt::QueuedConnection);
    }

    updateTabsOrder();
}

void ShowBox::addItem(const char *title, const char *icon, bool current)
{
    // It seems the insertRows call triggers signals (e.g.
    // rowsAboutToBeInserted, rowsInserted or so) which are queued or cause
    // delays in some other way for widgets' currentIndexChanged and
    // currentItemChanged signals (probably for setData calls...).
    // That is why QComboBox object's index update is queued/delayed and we have
    // to use Qt::QueuedConnection for connections for objects of this type.

    if (currentView) {
        if (widgetType(currentListWidget) == TableWidget) {
            ((CustomTableWidget *)currentListWidget)->addRowData(QString(title).split(';'));
            return;
        }
        QAbstractItemModel *model = currentView->model();

        model->insertRows(viewIndex, 1);
        model->setData(model->index(viewIndex, 0), IconHelper::loadIcon(icon),
                       Qt::DecorationRole);
        model->setData(model->index(viewIndex, 0), QString(title),
                       Qt::DisplayRole);
        if (current || model->rowCount() == 1) {
            if (widgetType(currentListWidget) == ComboBoxWidget)
                ((QComboBox *)currentListWidget)->setCurrentIndex(viewIndex);
            else
                currentView->setCurrentIndex(model->index(viewIndex, 0));
        }
        viewIndex++;
    }
}

void ShowBox::addSeparator(const char *name, bool vertical,
                             unsigned int style)
{
    unsigned int shadow = QFrame::Sunken;
    QFrame *separator = new QFrame;

    separator->setObjectName(name);

    if (vertical) {
        separator->setFrameShape(QFrame::VLine);
        separator->setSizePolicy(QSizePolicy::Fixed,
                                 QSizePolicy::MinimumExpanding);
    } else {
        separator->setFrameShape(QFrame::HLine);
        separator->setSizePolicy(QSizePolicy::MinimumExpanding,
                                 QSizePolicy::Fixed);
    }

    // Style is a DialogCommandTokens::Control value
    if (style & SeparatorWidget) {
        style &= PropertyMask;
        shadow = style & PropertyRaised ? QFrame::Raised
                 : style & PropertyPlain ? QFrame::Plain
                 : QFrame::Sunken;
    }

    separator->setFrameShadow(QFrame::Shadow(shadow));

    if (groupLayout)
        groupLayout->insertWidget(groupIndex++, separator);
    else
        currentLayout->insertWidget(currentIndex++, separator);
}

void ShowBox::addProgressBar(const char *name, bool vertical, bool busy)
{
    QProgressBar *pb = new QProgressBar;

    pb->setObjectName(QString(name));

    if (vertical) {
        pb->setOrientation(Qt::Vertical);
        pb->setTextDirection(QProgressBar::BottomToTop);
    }

    if (busy)
        pb->setRange(0, 0);
    else
        pb->setRange(0, 100);

    if (groupLayout)
        groupLayout->insertWidget(groupIndex++, pb);
    else
        currentLayout->insertWidget(currentIndex++, pb);
}

void ShowBox::addSlider(const char *name, bool vertical, int min, int max)
{
    QSlider *slider = new QSlider;

    slider->setObjectName(QString(name));
    if (!vertical)
        slider->setOrientation(Qt::Horizontal);
    slider->setTickPosition(QSlider::TicksAbove);
    slider->setTracking(false);
    connect(slider, SIGNAL(valueChanged(int)), this,
            SLOT(sliderValueChanged(int)));
    connect(slider, SIGNAL(rangeChanged(int, int)), this,
            SLOT(sliderRangeChanged(int, int)));

    slider->setRange(min, max);

    if (groupLayout)
        groupLayout->insertWidget(groupIndex++, slider);
    else
        currentLayout->insertWidget(currentIndex++, slider);

    updateTabsOrder();
}

void ShowBox::addTextView(const char *name, const char *file)
{
    QTextEdit *viewer = new QTextEdit;
    QFile txt(file);

    viewer->setObjectName(QString(name));
    viewer->setReadOnly(true);
    if (txt.open(QFile::ReadOnly))
        viewer->setText(QTextStream(&txt).readAll());

    // Below includes TextSelectableByMouse | LinksAccessibleByMouse
    // | LinksAccessibleByKeyboard.
    // This allows to select by mouse and copy text or link by context menu.
    // Qt::TextSelectableByKeyboard adds keyboard cursor which makes scrolling
    // inconvenient.
    viewer->setTextInteractionFlags(Qt::TextBrowserInteraction);

    if (groupLayout)
        groupLayout->insertWidget(groupIndex++, viewer);
    else
        currentLayout->insertWidget(currentIndex++, viewer);

    updateTabsOrder();
}

void ShowBox::addTabs(const char *name, unsigned int position)
{
    QTabWidget *tabs = new QTabWidget;

    tabs->setObjectName(QString(name));

    position &= PropertyMask;
    // Reset this bit instead of masking another 4 bits
    position &= ~PropertyIconSize;
    if (position) {
        tabs->setTabPosition(position & PropertyPositionTop
                             ? QTabWidget::North
                             : position & PropertyPositionBottom
                             ? QTabWidget::South
                             : position & PropertyPositionLeft
                             ? QTabWidget::West : QTabWidget::East);
    }

    currentTabsWidget = tabs;
    tabsIndex = 0;

    if (groupLayout)
        groupLayout->insertWidget(groupIndex++, tabs);
    else
        currentLayout->insertWidget(currentIndex++, tabs);

    updateTabsOrder();
}

void ShowBox::addPage(const char *title, const char *name, const char *icon,
                        bool current)
{
    if (currentTabsWidget) {
        endGroup();

        QWidget *page = new QWidget;

        page->setObjectName(QString(name));

        QVBoxLayout *ml = new QVBoxLayout;
        QHBoxLayout *hl = new QHBoxLayout;
        currentLayout = new QVBoxLayout;
        currentIndex = 0;

        page->setLayout(ml);
        ml->addLayout(hl);
        hl->addLayout(currentLayout);

        ml->setSizeConstraint(QLayout::SetFixedSize);
        ml->setAlignment(LAYOUTS_ALIGNMENT);
        currentLayout->setAlignment(LAYOUTS_ALIGNMENT);

        pages.append(page);

        connect(page, SIGNAL(destroyed(QObject *)), this,
                SLOT(removePage(QObject *)));

        currentTabsWidget->insertTab(tabsIndex, page, IconHelper::loadIcon(icon),
                                     QString(title));
        if (current)
            currentTabsWidget->setCurrentIndex(tabsIndex);
        tabsIndex++;
    }
}

void ShowBox::addCalendar(const char *title, const char *name, const char *date,
                           const char *min, const char *max, const char *format,
                           bool selection)
{
    Q_UNUSED(title);
    Q_UNUSED(format);

    QCalendarWidget *calendar = new QCalendarWidget();
    calendar->setObjectName(QString(name));

    if (date) {
        QDate d = QDate::fromString(QString(date), Qt::ISODate);
        if (d.isValid())
            calendar->setSelectedDate(d);
    }

    if (min) {
        QDate d = QDate::fromString(QString(min), Qt::ISODate);
        if (d.isValid())
            calendar->setMinimumDate(d);
    }

    if (max) {
        QDate d = QDate::fromString(QString(max), Qt::ISODate);
        if (d.isValid())
            calendar->setMaximumDate(d);
    }

    if (selection) {
        connect(calendar, SIGNAL(selectionChanged()), this,
                SLOT(calendarSelected()));
    }

    if (groupLayout)
        groupLayout->insertWidget(groupIndex++, calendar);
    else
        currentLayout->insertWidget(currentIndex++, calendar);

    empty = false;
}

void ShowBox::addTable(const char *headers, const char *name, const char *file,
                        bool readonly, bool selection, bool search)
{
    CustomTableWidget *container = new CustomTableWidget();
    container->setObjectName(QString(name));
    QTableWidget *table = container->table();

    if (search) {
        container->setSearchVisible(true);
    }

    if (headers) {
        QStringList headerList = QString(headers).split(';');
        table->setColumnCount(headerList.size());
        table->setHorizontalHeaderLabels(headerList);
    }

    if (readonly) {
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    } else {
        table->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed | QAbstractItemView::AnyKeyPressed);
    }

    if (selection) {
        connect(container, SIGNAL(rowSelected(int)), this, SLOT(tableRowSelected(int)));
    }

    connect(container, SIGNAL(cellEdited(int, int, const QString &)), this, SLOT(tableCellEdited(int, int, const QString &)));

    if (file) {
        container->loadFromFile(QString(file));
    }

    currentView = table;
    currentListWidget = container;

    if (groupLayout)
        groupLayout->insertWidget(groupIndex++, container);
    else
        currentLayout->insertWidget(currentIndex++, container);

    updateTabsOrder();
}

void ShowBox::addChart(const char *title, const char *name)
{
    CustomChartWidget *chartView = new CustomChartWidget();
    chartView->setObjectName(QString(name));
    if (title)
        chartView->chart()->setTitle(QString(title));

    connect(chartView, &CustomChartWidget::itemClicked, this, &ShowBox::chartItemClicked);

    if (groupLayout)
        groupLayout->insertWidget(groupIndex++, chartView);
    else
        currentLayout->insertWidget(currentIndex++, chartView);

    updateTabsOrder();
}

void ShowBox::endPage()
{
    QWidget *page = currentLayout->parentWidget();

    if (page != (QWidget *)this
        && currentTabsWidget == page->parent()->parent()) {
        QLayout *layout;
        QObject *parent;

        endGroup();
        layout = findLayout(currentTabsWidget);
        parent = layout->parent();
        if (parent->isWidgetType()) {
            groupLayout = (QBoxLayout *)layout;
            groupIndex = layout->indexOf(currentTabsWidget) + 1;
            currentLayout = (QBoxLayout *)findLayout((QWidget *)parent);
            currentIndex = currentLayout->indexOf((QWidget *)parent) + 1;
        } else {
            currentLayout = (QBoxLayout *)layout;
            currentIndex = layout->indexOf(currentTabsWidget) + 1;
        }
    }
}

void ShowBox::endTabs()
{
    endPage();

    QWidget *page = currentLayout->parentWidget();

    if (page != (QWidget *)this) {
        currentTabsWidget = (QTabWidget *)page->parent()->parent();
        tabsIndex = currentTabsWidget->count();
    } else {
        currentTabsWidget = nullptr;
    }
}

bool ShowBox::isLayoutOnPage(QWidget *page, QLayout *layout)
{
    if (!layout)
        return (false);

    QObject *parent = layout->parent();
    QWidget *parentWidget = parent->isWidgetType()
                            ? ((QWidget *)parent)->parentWidget()
                            : layout->parentWidget();

    if (parentWidget == page)
        return true;

    if (parentWidget == this)
        return false;

    return isLayoutOnPage(page,
            findLayout((QWidget *)parentWidget->parent()->parent()));
}

bool ShowBox::isWidgetOnPage(QWidget *page, QWidget *widget)
{
    if (!widget)
        return false;

    return isLayoutOnPage(page, findLayout(widget));
}

bool ShowBox::isLayoutOnContainer(QWidget *container, QLayout *layout)
{
    if (!layout)
        return false;

    QObject *parent = layout->parent();
    QWidget *page;

    if (parent->isWidgetType()) {
        if (parent == container)
            return true;

        page = ((QWidget *)parent)->parentWidget();
    } else {
        page = layout->parentWidget();
    }

    if (page == this)
        return false;

    return isLayoutOnContainer(container,
            findLayout((QWidget *)page->parent()->parent()));
}

bool ShowBox::isWidgetOnContainer(QWidget *container, QWidget *widget)
{
    if (!widget)
        return false;

    return isLayoutOnContainer(container, findLayout(widget));
}

static int indexOf(QLayout *layout)
{
    int i = -1;
    int j;

    if (QLayout *parent = (QLayout *)layout->parent()) {
        for (i = 0, j = parent->count(); i < j; i++) {
            if (parent->itemAt(i)->layout() == layout)
                return i;
        }
    }
    return i;
}

void ShowBox::stepHorizontal()
{
    QBoxLayout *oldLayout = currentLayout;
    QVBoxLayout *vBox = new QVBoxLayout;

    ((QBoxLayout *)currentLayout->parent())->insertLayout(
            indexOf(currentLayout) + 1, vBox);
    currentLayout = vBox;
    currentIndex = 0;
    currentLayout->setContentsMargins(0, 0, 0, 0);
    currentLayout->setSpacing(6);
    currentLayout->setAlignment(LAYOUTS_ALIGNMENT);
    endGroup();
    sanitizeLayout(oldLayout);
}

void ShowBox::stepVertical()
{
    QBoxLayout *oldLayout = currentLayout;
    QVBoxLayout *vBox = new QVBoxLayout;
    QHBoxLayout *hBox = new QHBoxLayout;
    QBoxLayout *rootLayout = (QBoxLayout *)currentLayout->parent()->parent();

    rootLayout->insertLayout(indexOf((QLayout *)currentLayout->parent()) + 1,
                             hBox);
    hBox->addLayout(vBox);
    currentLayout = vBox;
    currentIndex = 0;
    currentLayout->setContentsMargins(0, 0, 0, 0);
    currentLayout->setSpacing(6);
    currentLayout->setAlignment(LAYOUTS_ALIGNMENT);
    endGroup();
    sanitizeLayout(oldLayout);
}

void ShowBox::clear(char *name)
{
    QWidget *widget;

    if (name[0]) {
        if ( (widget = findWidget(name)) ) {
            switch ((unsigned)widgetType(widget)) {
            case ListBoxWidget:
            case ComboBoxWidget:
                clearChosenList();
                break;
            case PageWidget:
                clearPage(widget);
                break;
            case TabsWidget:
                clearTabs((QTabWidget *)widget);
                break;
            }
        }
    } else {
        clearDialog();
    }
}

void ShowBox::clearChosenList()
{
    if (chosenView && !chosenRowFlag) {
        QAbstractItemModel *model = chosenView->model();

        model->removeRows(0, model->rowCount());
        if (chosenView == currentView)
            viewIndex = model->rowCount();
    }
}

void ShowBox::clearTabs(QTabWidget *widget)
{
    for (int i = widget->count(); i > 0; i--) {
        QWidget *page = widget->widget(i - 1);

        while (isWidgetOnPage(page, currentTabsWidget))
            endTabs();
        if (isWidgetOnPage(page, currentListWidget))
            endList();
        if (isWidgetOnPage(page, defaultPushButton))
            defaultPushButton = nullptr;

        if (isLayoutOnPage(page, currentLayout)) {
            endGroup();
            QLayout *layout = findLayout(widget);
            QObject *parent = layout->parent();
            if (parent->isWidgetType()) {
                groupLayout = (QBoxLayout *)layout;
                groupIndex = layout->indexOf(widget) + 1;
                currentLayout = (QBoxLayout *)findLayout((QWidget *)parent);
                currentIndex = currentLayout->indexOf((QWidget *)parent) + 1;
            } else {
                currentLayout = (QBoxLayout *)layout;
                currentIndex = layout->indexOf(widget) + 1;
            }
        }

        delete page;
    }
}

void ShowBox::clearPage(QWidget *widget)
{
    QLayout *layout0;
    QLayout *layout1;
    QLayout *layout2;
    QLayout *layout3;

    layout0 = widget->layout(); // main (vertical one)

    while (isWidgetOnPage(widget, currentTabsWidget))
        endTabs();
    if (isWidgetOnPage(widget, currentListWidget))
        endList();
    if (isWidgetOnPage(widget, defaultPushButton))
        defaultPushButton = nullptr;

    if (isLayoutOnPage(widget, currentLayout)) {
        endGroup();
        currentLayout = (QBoxLayout *)layout0->itemAt(0)->layout()->itemAt(0)
                        ->layout();
        currentIndex = 0;
    }

    for (int i = layout0->count() - 1; i >= 0; i--) {
        layout1 = layout0->itemAt(i)->layout(); // Horizontal ones

        for (int j = layout1->count() - 1; j >= 0; j--) {
            layout2 = layout1->itemAt(j)->layout(); // Vertical ones

            QLayoutItem *li;
            QLayoutItem *wi;
            QWidget *w;

            while ( (li = layout2->takeAt(0)) ) {
                if ((layout3 = li->layout())) { // Joint widgets' layout
                    while ( (wi = layout3->takeAt(0)) ) {
                        if ( (w = wi->widget()) ) {
                            // QWidget isn't inherited by QWidgetItem and must
                            // be deleted separately
                            delete w;
                        }
                        delete wi;
                    }
                }
                if ( (w = li->widget()) ) {
                    // Container widget owns installed layout and its widgets
                    // and deletes them
                    delete w;
                }
                delete li;
            }
            if (i != 0 || j != 0)
                delete layout1->takeAt(j);
            // else that is the last one layout made the current one
            // (assigned to currentLayout)
        }
        if (i != 0)
            delete layout0->takeAt(i);
    }
}

void ShowBox::clearDialog()
{
    defaultPushButton = nullptr;
    currentView = nullptr;
    currentListWidget = nullptr;
    currentTabsWidget = nullptr;
    groupLayout = nullptr;
    currentLayout = (QBoxLayout *)layout()->itemAt(0)->layout()->itemAt(0)
                    ->layout();
    currentIndex = 0;

    while (pages.count() > 1)
        delete pages.takeLast();

    clearPage(this);
}

void ShowBox::removeWidget(char *name)
{
    QWidget *widget;

    if ( (widget = findWidget(name)) ) {
        int type = widgetType(widget);

        if (type == ItemWidget) {
            if (chosenRow >= 0) {
                if (chosenView == currentView && chosenRow < viewIndex)
                    viewIndex--;
                chosenView->model()->removeRows(chosenRow, 1);
            }
            chosenRowFlag = false;
        } else if (QLayout *layout = findLayout(widget)) {
            switch (type) {
            case TabsWidget:
                for (int i = 0, j = ((QTabWidget *)widget)->count(); i < j;
                     i++) {
                    QWidget *page = ((QTabWidget *)widget)->widget(i);

                    while (isWidgetOnPage(page, currentTabsWidget))
                        endTabs();
                    if (currentTabsWidget == widget)
                        endTabs();
                    if (isWidgetOnPage(page, currentListWidget))
                        endList();
                    if (isWidgetOnPage(page, defaultPushButton))
                        defaultPushButton = nullptr;
                    if (isLayoutOnPage(page, currentLayout)) {
                        // Position focus behind the parent QTabWidget
                        endGroup();
                        QObject *parent = layout->parent();
                        if (parent->isWidgetType()) {
                            groupLayout = (QBoxLayout *)layout;
                            groupIndex = layout->indexOf(widget);
                            currentLayout = (QBoxLayout *)findLayout(
                                    (QWidget *)parent);
                            currentIndex = currentLayout->indexOf(
                                    (QWidget *)parent) + 1;
                        } else {
                            currentLayout = (QBoxLayout *)layout;
                            currentIndex = layout->indexOf(widget);
                        }
                    }
                }
                break;
            case FrameWidget:
            case GroupBoxWidget:
                while (isWidgetOnContainer(widget, currentTabsWidget))
                    endTabs();
                if (isWidgetOnContainer(widget, currentListWidget))
                    endList();
                if (isWidgetOnContainer(widget, defaultPushButton))
                    defaultPushButton = nullptr;
                if (isLayoutOnContainer(widget, currentLayout)) {
                    endGroup();
                    currentLayout = (QBoxLayout *)layout;
                    currentIndex = layout->indexOf(widget);
                }
                if (groupLayout && widget->layout() == groupLayout)
                    endGroup();
                break;
            case ListBoxWidget:
            case ComboBoxWidget:
                if (currentView && chosenView == currentView)
                    endList();
                break;
            case PushButtonWidget:
                if ((QPushButton *)widget == defaultPushButton)
                    defaultPushButton = nullptr;
                break;
            }

            if (layout == groupLayout && layout->indexOf(widget) < groupIndex)
                groupIndex--;
            if (layout == currentLayout
                && layout->indexOf(widget) < currentIndex) {
                currentIndex--;
            }

            if (QWidget *proxywidget = widget->focusProxy()) {
                // For joint widgets
                layout->removeWidget(proxywidget);
                delete proxywidget;
            }

            layout->removeWidget(widget);
            delete widget;  // This also deletes child layouts and widgets
            // (parented to parentWidget by addWidget and addLayout) and so
            // forth. For QTabWidget it deletes child pages which triggers
            // removePage slot.

            sanitizeLayout(layout);
        } else {
            // This is page
            while (isWidgetOnPage(widget, currentTabsWidget))
                endTabs();
            if (isWidgetOnPage(widget, currentListWidget))
                endList();
            if (isWidgetOnPage(widget, defaultPushButton))
                defaultPushButton = nullptr;

            if (isLayoutOnPage(widget, currentLayout)) {
                // position focus behind the parent QTabWidget
                endGroup();

                QWidget *tabs = (QWidget *)widget->parent()->parent();
                layout = (QBoxLayout *)findLayout(tabs);
                QObject *parent = layout->parent();

                if (parent->isWidgetType()) {
                    groupLayout = (QBoxLayout *)layout;
                    groupIndex = layout->indexOf(tabs) + 1;
                    currentLayout = (QBoxLayout *)findLayout((QWidget *)parent);
                    currentIndex = currentLayout->indexOf((QWidget *)parent) + 1;
                } else {
                    currentLayout = (QBoxLayout *)layout;
                    currentIndex = layout->indexOf(tabs) + 1;
                }
            }
            delete widget;
        }
    }
}

void ShowBox::position(char *name, bool behind, bool onto)
{
    QWidget *widget;
    QBoxLayout *layout;
    bool tabs_set = false;

    if ( (widget = findWidget(name)) ) {
        if ( !(layout = (QBoxLayout *)findLayout(widget)) ) {
            // This is page.
            // widget != this isn't tested as root page has no name.
            currentTabsWidget = (QTabWidget *)widget->parent()->parent();
            tabsIndex = currentTabsWidget->indexOf(widget);
            if (behind) {
                tabsIndex++;
                behind = false;
            }
            if (onto) {
                endGroup();
                layout = (QBoxLayout *)widget->layout();
                layout = (QBoxLayout *)layout->itemAt(layout->count() - 1)
                         ->layout();
                currentLayout = (QBoxLayout *)layout
                                ->itemAt(layout->count() - 1)->layout();
                currentIndex = currentLayout->count();
                return;
            }
            widget = currentTabsWidget;
            layout = (QBoxLayout *)findLayout(widget);
            tabs_set = true;
        }

        if (layout) {
            QWidget *page;
            QObject *parent;
            int index;
            int type = widgetType(widget);

            if (widget->focusProxy() && !(type & TabsWidget)) {
                // Joint widget
                parent = layout->parent()->parent();
                index = indexOf(layout);
                layout = (QBoxLayout *)layout->parent();
            } else {
                parent = layout->parent();
                index = layout->indexOf(widget);
            }

            if (onto && type & (ListBoxWidget | ComboBoxWidget)) {
                currentView = chosenView;
                currentListWidget = chosenListWidget;
                viewIndex = currentView->model()->rowCount();
            }
            if (type & ItemWidget) {
                currentView = chosenView;
                currentListWidget = chosenListWidget;
                viewIndex = chosenRow >= 0 ? chosenRow
                            : currentView->model()->rowCount();
                if (behind) {
                    viewIndex++;
                    behind = false;
                }
            }

            if (parent->isWidgetType()) {
                // The widget is installed onto QGroupBox/QFrame
                page = ((QWidget *)parent)->parentWidget();

                groupLayout = layout;
                groupIndex = index;
                if (behind)
                    groupIndex++;

                layout = (QBoxLayout *)findLayout((QWidget *)parent);
                if (layout) {
                    currentLayout = layout;
                    currentIndex = layout->indexOf((QWidget *)parent) + 1;
                }
            } else {
                page = widget->parentWidget();
                endGroup();

                currentLayout = layout;
                currentIndex = index;
                if (behind)
                    currentIndex++;

                if (onto && (layout = (QBoxLayout *)widget->layout())) {
                    // Position onto QGroupBox/QFrame object
                    groupLayout = layout;
                    groupIndex = layout->count();
                }
            }

            if (!tabs_set) {
                if (onto && type & TabsWidget) {
                    currentTabsWidget = (QTabWidget *)widget;
                    tabsIndex = ((QTabWidget *)widget)->count();
                } else {
                    if (page == this) {
                        currentTabsWidget = nullptr;
                    } else {
                        currentTabsWidget = (QTabWidget *)page->parent()
                                            ->parent();
                        tabsIndex = currentTabsWidget->indexOf(page);
                    }
                }
            }
        }
    }
}

void ShowBox::setEnabled(QWidget *widget, bool enable)
{
    switch ((unsigned)widgetType(widget)) {
    case PageWidget: {
        QTabWidget *tabs = (QTabWidget *)widget->parent()->parent();
        tabs->setTabEnabled(tabs->indexOf(widget), enable);
        break;
    }
    default:
        widget->setEnabled(enable);
        if (QWidget *proxywidget = widget->focusProxy())
            proxywidget->setEnabled(enable);
    }
}

/*******************************************************************************
 *  findWidget searches for the widget with the given name. It does some
 *  analysis of the name to recognize list item references (#number and :text)
 *  and sets internal pointer to the QAbstractItemView and to that item.
 ******************************************************************************/
QWidget *ShowBox::findWidget(char *name)
{
    QWidget *widget = nullptr;

    chosenView = nullptr;
    chosenRowFlag = false;

    if (name) {
        int li_row = -1;
        char *li_name = nullptr;

        for (int i = 0; name[i] != 0; i++) {
            if (name[i] == '#') {
                name[i] = 0;
                if (name[i + 1])
                    sscanf(name + i + 1, "%d", &li_row);
                break;
            }
            if (name[i] == ':') {
                name[i] = 0;
                li_name = name + i + 1;
                break;
            }
        }

        if (name[0])
            for (int i = 0, j = pages.count(); i < j; i++) {
                widget = pages.at(i);
                if (!strcmp(widget->objectName().toLocal8Bit().constData(),
                            name)) {
                    // The widget is page
                    break;
                }
                if ( (widget = findWidgetRecursively(widget->layout(), name)) )
                    break;
            }

        switch ((unsigned)widgetType(widget)) {
        case ListBoxWidget:
            chosenListWidget = widget->focusProxy();
            chosenView = (ListBox *)chosenListWidget;
            break;
        case ComboBoxWidget:
            chosenListWidget = widget->focusProxy();
            chosenView = ((QComboBox *)chosenListWidget)->view();
            break;
        }
        if (chosenView) {
            if (li_row >= 0) {
                chosenRow = li_row;
                chosenRowFlag = true;
            }
            if (li_name) {
                QAbstractItemModel *model = chosenView->model();
                int i;
                int j;
                for (i = 0, j = model->rowCount(); i < j; i++) {
                    if (!strcmp(model->data(model->index(i, 0), Qt::DisplayRole)
                                .toString().toLocal8Bit().constData(),
                                li_name)) {
                        chosenRow = i;
                        chosenRowFlag = true;
                        break;
                    }
                }
                if (i == j) {
                    chosenRow = i;
                    chosenRowFlag = true;
                }
            }
        }
    }

    return widget;
}

/*******************************************************************************
 *  findLayout searches for the layout the given widget is laid on.
 ******************************************************************************/
QLayout *ShowBox::findLayout(QWidget *widget)
{
    QLayout *layout = nullptr;
    QWidget *page;

    for (int i = 0, j = pages.count(); i < j; i++) {
        if ( (page = pages.at(i)) == widget ) {
            // The widget is page
            break;
        }
        if ( (layout = findLayoutRecursively(page->layout(), widget)) )
            break;
    }

    return layout;
}

/*******************************************************************************
 *  Duplicate of void QDialogPrivate::hideDefault() which is made private.
 *  Is called by listbox widget with activation option set when it gets focus.
 ******************************************************************************/
void ShowBox::holdDefaultPushButton()
{
    QList<QPushButton *> list = findChildren<QPushButton *>();
    for (int i = 0; i < list.size(); i++)
        list.at(i)->setDefault(false);
}

/*******************************************************************************
 *  Is called by listbox widget with activation option set when it loses focus.
 *  Unfortunately QDialog keeps all properties/methods for default/autodefault
 *  pushbuttons management private. Don't see a graceful way to restore default
 *  indicator for an autodefault pushbutton...
 ******************************************************************************/
void ShowBox::unholdDefaultPushButton()
{
    if (defaultPushButton)
        defaultPushButton->setDefault(true);
}

/*******************************************************************************
 *  widgetType returns the type of the widget as value of
 *  DialogCommandTokens::Control enum
 ******************************************************************************/
DialogCommandTokens::Control ShowBox::widgetType(QWidget *widget)
{
    if (widget) {
        if (qobject_cast<QCalendarWidget *>(widget))
        return CalendarWidget;
        if (qobject_cast<CustomTableWidget *>(widget))
        return TableWidget;
        if (qobject_cast<CustomChartWidget *>(widget))
        return ChartWidget;

    const char *name = widget->metaObject()->className();

        if (!strcmp(name, "ShowBox"))
            return DialogWidget;
        if (!strcmp(name, "QPushButton"))
            return PushButtonWidget;
        if (!strcmp(name, "QRadioButton"))
            return RadioButtonWidget;
        if (!strcmp(name, "QCheckBox"))
            return CheckBoxWidget;
        if (!strcmp(name, "QLabel")) {
            if (QWidget *proxywidget = widget->focusProxy()) {
                const char *proxyname = proxywidget->metaObject()->className();

                if (!strcmp(proxyname, "QLineEdit"))
                    return TextBoxWidget;
                if (!strcmp(proxyname, "QComboBox")) {
                    if (chosenRowFlag)
                        return ItemWidget;
                    return ComboBoxWidget;
                }
                if (!strcmp(proxyname, "ListBox")) {
                    if (chosenRowFlag)
                        return ItemWidget;
                    return ListBoxWidget;
                }
            }
            return LabelWidget;
        }
        if (!strcmp(name, "QGroupBox"))
            return GroupBoxWidget;
        if (!strcmp(name, "QFrame")) {
            int shape = ((QFrame *)widget)->frameStyle() & QFrame::Shape_Mask;

            if (shape == QFrame::HLine || shape == QFrame::VLine)
                return SeparatorWidget;
            return FrameWidget;
        }
        if (!strcmp(name, "QLineEdit")) {
            // If the object queried directly
            return TextBoxWidget;
        }
        if (!strcmp(name, "ListBox")) {
            // If the object queried directly
            if (chosenRowFlag)
                return ItemWidget;
            return ListBoxWidget;
        }
        if (!strcmp(name, "QComboBox")) {
            // If the object queried directly
            if (chosenRowFlag)
                return ItemWidget;
            return ComboBoxWidget;
        }

        if (!strcmp(name, "QTabWidget"))
            return TabsWidget;
        if (!strcmp(name, "QWidget"))
            return PageWidget;

        if (!strcmp(name, "QProgressBar"))
            return ProgressBarWidget;
        if (!strcmp(name, "QSlider"))
            return SliderWidget;
        if (!strcmp(name, "QTextEdit"))
            return TextViewWidget;
    }

    return NoneWidget;
}

/*******************************************************************************
 *
 *  ListBox class is introduced to prevent Enter key hit propagation to default
 *  pushbutton when listbox activation option is on or combobox widget is
 *  editable. In other words to prevent two controls to respond to a single key.
 *
 ******************************************************************************/

void ListBox::focusInEvent(QFocusEvent *event)
{
    if (activateFlag) {
        ShowBox *dialog = (ShowBox *)window();
        if (dialog)
            dialog->holdDefaultPushButton();
    }
    QListWidget::focusInEvent(event);
}

void ListBox::focusOutEvent(QFocusEvent *event)
{
    if (activateFlag) {
        ShowBox *dialog = (ShowBox *)window();
        if (dialog)
            dialog->unholdDefaultPushButton();
    }
    QListWidget::focusOutEvent(event);
}

void ListBox::setActivateFlag(bool flag)
{
    ShowBox *dialog;

    if (activateFlag != flag && hasFocus()
        && (dialog = (ShowBox *)window())) {
        if (flag)
            dialog->holdDefaultPushButton();
        else
            dialog->unholdDefaultPushButton();
    }
    activateFlag = flag;
}

/*******************************************************************************
 *
 *  NON-CLASS MEMBER FUNCTIONS
 *
 ******************************************************************************/

QWidget *findWidgetRecursively(QLayoutItem *item, const char *name)
{
    QLayout *layout;
    QWidget *widget;

    if ((layout = item->layout())) {
        for (int i = 0, j = layout->count(); i < j; i++)
            if ((widget = findWidgetRecursively(layout->itemAt(i), name)))
                return widget;
    } else if ((widget = item->widget())) {
        if (!strcmp(widget->objectName().toLocal8Bit().constData(), name))
            return widget;
        if ((layout = widget->layout()))
            return findWidgetRecursively(layout, name);
    }
    return nullptr;
}

QLayout *findLayoutRecursively(QLayout *layout, QWidget *widget)
{
    if (widget && layout) {
        for (int i = 0, j = layout->count(); i < j; i++) {
            QLayoutItem *item = layout->itemAt(i);
            QWidget *w = item->widget();
            QLayout *retlayout;

            if (w == widget) {
                return layout;
            } else {
                retlayout = findLayoutRecursively(w ? w->layout()
                                                  : item->layout(), widget);
                if (retlayout)
                    return retlayout;
            }
        }
    }
    return nullptr;
}
