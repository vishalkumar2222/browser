#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QWebEnginePage>

namespace Ui {
class MainWindow;
}

class BrowserTabWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QList<BrowserTabWidget*> m_tabs;
    void Initialize();
};

#endif // MAINWINDOW_H
