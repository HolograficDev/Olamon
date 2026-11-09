#ifndef WRITINGTAB_H
#define WRITINGTAB_H

#include <QWidget>

class QAbstractButton;
class QButtonGroup;
class QLabel;
class QStackedWidget;
class CanvasView;
class Document;

class WritingTab : public QWidget
{
    Q_OBJECT

public:
    explicit WritingTab(QWidget *parent = nullptr);

    void setStatus(const QString &text);   // shows in the box under the tools
    void newCanvas();                      // opens the dialog and makes a new document

private:
    QWidget *createToolPanel();
    QWidget *createOptionsPanel();
    QWidget *createCanvasArea();
    void onToolPicked(QAbstractButton *button);

    QButtonGroup *toolGroup;
    QLabel *optionsTitle;
    QLabel *statusLabel;

    Document *document = nullptr;
    QStackedWidget *canvasStack;   // 0 = "no canvas yet", 1 = the editor
    CanvasView *canvasView;
    QLabel *zoomLabel;
};

#endif // WRITINGTAB_H