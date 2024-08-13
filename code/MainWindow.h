#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QWebEnginePage>

namespace Ui {
class MainWindow;
}
class BrowserCore;
class WebView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(BrowserCore *browser, QWebEngineProfile* profile,
                        bool forDevTools = false);
    ~MainWindow();
    QSize sizeHint() const override;

    QTabWidget* GetTabWidget() const;
    WebView* GetCurrentTab() const;
    BrowserCore* GetBrowser() {return m_browser;}

private:
    BrowserCore* m_browser;
    QWebEngineProfile* m_profile;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
