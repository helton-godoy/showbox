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

#ifndef SHOWBOX_H_
#define SHOWBOX_H_

#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#define DEFAULT_ALIGNMENT Qt::Alignment(0)
#define GRAPHICS_ALIGNMENT Qt::AlignCenter
#define TEXT_ALIGNMENT Qt::AlignTop | Qt::AlignLeft
#define WIDGETS_ALIGNMENT TEXT_ALIGNMENT
#define LAYOUTS_ALIGNMENT WIDGETS_ALIGNMENT

#define BUFFER_SIZE 1024

#include "theme_manager.h"
#include <QCalendarWidget>
#include <QTabWidget>

namespace DialogCommandTokens {
// These enumerations are used as flags as well as masks thus we can't apply
// QFlags template to them.

enum Command {
  // Commands
  NoopCommand = 0x00000000,
  AddCommand = 0x00000100,
  EndCommand = 0x00000200,
  StepCommand = 0x00000400,
  SetCommand = 0x00000800,
  UnsetCommand = 0x00001000,
  RemoveCommand = 0x00002000,
  ClearCommand = 0x00004000,
  PositionCommand = 0x00008000,
  QueryCommand = 0x00010000,
  PrintCommand = 0x80000000,

  // Masks
  OptionMask = 0x0000000F,  // Property bits (4 properties max.)
  CommandMask = 0xFFFFFFF0, // Command bits (28 commands max.)

  // Options available for widgets of any type
  OptionEnabled = SetCommand | UnsetCommand | 0x00000001,
  OptionFocus = SetCommand | 0x00000002,
  OptionStyleSheet = SetCommand | UnsetCommand | 0x00000004,
  OptionVisible = SetCommand | UnsetCommand | 0x00000008,

  // Options specific for some commands (say, sub-commands)
  OptionVertical = StepCommand | 0x00000001,
  OptionBehind = PositionCommand | 0x00000001,
  OptionOnto = PositionCommand | 0x00000002,
  OptionSpace = AddCommand | 0x00000001,
  OptionStretch = AddCommand | 0x00000002
};

enum Control {
  NoneWidget = 0x00000000,

  // Widgets
  DialogWidget = 0x80000000, // Main window
  FrameWidget = 0x40000000,
  SeparatorWidget = 0x20000000,
  LabelWidget = 0x10000000,
  GroupBoxWidget = 0x08000000,
  PushButtonWidget = 0x04000000,
  RadioButtonWidget = 0x02000000,
  CheckBoxWidget = 0x01000000,
  TextBoxWidget = 0x00800000,
  ListBoxWidget = 0x00400000,
  ComboBoxWidget = 0x00200000,
  ItemWidget = 0x00100000,
  ProgressBarWidget = 0x00080000,
  SliderWidget = 0x00040000,
  TextViewWidget = 0x00020000,
  TabsWidget = 0x00010000,
  PageWidget = 0x00008000,
  CalendarWidget = 0x00004000,
  TableWidget = 0x00002000,
  ChartWidget = 0x00001000,

  // Masks
  PropertyMask = 0x000001FF, // Property bits (9 properties max.)
  WidgetMask = 0xFFFFFE00,   // All widgets listed above (23 types max.)
  CaptionWidgetsMask = WidgetMask ^ FrameWidget ^ SeparatorWidget ^
                       ProgressBarWidget ^ SliderWidget ^ TextViewWidget ^
                       TabsWidget ^ TableWidget,

