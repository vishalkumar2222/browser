#ifndef BROWSERTABWIDGET_H
#define BROWSERTABWIDGET_H

#include <QFrame>

namespace Ui {
class BrowserTabWidget;
}

class BrowserTabWidget : public QFrame
{
    Q_OBJECT

public:
    explicit BrowserTabWidget(QWidget *parent = nullptr);
    ~BrowserTabWidget();

private:
    Ui::BrowserTabWidget *ui;
};

#endif // BROWSERTABWIDGET_H
