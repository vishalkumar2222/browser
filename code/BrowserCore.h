#ifndef BROWSERCORE_H
#define BROWSERCORE_H

#include <QList>
#include <QWebEngineProfile>

class MainWindow;

class BrowserCore
{
public:
    BrowserCore();

    QList<MainWindow*> GetWindows() {return m_windows;}

    MainWindow* CreateHiddenWindow(bool offTheRecord = false);
    MainWindow* CreateWindow(bool offTheRecord = false);
    MainWindow* CreateDevToolsWindow();

private:
    QList<MainWindow*> m_windows;
    QScopedPointer<QWebEngineProfile> m_profile;
};

#endif // BROWSERCORE_H
