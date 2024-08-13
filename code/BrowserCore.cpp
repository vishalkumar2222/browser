#include "BrowserCore.h"
#include "MainWindow.h"

#include <QWebEngineSettings>

using namespace Qt::StringLiterals;

BrowserCore::BrowserCore() {}

MainWindow* BrowserCore::CreateHiddenWindow(bool offTheRecord){
    if(!offTheRecord && !m_profile){
        m_profile.reset(new QWebEngineProfile("MyBrowser"));
        m_profile->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
        m_profile->settings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
        m_profile->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
        m_profile->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, false);
        m_profile->settings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, true);
    }

    auto profile = !offTheRecord ? m_profile.get() : QWebEngineProfile::defaultProfile();
    auto mainwindow = new MainWindow(this, profile, false);
    m_windows.append(mainwindow);
    QObject::connect(mainwindow, &QObject::destroyed, [=](){
        m_windows.removeOne(mainwindow);
    });
    return mainwindow;
}

MainWindow *BrowserCore::CreateWindow(bool offTheRecord)
{
    auto mainwindow = CreateHiddenWindow(offTheRecord);
    mainwindow->show();
    return mainwindow;
}

MainWindow *BrowserCore::CreateDevToolsWindow()
{
    auto profile = m_profile ? m_profile.get() : QWebEngineProfile::defaultProfile();
    auto mainwindow = new MainWindow(this, profile, true);
    m_windows.append(mainwindow);
    QObject::connect(mainwindow, &QObject::destroyed, [this, mainwindow]() {
        m_windows.removeOne(mainwindow);
    });
    mainwindow->show();
    return mainwindow;
}


