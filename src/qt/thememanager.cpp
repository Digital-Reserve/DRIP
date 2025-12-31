// Copyright (c) 2025-present The DRIP developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/thememanager.h>

#include <QApplication>
#include <QFile>
#include <QPalette>
#include <QSettings>
#include <QStyleHints>
#include <QTextStream>

ThemeManager::ThemeManager()
    : QObject(nullptr)
{
}

ThemeManager& ThemeManager::instance()
{
    static ThemeManager instance;
    return instance;
}

void ThemeManager::initialize()
{
    QSettings settings;
    QString savedTheme = settings.value("theme", "dark").toString();
    m_currentTheme = stringToTheme(savedTheme);
    applyStylesheet(m_currentTheme);
}

QStringList ThemeManager::availableThemes()
{
    return QStringList() << "Dark" << "Light" << "System";
}

QString ThemeManager::themeToString(Theme theme)
{
    switch (theme) {
    case Dark:   return "dark";
    case Light:  return "light";
    case System: return "system";
    }
    return "dark";
}

ThemeManager::Theme ThemeManager::stringToTheme(const QString& str)
{
    QString lower = str.toLower();
    if (lower == "light") return Light;
    if (lower == "system") return System;
    return Dark; // Default
}

bool ThemeManager::isSystemDarkMode()
{
    // Qt 6.5+ has colorScheme(), fallback to palette check for older versions
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    return QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
#else
    // Fallback: check if window background is dark
    QPalette palette = QApplication::palette();
    QColor bg = palette.color(QPalette::Window);
    return bg.lightness() < 128;
#endif
}

ThemeManager::Theme ThemeManager::effectiveTheme() const
{
    if (m_currentTheme == System) {
        return isSystemDarkMode() ? Dark : Light;
    }
    return m_currentTheme;
}

void ThemeManager::setTheme(Theme theme)
{
    if (m_currentTheme != theme) {
        m_currentTheme = theme;
        
        // Save preference
        QSettings settings;
        settings.setValue("theme", themeToString(theme));
        
        applyStylesheet(theme);
        Q_EMIT themeChanged(theme);
    }
}

void ThemeManager::reloadTheme()
{
    applyStylesheet(m_currentTheme);
}

QString ThemeManager::loadStylesheet(Theme theme) const
{
    Theme effective = (theme == System) ? effectiveTheme() : theme;
    QString filename = (effective == Dark) ? ":/stylesheets/drip_dark.qss" 
                                           : ":/stylesheets/drip_light.qss";
    
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        return stream.readAll();
    }
    
    // Return empty string if file not found - will use default Qt style
    return QString();
}

void ThemeManager::applyStylesheet(Theme theme)
{
    QString stylesheet = loadStylesheet(theme);
    if (qApp) {
        qApp->setStyleSheet(stylesheet);
    }
}

