#ifndef UIUTILS_H
#define UIUTILS_H

#include <QColor>

class QPushButton;

// makes a button show a color, used by every color picker button in the app
void styleColorButton(QPushButton *button, const QColor &color);

#endif // UIUTILS_H