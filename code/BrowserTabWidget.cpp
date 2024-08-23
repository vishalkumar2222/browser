#include "BrowserTabWidget.h"
#include "ui_BrowserTabWidget.h"

BrowserTabWidget::BrowserTabWidget(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::BrowserTabWidget)
{
    ui->setupUi(this);
    connect(ui->m_tool_btn, &QToolButton::clicked, this, &BrowserTabWidget::OnBtnCloseClicked);

    m_web_page = new WebPage();
}

BrowserTabWidget::~BrowserTabWidget()
{
    delete ui;
}

QLabel *BrowserTabWidget::GetLableTittle() const
{
    return ui->m_label_title;
}

QLabel *BrowserTabWidget::GetLabelIcon() const
{
    return ui->m_label_icon;
}

WebPage *BrowserTabWidget::GetPage()
{
    return m_web_page;
}

void BrowserTabWidget::OnBtnCloseClicked()
{
    emit BtnCloseClicked();
}
