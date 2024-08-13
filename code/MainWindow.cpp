#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QWebEngineFindTextResult>
#include <QWebEngineProfile>
#include "BrowserCore.h"

using namespace Qt::StringLiterals;


MainWindow::MainWindow(BrowserCore *browser, QWebEngineProfile *profile, bool forDevTools)
    :m_browser(browser),
    m_profile(profile),
    ui(new Ui::MainWindow)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setFocusPolicy(Qt::ClickFocus);

    if(!forDevTools){
        ui->m_progress_bar->setMaximumHeight(1);
        ui->m_progress_bar->setTextVisible(false);
        ui->m_progress_bar->setStyleSheet(u"QProgressBar {border: 0px} QProgressBar::chunk {background-color: #da4453}"_s);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

QSize MainWindow::sizeHint() const
{

}

QTabWidget *MainWindow::GetTabWidget() const
{

}

WebView *MainWindow::GetCurrentTab() const
{

}
