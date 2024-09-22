#include "Browser.h"
#include "BrowserWindow.h"
#include "DownloadManagerWidget.h"
#include "TabWidget.h"
#include "WebView.h"
#include <QApplication>
#include <QCloseEvent>
#include <QEvent>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QProgressBar>
#include <QScreen>
#include <QStatusBar>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWebEngineFindTextResult>
#include <QWebEngineProfile>

BrowserWindow::BrowserWindow(Browser *browser, QWebEngineProfile *profile, bool forDevTools)
    : mBrowser(browser)
    , mProfile(profile)
    , mTabWidget(new TabWidget(profile, this))
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setFocusPolicy(Qt::ClickFocus);

    if (!forDevTools) {
        mProgressBar = new QProgressBar(this);

        QToolBar *toolbar = CreateToolBar();
        addToolBar(toolbar);
        menuBar()->addMenu(CreateFileMenu(mTabWidget));
        menuBar()->addMenu(CreateEditMenu());
        menuBar()->addMenu(CreateViewMenu(toolbar));
        menuBar()->addMenu(CreateWindowMenu(mTabWidget));
        menuBar()->addMenu(CreateHelpMenu());
    }

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    if (!forDevTools) {
        addToolBarBreak();

        mProgressBar->setMaximumHeight(1);
        mProgressBar->setTextVisible(false);
        mProgressBar->setStyleSheet("QProgressBar {border: 0px} QProgressBar::chunk {background-color: #da4453}");

        layout->addWidget(mProgressBar);
    }

    layout->addWidget(mTabWidget);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    connect(mTabWidget, &TabWidget::TitleChanged, this, &BrowserWindow::HandleWebViewTitleChanged);
    if (!forDevTools) {
        connect(mTabWidget, &TabWidget::LinkHovered, [this](const QString& url) {
            statusBar()->showMessage(url);
        });
        connect(mTabWidget, &TabWidget::LoadProgress, this, &BrowserWindow::HandleWebViewLoadProgress);
        connect(mTabWidget, &TabWidget::WebActionEnabledChanged, this, &BrowserWindow::HandleWebActionEnabledChanged);
        connect(mTabWidget, &TabWidget::UrlChanged, [this](const QUrl &url) {
            mUrlLineEdit->setText(url.toDisplayString());
        });
        connect(mTabWidget, &TabWidget::FavIconChanged, mFavAction, &QAction::setIcon);
        connect(mTabWidget, &TabWidget::DevToolsRequested, this, &BrowserWindow::HandleDevToolsRequested);
        connect(mUrlLineEdit, &QLineEdit::returnPressed, [this]() {
            mTabWidget->SetUrl(QUrl::fromUserInput(mUrlLineEdit->text()));
        });
        connect(mTabWidget, &TabWidget::FindTextFinished, this, &BrowserWindow::HandleFindTextFinished);

        QAction *focusUrlLineEditAction = new QAction(this);
        addAction(focusUrlLineEditAction);
        focusUrlLineEditAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
        connect(focusUrlLineEditAction, &QAction::triggered, this, [this] () {
            mUrlLineEdit->setFocus(Qt::ShortcutFocusReason);
        });
    }

    HandleWebViewTitleChanged(QString());
    mTabWidget->CreateTab();
}

QSize BrowserWindow::sizeHint() const
{
    QRect desktopRect = QApplication::primaryScreen()->geometry();
    QSize size = desktopRect.size() * qreal(0.9);
    return size;
}

