#include "TabWidget.h"
#include "WebPage.h"
#include "WebView.h"
#include <QLabel>
#include <QMenu>
#include <QTabBar>
#include <QWebEngineProfile>

TabWidget::TabWidget(QWebEngineProfile *profile, QWidget *parent)
    : QTabWidget(parent)
    , m_profile(profile)
{
    QTabBar *tabBar = this->tabBar();
    tabBar->setTabsClosable(true);
    tabBar->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
    tabBar->setMovable(true);
    tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tabBar, &QTabBar::customContextMenuRequested, this, &TabWidget::HandleContextMenuRequested);
    connect(tabBar, &QTabBar::tabCloseRequested, [this](int index) {
        if (WebView *view = GetWebView(index))
            view->page()->triggerAction(QWebEnginePage::WebAction::RequestClose);
    });
    connect(tabBar, &QTabBar::tabBarDoubleClicked, [this](int index) {
        if (index == -1)
            CreateTab();
    });

    setDocumentMode(true);
    setElideMode(Qt::ElideRight);

    connect(this, &QTabWidget::currentChanged, this, &TabWidget::HandleCurrentChanged);

    if (profile->isOffTheRecord()) {
        QLabel *icon = new QLabel(this);
        QPixmap pixmap(":ninja.png");
        icon->setPixmap(pixmap.scaledToHeight(tabBar->height()));
		setStyleSheet(QString::asprintf("QTabWidget::tab-bar { left: %dpx; },", icon->pixmap().width()));
    }
}

void TabWidget::HandleCurrentChanged(int index)
{
    if (index != -1) {
        WebView *view = GetWebView(index);
        if (!view->url().isEmpty())
            view->setFocus();
        emit TitleChanged(view->title());
        emit LoadProgress(view->LoadProgress());
        emit UrlChanged(view->url());
        emit FavIconChanged(view->FavIcon());
        emit WebActionEnabledChanged(QWebEnginePage::Back, view->IsWebActionEnabled(QWebEnginePage::Back));
        emit WebActionEnabledChanged(QWebEnginePage::Forward, view->IsWebActionEnabled(QWebEnginePage::Forward));
        emit WebActionEnabledChanged(QWebEnginePage::Stop, view->IsWebActionEnabled(QWebEnginePage::Stop));
        emit WebActionEnabledChanged(QWebEnginePage::Reload,view->IsWebActionEnabled(QWebEnginePage::Reload));
    } else {
        emit TitleChanged(QString());
        emit LoadProgress(0);
        emit UrlChanged(QUrl());
        emit FavIconChanged(QIcon());
        emit WebActionEnabledChanged(QWebEnginePage::Back, false);
        emit WebActionEnabledChanged(QWebEnginePage::Forward, false);
        emit WebActionEnabledChanged(QWebEnginePage::Stop, false);
        emit WebActionEnabledChanged(QWebEnginePage::Reload, true);
    }
}

void TabWidget::HandleContextMenuRequested(const QPoint &pos)
{
    QMenu menu;
#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
    menu.addAction(tr("New &Tab"), QKeySequence::AddTab, this, &TabWidget::CreateTab);
#else
    menu.addAction(tr("New &Tab"), this, &TabWidget::createTab, QKeySequence::AddTab);
#endif
    int index = tabBar()->tabAt(pos);
    if (index != -1) {
        QAction *action = menu.addAction(tr("Clone Tab"));
        connect(action, &QAction::triggered, this, [this,index]() {
            CloneTab(index);
        });
        menu.addSeparator();
        action = menu.addAction(tr("&Close Tab"));
        action->setShortcut(QKeySequence::Close);
        connect(action, &QAction::triggered, this, [this,index]() {
            CloseTab(index);
        });
        action = menu.addAction(tr("Close &Other Tabs"));
        connect(action, &QAction::triggered, this, [this,index]() {
            CloseOtherTabs(index);
        });
        menu.addSeparator();
        action = menu.addAction(tr("Reload Tab"));
        action->setShortcut(QKeySequence::Refresh);
        connect(action, &QAction::triggered, this, [this,index]() {
            ReloadTab(index);
        });
    } else {
        menu.addSeparator();
    }
    menu.addAction(tr("Reload All Tabs"), this, &TabWidget::ReloadAllTabs);
    menu.exec(QCursor::pos());
}

WebView *TabWidget::GetCurrentWebView() const
{
    return GetWebView(currentIndex());
}

