#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebEnginePage>

class WebPage : public QWebEnginePage
{
    Q_OBJECT

public:
    explicit WebPage(QObject *parent = nullptr);
    ~WebPage();

private:
};

#endif // WEBPAGE_H
