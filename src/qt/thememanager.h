// Copyright (c) 2025-present The DRIP developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_THEMEMANAGER_H
#define BITCOIN_QT_THEMEMANAGER_H

#include <QObject>
#include <QString>
#include <QStringList>

class QApplication;

/**
 * ThemeManager handles loading and switching between UI themes.
 * Supports dark and light themes with smooth transitions.
 */
class ThemeManager : public QObject
{
    Q_OBJECT

public:
    enum Theme {
        Dark,
        Light,
        System  // Follow system preference
    };
    Q_ENUM(Theme)

    static ThemeManager& instance();

    /** Initialize the theme manager and load saved preference */
    void initialize();

    /** Get the current active theme */
    Theme currentTheme() const { return m_currentTheme; }

    /** Get available theme names for UI display */
    static QStringList availableThemes();

    /** Convert theme enum to display string */
    static QString themeToString(Theme theme);

    /** Convert string to theme enum */
    static Theme stringToTheme(const QString& str);

    /** Check if system is in dark mode */
    static bool isSystemDarkMode();

public Q_SLOTS:
    /** Switch to a different theme */
    void setTheme(Theme theme);

    /** Reload the current theme (useful after style changes) */
    void reloadTheme();

Q_SIGNALS:
    /** Emitted when theme changes */
    void themeChanged(Theme newTheme);

private:
    ThemeManager();
    ~ThemeManager() = default;

    /** Load and apply stylesheet for given theme */
    void applyStylesheet(Theme theme);

    /** Get the stylesheet content for a theme */
    QString loadStylesheet(Theme theme) const;

    /** Get the effective theme (resolves System to actual dark/light) */
    Theme effectiveTheme() const;

    Theme m_currentTheme{Theme::Dark};
    
    // Prevent copying
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;
};

#endif // BITCOIN_QT_THEMEMANAGER_H

