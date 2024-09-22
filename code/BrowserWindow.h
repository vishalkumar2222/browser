#pragma once

#include <QMainWindow>
#include <QTime>
#include <QWebEnginePage>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QProgressBar;
QT_END_NAMESPACE

class Browser;
class TabWidget;
class WebView;

class BrowserWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BrowserWindow(Browser *browser, QWebEngineProfile *profile,
                           bool forDevTools = false);
    QSize sizeHint() const override;
    TabWidget *GetTabWidget() const;
    WebView *GetCurrentTab() const;
    Browser *GetBrowser() { return mBrowser; }

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void HandleNewWindowTriggered();
    void HandleNewIncognitoWindowTriggered();
    void HandleFileOpenTriggered();
    void HandleFindActionTriggered();
    void HandleShowWindowTriggered();
    void HandleWebViewLoadProgress(int);
    void HandleWebViewTitleChanged(const QString &title);
    void HandleWebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled);
    void HandleDevToolsRequested(QWebEnginePage *source);
    void HandleFindTextFinished(const QWebEngineFindTextResult &result);

private:
    QMenu *CreateFileMenu(TabWidget *tabWidget);
    QMenu *CreateEditMenu();
    QMenu *CreateViewMenu(QToolBar *toolBar);
    QMenu *CreateWindowMenu(TabWidget *tabWidget);
    QMenu *CreateHelpMenu();
    QToolBar *CreateToolBar();

private:
    Browser *mBrowser;
    QWebEngineProfile *mProfile;
    TabWidget *mTabWidget;
    QProgressBar *mProgressBar = nullptr;
    QAction *mHistoryBackAction = nullptr;
    QAction *mHistoryForwardAction = nullptr;
    QAction *mStopAction = nullptr;
    QAction *mReloadAction = nullptr;
    QAction *mStopReloadAction = nullptr;
    QLineEdit *mUrlLineEdit = nullptr;
    QAction *mFavAction = nullptr;
    QString mLastSearch;
};

