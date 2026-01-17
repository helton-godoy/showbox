#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QScrollArea>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTabWidget>
#include "ShowboxBuilder.h"
#include "WidgetConfigs.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ShowboxBuilder builder;

    // 1. Main Window
    Showbox::Models::WindowConfig winConfig;
    winConfig.title = "Showbox Visual Parity Verifier (Phases 1-3)";
    winConfig.width = 600;
    winConfig.height = 800;
    QWidget* mainWindow = builder.buildWindow(winConfig);
    QVBoxLayout* mainLayout = qobject_cast<QVBoxLayout*>(mainWindow->layout());

    // Scroll Area for content
    QScrollArea* scroll = new QScrollArea();
    QWidget* content = new QWidget();
    QVBoxLayout* contentLayout = new QVBoxLayout(content);
    scroll->setWidget(content);
    scroll->setWidgetResizable(true);
    mainLayout->addWidget(scroll);

    // --- Phase 1: Basic Widgets ---
    QGroupBox* gbBasic = new QGroupBox("Phase 1: Basic Widgets");
    QVBoxLayout* layBasic = new QVBoxLayout(gbBasic);
    
    Showbox::Models::LabelConfig lbl;
    lbl.text = "This is a Label created by ShowboxBuilder";
    layBasic->addWidget(builder.buildLabel(lbl));

    Showbox::Models::ButtonConfig btn;
    btn.text = "Standard Button";
    layBasic->addWidget(builder.buildButton(btn));

    Showbox::Models::LineEditConfig le;
    le.placeholder = "Type something here...";
    layBasic->addWidget(builder.buildLineEdit(le));

    contentLayout->addWidget(gbBasic);

    // --- Phase 2: Selection & Utility ---
    QGroupBox* gbSelect = new QGroupBox("Phase 2: Selection & Utility");
    QVBoxLayout* laySelect = new QVBoxLayout(gbSelect);

    Showbox::Models::CheckBoxConfig cb;
    cb.text = "Enable Turbo Mode (CheckBox)";
    cb.checked = true;
    laySelect->addWidget(builder.buildCheckBox(cb));

    Showbox::Models::RadioButtonConfig rb1;
    rb1.text = "Option A (RadioButton)";
    rb1.checked = true;
    laySelect->addWidget(builder.buildRadioButton(rb1));

    Showbox::Models::RadioButtonConfig rb2;
    rb2.text = "Option B (RadioButton)";
    laySelect->addWidget(builder.buildRadioButton(rb2));

    Showbox::Models::SeparatorConfig sep;
    sep.orientation = Qt::Horizontal;
    laySelect->addWidget(builder.buildSeparator(sep));

    Showbox::Models::CalendarConfig cal;
    laySelect->addWidget(builder.buildCalendar(cal));

    contentLayout->addWidget(gbSelect);

    // --- Phase 3: Numerical & Text ---
    QGroupBox* gbInput = new QGroupBox("Phase 3: Numerical & Text");
    QVBoxLayout* layInput = new QVBoxLayout(gbInput);

    Showbox::Models::SpinBoxConfig sb;
    sb.min = 0;
    sb.max = 100;
    sb.value = 42;
    sb.suffix = " %";
    layInput->addWidget(builder.buildSpinBox(sb));

    Showbox::Models::SliderConfig sl;
    sl.orientation = Qt::Horizontal;
    sl.min = 0;
    sl.max = 100;
    sl.value = 75;
    layInput->addWidget(builder.buildSlider(sl));

    Showbox::Models::TextEditConfig txt;
    txt.text = "<b>Rich Text</b> supported.<br><ul><li>Item 1</li><li>Item 2</li></ul>";
    txt.richText = true;
    layInput->addWidget(builder.buildTextEdit(txt));

    contentLayout->addWidget(gbInput);

    // --- Phase 4: Containers & Tabs ---
    QGroupBox* gbContainers = new QGroupBox("Phase 4: Containers & Tabs");
    QVBoxLayout* layContainers = new QVBoxLayout(gbContainers);

    // Frame
    Showbox::Models::FrameConfig fr;
    fr.layout.type = Showbox::Models::LayoutConfig::HBox;
    QWidget* frame = builder.buildFrame(fr);
    // Add some content to frame manually for demo since config doesn't list children
    if (frame->layout()) {
        frame->layout()->addWidget(new QLabel("Inside Frame (HBox)"));
        frame->layout()->addWidget(new QPushButton("Frame Button"));
    }
    layContainers->addWidget(frame);

    // Tab Widget
    Showbox::Models::TabWidgetConfig tabs;
    
    Showbox::Models::PageConfig p1;
    p1.title = "Page 1";
    p1.layout.type = Showbox::Models::LayoutConfig::VBox;
    
    Showbox::Models::PageConfig p2;
    p2.title = "Page 2";
    p2.layout.type = Showbox::Models::LayoutConfig::VBox;

    tabs.pages << p1 << p2;

    QWidget* tw = builder.buildTabWidget(tabs);
    // Populate tabs manually for visual verification
    QTabWidget* realTab = qobject_cast<QTabWidget*>(tw);
    if (realTab && realTab->count() >= 2) {
        if (realTab->widget(0)->layout())
            realTab->widget(0)->layout()->addWidget(new QLabel("Content of Page 1"));
        if (realTab->widget(1)->layout())
            realTab->widget(1)->layout()->addWidget(new QPushButton("Button on Page 2"));
    }

    layContainers->addWidget(tw);
    contentLayout->addWidget(gbContainers);

    // Show
    mainWindow->show();

    return app.exec();
}
