#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QObject>
#include "canvassettings.h"

class Document : public QObject
{
    Q_OBJECT

public:
    explicit Document(const CanvasSettings &settings, QObject *parent = nullptr);

    const CanvasSettings &settings() const;
    void setSettings(const CanvasSettings &settings);

    bool isModified() const;
    void setModified(bool modified);

signals:
    void settingsChanged();
    void modifiedChanged(bool modified);

private:
    CanvasSettings canvas;
    bool modified = false;
};

#endif // DOCUMENT_H