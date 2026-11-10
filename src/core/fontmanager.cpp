#include "fontmanager.h"

#include <QDebug>
#include <QFontDatabase>

QString FontManager::fontFamily;

bool FontManager::load()
{
    int id = QFontDatabase::addApplicationFont(":/fonts/OlamonGrunge.otf");
    if (id < 0) {
        qWarning() << "couldn't load the olamon font";
        return false;
    }

    // use the name the font reports instead of hardcoding it
    fontFamily = QFontDatabase::applicationFontFamilies(id).value(0);
    return !fontFamily.isEmpty();
}

QString FontManager::family()
{
    return fontFamily;
}

QFont FontManager::font(int pixelSize, int styleSet)
{
    QFont f(fontFamily);
    f.setPixelSize(pixelSize);
    applyStyleSet(f, styleSet);
    return f;
}

// turns on one of the stylistic sets and turns the others off
void FontManager::applyStyleSet(QFont &font, int styleSet)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    const char *tags[] = {"ss01", "ss02", "ss03"};
    for (int i = 0; i < 3; ++i) {
        QFont::Tag tag = QFont::Tag::fromString(tags[i]).value();
        font.setFeature(tag, (i + 1 == styleSet) ? 1 : 0);
    }
#else
    Q_UNUSED(font);
    Q_UNUSED(styleSet);
#endif
}
QString FontManager::readingName(int styleSet)
{
    const char *names[] = {"Doko", "Doro", "Azro", "Baro"};
    return names[qBound(0, styleSet, 3)];
}