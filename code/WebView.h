#pragma once

#include <QIcon>
#include <QWebEngineView>
#include <QWebEngineCertificateError>
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
#include <QWebEngineFileSystemAccessRequest>
#endif
#include <QWebEnginePage>
#include <QWebEngineRegisterProtocolHandlerRequest>
#include <QWebEngineWebAuthUxRequest>
#include <QWebEngineSettings>
#include <QWebEnginePermission>
#include <QActionGroup>

class WebPage;
class WebAuthDialog;

class WebView : public QWebEngineView
{
    Q_OBJECT

public:
    explicit WebView(QWidget *parent = nullptr);
    ~WebView();
    void SetPage(WebPage *page);

    int LoadProgress() const;
    bool IsWebActionEnabled(QWebEnginePage::WebAction webAction) const;
    QIcon FavIcon() const;

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    QWebEngineView *createWindow(QWebEnginePage::WebWindowType type) override;

signals:
    void WebActionEnabledChanged(QWebEnginePage::WebAction webAction, bool enabled);
    void FavIconChanged(const QIcon &icon);
    void DevToolsRequested(QWebEnginePage *source);
private slots:
    void HandleCertificateError(QWebEngineCertificateError error);
    void HandleAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth);
    void HandlePermissionRequested(QWebEnginePermission permission);
    void HandleProxyAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth,
                                           const QString &proxyHost);
    void HandleRegisterProtocolHandlerRequested(QWebEngineRegisterProtocolHandlerRequest request);
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
    void HandleFileSystemAccessRequested(QWebEngineFileSystemAccessRequest request);
    void HandleWebAuthUxRequested(QWebEngineWebAuthUxRequest *request);
#endif
    void HandleImageAnimationPolicyChange(QWebEngineSettings::ImageAnimationPolicy policy);

private:
    void CreateWebActionTrigger(QWebEnginePage *page, QWebEnginePage::WebAction);
    void OnStateChanged(QWebEngineWebAuthUxRequest::WebAuthUxState state);

private:
    int mLoadProgress = 100;
    WebAuthDialog *mAuthDialog = nullptr;
    QActionGroup *mImageAnimationGroup = nullptr;
};
