#ifndef NEWCANVASDIALOG_H
#define NEWCANVASDIALOG_H

#include <QColor>
#include <QDialog>
#include "canvassettings.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QPushButton;
class QSpinBox;

class NewCanvasDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewCanvasDialog(QWidget *parent = nullptr);

    CanvasSettings settings() const;

    void accept() override;   // checks the input before closing

private:
    void applyPreset(int index);
    void sizeEdited();
    void pickColor();
    void pickFolder();
    void updateColorButton();

    QLineEdit *nameEdit;
    QLineEdit *folderEdit;
    QComboBox *presetBox;
    QSpinBox *widthBox;
    QSpinBox *heightBox;
    QPushButton *colorButton;
    QCheckBox *transparentBox;
    QColor color = Qt::white;
};

#endif // NEWCANVASDIALOG_H