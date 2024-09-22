#include "Browser.h"
#include "BrowserWindow.h"
#include "TabWidget.h"
#include "WebPage.h"
#include "WebPopUpWindow.h"
#include "WebView.h"
#include "CertificateDialog.h"
#include "PasswordDialog.h"
#include "WebAuthDialog.h"
#include <QContextMenuEvent>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QAuthenticator>
#include <QTimer>
#include <QStyle>

WebView::WebView(QWidget *parent)
    : QWebEngineView(parent)
{
    connect(this, &QWebEngineView::loadStarted, [this]() {
        mLoadProgress = 0;
        emit FavIconChanged(FavIcon());
    });
    connect(this, &QWebEngineView::loadProgress, [this](int progress) {
        mLoadProgress = progress;
    });
    connect(this, &QWebEngineView::loadFinished, [this](bool success) {
        mLoadProgress = success ? 100 : -1;
        emit FavIconChanged(FavIcon());
    });
    connect(this, &QWebEngineView::iconChanged, [this](const QIcon &) {
        emit FavIconChanged(FavIcon());
    });

    connect(this, &QWebEngineView::renderProcessTerminated,
            [this](QWebEnginePage::RenderProcessTerminationStatus termStatus, int statusCode) {
        QString status;
        switch (termStatus) {
        case QWebEnginePage::NormalTerminationStatus:
            status = tr("Render process normal exit");
            break;
        case QWebEnginePage::AbnormalTerminationStatus:
            status = tr("Render process abnormal exit");
            break;
        case QWebEnginePage::CrashedTerminationStatus:
            status = tr("Render process crashed");
            break;
        case QWebEnginePage::KilledTerminationStatus:
            status = tr("Render process killed");
            break;
        }
        QMessageBox::StandardButton btn = QMessageBox::question(window(), status,
                                                   tr("Render process exited with code: %1\n"
                                                      "Do you want to reload the page ?").arg(statusCode));
        if (btn == QMessageBox::Yes)
            QTimer::singleShot(0, this, &WebView::reload);
    });
}

WebView::~WebView()
{
    if (mImageAnimationGroup)
        delete mImageAnimationGroup;

    mImageAnimationGroup = nullptr;
}

inline QString questionForPermissionType(QWebEnginePermission::PermissionType permissionType)
{
    switch (permissionType) {
    case QWebEnginePermission::PermissionType::Geolocation:
        return QObject::tr("Allow %1 to access your location information?");
    case QWebEnginePermission::PermissionType::MediaAudioCapture:
        return QObject::tr("Allow %1 to access your microphone?");
    case QWebEnginePermission::PermissionType::MediaVideoCapture:
        return QObject::tr("Allow %1 to access your webcam?");
    case QWebEnginePermission::PermissionType::MediaAudioVideoCapture:
        return QObject::tr("Allow %1 to access your microphone and webcam?");
    case QWebEnginePermission::PermissionType::MouseLock:
        return QObject::tr("Allow %1 to lock your mouse cursor?");
    case QWebEnginePermission::PermissionType::DesktopVideoCapture:
        return QObject::tr("Allow %1 to capture video of your desktop?");
    case QWebEnginePermission::PermissionType::DesktopAudioVideoCapture:
        return QObject::tr("Allow %1 to capture audio and video of your desktop?");
    case QWebEnginePermission::PermissionType::Notifications:
        return QObject::tr("Allow %1 to show notification on your desktop?");
    case QWebEnginePermission::PermissionType::ClipboardReadWrite:
        return QObject::tr("Allow %1 to read from and write to the clipboard?");
    case QWebEnginePermission::PermissionType::LocalFontsAccess:
        return QObject::tr("Allow %1 to access fonts stored on this machine?");
    case QWebEnginePermission::PermissionType::Unsupported:
        break;
    }
    return QString();
}

