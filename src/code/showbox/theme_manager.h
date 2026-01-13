#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <QObject>
#include <QGuiApplication>
#include <QStyleHints>
#include <QtGlobal>

class ThemeManager : public QObject
{
    Q_OBJECT
public:
    explicit ThemeManager(QObject *parent = nullptr);
    
    enum ThemeMode {
        Light,
        Dark,
        Unknown
    };
    
    ThemeMode currentTheme() const;

signals:
    void themeChanged(ThemeMode newTheme);

private slots:
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    void onColorSchemeChanged(Qt::ColorScheme scheme);
#endif

private:
    ThemeMode m_currentTheme;
    void detectSystemTheme();
};

#endif // THEME_MANAGER_H
