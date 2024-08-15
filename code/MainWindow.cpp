#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <QWebEngineFindTextResult>
#include <QWebEngineProfile>
#include <QApplication>
#include <QScreen>
#include <QTabBar>
#include <QToolButton>
#include "BrowserCore.h"
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

void MainWindow::Initialize()
{
    QToolButton *plus_btn = new QToolButton();
    plus_btn->setText("+");
    auto default_tab = new BrowserTabWidget(this);
    default_tab->setFrameShape(QFrame::Shape::Panel);
    m_tabs.append(default_tab);

    QHBoxLayout* layout_tabs = new QHBoxLayout();
    ui->frame_tabs->setLayout(layout_tabs);

    layout_tabs->addWidget(default_tab);
    layout_tabs->addWidget(plus_btn);
    layout_tabs->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
}
