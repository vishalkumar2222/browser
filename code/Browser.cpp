#include "Browser.h"
#include "Browserwindow.h"

#include <QWebEngineSettings>

Browser::Browser()
{
    mDownloadManagerWidget.setAttribute(Qt::WA_QuitOnClose, false);

    QObject::connect(QWebEngineProfile::defaultProfile(), &QWebEngineProfile::downloadRequested,
        &mDownloadManagerWidget, &DownloadManagerWidget::DownloadRequested);
}

BrowserWindow *Browser::CreateHiddenWindow(bool offTheRecord)
{
    if (!offTheRecord && !mProfile) {
        const QString name = "OpenBrowser" + QLatin1StringView(qWebEngineChromiumVersion());
        mProfile.reset(new QWebEngineProfile(name));
        mProfile->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
        mProfile->settings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
        mProfile->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
        mProfile->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, false);
        mProfile->settings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, true);
        QObject::connect(mProfile.get(), &QWebEngineProfile::downloadRequested,
                         &mDownloadManagerWidget, &DownloadManagerWidget::DownloadRequested);
    }
    auto profile = !offTheRecord ? mProfile.get() : QWebEngineProfile::defaultProfile();
    auto mainWindow = new BrowserWindow(this, profile, false);
    profile->setPersistentPermissionsPolicy(QWebEngineProfile::PersistentPermissionsPolicy::AskEveryTime);
    mWindows.append(mainWindow);
    QObject::connect(mainWindow, &QObject::destroyed, [this, mainWindow]() {
        mWindows.removeOne(mainWindow);
    });
    return mainWindow;
}

BrowserWindow *Browser::CreateWindow(bool offTheRecord)
{
    auto *mainWindow = CreateHiddenWindow(offTheRecord);
    mainWindow->show();
    return mainWindow;
}

BrowserWindow *Browser::CreateDevToolsWindow()
{
    auto profile = mProfile ? mProfile.get() : QWebEngineProfile::defaultProfile();
    auto mainWindow = new BrowserWindow(this, profile, true);
    mWindows.append(mainWindow);
    QObject::connect(mainWindow, &QObject::destroyed, [this, mainWindow]() {
        mWindows.removeOne(mainWindow);
    });
    mainWindow->show();
    return mainWindow;
}