void WebView::SetPage(WebPage *page)
{
    if (auto oldPage = qobject_cast<WebPage *>(QWebEngineView::page())) {
        disconnect(oldPage, &WebPage::CreateCertificateErrorDialog, this,
                   &WebView::HandleCertificateError);
        disconnect(oldPage, &QWebEnginePage::authenticationRequired, this,
                   &WebView::HandleAuthenticationRequired);
        disconnect(oldPage, &QWebEnginePage::permissionRequested, this,
                   &WebView::HandlePermissionRequested);
        disconnect(oldPage, &QWebEnginePage::proxyAuthenticationRequired, this,
                   &WebView::HandleProxyAuthenticationRequired);
        disconnect(oldPage, &QWebEnginePage::registerProtocolHandlerRequested, this,
                   &WebView::HandleRegisterProtocolHandlerRequested);
        disconnect(oldPage, &QWebEnginePage::webAuthUxRequested, this,
                   &WebView::HandleWebAuthUxRequested);
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
        disconnect(oldPage, &QWebEnginePage::fileSystemAccessRequested, this,
                   &WebView::HandleFileSystemAccessRequested);
#endif
    }
    CreateWebActionTrigger(page,QWebEnginePage::Forward);
    CreateWebActionTrigger(page,QWebEnginePage::Back);
    CreateWebActionTrigger(page,QWebEnginePage::Reload);
    CreateWebActionTrigger(page,QWebEnginePage::Stop);
    QWebEngineView::setPage(page);
    connect(page, &WebPage::CreateCertificateErrorDialog, this, &WebView::HandleCertificateError);
    connect(page, &QWebEnginePage::authenticationRequired, this,
            &WebView::HandleAuthenticationRequired);
    connect(page, &QWebEnginePage::permissionRequested, this,
            &WebView::HandlePermissionRequested);
    connect(page, &QWebEnginePage::proxyAuthenticationRequired, this,
            &WebView::HandleProxyAuthenticationRequired);
    connect(page, &QWebEnginePage::registerProtocolHandlerRequested, this,
            &WebView::HandleRegisterProtocolHandlerRequested);
#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
    connect(page, &QWebEnginePage::fileSystemAccessRequested, this,
            &WebView::HandleFileSystemAccessRequested);
#endif
    connect(page, &QWebEnginePage::webAuthUxRequested, this, &WebView::HandleWebAuthUxRequested);
}

int WebView::LoadProgress() const
{
    return mLoadProgress;
}

void WebView::CreateWebActionTrigger(QWebEnginePage *page, QWebEnginePage::WebAction webAction)
{
    QAction *action = page->action(webAction);
    connect(action, &QAction::changed, [this, action, webAction]{
        emit WebActionEnabledChanged(webAction, action->isEnabled());
    });
}

bool WebView::IsWebActionEnabled(QWebEnginePage::WebAction webAction) const
{
    return page()->action(webAction)->isEnabled();
}

QIcon WebView::FavIcon() const
{
    QIcon favIcon = icon();
    if (!favIcon.isNull())
        return favIcon;

    if (mLoadProgress < 0) {
        static QIcon errorIcon(":dialog-error.png");
        return errorIcon;
    }
    if (mLoadProgress < 100) {
        static QIcon loadingIcon = QIcon::fromTheme(QIcon::ThemeIcon::ViewRefresh,
                                                    QIcon(":view-refresh.png"));
        return loadingIcon;
    }

    static QIcon defaultIcon(":text-html.png");
    return defaultIcon;
}

QWebEngineView *WebView::createWindow(QWebEnginePage::WebWindowType type)
{
    BrowserWindow *mainWindow = qobject_cast<BrowserWindow*>(window());
    if (!mainWindow)
        return nullptr;

    switch (type) {
    case QWebEnginePage::WebBrowserTab: {
        return mainWindow->GetTabWidget()->CreateTab();
    }
    case QWebEnginePage::WebBrowserBackgroundTab: {
        return mainWindow->GetTabWidget()->CreateBackgroundTab();
    }
    case QWebEnginePage::WebBrowserWindow: {
        return mainWindow->GetBrowser()->CreateWindow()->GetCurrentTab();
    }
    case QWebEnginePage::WebDialog: {
        WebPopupWindow *popup = new WebPopupWindow(page()->profile());
        connect(popup->View(), &WebView::DevToolsRequested, this, &WebView::DevToolsRequested);
        return popup->View();
    }
    }
    return nullptr;
}

