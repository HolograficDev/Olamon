#ifndef WRITINGTAB_H
#define WRITINGTAB_H

#include <QWidget>

class QAbstractButton;
class QButtonGroup;
class QLabel;

class WritingTab : public QWidget
{
    Q_OBJECT

public:
    explicit WritingTab(QWidget *parent = nullptr);

    void setStatus(const QString &text);   // shows in the box under the tools

private:
    QWidget *createToolPanel();
    QWidget *createOptionsPanel();
    QWidget *createCanvasArea();
    void onToolPicked(QAbstractButton *button);

    QButtonGroup *toolGroup;
    QLabel *optionsTitle;
    QLabel *statusLabel;
};

#endif // WRITINGTAB_H