  // Properties specific for particular widget types
  PropertyTitle = CaptionWidgetsMask | 0x00000001,
  PropertyText = CaptionWidgetsMask | 0x00000002,
  PropertyIcon = DialogWidget | ItemWidget | PageWidget | PushButtonWidget |
                 RadioButtonWidget | CheckBoxWidget | 0x00000004,
  PropertyChecked = GroupBoxWidget | PushButtonWidget | RadioButtonWidget |
                    CheckBoxWidget | 0x00000008,
  PropertyCheckable = GroupBoxWidget | PushButtonWidget | RadioButtonWidget |
                      CheckBoxWidget | 0x00000010,
  PropertyIconSize = TabsWidget | ListBoxWidget | ComboBoxWidget |
                     PushButtonWidget | RadioButtonWidget | CheckBoxWidget |
                     0x00000020,
  PropertyVertical = GroupBoxWidget | FrameWidget | SeparatorWidget |
                     ProgressBarWidget | SliderWidget | 0x00000080,
  PropertyApply = PushButtonWidget | 0x00000040,
  PropertyExit = PushButtonWidget | 0x00000080,
  // Note: pushbutton properties exceed 8 bits
  PropertyDefault = PushButtonWidget | 0x00000100,
  PropertyPassword = TextBoxWidget | 0x00000004,
  PropertyPlaceholder = TextBoxWidget | 0x00000008,
  PropertyPicture = LabelWidget | 0x00000004,
  PropertyAnimation = LabelWidget | 0x00000008,
  PropertyPlain = FrameWidget | SeparatorWidget | 0x00000010,
  PropertyRaised = FrameWidget | SeparatorWidget | 0x00000020,
  PropertySunken = FrameWidget | SeparatorWidget | 0x00000040,
  PropertyFlat = FrameWidget | SeparatorWidget | 0x00000080,
  PropertyNoframe = FrameWidget | 0x00000001,
  PropertyBox = FrameWidget | 0x00000002,
  PropertyPanel = FrameWidget | 0x00000004,
  PropertyStyled = FrameWidget | 0x00000008,
  PropertyCurrent = ItemWidget | PageWidget | 0x00000008,
  PropertyEditable = ComboBoxWidget | 0x00000004,
  PropertySelection = ComboBoxWidget | ListBoxWidget | CalendarWidget |
                      TableWidget | 0x00000008,
  PropertyActivation = ListBoxWidget | 0x00000004,
  PropertyMinimum =
      ProgressBarWidget | SliderWidget | CalendarWidget | 0x00000001,
  PropertyMaximum =
      ProgressBarWidget | SliderWidget | CalendarWidget | 0x00000002,
  PropertyValue = ProgressBarWidget | SliderWidget | ChartWidget | 0x00000004,
  PropertyBusy = ProgressBarWidget | 0x00000008,
  PropertyFile = TextViewWidget | TableWidget | ChartWidget | 0x00000004,
  PropertyDate = CalendarWidget | 0x00000001,
  PropertyFormat = CalendarWidget | 0x00000002,
  PropertyNavigation = CalendarWidget | 0x00000004,
  PropertyPositionTop = TabsWidget | 0x00000001,
  PropertyPositionBottom = TabsWidget | 0x00000002,
  PropertyPositionLeft = TabsWidget | 0x00000004,
  PropertyPositionRight = TabsWidget | 0x00000008,
  PropertyReadOnly = TableWidget | 0x00000010,
  PropertyHeaders = TableWidget | 0x00000020,
  PropertyAddLine = TableWidget | 0x00000040,
  PropertyAppend = ChartWidget | 0x00000040,
  PropertyAxis = ChartWidget | 0x00000080,
  PropertyDelLine = TableWidget | 0x00000080,
  PropertyExport = ChartWidget | 0x00000020,
  PropertySearch = TableWidget | 0x00000100,
};

} // namespace DialogCommandTokens

struct DialogCommand {
  DialogCommand() {
    buffer[BUFFER_SIZE - 1] = '\0';
    title = name = text = auxtext = BUFFER_SIZE - 1;
  }

  char *getTitle() { return buffer + title; }
  char *getName() { return buffer + name; }
  char *getText() { return buffer + text; }
  char *getAuxText() { return buffer + auxtext; }

  char buffer[BUFFER_SIZE];
  size_t title;
  size_t name;
  size_t text;
  size_t auxtext;

  unsigned int command;
  unsigned int control;
};

class ShowBox : public QDialog {
  Q_OBJECT

public:
  enum ContentType {
    TextContent,
    PixmapContent,
    MovieContent,
  };

  ShowBox(const char *title, const char *about = nullptr,
          bool resizable = false, FILE *out = stdout);
  virtual ~ShowBox() = default;

  void setThemeManager(ThemeManager *manager);
  void applyTheme(ThemeManager::ThemeMode mode);

  virtual void addPushButton(const char *title, const char *name,
                             bool apply = false, bool exit = false,
                             bool def = false);
  virtual void addCheckBox(const char *title, const char *name,
                           bool checked = false);
  virtual void addRadioButton(const char *title, const char *name,
                              bool checked = false);
  virtual void addTextBox(const char *title, const char *name,
                          const char *text = nullptr,
                          const char *placeholder = nullptr,
                          bool password = false);
  virtual void addLabel(const char *title, const char *name = nullptr,
                        enum ContentType content = TextContent);
  virtual void addGroupBox(const char *title, const char *name,
                           bool vertical = true, bool checkable = false,
                           bool checked = false);
  virtual void addFrame(const char *name, bool vertical = true,
                        unsigned int style = 0);

  void endGroup() { groupLayout = nullptr; }

  virtual void addListBox(const char *title, const char *name,
                          bool activation = false, bool selection = false);
  virtual void addComboBox(const char *title, const char *name,
                           bool editable = false, bool selection = false);
  virtual void addItem(const char *title, const char *icon = nullptr,
                       bool current = false);

  virtual void endList() {
    currentView = nullptr;
    currentListWidget = nullptr;
  }

  virtual void addStretch(int stretch = 1) {
    if (groupLayout)
      groupLayout->insertStretch(groupIndex++, stretch);
    else
      currentLayout->insertStretch(currentIndex++, stretch);
  }

  virtual void addSpace(int space = 1) {
    if (groupLayout)
      groupLayout->insertSpacing(groupIndex++, space);
    else
      currentLayout->insertSpacing(currentIndex++, space);
  }

