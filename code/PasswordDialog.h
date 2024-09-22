#pragma once
#include <QtCore/QVariant>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>

class PasswordDialog
{
public:
    QGridLayout* mGridLayout;
    QLabel* mIconLabel;
    QLabel* mInfoLabel;
    QLabel* mUserLabel;
    QLineEdit* mUserNameLineEdit;
    QLabel* mPasswordLabel;
    QLineEdit* mPasswordLineEdit;
    QDialogButtonBox* mButtonBox;
	void Initialize(QDialog* PasswordDialog);
};