void WebView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();
    const QList<QAction *> actions = menu->actions();
    auto inspectElement = std::find(actions.cbegin(), actions.cend(), page()->action(QWebEnginePage::InspectElement));
    if (inspectElement == actions.cend()) {
        auto viewSource = std::find(actions.cbegin(), actions.cend(), page()->action(QWebEnginePage::ViewSource));
        if (viewSource == actions.cend())
            menu->addSeparator();

        QAction *action = menu->addAction("Open inspector in new window");
        connect(action, &QAction::triggered, [this]() { emit DevToolsRequested(page()); });
    } else {
        (*inspectElement)->setText(tr("Inspect element"));
    }

    // add conext menu for image policy
    QMenu *editImageAnimation = new QMenu(tr("Image animation policy"));

    mImageAnimationGroup = new QActionGroup(editImageAnimation);
    mImageAnimationGroup->setExclusive(true);

    QAction *disableImageAnimation =
            editImageAnimation->addAction(tr("Disable all image animation"));
    disableImageAnimation->setCheckable(true);
    mImageAnimationGroup->addAction(disableImageAnimation);
    connect(disableImageAnimation, &QAction::triggered, [this]() {
        HandleImageAnimationPolicyChange(QWebEngineSettings::ImageAnimationPolicy::Disallow);
    });
    QAction *allowImageAnimationOnce =
            editImageAnimation->addAction(tr("Allow animated images, but only once"));
    allowImageAnimationOnce->setCheckable(true);
    mImageAnimationGroup->addAction(allowImageAnimationOnce);
    connect(allowImageAnimationOnce, &QAction::triggered,
            [this]() { HandleImageAnimationPolicyChange(QWebEngineSettings::ImageAnimationPolicy::AnimateOnce); });
    QAction *allowImageAnimation = editImageAnimation->addAction(tr("Allow all animated images"));
    allowImageAnimation->setCheckable(true);
    mImageAnimationGroup->addAction(allowImageAnimation);
    connect(allowImageAnimation, &QAction::triggered, [this]() {
        HandleImageAnimationPolicyChange(QWebEngineSettings::ImageAnimationPolicy::Allow);
    });

    switch (page()->settings()->imageAnimationPolicy()) {
    case QWebEngineSettings::ImageAnimationPolicy::Allow:
        allowImageAnimation->setChecked(true);
        break;
    case QWebEngineSettings::ImageAnimationPolicy::AnimateOnce:
        allowImageAnimationOnce->setChecked(true);
        break;
    case QWebEngineSettings::ImageAnimationPolicy::Disallow:
        disableImageAnimation->setChecked(true);
        break;
    default:
        allowImageAnimation->setChecked(true);
        break;
    }

    menu->addMenu(editImageAnimation);
    menu->popup(event->globalPos());
}

void WebView::HandleCertificateError(QWebEngineCertificateError error)
{
    QDialog dialog(window());
    dialog.setModal(true);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    CertificateDialog certificateDialog;
    certificateDialog.Initialize(&dialog);
    certificateDialog.mIconLabel->setText(QString());
    QIcon icon(window()->style()->standardIcon(QStyle::SP_MessageBoxWarning, 0, window()));
    certificateDialog.mIconLabel->setPixmap(icon.pixmap(32, 32));
    certificateDialog.mErrorLabel->setText(error.description());
    dialog.setWindowTitle(tr("Certificate Error"));

    if (dialog.exec() == QDialog::Accepted)
        error.acceptCertificate();
    else
        error.rejectCertificate();
}

void WebView::HandleAuthenticationRequired(const QUrl &requestUrl, QAuthenticator *auth)
{
    QDialog dialog(window());
    dialog.setModal(true);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    PasswordDialog passwordDialog;
	passwordDialog.Initialize(&dialog);
    passwordDialog.mIconLabel->setText(QString());
    QIcon icon(window()->style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, window()));
    passwordDialog.mIconLabel->setPixmap(icon.pixmap(32, 32));

    QString introMessage(tr("Enter username and password for \"%1\" at %2")
                                 .arg(auth->realm(),
                                      requestUrl.toString().toHtmlEscaped()));
    passwordDialog.mInfoLabel->setText(introMessage);
    passwordDialog.mInfoLabel->setWordWrap(true);

    if (dialog.exec() == QDialog::Accepted) {
        auth->setUser(passwordDialog.mUserNameLineEdit->text());
        auth->setPassword(passwordDialog.mPasswordLineEdit->text());
    } else {
        // Set authenticator null if dialog is cancelled
        *auth = QAuthenticator();
    }
}