QMenu *BrowserWindow::CreateFileMenu(TabWidget *tabWidget)
{
    QMenu *fileMenu = new QMenu(tr("&File"));
#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
    fileMenu->addAction(tr("&New Window"), QKeySequence::New, this, &BrowserWindow::HandleNewWindowTriggered);
#else
    fileMenu->addAction(tr("&New Window"), this, &BrowserWindow::HandleNewWindowTriggered, QKeySequence::New);
#endif
    fileMenu->addAction(tr("New &Incognito Window"), this, &BrowserWindow::HandleNewIncognitoWindowTriggered);

    QAction *newTabAction = new QAction(tr("New &Tab"), this);
    newTabAction->setShortcuts(QKeySequence::AddTab);
    connect(newTabAction, &QAction::triggered, this, [this]() {
        mTabWidget->CreateTab();
        mUrlLineEdit->setFocus();
    });
    fileMenu->addAction(newTabAction);

#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
    fileMenu->addAction(tr("&Open File..."), QKeySequence::Open, this, &BrowserWindow::HandleFileOpenTriggered);
#else
    fileMenu->addAction(tr("&Open File..."), this, &BrowserWindow::HandleFileOpenTriggered, QKeySequence::Open);
#endif
    fileMenu->addSeparator();

    QAction *closeTabAction = new QAction(tr("&Close Tab"), this);
    closeTabAction->setShortcuts(QKeySequence::Close);
    connect(closeTabAction, &QAction::triggered, [tabWidget]() {
        tabWidget->CloseTab(tabWidget->currentIndex());
    });
    fileMenu->addAction(closeTabAction);

    QAction *closeAction = new QAction(tr("&Quit"),this);
    closeAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Q));
    connect(closeAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(closeAction);

    connect(fileMenu, &QMenu::aboutToShow, [this, closeAction]() {
        if (mBrowser->GetWindows().count() == 1)
            closeAction->setText(tr("&Quit"));
        else
            closeAction->setText(tr("&Close Window"));
    });
    return fileMenu;
}

QMenu *BrowserWindow::CreateEditMenu()
{
    QMenu *editMenu = new QMenu(tr("&Edit"));
    QAction *findAction = editMenu->addAction(tr("&Find"));
    findAction->setShortcuts(QKeySequence::Find);
    connect(findAction, &QAction::triggered, this, &BrowserWindow::HandleFindActionTriggered);

    QAction *findNextAction = editMenu->addAction(tr("Find &Next"));
    findNextAction->setShortcut(QKeySequence::FindNext);
    connect(findNextAction, &QAction::triggered, [this]() {
        if (!GetCurrentTab() || mLastSearch.isEmpty())
            return;
        GetCurrentTab()->findText(mLastSearch);
    });

    QAction *findPreviousAction = editMenu->addAction(tr("Find &Previous"));
    findPreviousAction->setShortcut(QKeySequence::FindPrevious);
    connect(findPreviousAction, &QAction::triggered, [this]() {
        if (!GetCurrentTab() || mLastSearch.isEmpty())
            return;
        GetCurrentTab()->findText(mLastSearch, QWebEnginePage::FindBackward);
    });

    return editMenu;
}

