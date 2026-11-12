#include "newcanvasdialog.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDir>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QSettings>
#include <QSpinBox>
#include <QStandardPaths>
#include <QVBoxLayout>

struct Preset
{
    const char *name;
    int width;
    int height;
};

// first one is custom, the size boxes are left alone for it
static const Preset presets[] = {
    {"Custom", 0, 0},
    {"A4 portrait (300 dpi)", 2480, 3508},
    {"A4 landscape (300 dpi)", 3508, 2480},
    {"Square", 2000, 2000},
    {"Full HD", 1920, 1080},
    {"4K", 3840, 2160},
    };

NewCanvasDialog::NewCanvasDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("New Canvas");
    setMinimumWidth(420);

    nameEdit = new QLineEdit("Untitled");

    presetBox = new QComboBox;
    for (const Preset &p : presets)
        presetBox->addItem(p.name);

    widthBox = new QSpinBox;
    heightBox = new QSpinBox;
    for (QSpinBox *box : {widthBox, heightBox}) {
        box->setRange(1, 20000);
        box->setSuffix(" px");
    }

    colorButton = new QPushButton;
    transparentBox = new QCheckBox("Transparent");
    updateColorButton();

    // default folder comes from the settings, app options will change this later
    QSettings settings;
    QString defaultFolder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/Olamon";
    folderEdit = new QLineEdit(settings.value("defaultLocation", defaultFolder).toString());
    QPushButton *browseButton = new QPushButton("Browse...");

    QHBoxLayout *colorRow = new QHBoxLayout;
    colorRow->addWidget(colorButton, 1);
    colorRow->addWidget(transparentBox);

    QHBoxLayout *folderRow = new QHBoxLayout;
    folderRow->addWidget(folderEdit, 1);
    folderRow->addWidget(browseButton);

    QFormLayout *form = new QFormLayout;
    form->addRow("File name", nameEdit);
    form->addRow("Preset", presetBox);
    form->addRow("Width", widthBox);
    form->addRow("Height", heightBox);
    form->addRow("Background", colorRow);
    form->addRow("Location", folderRow);

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText("Create");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(buttons);

    connect(presetBox, &QComboBox::currentIndexChanged, this, &NewCanvasDialog::applyPreset);
    connect(widthBox, &QSpinBox::valueChanged, this, &NewCanvasDialog::sizeEdited);
    connect(heightBox, &QSpinBox::valueChanged, this, &NewCanvasDialog::sizeEdited);
    connect(colorButton, &QPushButton::clicked, this, &NewCanvasDialog::pickColor);
    connect(transparentBox, &QCheckBox::toggled, colorButton, &QPushButton::setDisabled);
    connect(browseButton, &QPushButton::clicked, this, &NewCanvasDialog::pickFolder);
    connect(buttons, &QDialogButtonBox::accepted, this, &NewCanvasDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &NewCanvasDialog::reject);

    presetBox->setCurrentIndex(1);   // a4 portrait
}

CanvasSettings NewCanvasDialog::settings() const
{
    CanvasSettings s;
    s.name = nameEdit->text().trimmed();
    s.width = widthBox->value();
    s.height = heightBox->value();
    s.background = transparentBox->isChecked() ? QColor(0, 0, 0, 0) : color;
    s.folder = QDir::cleanPath(folderEdit->text().trimmed());
    return s;
}

void NewCanvasDialog::accept()
{
    QString name = nameEdit->text().trimmed();
    if (name.isEmpty() || name.contains(QRegularExpression("[\\\\/:*?\"<>|]"))) {
        QMessageBox::warning(this, "New Canvas", "The file name is empty or has characters that aren't allowed.");
        return;
    }

    // makes the folder if it isn't there yet
    QString folder = folderEdit->text().trimmed();
    if (folder.isEmpty() || !QDir().mkpath(folder)) {
        QMessageBox::warning(this, "New Canvas", "Can't use that location.");
        return;
    }

    QDialog::accept();
}

void NewCanvasDialog::applyPreset(int index)
{
    if (index == 0)
        return;

    // block the signals so the preset doesn't flip back to custom
    widthBox->blockSignals(true);
    heightBox->blockSignals(true);
    widthBox->setValue(presets[index].width);
    heightBox->setValue(presets[index].height);
    widthBox->blockSignals(false);
    heightBox->blockSignals(false);
}

// typing a size by hand makes it custom
void NewCanvasDialog::sizeEdited()
{
    presetBox->blockSignals(true);
    presetBox->setCurrentIndex(0);
    presetBox->blockSignals(false);
}

void NewCanvasDialog::pickColor()
{
    QColor picked = QColorDialog::getColor(color, this, "Background color");
    if (picked.isValid()) {
        color = picked;
        updateColorButton();
    }
}

void NewCanvasDialog::pickFolder()
{
    QString folder = QFileDialog::getExistingDirectory(this, "Choose location", folderEdit->text());
    if (!folder.isEmpty())
        folderEdit->setText(folder);
}

void NewCanvasDialog::updateColorButton()
{
    QString textColor = color.lightness() > 128 ? "black" : "white";
    colorButton->setText(color.name().toUpper());
    colorButton->setStyleSheet(QString(
                                   "QPushButton { background: %1; color: %2; border: 1px solid gray; padding: 4px; } "
                                   "QPushButton:disabled { background: palette(window); color: palette(mid); border: 1px solid palette(mid); }")
                                   .arg(color.name(), textColor));
}