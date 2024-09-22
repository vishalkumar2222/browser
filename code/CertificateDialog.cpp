#include "CertificateDialog.h"

void CertificateDialog::Initialize(QDialog* parent)
{
	parent->resize(370, 141);
	mVerticalLayout = new QVBoxLayout(parent);
	mVerticalLayout->setContentsMargins(20, -1, 20, -1);
	mIconLabel = new QLabel(parent);
	mIconLabel->setAlignment(Qt::AlignCenter);
	mVerticalLayout->addWidget(mIconLabel);
	mInfoLabel = new QLabel(parent);
	QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(mInfoLabel->sizePolicy().hasHeightForWidth());
	mInfoLabel->setSizePolicy(sizePolicy);
	mInfoLabel->setWordWrap(true);
	mVerticalLayout->addWidget(mInfoLabel);
	mErrorLabel = new QLabel(parent);
	mErrorLabel->setWordWrap(true);
	mVerticalLayout->addWidget(mErrorLabel);
	mVerticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
	mVerticalLayout->addItem(mVerticalSpacer);
	mButtonBox = new QDialogButtonBox(parent);
	mButtonBox->setOrientation(Qt::Horizontal);
	mButtonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
	mVerticalLayout->addWidget(mButtonBox);
	QObject::connect(mButtonBox, &QDialogButtonBox::accepted, parent, qOverload<>(&QDialog::accept));
	QObject::connect(mButtonBox, &QDialogButtonBox::rejected, parent, qOverload<>(&QDialog::reject));
}
