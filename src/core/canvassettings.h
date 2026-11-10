#ifndef CANVASSETTINGS_H
#define CANVASSETTINGS_H

#include <QColor>
#include <QString>

// everything chosen in the new canvas dialog
struct CanvasSettings
{
    QString name = "Untitled";
    int width = 2480;
    int height = 3508;
    QColor background = Qt::white;   // alpha 0 = transparent
    QString folder;

    QString filePath() const { return folder + "/" + name + ".olamon"; }
};

#endif // CANVASSETTINGS_H