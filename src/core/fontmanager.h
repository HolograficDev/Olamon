#ifndef FONTMANAGER_H
#define FONTMANAGER_H

#include <QFont>
#include <QString>

// loads the olamon font once and makes fonts from it
class FontManager
{
public:
    static bool load();                 // call once at startup
    static QString family();

    // styleSet: 0 = default letters, 1-3 = ss01-ss03
    static QFont font(int pixelSize = 16, int styleSet = 0);
    static void applyStyleSet(QFont &font, int styleSet);
    // the four readings of a letter, same numbers as the style sets
    enum Reading { Doko, Doro, Azro, Baro };
    static QString readingName(int styleSet);

private:
    static QString fontFamily;
};

#endif // FONTMANAGER_H