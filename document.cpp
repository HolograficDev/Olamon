#include "document.h"

Document::Document(const CanvasSettings &settings, QObject *parent)
    : QObject(parent), canvas(settings)
{
}

const CanvasSettings &Document::settings() const
{
    return canvas;
}

void Document::setSettings(const CanvasSettings &settings)
{
    canvas = settings;
    setModified(true);
    emit settingsChanged();
}

bool Document::isModified() const
{
    return modified;
}

void Document::setModified(bool value)
{
    if (modified == value)
        return;
    modified = value;
    emit modifiedChanged(modified);
}