WebView *TabWidget::GetWebView(int index) const
{
    return qobject_cast<WebView*>(widget(index));
}

void TabWidget::SetupView(WebView *webView)
{
    QWebEnginePage *webPage = webView->page();

    connect(webView, &QWebEngineView::titleChanged, [this, webView](const QString &title) {
        int index = indexOf(webView);
        if (index != -1) {
            setTabText(index, title);
            setTabToolTip(index, title);
        }
        if (currentIndex() == index)
            emit TitleChanged(title);
    });
    connect(webView, &QWebEngineView::urlChanged, [this, webView](const QUrl &url) {
        int index = indexOf(webView);
        if (index != -1)
            tabBar()->setTabData(index, url);
        if (currentIndex() == index)
            emit UrlChanged(url);
    });
    connect(webView, &QWebEngineView::loadProgress, [this, webView](int progress) {
        if (currentIndex() == indexOf(webView))
            emit LoadProgress(progress);
    });
    connect(webPage, &QWebEnginePage::linkHovered, [this, webView](const QString &url) {
        if (currentIndex() == indexOf(webView))
            emit LinkHovered(url);
    });
    connect(webView, &WebView::FavIconChanged, [this, webView](const QIcon &icon) {
        int index = indexOf(webView);
        if (index != -1)
            setTabIcon(index, icon);
        if (currentIndex() == index)
            emit FavIconChanged(icon);
    });
    connect(webView, &WebView::WebActionEnabledChanged, [this, webView](QWebEnginePage::WebAction action, bool enabled) {
        if (currentIndex() ==  indexOf(webView))
            emit WebActionEnabledChanged(action,enabled);
    });
    connect(webPage, &QWebEnginePage::windowCloseRequested, [this, webView]() {
        int index = indexOf(webView);
        if (webView->page()->inspectedPage())
            window()->close();
        else if (index >= 0)
            CloseTab(index);
    });
    connect(webView, &WebView::DevToolsRequested, this, &TabWidget::DevToolsRequested);
    connect(webPage, &QWebEnginePage::findTextFinished, [this, webView](const QWebEngineFindTextResult &result) {
        if (currentIndex() == indexOf(webView))
            emit FindTextFinished(result);
    });
}

WebView *TabWidget::CreateTab()
{
    WebView *webView = CreateBackgroundTab();
    setCurrentWidget(webView);
    return webView;
}

WebView *TabWidget::CreateBackgroundTab()
{
    WebView *webView = new WebView;
    WebPage *webPage = new WebPage(m_profile, webView);
    webView->setPage(webPage);
    SetupView(webView);
    int index = addTab(webView, tr("(Untitled)"));
    setTabIcon(index, webView->FavIcon());
    // Workaround for QTBUG-61770
    webView->resize(currentWidget()->size());
    webView->show();
    return webView;
}

void TabWidget::ReloadAllTabs()
{
    for (int i = 0; i < count(); ++i)
        GetWebView(i)->reload();
}

void TabWidget::CloseOtherTabs(int index)
{
    for (int i = count() - 1; i > index; --i)
        CloseTab(i);
    for (int i = index - 1; i >= 0; --i)
        CloseTab(i);
}

void TabWidget::CloseTab(int index)
{
    if (WebView *view = GetWebView(index)) {
        bool hasFocus = view->hasFocus();
        removeTab(index);
        if (hasFocus && count() > 0)
            GetCurrentWebView()->setFocus();
        if (count() == 0)
            CreateTab();
        view->deleteLater();
    }
}

void TabWidget::CloneTab(int index)
{
    if (WebView *view = GetWebView(index)) {
        WebView *tab = CreateTab();
        tab->setUrl(view->url());
    }
}

void TabWidget::SetUrl(const QUrl &url)
{
    if (WebView *view = GetCurrentWebView()) {
        view->setUrl(url);
        view->setFocus();
    }
}

void TabWidget::TriggerWebPageAction(QWebEnginePage::WebAction action)
{
    if (WebView *webView = GetCurrentWebView()) {
        webView->triggerPageAction(action);
        webView->setFocus();
    }
}

void TabWidget::NextTab()
{
    int next = currentIndex() + 1;
    if (next == count())
        next = 0;
    setCurrentIndex(next);
}

void TabWidget::PreviousTab()
{
    int next = currentIndex() - 1;
    if (next < 0)
        next = count() - 1;
    setCurrentIndex(next);
}

void TabWidget::ReloadTab(int index)
{
    if (WebView *view = GetWebView(index))
        view->reload();
}
