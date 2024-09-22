#pragma once
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

class CertificateDialog
{
public:
	QVBoxLayout* mVerticalLayout;
	QLabel* mIconLabel;
	QLabel* mInfoLabel;
	QLabel* mErrorLabel;
	QSpacerItem* mVerticalSpacer;
	QDialogButtonBox* mButtonBox;
	void Initialize(QDialog* parent);
};