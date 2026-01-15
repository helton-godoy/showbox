#include "theme_manager.h"
#include "logger.h"
#include <QDebug>
#include <QPalette>

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
{
    // Check for environment variable override
    QByteArray envTheme = qgetenv("SHOWBOX_THEME").toLower();
    if (envTheme == "dark") {
        m_currentTheme = Dark;
    } else if (envTheme == "light") {
        m_currentTheme = Light;
    } else {
        // Initialize current theme from system
        detectSystemTheme();
    }
    
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    // Qt 6.5+ has colorSchemeChanged signal
    connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged,
            this, &ThemeManager::onColorSchemeChanged);
#endif
}

void ThemeManager::detectSystemTheme()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    // Qt 6.5+ has colorScheme()
    Qt::ColorScheme scheme = QGuiApplication::styleHints()->colorScheme();
    switch (scheme) {
        case Qt::ColorScheme::Dark:
            m_currentTheme = Dark;
            break;
        case Qt::ColorScheme::Light:
            m_currentTheme = Light;
            break;
        default:
            m_currentTheme = Unknown;
            break;
    }
#else
    // Qt 6.4.x fallback: detect theme from palette
    QPalette palette = QGuiApplication::palette();
    QColor windowColor = palette.color(QPalette::Window);
    // If window background is dark (luminance < 128), assume dark theme
    int luminance = (windowColor.red() * 299 + windowColor.green() * 587 + windowColor.blue() * 114) / 1000;
    m_currentTheme = (luminance < 128) ? Dark : Light;
#endif
    qCDebug(guiLog) << "Theme changed to:" << (m_currentTheme == Dark ? "Dark" : (m_currentTheme == Light ? "Light" : "Unknown"));
}

ThemeManager::ThemeMode ThemeManager::currentTheme() const
{
    return m_currentTheme;
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
void ThemeManager::onColorSchemeChanged(Qt::ColorScheme scheme)
{
    ThemeMode newTheme;
    switch (scheme) {
        case Qt::ColorScheme::Dark:
            newTheme = Dark;
            break;
        case Qt::ColorScheme::Light:
            newTheme = Light;
            break;
        default:
            newTheme = Unknown;
            break;
    }
    
    if (newTheme != m_currentTheme) {
        m_currentTheme = newTheme;
        qCDebug(guiLog) << "Theme changed to:" << (m_currentTheme == Dark ? "Dark" : (m_currentTheme == Light ? "Light" : "Unknown"));
        emit themeChanged(m_currentTheme);
    }
}
#endif
