#pragma once

#include <QWidget>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QWebEngineProfile;
class QWebEngineView;
QT_END_NAMESPACE

class WebView;

class WebPopupWindow : public QWidget
{
    Q_OBJECT

public:
    explicit WebPopupWindow(QWebEngineProfile *profile);
    WebView *View() const;

private slots:
    void HandleGeometryChangeRequested(const QRect &newGeometry);

private:
    QLineEdit *mUrlLineEdit;
    QAction *mFavAction;
    WebView *mView;
};
