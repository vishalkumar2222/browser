#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWidget>

class WebPage : public QWidget
{
    Q_OBJECT

public:
    explicit WebPage(QWidget *parent = nullptr);
    ~WebPage();

private:
};

#endif // WEBPAGE_H
