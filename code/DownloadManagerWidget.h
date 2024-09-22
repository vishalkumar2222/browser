#pragma once
#include <QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE
class QWebEngineDownloadRequest;
QT_END_NAMESPACE

class DownloadWidget;

class DownloadManagerWidget final : public QWidget
{
    Q_OBJECT
public:
    explicit DownloadManagerWidget(QWidget *parent = nullptr);

    void DownloadRequested(QWebEngineDownloadRequest *webItem);

private:
    void Add(DownloadWidget *downloadWidget);
    void Remove(DownloadWidget *downloadWidget);
	void Initialize();
    int mNumDownloads = 0;
    QVBoxLayout* mTopLevelLayout;
    QScrollArea* mScrollArea;
    QWidget* mItems;
    QVBoxLayout* mItemsLayout;
    QLabel* mZeroItemsLabel;
};
