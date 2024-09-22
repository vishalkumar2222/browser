#include "PasswordDialog.h"

void PasswordDialog::Initialize(QDialog* PasswordDialog)
{
    PasswordDialog->resize(399, 148);
    mGridLayout = new QGridLayout(PasswordDialog);
    mIconLabel = new QLabel(PasswordDialog);
    mIconLabel->setAlignment(Qt::AlignCenter);

    mGridLayout->addWidget(mIconLabel, 0, 0, 1, 1);

    mInfoLabel = new QLabel(PasswordDialog);
    QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(mInfoLabel->sizePolicy().hasHeightForWidth());
    mInfoLabel->setSizePolicy(sizePolicy);
    mInfoLabel->setWordWrap(true);

    mGridLayout->addWidget(mInfoLabel, 0, 1, 1, 1);

    mUserLabel = new QLabel(PasswordDialog);

    mGridLayout->addWidget(mUserLabel, 1, 0, 1, 1);

    mUserNameLineEdit = new QLineEdit(PasswordDialog);

    mGridLayout->addWidget(mUserNameLineEdit, 1, 1, 1, 1);

    mPasswordLabel = new QLabel(PasswordDialog);
    mGridLayout->addWidget(mPasswordLabel, 2, 0, 1, 1);

    mPasswordLineEdit = new QLineEdit(PasswordDialog);
    mPasswordLineEdit->setEchoMode(QLineEdit::Password);

    mGridLayout->addWidget(mPasswordLineEdit, 2, 1, 1, 1);

    mButtonBox = new QDialogButtonBox(PasswordDialog);
    mButtonBox->setOrientation(Qt::Horizontal);
    mButtonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

    mGridLayout->addWidget(mButtonBox, 3, 0, 1, 2);

    mUserLabel->raise();
    mUserNameLineEdit->raise();
    mPasswordLabel->raise();
    mPasswordLineEdit->raise();
    mButtonBox->raise();
    mIconLabel->raise();
    mInfoLabel->raise();

    QObject::connect(mButtonBox, &QDialogButtonBox::accepted, PasswordDialog, qOverload<>(&QDialog::accept));
    QObject::connect(mButtonBox, &QDialogButtonBox::rejected, PasswordDialog, qOverload<>(&QDialog::reject));

    mIconLabel->setText("Icon");
    mInfoLabel->setText("Info");
    mUserLabel->setText("Username:");
    mPasswordLabel->setText("Password:");
}
