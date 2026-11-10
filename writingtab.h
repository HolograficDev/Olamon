#ifndef WRITINGTAB_H
#define WRITINGTAB_H

#include <QList>
#include <QWidget>

class QButtonGroup;
class QLabel;
class QStackedWidget;
class CanvasView;
class Document;
class Tool;

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
    void setTool(int index);

    QList<Tool *> tools;          // same order as the buttons
    QButtonGroup *toolGroup;
    QLabel *optionsTitle;
    QStackedWidget *optionsStack; // one options widget per tool
    QLabel *statusLabel;

    Document *document = nullptr;
    QStackedWidget *canvasStack;  // 0 = "no canvas yet", 1 = the editor
    CanvasView *canvasView;
    QLabel *zoomLabel;
};

#endif // WRITINGTAB_H