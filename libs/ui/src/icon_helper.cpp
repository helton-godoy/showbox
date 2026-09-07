#include "icon_helper.h"
#include <QFile>
#include <QDebug>
#include <QApplication>
#include "logger.h"

// Initialize fallback theme on first use
static bool s_themeInitialized = false;

static void initializeIconTheme() {
    if (s_themeInitialized) return;
    s_themeInitialized = true;
    
    // Set fallback theme paths and name if no theme is set
    if (QIcon::themeName().isEmpty()) {
        // Try common icon themes in order of preference
        QStringList themes = {"Adwaita", "breeze", "hicolor", "oxygen"};
        for (const auto &theme : themes) {
            QIcon::setThemeName(theme);
            // Test if theme works by trying a common icon
            if (!QIcon::fromTheme("document-new").isNull()) {
                qCDebug(guiLog) << "Using icon theme:" << theme;
                return;
            }
        }
        // Fallback to hicolor which should always exist
        QIcon::setThemeName("hicolor");
        qCDebug(guiLog) << "Fallback to hicolor icon theme";
    }
}

QIcon IconHelper::loadIcon(const QString &iconSource)
{
    if (iconSource.isEmpty()) {
        return QIcon();
    }

    // Try as local file first
    if (QFile::exists(iconSource)) {
        qCDebug(guiLog) << "Loading icon from file:" << iconSource;
        return QIcon(iconSource);
    }

    // Initialize theme if needed
    initializeIconTheme();

    // Try from system theme
    QIcon themeIcon = QIcon::fromTheme(iconSource);
    
    if (!themeIcon.isNull()) {
        qCDebug(guiLog) << "Loaded icon from theme:" << iconSource;
        return themeIcon;
    }

    // Try with fallback icon
    QIcon fallbackIcon = QIcon::fromTheme(iconSource, QIcon::fromTheme("application-x-executable"));
    if (!fallbackIcon.isNull()) {
        qCDebug(guiLog) << "Using fallback icon for:" << iconSource;
        return fallbackIcon;
    }

    qCWarning(guiLog) << "Could not find icon:" << iconSource;
    return QIcon();
}