QMenu *BrowserWindow::CreateViewMenu(QToolBar *toolbar)
{
    QMenu *viewMenu = new QMenu(tr("&View"));
    mStopAction = viewMenu->addAction(tr("&Stop"));
    QList<QKeySequence> shortcuts;
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Period));
    shortcuts.append(Qt::Key_Escape);
    mStopAction->setShortcuts(shortcuts);
    connect(mStopAction, &QAction::triggered, [this]() {
        mTabWidget->TriggerWebPageAction(QWebEnginePage::Stop);
    });

    mReloadAction = viewMenu->addAction(tr("Reload Page"));
    mReloadAction->setShortcuts(QKeySequence::Refresh);
    connect(mReloadAction, &QAction::triggered, [this]() {
        mTabWidget->TriggerWebPageAction(QWebEnginePage::Reload);
    });

    QAction *zoomIn = viewMenu->addAction(tr("Zoom &In"));
    zoomIn->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Plus));
    connect(zoomIn, &QAction::triggered, [this]() {
        if (GetCurrentTab())
            GetCurrentTab()->setZoomFactor(GetCurrentTab()->zoomFactor() + 0.1);
    });

    QAction *zoomOut = viewMenu->addAction(tr("Zoom &Out"));
    zoomOut->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Minus));
    connect(zoomOut, &QAction::triggered, [this]() {
        if (GetCurrentTab())
            GetCurrentTab()->setZoomFactor(GetCurrentTab()->zoomFactor() - 0.1);
    });

    QAction *resetZoom = viewMenu->addAction(tr("Reset &Zoom"));
    resetZoom->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    connect(resetZoom, &QAction::triggered, [this]() {
        if (GetCurrentTab())
            GetCurrentTab()->setZoomFactor(1.0);
    });


    viewMenu->addSeparator();
    QAction *viewToolbarAction = new QAction(tr("Hide Toolbar"),this);
    viewToolbarAction->setShortcut(tr("Ctrl+|"));
    connect(viewToolbarAction, &QAction::triggered, [toolbar,viewToolbarAction]() {
        if (toolbar->isVisible()) {
            viewToolbarAction->setText(tr("Show Toolbar"));
            toolbar->close();
        } else {
            viewToolbarAction->setText(tr("Hide Toolbar"));
            toolbar->show();
        }
    });
    viewMenu->addAction(viewToolbarAction);

    QAction *viewStatusbarAction = new QAction(tr("Hide Status Bar"), this);
    viewStatusbarAction->setShortcut(tr("Ctrl+/"));
    connect(viewStatusbarAction, &QAction::triggered, [this, viewStatusbarAction]() {
        if (statusBar()->isVisible()) {
            viewStatusbarAction->setText(tr("Show Status Bar"));
            statusBar()->close();
        } else {
            viewStatusbarAction->setText(tr("Hide Status Bar"));
            statusBar()->show();
        }
    });
    viewMenu->addAction(viewStatusbarAction);
    return viewMenu;
}

QMenu *BrowserWindow::CreateWindowMenu(TabWidget *tabWidget)
{
    QMenu *menu = new QMenu(tr("&Window"));

    QAction *nextTabAction = new QAction(tr("Show Next Tab"), this);
    QList<QKeySequence> shortcuts;
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceRight));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageDown));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BracketRight));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Less));
    nextTabAction->setShortcuts(shortcuts);
    connect(nextTabAction, &QAction::triggered, tabWidget, &TabWidget::NextTab);

    QAction *previousTabAction = new QAction(tr("Show Previous Tab"), this);
    shortcuts.clear();
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BraceLeft));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_PageUp));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_BracketLeft));
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::Key_Greater));
    previousTabAction->setShortcuts(shortcuts);
    connect(previousTabAction, &QAction::triggered, tabWidget, &TabWidget::PreviousTab);

    QAction *inspectorAction = new QAction(tr("Open inspector in new window"), this);
    shortcuts.clear();
    shortcuts.append(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_I));
    inspectorAction->setShortcuts(shortcuts);
    connect(inspectorAction, &QAction::triggered, [this]() { emit GetCurrentTab()->DevToolsRequested(GetCurrentTab()->page()); });

    connect(menu, &QMenu::aboutToShow, [this, menu, nextTabAction, previousTabAction, inspectorAction]() {
        menu->clear();
        menu->addAction(nextTabAction);
        menu->addAction(previousTabAction);
        menu->addSeparator();
        menu->addAction(inspectorAction);
        menu->addSeparator();

        QList<BrowserWindow*> windows = mBrowser->GetWindows();
        int index(-1);
        for (auto window : windows) {
            QAction *action = menu->addAction(window->windowTitle(), this, &BrowserWindow::HandleShowWindowTriggered);
            action->setData(++index);
            action->setCheckable(true);
            if (window == this)
                action->setChecked(true);
        }
    });
    return menu;
}

QMenu *BrowserWindow::CreateHelpMenu()
{
    QMenu *helpMenu = new QMenu(tr("&Help"));
    helpMenu->addAction(tr("About &Qt"), qApp, QApplication::aboutQt);
    return helpMenu;
}

static bool isBackspace(const QKeySequence &k)
{
    return (k[0].key() & Qt::Key_unknown) == Qt::Key_Backspace;
}

