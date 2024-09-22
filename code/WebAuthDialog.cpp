#include "WebAuthDialog.h"

#include <QVBoxLayout>
#include <QRadioButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QWebEngineView>

WebAuthDialog::WebAuthDialog(QWebEngineWebAuthUxRequest *request, QWidget *parent)
    : QDialog(parent), UxRequest(request), mWebAuthDialog(new WebAuthWindowDialog)
{
    mWebAuthDialog->Initialize(this);

    ButtonGroup = new QButtonGroup(this);
    ButtonGroup->setExclusive(true);

    ScrollArea = new QScrollArea(this);
    SelectAccountWidget = new QWidget(this);
    ScrollArea->setWidget(SelectAccountWidget);
    ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    SelectAccountWidget->resize(400, 150);
    SelectAccountLayout = new QVBoxLayout(SelectAccountWidget);
    mWebAuthDialog->mMainVerticalLayout->addWidget(ScrollArea);
    SelectAccountLayout->setAlignment(Qt::AlignTop);

    UpdateDisplay();

    connect(mWebAuthDialog->mButtonBox, &QDialogButtonBox::rejected, this,
            qOverload<>(&WebAuthDialog::onCancelRequest));

    connect(mWebAuthDialog->mButtonBox, &QDialogButtonBox::accepted, this,
            qOverload<>(&WebAuthDialog::onAcceptRequest));
    QAbstractButton *button = mWebAuthDialog->mButtonBox->button(QDialogButtonBox::Retry);
    connect(button, &QAbstractButton::clicked, this, qOverload<>(&WebAuthDialog::onRetry));
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

WebAuthDialog::~WebAuthDialog()
{
    QList<QAbstractButton *> buttons = ButtonGroup->buttons();
    auto itr = buttons.begin();
    while (itr != buttons.end()) {
        QAbstractButton *radioButton = *itr;
        delete radioButton;
        itr++;
    }

    if (ButtonGroup) {
        delete ButtonGroup;
        ButtonGroup = nullptr;
    }

    if (mWebAuthDialog) {
        delete mWebAuthDialog;
        mWebAuthDialog = nullptr;
    }

    if (ScrollArea) {
        delete ScrollArea;
        ScrollArea = nullptr;
    }
}

void WebAuthDialog::UpdateDisplay()
{
    switch (UxRequest->state()) {
    case QWebEngineWebAuthUxRequest::WebAuthUxState::SelectAccount:
        setupSelectAccountUI();
        break;
    case QWebEngineWebAuthUxRequest::WebAuthUxState::CollectPin:
        setupCollectPinUI();
        break;
    case QWebEngineWebAuthUxRequest::WebAuthUxState::FinishTokenCollection:
        setupFinishCollectTokenUI();
        break;
    case QWebEngineWebAuthUxRequest::WebAuthUxState::RequestFailed:
        setupErrorUI();
        break;
    default:
        break;
    }
    adjustSize();
}

void WebAuthDialog::Initialize()
{
}

void WebAuthDialog::setupSelectAccountUI()
{
    mWebAuthDialog->mHeadingLabel->setText(tr("Choose a Passkey"));
    mWebAuthDialog->mDescription->setText(tr("Which passkey do you want to use for ")
                                            + UxRequest->relyingPartyId() + tr("? "));
    mWebAuthDialog->mPinGroupBox->setVisible(false);
    mWebAuthDialog->mMainVerticalLayout->removeWidget(mWebAuthDialog->mPinGroupBox);
    mWebAuthDialog->mButtonBox->button(QDialogButtonBox::Retry)->setVisible(false);

    clearSelectAccountButtons();
    ScrollArea->setVisible(true);
    SelectAccountWidget->resize(this->width(), this->height());
    QStringList userNames = UxRequest->userNames();
    // Create radio buttons for each name
    for (const QString &name : userNames) {
        QRadioButton *radioButton = new QRadioButton(name);
        SelectAccountLayout->addWidget(radioButton);
        ButtonGroup->addButton(radioButton);
    }

    mWebAuthDialog->mButtonBox->button(QDialogButtonBox::Ok)->setText(tr("Ok"));
    mWebAuthDialog->mButtonBox->button(QDialogButtonBox::Ok)->setVisible(true);
    mWebAuthDialog->mButtonBox->button(QDialogButtonBox::Cancel)->setVisible(true);
    mWebAuthDialog->mButtonBox->button(QDialogButtonBox::Retry)->setVisible(false);
}

void WebAuthDialog::setupFinishCollectTokenUI()
{
    clearSelectAccountButtons();
    mWebAuthDialog->mHeadingLabel->setText(tr("Use your security key with")
                                             + UxRequest->relyingPartyId());
    mWebAuthDialog->mDescription->setText(
            tr("Touch your security key again to complete the request."));
    mWebAuthDialog->mPinGroupBox->setVisible(false);
    mWebAuthDialog->mButtonBox->button(QDialogButtonBox::Ok)->setVisible(false);
    mWebAuthDialog->mButtonBox->button(QDialogButtonBox::Retry)->setVisible(false);
    ScrollArea->setVisible(false);
}
void WebAuthDialog::setupCollectPinUI()
{
    clearSelectAccountButtons();
    mWebAuthDialog->mMainVerticalLayout->addWidget(mWebAuthDialog->mPinGroupBox);
    mWebAuthDialog->mPinGroupBox->setVisible(true);
    mWebAuthDialog->mConfirmPinLabel->setVisible(false);
    mWebAuthDialog->mConfirmPinLineEdit->setVisible(false);
    mWebAuthDialog->mButtonBox->button(QDialogButtonBox::Ok)->setText(tr("Next"));
    mWebAuthDialog->mButtonBox->button(QDialogButtonBox::Ok)->setVisible(true);
    mWebAuthDialog->mButtonBox->button(QDialogButtonBox::Cancel)->setVisible(true);
    mWebAuthDialog->mButtonBox->button(QDialogButtonBox::Retry)->setVisible(false);
    ScrollArea->setVisible(false);

    QWebEngineWebAuthPinRequest pinRequestInfo = UxRequest->pinRequest();

    if (pinRequestInfo.reason == QWebEngineWebAuthUxRequest::PinEntryReason::Challenge) {
        mWebAuthDialog->mHeadingLabel->setText(tr("PIN Required"));
        mWebAuthDialog->mDescription->setText(tr("Enter the PIN for your security key"));
        mWebAuthDialog->mConfirmPinLabel->setVisible(false);
        mWebAuthDialog->mConfirmPinLineEdit->setVisible(false);
    } else {
        if (pinRequestInfo.reason == QWebEngineWebAuthUxRequest::PinEntryReason::Set) {
            mWebAuthDialog->mHeadingLabel->setText(tr("New PIN Required"));
            mWebAuthDialog->mDescription->setText(tr("Set new PIN for your security key"));
        } else {
            mWebAuthDialog->mHeadingLabel->setText(tr("Change PIN Required"));
            mWebAuthDialog->mDescription->setText(tr("Change PIN for your security key"));
        }
        mWebAuthDialog->mConfirmPinLabel->setVisible(true);
        mWebAuthDialog->mConfirmPinLineEdit->setVisible(true);
    }

    QString errorDetails;
    switch (pinRequestInfo.error) {
    case QWebEngineWebAuthUxRequest::PinEntryError::NoError:
        break;
    case QWebEngineWebAuthUxRequest::PinEntryError::InternalUvLocked:
        errorDetails = tr("Internal User Verification Locked ");
        break;
    case QWebEngineWebAuthUxRequest::PinEntryError::WrongPin:
        errorDetails = tr("Wrong PIN");
        break;
    case QWebEngineWebAuthUxRequest::PinEntryError::TooShort:
        errorDetails = tr("Too Short");
        break;
    case QWebEngineWebAuthUxRequest::PinEntryError::InvalidCharacters:
        errorDetails = tr("Invalid Characters");
        break;
    case QWebEngineWebAuthUxRequest::PinEntryError::SameAsCurrentPin:
        errorDetails = tr("Same as current PIN");
        break;
    }
    if (!errorDetails.isEmpty()) {
        errorDetails += tr(" ") + QString::number(pinRequestInfo.remainingAttempts)
                + tr(" attempts remaining");
    }
    mWebAuthDialog->mPinEntryErrorLabel->setText(errorDetails);
}

void WebAuthDialog::onCancelRequest()
{
    UxRequest->cancel();
}

void WebAuthDialog::onAcceptRequest()
{
    switch (UxRequest->state()) {
    case QWebEngineWebAuthUxRequest::WebAuthUxState::SelectAccount:
        if (ButtonGroup->checkedButton()) {
            UxRequest->setSelectedAccount(ButtonGroup->checkedButton()->text());
        }
        break;
    case QWebEngineWebAuthUxRequest::WebAuthUxState::CollectPin:
        UxRequest->setPin(mWebAuthDialog->mPinLineEdit->text());
        break;
    default:
        break;
    }
}

void WebAuthDialog::setupErrorUI()
{
    clearSelectAccountButtons();
    QString errorDescription;
    QString errorHeading = tr("Something went wrong");
    bool isVisibleRetry = false;
    switch (UxRequest->requestFailureReason()) {
    case QWebEngineWebAuthUxRequest::RequestFailureReason::Timeout:
        errorDescription = tr("Request Timeout");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::KeyNotRegistered:
        errorDescription = tr("Key not registered");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::KeyAlreadyRegistered:
        errorDescription = tr("You already registered this device."
                              "Try again with device");
        isVisibleRetry = true;
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::SoftPinBlock:
        errorDescription =
                tr("The security key is locked because the wrong PIN was entered too many times."
                   "To unlock it, remove and reinsert it.");
        isVisibleRetry = true;
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::HardPinBlock:
        errorDescription =
                tr("The security key is locked because the wrong PIN was entered too many times."
                   " You'll need to reset the security key.");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::AuthenticatorRemovedDuringPinEntry:
        errorDescription =
                tr("Authenticator removed during verification. Please reinsert and try again");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::AuthenticatorMissingResidentKeys:
        errorDescription = tr("Authenticator doesn't have resident key support");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::AuthenticatorMissingUserVerification:
        errorDescription = tr("Authenticator missing user verification");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::AuthenticatorMissingLargeBlob:
        errorDescription = tr("Authenticator missing Large Blob support");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::NoCommonAlgorithms:
        errorDescription = tr("Authenticator missing Large Blob support");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::StorageFull:
        errorDescription = tr("Storage Full");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::UserConsentDenied:
        errorDescription = tr("User consent denied");
        break;
    case QWebEngineWebAuthUxRequest::RequestFailureReason::WinUserCancelled:
        errorDescription = tr("User Cancelled Request");
        break;
    }

    mWebAuthDialog->mHeadingLabel->setText(errorHeading);
    mWebAuthDialog->mDescription->setText(errorDescription);
    mWebAuthDialog->mDescription->adjustSize();
    mWebAuthDialog->mPinGroupBox->setVisible(false);
    mWebAuthDialog->mButtonBox->button(QDialogButtonBox::Ok)->setVisible(false);
    mWebAuthDialog->mButtonBox->button(QDialogButtonBox::Retry)->setVisible(isVisibleRetry);
    if (isVisibleRetry)
        mWebAuthDialog->mButtonBox->button(QDialogButtonBox::Retry)->setFocus();
    mWebAuthDialog->mButtonBox->button(QDialogButtonBox::Cancel)->setVisible(true);
    mWebAuthDialog->mButtonBox->button(QDialogButtonBox::Cancel)->setText(tr("Close"));
    ScrollArea->setVisible(false);
}

void WebAuthDialog::onRetry()
{
    UxRequest->retry();
}

void WebAuthDialog::clearSelectAccountButtons()
{
    QList<QAbstractButton *> buttons = ButtonGroup->buttons();
    auto itr = buttons.begin();
    while (itr != buttons.end()) {
        QAbstractButton *radioButton = *itr;
        SelectAccountLayout->removeWidget(radioButton);
        ButtonGroup->removeButton(radioButton);
        delete radioButton;
        itr++;
    }
}
