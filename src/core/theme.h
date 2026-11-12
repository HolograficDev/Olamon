#ifndef THEME_H
#define THEME_H

#include <QColor>

// app colors, only the amoled one for now, the options dialog will pick between more later
class Theme
{
public:
    static void applyAmoled();

    // colors for stuff the palette doesn't cover
    static QColor workspace();   // the area around the page
    static QColor mutedText();
    static QColor accent();

private:
    static QColor workspaceColor;
    static QColor mutedColor;
    static QColor accentColor;
};

#endif // THEME_H