// Chromium already handles navigate on backspace when appropriate.
static QList<QKeySequence> removeBackspace(QList<QKeySequence> keys)
{
    const auto it = std::find_if(keys.begin(), keys.end(), isBackspace);
    if (it != keys.end())
        keys.erase(it);
    return keys;
}

QToolBar *BrowserWindow::CreateToolBar()
{
    QToolBar *navigationBar = new QToolBar(tr("Navigation"));
    navigationBar->setMovable(false);
    navigationBar->toggleViewAction()->setEnabled(false);

    mHistoryBackAction = new QAction(this);
    auto backShortcuts = removeBackspace(QKeySequence::keyBindings(QKeySequence::Back));
    // For some reason Qt doesn't bind the dedicated Back key to Back.
    backShortcuts.append(QKeySequence(Qt::Key_Back));
    mHistoryBackAction->setShortcuts(backShortcuts);
    mHistoryBackAction->setIconVisibleInMenu(false);
    mHistoryBackAction->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoPrevious,
                                                  QIcon(":go-previous.png")));
    mHistoryBackAction->setToolTip(tr("Go back in history"));
    connect(mHistoryBackAction, &QAction::triggered, [this]() {
        mTabWidget->TriggerWebPageAction(QWebEnginePage::Back);
    });
    navigationBar->addAction(mHistoryBackAction);

    mHistoryForwardAction = new QAction(this);
    auto fwdShortcuts = removeBackspace(QKeySequence::keyBindings(QKeySequence::Forward));
    fwdShortcuts.append(QKeySequence(Qt::Key_Forward));
    mHistoryForwardAction->setShortcuts(fwdShortcuts);
    mHistoryForwardAction->setIconVisibleInMenu(false);
    mHistoryForwardAction->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::GoNext,
                                                     QIcon(":go-next.png")));
    mHistoryForwardAction->setToolTip(tr("Go forward in history"));
    connect(mHistoryForwardAction, &QAction::triggered, [this]() {
        mTabWidget->TriggerWebPageAction(QWebEnginePage::Forward);
    });
    navigationBar->addAction(mHistoryForwardAction);

    mStopReloadAction = new QAction(this);
    connect(mStopReloadAction, &QAction::triggered, [this]() {
        mTabWidget->TriggerWebPageAction(QWebEnginePage::WebAction(mStopReloadAction->data().toInt()));
    });
    navigationBar->addAction(mStopReloadAction);

    mUrlLineEdit = new QLineEdit(this);
    mFavAction = new QAction(this);
    mUrlLineEdit->addAction(mFavAction, QLineEdit::LeadingPosition);
    mUrlLineEdit->setClearButtonEnabled(true);
    navigationBar->addWidget(mUrlLineEdit);

    auto downloadsAction = new QAction(this);
    downloadsAction->setIcon(QIcon(":go-bottom.png"));
    downloadsAction->setToolTip(tr("Show downloads"));
    navigationBar->addAction(downloadsAction);
    connect(downloadsAction, &QAction::triggered,
            &mBrowser->GetDownloadManagerWidget(), &QWidget::show);

    return navigationBar;
}

void BrowserWindow::HandleWebActionEnabledChanged(QWebEnginePage::WebAction action, bool enabled)
{
    switch (action) {
    case QWebEnginePage::Back:
        mHistoryBackAction->setEnabled(enabled);
        break;
    case QWebEnginePage::Forward:
        mHistoryForwardAction->setEnabled(enabled);
        break;
    case QWebEnginePage::Reload:
        mReloadAction->setEnabled(enabled);
        break;
    case QWebEnginePage::Stop:
        mStopAction->setEnabled(enabled);
        break;
    default:
        qWarning("Unhandled webActionChanged signal");
    }
}

void BrowserWindow::HandleWebViewTitleChanged(const QString &title)
{
    QString suffix = mProfile->isOffTheRecord()
        ? tr("Qt Simple Browser (Incognito)")
        : tr("Qt Simple Browser");

    if (title.isEmpty())
        setWindowTitle(suffix);
    else
        setWindowTitle(title + " - " + suffix);
}

