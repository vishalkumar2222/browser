#ifndef BROWSERTABWIDGET_H
#define BROWSERTABWIDGET_H

#include <QFrame>
#include <QLabel>
#include "WebPage.h"

namespace Ui {
class BrowserTabWidget;
}

class WebPage;

class BrowserTabWidget : public QFrame
{
    Q_OBJECT

public:
    explicit BrowserTabWidget(QWidget *parent = nullptr);
    ~BrowserTabWidget();
    QLabel *GetLableTittle() const;
    QLabel *GetLabelIcon() const;
    WebPage *GetPage();
signals:
    void BtnCloseClicked();

public slots:
    void OnBtnCloseClicked();

private:
    Ui::BrowserTabWidget *ui;
    WebPage* m_web_page;
};

#endif // BROWSERTABWIDGET_H