void WebView::HandlePermissionRequested(QWebEnginePermission permission)
{
    QString title = tr("Permission Request");
    QString question = questionForPermissionType(permission.permissionType()).arg(permission.origin().host());
    if (!question.isEmpty() && QMessageBox::question(window(), title, question) == QMessageBox::Yes)
        permission.grant();
    else
        permission.deny();
}

void WebView::HandleProxyAuthenticationRequired(const QUrl &, QAuthenticator *auth,
                                                const QString &proxyHost)
{
    QDialog dialog(window());
    dialog.setModal(true);
    dialog.setWindowFlags(dialog.windowFlags() & ~Qt::WindowContextHelpButtonHint);

    PasswordDialog passwordDialog;
	passwordDialog.Initialize(&dialog);
    passwordDialog.mIconLabel->setText(QString());
    QIcon icon(window()->style()->standardIcon(QStyle::SP_MessageBoxQuestion, 0, window()));
    passwordDialog.mIconLabel->setPixmap(icon.pixmap(32, 32));

    QString introMessage = tr("Connect to proxy \"%1\" using:");
    introMessage = introMessage.arg(proxyHost.toHtmlEscaped());
    passwordDialog.mInfoLabel->setText(introMessage);
    passwordDialog.mInfoLabel->setWordWrap(true);

    if (dialog.exec() == QDialog::Accepted) {
        auth->setUser(passwordDialog.mUserNameLineEdit->text());
        auth->setPassword(passwordDialog.mPasswordLineEdit->text());
    } else {
        // Set authenticator null if dialog is cancelled
        *auth = QAuthenticator();
    }
}

void WebView::HandleWebAuthUxRequested(QWebEngineWebAuthUxRequest *request)
{
    if (mAuthDialog)
        delete mAuthDialog;

    mAuthDialog = new WebAuthDialog(request, window());
    mAuthDialog->setModal(false);
    mAuthDialog->setWindowFlags(mAuthDialog->windowFlags() & ~Qt::WindowContextHelpButtonHint);

    connect(request, &QWebEngineWebAuthUxRequest::stateChanged, this, &WebView::OnStateChanged);
    mAuthDialog->show();
}

void WebView::OnStateChanged(QWebEngineWebAuthUxRequest::WebAuthUxState state)
{
    if (QWebEngineWebAuthUxRequest::WebAuthUxState::Completed == state
        || QWebEngineWebAuthUxRequest::WebAuthUxState::Cancelled == state) {
        if (mAuthDialog) {
            delete mAuthDialog;
            mAuthDialog = nullptr;
        }
    } else {
        mAuthDialog->UpdateDisplay();
    }
}

void WebView::HandleRegisterProtocolHandlerRequested(
        QWebEngineRegisterProtocolHandlerRequest request)
{
    auto answer = QMessageBox::question(window(), tr("Permission Request"),
                                        tr("Allow %1 to open all %2 links?")
                                                .arg(request.origin().host())
                                                .arg(request.scheme()));
    if (answer == QMessageBox::Yes)
        request.accept();
    else
        request.reject();
}
//! [registerProtocolHandlerRequested]

#if QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
void WebView::HandleFileSystemAccessRequested(QWebEngineFileSystemAccessRequest request)
{
    QString accessType;
    switch (request.accessFlags()) {
    case QWebEngineFileSystemAccessRequest::Read:
        accessType = "read";
        break;
    case QWebEngineFileSystemAccessRequest::Write:
        accessType = "write";
        break;
    case QWebEngineFileSystemAccessRequest::Read | QWebEngineFileSystemAccessRequest::Write:
        accessType = "read and write";
        break;
    default:
        Q_UNREACHABLE();
    }

    auto answer = QMessageBox::question(window(), tr("File system access request"),
                                        tr("Give %1 %2 access to %3?")
                                                .arg(request.origin().host())
                                                .arg(accessType)
                                                .arg(request.filePath().toString()));
    if (answer == QMessageBox::Yes)
        request.accept();
    else
        request.reject();
}

void WebView::HandleImageAnimationPolicyChange(QWebEngineSettings::ImageAnimationPolicy policy)
{
    if (!page())
        return;

    page()->settings()->setImageAnimationPolicy(policy);
}
#endif // QT_VERSION >= QT_VERSION_CHECK(6, 4, 0)
