#include "theme.h"

#include <QApplication>
#include <QGuiApplication>
#include <QPalette>
#include <QStyleHints>

QColor Theme::workspaceColor;
QColor Theme::mutedColor;
QColor Theme::accentColor;
QColor Theme::guideColor;
QColor Theme::guide() { return guideColor; }
QColor Theme::workspace() { return workspaceColor; }
QColor Theme::mutedText() { return mutedColor; }
QColor Theme::accent() { return accentColor; }

// true black interface, panels are black and only the accent color is bright
void Theme::applyAmoled()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
    // makes the windows title bar dark too
    QGuiApplication::styleHints()->setColorScheme(Qt::ColorScheme::Dark);
#endif
    QApplication::setStyle("Fusion");

    const QColor black(0, 0, 0);
    const QColor text(230, 230, 230);
    const QColor dimText(110, 110, 110);

    workspaceColor = QColor(18, 18, 18);
    mutedColor = QColor(170, 170, 170);
    accentColor = QColor(74, 140, 255);
    guideColor = QColor(255, 64, 200);
    QPalette p;
    p.setColor(QPalette::Window, black);
    p.setColor(QPalette::WindowText, text);
    p.setColor(QPalette::Base, QColor(18, 18, 18));            // text fields, lists
    p.setColor(QPalette::AlternateBase, QColor(10, 10, 10));
    p.setColor(QPalette::Text, text);
    p.setColor(QPalette::Button, QColor(24, 24, 24));
    p.setColor(QPalette::ButtonText, text);
    p.setColor(QPalette::BrightText, Qt::white);
    p.setColor(QPalette::ToolTipBase, QColor(24, 24, 24));
    p.setColor(QPalette::ToolTipText, text);
    p.setColor(QPalette::PlaceholderText, dimText);
    p.setColor(QPalette::Highlight, accentColor);
    p.setColor(QPalette::HighlightedText, Qt::white);
    p.setColor(QPalette::Link, accentColor);

    // borders and hover, the stylesheets in the app use mid and midlight
    p.setColor(QPalette::Light, QColor(50, 50, 50));
    p.setColor(QPalette::Midlight, QColor(38, 38, 38));
    p.setColor(QPalette::Mid, QColor(70, 70, 70));
    p.setColor(QPalette::Dark, QColor(30, 30, 30));
    p.setColor(QPalette::Shadow, black);

    // greyed out widgets
    p.setColor(QPalette::Disabled, QPalette::WindowText, dimText);
    p.setColor(QPalette::Disabled, QPalette::Text, dimText);
    p.setColor(QPalette::Disabled, QPalette::ButtonText, dimText);
    p.setColor(QPalette::Disabled, QPalette::Highlight, QColor(50, 50, 50));
    p.setColor(QPalette::Disabled, QPalette::HighlightedText, dimText);

    QApplication::setPalette(p);
}