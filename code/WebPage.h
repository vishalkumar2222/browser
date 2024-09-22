#pragma once

#include <QWebEnginePage>
#include <QWebEngineRegisterProtocolHandlerRequest>
#include <QWebEngineCertificateError>
#include <QWebEngineDesktopMediaRequest>

class WebPage : public QWebEnginePage
{
    Q_OBJECT

public:
    explicit WebPage(QWebEngineProfile* profile, QObject* parent = nullptr);

signals:
    void CreateCertificateErrorDialog(QWebEngineCertificateError error);

private slots:
    void HandleCertificateError(QWebEngineCertificateError error);
    void HandleSelectClientCertificate(QWebEngineClientCertificateSelection clientCertSelection);
    void HandleDesktopMediaRequest(const QWebEngineDesktopMediaRequest& request);
};
