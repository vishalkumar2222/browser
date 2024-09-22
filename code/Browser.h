#pragma once

#include "DownloadManagerWidget.h"

#include <QList>
#include <QWebEngineProfile>

class BrowserWindow;

class Browser
{
public:
    Browser();

    QList<BrowserWindow*> GetWindows() { return mWindows; }

    BrowserWindow *CreateHiddenWindow(bool offTheRecord = false);
    BrowserWindow *CreateWindow(bool offTheRecord = false);
    BrowserWindow *CreateDevToolsWindow();

    DownloadManagerWidget &GetDownloadManagerWidget() { return mDownloadManagerWidget; }

private:
    QList<BrowserWindow*> mWindows;
    DownloadManagerWidget mDownloadManagerWidget;
    QScopedPointer<QWebEngineProfile> mProfile;
};
