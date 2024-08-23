#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QWebEngineView>

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

private slots:
    void OnLineEditAddressReturnPressed();
    void OnWebPageLoaded(bool loaded);

private:
    Ui::MainWindow *ui;
    QList<BrowserTabWidget*> m_tabs;
    QWebEngineView* m_web_view;
    void Initialize();
    void LoadTab(BrowserTabWidget* tab);
    BrowserTabWidget* m_current_tab;
};

#endif // MAINWINDOW_H
