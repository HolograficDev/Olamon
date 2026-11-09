#ifndef WRITINGTAB_H
#define WRITINGTAB_H

#include <QWidget>

class QAbstractButton;
class QButtonGroup;
class QLabel;
class Document;
class QPushButton;

class WritingTab : public QWidget
{
    Q_OBJECT

public:
    explicit WritingTab(QWidget *parent = nullptr);

    void setStatus(const QString &text);   // shows in the box under the tools
    void newCanvas();   // opens the dialog and makes a new document

private:
    QWidget *createToolPanel();
    QWidget *createOptionsPanel();
    QWidget *createCanvasArea();
    void onToolPicked(QAbstractButton *button);

    QButtonGroup *toolGroup;
    QLabel *optionsTitle;
    QLabel *statusLabel;
    Document *document = nullptr;
    QLabel *canvasLabel;
    QPushButton *newCanvasButton;
};

#endif // WRITINGTAB_H