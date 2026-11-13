#include "uiutils.h"

#include <QPushButton>

void styleColorButton(QPushButton *button, const QColor &color)
{
    QString textColor = color.lightness() > 128 ? "black" : "white";
    button->setText(color.name().toUpper());
    button->setStyleSheet(QString(
                              "QPushButton { background: %1; color: %2; border: 1px solid gray; padding: 4px; } "
                              "QPushButton:disabled { background: palette(window); color: palette(mid); border: 1px solid palette(mid); }")
                              .arg(color.name(), textColor));
}