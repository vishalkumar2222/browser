#include "WebPage.h"
#include "WebView.h"
#include "BrowserWindow.h"
#include "TabWidget.h"
#include <QTimer>

WebPage::WebPage(QWebEngineProfile* profile, QObject* parent)
    : QWebEnginePage(profile, parent)
{
    connect(this, &QWebEnginePage::selectClientCertificate, this, &WebPage::HandleSelectClientCertificate);
    connect(this, &QWebEnginePage::certificateError, this, &WebPage::HandleCertificateError);
    connect(this, &QWebEnginePage::desktopMediaRequested, this,
        &WebPage::HandleDesktopMediaRequest);
}

void WebPage::HandleCertificateError(QWebEngineCertificateError error)
{
    if (!error.isMainFrame()) {
        error.rejectCertificate();
        return;
    }

    error.defer();
    QTimer::singleShot(0, this,
        [this, error]() mutable { emit CreateCertificateErrorDialog(error); });
}

void WebPage::HandleSelectClientCertificate(QWebEngineClientCertificateSelection selection)
{
    selection.select(selection.certificates().at(0));
}

void WebPage::HandleDesktopMediaRequest(const QWebEngineDesktopMediaRequest& request)
{
    request.selectScreen(request.screensModel()->index(0));
}