void BrowserWindow::HandleNewWindowTriggered()
{
    BrowserWindow *window = mBrowser->CreateWindow();
    window->mUrlLineEdit->setFocus();
}

void BrowserWindow::HandleNewIncognitoWindowTriggered()
{
    BrowserWindow *window = mBrowser->CreateWindow(/* offTheRecord: */ true);
    window->mUrlLineEdit->setFocus();
}

void BrowserWindow::HandleFileOpenTriggered()
{
    QUrl url = QFileDialog::getOpenFileUrl(this, tr("Open Web Resource"), QString(),
                                                tr("Web Resources (*.html *.htm *.svg *.png *.gif *.svgz);;All files (*.*)"));
    if (url.isEmpty())
        return;
    GetCurrentTab()->setUrl(url);
}

void BrowserWindow::HandleFindActionTriggered()
{
    if (!GetCurrentTab())
        return;
    bool ok = false;
    QString search = QInputDialog::getText(this, tr("Find"),
                                           tr("Find:"), QLineEdit::Normal,
                                           mLastSearch, &ok);
    if (ok && !search.isEmpty()) {
        mLastSearch = search;
        GetCurrentTab()->findText(mLastSearch);
    }
}

void BrowserWindow::closeEvent(QCloseEvent *event)
{
    if (mTabWidget->count() > 1) {
        int ret = QMessageBox::warning(this, tr("Confirm close"),
                                       tr("Are you sure you want to close the window ?\n"
                                          "There are %1 tabs open.").arg(mTabWidget->count()),
                                       QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
        if (ret == QMessageBox::No) {
            event->ignore();
            return;
        }
    }
    event->accept();
    deleteLater();
}

TabWidget *BrowserWindow::GetTabWidget() const
{
    return mTabWidget;
}

WebView *BrowserWindow::GetCurrentTab() const
{
    return mTabWidget->GetCurrentWebView();
}

void BrowserWindow::HandleWebViewLoadProgress(int progress)
{
    static QIcon stopIcon = QIcon::fromTheme(QIcon::ThemeIcon::ProcessStop,
                                             QIcon(":process-stop.png"));
    static QIcon reloadIcon = QIcon::fromTheme(QIcon::ThemeIcon::ViewRefresh,
                                               QIcon(":view-refresh.png"));

    if (0 < progress && progress < 100) {
        mStopReloadAction->setData(QWebEnginePage::Stop);
        mStopReloadAction->setIcon(stopIcon);
        mStopReloadAction->setToolTip(tr("Stop loading the current page"));
        mProgressBar->setValue(progress);
    } else {
        mStopReloadAction->setData(QWebEnginePage::Reload);
        mStopReloadAction->setIcon(reloadIcon);
        mStopReloadAction->setToolTip(tr("Reload the current page"));
        mProgressBar->setValue(0);
    }
}

void BrowserWindow::HandleShowWindowTriggered()
{
    if (QAction *action = qobject_cast<QAction*>(sender())) {
        int offset = action->data().toInt();
        QList<BrowserWindow*> windows = mBrowser->GetWindows();
        windows.at(offset)->activateWindow();
        windows.at(offset)->GetCurrentTab()->setFocus();
    }
}

void BrowserWindow::HandleDevToolsRequested(QWebEnginePage *source)
{
    source->setDevToolsPage(mBrowser->CreateDevToolsWindow()->GetCurrentTab()->page());
    source->triggerAction(QWebEnginePage::InspectElement);
}

void BrowserWindow::HandleFindTextFinished(const QWebEngineFindTextResult &result)
{
    if (result.numberOfMatches() == 0) {
        statusBar()->showMessage(tr("\"%1\" not found.").arg(mLastSearch));
    } else {
        statusBar()->showMessage(tr("\"%1\" found: %2/%3").arg(mLastSearch,
                                                               QString::number(result.activeMatch()),
                                                               QString::number(result.numberOfMatches())));
    }
}
