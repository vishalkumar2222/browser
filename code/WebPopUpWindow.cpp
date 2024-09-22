#include "WebPage.h"
#include "WebPopUpWindow.h"
#include "WebView.h"
#include <QAction>
#include <QIcon>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWindow>

WebPopupWindow::WebPopupWindow(QWebEngineProfile *profile)
    : mUrlLineEdit(new QLineEdit(this))
    , mFavAction(new QAction(this))
    , mView(new WebView(this))
{
    setAttribute(Qt::WA_DeleteOnClose);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    layout->addWidget(mUrlLineEdit);
    layout->addWidget(mView);

    mView->setPage(new WebPage(profile, mView));
    mView->setFocus();

    mUrlLineEdit->setReadOnly(true);
    mUrlLineEdit->addAction(mFavAction, QLineEdit::LeadingPosition);

    connect(mView, &WebView::titleChanged, this, &QWidget::setWindowTitle);
    connect(mView, &WebView::urlChanged, [this](const QUrl &url) {
        mUrlLineEdit->setText(url.toDisplayString());
    });
    connect(mView, &WebView::FavIconChanged, mFavAction, &QAction::setIcon);
    connect(mView->page(), &WebPage::geometryChangeRequested, this, &WebPopupWindow::HandleGeometryChangeRequested);
    connect(mView->page(), &WebPage::windowCloseRequested, this, &QWidget::close);
}

WebView *WebPopupWindow::View() const
{
    return mView;
}

void WebPopupWindow::HandleGeometryChangeRequested(const QRect &newGeometry)
{
    if (QWindow *window = windowHandle())
        setGeometry(newGeometry.marginsRemoved(window->frameMargins()));
    show();
    mView->setFocus();
}
