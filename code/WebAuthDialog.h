#pragma once

#include <QDialog>
#include <QButtonGroup>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QScrollArea>
#include "qwebenginewebauthuxrequest.h"

class WebAuthWindowDialog {
public:
    void Initialize(QDialog* WebAuthDialog) {
        WebAuthDialog->resize(563, 397);
        mButtonBox = new QDialogButtonBox(WebAuthDialog);
        mButtonBox->setObjectName("mButtonBox");
        mButtonBox->setGeometry(QRect(20, 320, 471, 32));
        mButtonBox->setOrientation(Qt::Horizontal);
        mButtonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok | QDialogButtonBox::Retry);
        mHeadingLabel = new QLabel(WebAuthDialog);
        mHeadingLabel->setObjectName("mHeadingLabel");
        mHeadingLabel->setGeometry(QRect(30, 20, 321, 16));
        mHeadingLabel->setWordWrap(false);
        mDescription = new QLabel(WebAuthDialog);
        mDescription->setObjectName("mDescription");
        mDescription->setGeometry(QRect(30, 60, 491, 31));
        mDescription->setWordWrap(false);
        mLayoutWidget = new QWidget(WebAuthDialog);
        mLayoutWidget->setObjectName("mLayoutWidget");
        mLayoutWidget->setGeometry(QRect(20, 100, 471, 171));
        mMainVerticalLayout = new QVBoxLayout(mLayoutWidget);
        mMainVerticalLayout->setObjectName("mMainVerticalLayout");
        mMainVerticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        mMainVerticalLayout->setContentsMargins(0, 0, 0, 0);
        mPinGroupBox = new QGroupBox(mLayoutWidget);
        mPinGroupBox->setObjectName("mPinGroupBox");
        mPinGroupBox->setFlat(true);
        mPinLabel = new QLabel(mPinGroupBox);
        mPinLabel->setObjectName("mPinLabel");
        mPinLabel->setGeometry(QRect(10, 20, 58, 16));
        mPinLineEdit = new QLineEdit(mPinGroupBox);
        mPinLineEdit->setObjectName("mPinLineEdit");
        mPinLineEdit->setGeometry(QRect(90, 20, 113, 21));
        mConfirmPinLabel = new QLabel(mPinGroupBox);
        mConfirmPinLabel->setObjectName("mConfirmPinLabel");
        mConfirmPinLabel->setGeometry(QRect(10, 50, 81, 16));
        mConfirmPinLineEdit = new QLineEdit(mPinGroupBox);
        mConfirmPinLineEdit->setObjectName("mConfirmPinLineEdit");
        mConfirmPinLineEdit->setGeometry(QRect(90, 50, 113, 21));
        mPinEntryErrorLabel = new QLabel(mPinGroupBox);
        mPinEntryErrorLabel->setObjectName("mPinEntryErrorLabel");
        mPinEntryErrorLabel->setGeometry(QRect(10, 80, 441, 16));

        mMainVerticalLayout->addWidget(mPinGroupBox);

        WebAuthDialog->setWindowTitle("WebAuthDialog");
        mHeadingLabel->setText("Heading");
        mDescription->setText("Description");
        mPinGroupBox->setTitle(QString());
        mPinLabel->setText("PIN");
        mConfirmPinLabel->setText("Confirm PIN");
        mPinEntryErrorLabel->setText("TextLabel");
    }
    QDialogButtonBox* mButtonBox;
    QLabel* mHeadingLabel;
    QLabel* mDescription;
    QWidget* mLayoutWidget;
    QVBoxLayout* mMainVerticalLayout;
    QGroupBox* mPinGroupBox;
    QLabel* mPinLabel;
    QLineEdit* mPinLineEdit;
    QLabel* mConfirmPinLabel;
    QLineEdit* mConfirmPinLineEdit;
    QLabel* mPinEntryErrorLabel;
};

class WebAuthDialog : public QDialog
{
    Q_OBJECT
public:
    WebAuthDialog(QWebEngineWebAuthUxRequest *request, QWidget *parent = nullptr);
    ~WebAuthDialog();

    void UpdateDisplay();

private:
    QWebEngineWebAuthUxRequest *UxRequest;
    QButtonGroup *ButtonGroup = nullptr;
    QScrollArea *ScrollArea = nullptr;
    QWidget *SelectAccountWidget = nullptr;
    QVBoxLayout *SelectAccountLayout = nullptr;

    void Initialize();
    void setupSelectAccountUI();
    void setupCollectPinUI();
    void setupFinishCollectTokenUI();
    void setupErrorUI();
    void onCancelRequest();
    void onRetry();
    void onAcceptRequest();
    void clearSelectAccountButtons();

    WebAuthWindowDialog *mWebAuthDialog;
};
