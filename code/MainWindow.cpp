#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QWebEngineFindTextResult>
#include <QWebEngineProfile>
#include <QApplication>
#include <QScreen>
#include <QTabBar>
#include <QToolButton>
#include <QLabel>
#include "BrowserTabWidget.h"


using namespace Qt::StringLiterals;


MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Initialize();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnLineEditAddressReturnPressed()
{
    QString url = ui->m_line_edit_address->text();
    QUrl url_address(url);
    if(url_address.isValid()){
        m_web_view->load(url_address);
    }
}

void MainWindow::OnWebPageLoaded(bool loaded)
{
    if(loaded){
        m_current_tab->GetLableTittle()->setText(m_web_view->title());
        m_current_tab->GetLabelIcon()->setPixmap(m_web_view->icon().pixmap(QSize(32,32)));
    }
}

void MainWindow::Initialize()
{
    QToolButton *plus_btn = new QToolButton();
    plus_btn->setText("+");
    m_current_tab = new BrowserTabWidget(this);
    m_current_tab->setFrameShape(QFrame::Shape::Panel);
    m_current_tab->GetLableTittle()->setText("New Tab");
    m_tabs.append(m_current_tab);

    QHBoxLayout* layout_tabs = new QHBoxLayout();
    ui->frame_tabs->setLayout(layout_tabs);

    layout_tabs->addWidget(m_current_tab);
    layout_tabs->addWidget(plus_btn);
    layout_tabs->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));

    QVBoxLayout* webview_layout = new QVBoxLayout();
    ui->frame_page->setLayout(webview_layout);

    m_web_view = new QWebEngineView(this);
    webview_layout->addWidget(m_web_view);

    connect(ui->m_line_edit_address, &QLineEdit::returnPressed, this, &MainWindow::OnLineEditAddressReturnPressed);
    connect(m_web_view, &QWebEngineView::loadFinished, this, &MainWindow::OnWebPageLoaded);
    connect(ui->m_tool_btn_refresh, &QToolButton::triggered, m_web_view, &QWebEngineView::reload);
    connect(ui->m_tool_btn_back, &QToolButton::triggered, m_web_view, &QWebEngineView::back);
    connect(ui->m_tool_btn_next, &QToolButton::triggered, m_web_view, &QWebEngineView::forward);

    LoadTab(m_current_tab);
}

void MainWindow::LoadTab(BrowserTabWidget *tab)
{
    m_web_view->setPage(tab->GetPage());
    m_web_view->reload();
}