  virtual void addSeparator(const char *name = nullptr, bool vertical = false,
                            unsigned int style = 0);
  virtual void addProgressBar(const char *name, bool vertical = false,
                              bool busy = false);
  virtual void addSlider(const char *name, bool vertical = false, int min = 0,
                         int max = 100);
  virtual void addTextView(const char *name, const char *file = nullptr);
  virtual void addTabs(const char *name, unsigned int position = 0);
  virtual void addPage(const char *title, const char *name,
                       const char *icon = nullptr, bool current = false);
  virtual void addCalendar(const char *title, const char *name,
                           const char *date = nullptr,
                           const char *min = nullptr, const char *max = nullptr,
                           const char *format = nullptr,
                           bool selection = false);
  virtual void addTable(const char *headers, const char *name,
                        const char *file = nullptr, bool readonly = false,
                        bool selection = false, bool search = false);
  virtual void addChart(const char *title, const char *name);
  virtual void endPage();
  virtual void endTabs();

  virtual void stepHorizontal();
  virtual void stepVertical();

  virtual void clear(char *name);
  virtual void clearChosenList(); // Clears the list chosen by findWidget()
  virtual void clearTabs(QTabWidget *);
  virtual void clearPage(QWidget *);
  virtual void clearDialog();

  virtual void removeWidget(char *name);
  virtual void position(char *name, bool behind = false, bool onto = false);

  virtual void setEnabled(QWidget *widget, bool enable);
  virtual void setOptions(QWidget *widget, unsigned int options,
                          unsigned int mask, const char *text);

  QWidget *findWidget(char *name);
  QLayout *findLayout(QWidget *widget);

  bool isLayoutOnPage(QWidget *page, QLayout *layout);
  bool isWidgetOnPage(QWidget *page, QWidget *widget);
  bool isLayoutOnContainer(QWidget *container, QLayout *layout);
  bool isWidgetOnContainer(QWidget *container, QWidget *widget);

  DialogCommandTokens::Control widgetType(QWidget *);

  void holdDefaultPushButton();
  void unholdDefaultPushButton();

public slots:
  void executeCommand(DialogCommand);
  void report();
  void done(int);

private slots:
  void pushButtonClicked();
  void pushButtonToggled(bool);
  void listBoxItemActivated(const QModelIndex &);
  void listBoxItemSelected(QListWidgetItem *);
  void comboBoxItemSelected(int);
  void tableCellEdited(int row, int col, const QString &text);
  void tableRowSelected(int row);
  void chartItemClicked(const QString &label);
  void calendarSelected();
  void sliderValueChanged(int);
  void sliderRangeChanged(int, int);
  void removePage(QObject *);

private:
  ThemeManager *m_themeManager = nullptr;
  QPushButton *defaultPushButton;

  QList<QWidget *> pages;

  QBoxLayout *currentLayout;
  int currentIndex;

  QBoxLayout *groupLayout;
  int groupIndex;

  QAbstractItemView *currentView; // The list items are added to
  int viewIndex;
  QWidget *currentListWidget;

  QAbstractItemView *chosenView; // The list modifications are made on (set
                                 // by the findWidget)
  int chosenRow;                 // The list item modifications are made on
  bool chosenRowFlag; // Flag that indicates the list item was set by the
                      // findWidget
  QWidget *chosenListWidget;

  QTabWidget *currentTabsWidget; // The pages are added to
  int tabsIndex;

  FILE *output;

  bool empty;

  void updateTabsOrder(QWidget *page = nullptr);
  void sanitizeLabel(QWidget *label, enum ContentType content);

  bool removeIfEmpty(QLayout *);
  bool isEmpty(QLayout *);
  void sanitizeLayout(QLayout *);

  void printWidgetsRecursively(QLayoutItem *);
  void printWidget(QWidget *);
};

class DialogParser : public QThread, private DialogCommand {
  Q_OBJECT

public:
  DialogParser(ShowBox *parent = 0, FILE *in = stdin);
  ~DialogParser();

  void setParent(ShowBox *parent);

signals:
  void sendCommand(DialogCommand);

protected:
  virtual void run();

private:
  enum Stage {
    StageCommand = 0x00000001,
    StageType = 0x00000002,
    StageTitle = 0x00000004,
    StageName = 0x00000008,
    StageText = 0x00000010,
    StageAuxText = 0x00000020,
    StageOptions = 0x00000040
  };

  void processToken();
  void issueCommand();

  ShowBox *dialog;
  FILE *input;
  unsigned int stage;
  size_t token;
  size_t bufferIndex;
};

//  Below class is the workaround to address QListWidget limitation:
//    the widget reports current item as "activated" (emits the signal) on the
//    Enter key press event but propagates this event to next widgets.
//    The default/autodefault pushbutton might respond to it even by closing
//    the dialog.
class ListBox : public QListWidget {
  Q_OBJECT

public:
  ListBox() = default;

  void setActivateFlag(bool flag);
  bool getActivateFlag() { return activateFlag; };

protected:
  void focusInEvent(QFocusEvent *event) final;
  void focusOutEvent(QFocusEvent *event) final;

private:
  bool activateFlag = false;
};

//  NON-CLASS MEMBERS
QWidget *findWidgetRecursively(QLayoutItem *item, const char *name);
QLayout *findLayoutRecursively(QLayout *layout, QWidget *widget);

#endif // SHOWBOX_H_
