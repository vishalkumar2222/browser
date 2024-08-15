#include "BrowserTabWidget.h"
#include "ui_BrowserTabWidget.h"

BrowserTabWidget::BrowserTabWidget(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::BrowserTabWidget)
{
    ui->setupUi(this);
}

BrowserTabWidget::~BrowserTabWidget()
{
    delete ui;
}
