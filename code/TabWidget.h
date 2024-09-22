#pragma once

#include <QTabWidget>
#include <QWebEngineFindTextResult>
#include <QWebEnginePage>

QT_BEGIN_NAMESPACE
class QUrl;
QT_END_NAMESPACE

class WebView;

class TabWidget : public QTabWidget
{
    Q_OBJECT

public:
    explicit TabWidget(QWebEngineProfile *profile, QWidget *parent = nullptr);

    WebView *GetCurrentWebView() const;

signals:
    void LinkHovered(const QString &link);
    void LoadProgress(int progress);
    void TitleChanged(const QString &title);
    void UrlChanged(const QUrl &url);
    void FavIconChanged(const QIcon &icon);
    void WebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled);
    void DevToolsRequested(QWebEnginePage *source);
    void FindTextFinished(const QWebEngineFindTextResult &result);

public slots:
    void SetUrl(const QUrl &url);
    void TriggerWebPageAction(QWebEnginePage::WebAction action);

    WebView *CreateTab();
    WebView *CreateBackgroundTab();
    void CloseTab(int index);
    void NextTab();
    void PreviousTab();

private slots:
    void HandleCurrentChanged(int index);
    void HandleContextMenuRequested(const QPoint &pos);
    void CloneTab(int index);
    void CloseOtherTabs(int index);
    void ReloadAllTabs();
    void ReloadTab(int index);

private:
    WebView *GetWebView(int index) const;
    void SetupView(WebView *webView);

    QWebEngineProfile *m_profile;
};
