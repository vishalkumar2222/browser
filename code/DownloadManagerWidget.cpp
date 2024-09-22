#include "DownloadManagerWidget.h"

#include "Browser.h"
#include "BrowserWindow.h"
#include "DownloadWidget.h"
#include <QFileDialog>
#include <QDir>
#include <QWebEngineDownloadRequest>

DownloadManagerWidget::DownloadManagerWidget(QWidget *parent)
    : QWidget(parent)
{
	Initialize();
}

void DownloadManagerWidget::Initialize()
{
    this->setStyleSheet(QString::fromUtf8("#DownloadManagerWidget {\n"
        "    background: palette(button)\n"
        "}"));
	this->setWindowTitle("Downloads");
    mTopLevelLayout = new QVBoxLayout(this);
    mTopLevelLayout->setSizeConstraint(QLayout::SetNoConstraint);
    mTopLevelLayout->setContentsMargins(0, 0, 0, 0);
    mScrollArea = new QScrollArea(this);
    mScrollArea->setObjectName("mScrollArea");
    mScrollArea->setStyleSheet(QString::fromUtf8("#mScrollArea {\n"
        "  margin: 2px;\n"
        "  border: none;\n"
        "}"));
    mScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    mScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mScrollArea->setWidgetResizable(true);
    mScrollArea->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignTop);
    mItems = new QWidget();
    mItems->setObjectName("mItems");
    mItems->setStyleSheet(QString::fromUtf8("#mItems {background: palette(mid)}"));
    mItemsLayout = new QVBoxLayout(mItems);
    mItemsLayout->setSpacing(2);
    mItemsLayout->setContentsMargins(3, 3, 3, 3);
    mZeroItemsLabel = new QLabel(mItems);
    QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(mZeroItemsLabel->sizePolicy().hasHeightForWidth());
    mZeroItemsLabel->setSizePolicy(sizePolicy);
    mZeroItemsLabel->setStyleSheet(QString::fromUtf8("color: palette(shadow)"));
    mZeroItemsLabel->setAlignment(Qt::AlignCenter);

    mItemsLayout->addWidget(mZeroItemsLabel);

    mScrollArea->setWidget(mItems);

    mTopLevelLayout->addWidget(mScrollArea);

    mZeroItemsLabel->setText("No downloads");
}

void DownloadManagerWidget::DownloadRequested(QWebEngineDownloadRequest *download)
{
    Q_ASSERT(download && download->state() == QWebEngineDownloadRequest::DownloadRequested);

    QString path = QFileDialog::getSaveFileName(this, tr("Save as"), QDir(download->downloadDirectory()).filePath(download->downloadFileName()));
    if (path.isEmpty())
        return;

    download->setDownloadDirectory(QFileInfo(path).path());
    download->setDownloadFileName(QFileInfo(path).fileName());
    download->accept();
    Add(new DownloadWidget(download));

    show();
}

void DownloadManagerWidget::Add(DownloadWidget *downloadWidget)
{
    connect(downloadWidget, &DownloadWidget::RemoveClicked, this, &DownloadManagerWidget::Remove);
    mItemsLayout->insertWidget(0, downloadWidget, 0, Qt::AlignTop);
    if (mNumDownloads++ == 0)
        mZeroItemsLabel->hide();
}

void DownloadManagerWidget::Remove(DownloadWidget *downloadWidget)
{
    mItemsLayout->removeWidget(downloadWidget);
    downloadWidget->deleteLater();
    if (--mNumDownloads == 0)
        mZeroItemsLabel->show();
}
