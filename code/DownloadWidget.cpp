#include "DownloadWidget.h"

#include <QFileInfo>
#include <QUrl>
#include <QWebEngineDownloadRequest>

DownloadWidget::DownloadWidget(QWebEngineDownloadRequest *download, QWidget *parent)
    : QFrame(parent)
    , mDownload(download)
    , mTimeAdded()
{
    mTimeAdded.start();
	Initialize();
    mDstName->setText(mDownload->downloadFileName());
    mSrcUrl->setText(mDownload->url().toDisplayString());

    connect(mCancelButton, &QPushButton::clicked,
            [this](bool) {
        if (mDownload->state() == QWebEngineDownloadRequest::DownloadInProgress)
            mDownload->cancel();
        else
            emit RemoveClicked(this);
    });

    connect(mDownload, &QWebEngineDownloadRequest::totalBytesChanged, this, &DownloadWidget::UpdateWidget);
    connect(mDownload, &QWebEngineDownloadRequest::receivedBytesChanged, this, &DownloadWidget::UpdateWidget);

    connect(mDownload, &QWebEngineDownloadRequest::stateChanged,
            this, &DownloadWidget::UpdateWidget);

    UpdateWidget();
}

void DownloadWidget::Initialize()
{
    this->setStyleSheet(QString::fromUtf8("#DownloadWidget {\n"
        "  background: palette(button);\n"
        "  border: 1px solid palette(dark);\n"
        "  margin: 0px;\n"
        "}"));
    mTopLevelLayout = new QGridLayout(this);
    mTopLevelLayout->setObjectName("mTopLevelLayout");
    mTopLevelLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    mDstName = new QLabel(this);
    mDstName->setObjectName("mDstName");
    mDstName->setStyleSheet(QString::fromUtf8("font-weight: bold\n"
        ""));

    mTopLevelLayout->addWidget(mDstName, 0, 0, 1, 1);

    mCancelButton = new QPushButton(this);
    mCancelButton->setObjectName("mCancelButton");
    QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(mCancelButton->sizePolicy().hasHeightForWidth());
    mCancelButton->setSizePolicy(sizePolicy);
    mCancelButton->setStyleSheet(QString::fromUtf8("QPushButton {\n"
        "  margin: 1px;\n"
        "  border: none;\n"
        "}\n"
        "QPushButton:pressed {\n"
        "  margin: none;\n"
        "  border: 1px solid palette(shadow);\n"
        "  background: palette(midlight);\n"
        "}"));
    mCancelButton->setFlat(false);

    mTopLevelLayout->addWidget(mCancelButton, 0, 1, 1, 1);

    mSrcUrl = new QLabel(this);
    mSrcUrl->setObjectName("mSrcUrl");
    mSrcUrl->setMaximumSize(QSize(350, 16777215));
    mSrcUrl->setStyleSheet(QString::fromUtf8(""));

    mTopLevelLayout->addWidget(mSrcUrl, 1, 0, 1, 2);

    mProgressBar = new QProgressBar(this);
    mProgressBar->setObjectName("mProgressBar");
    mProgressBar->setStyleSheet(QString::fromUtf8("font-size: 12px"));
    mProgressBar->setValue(24);

    mTopLevelLayout->addWidget(mProgressBar, 2, 0, 1, 2);

    mDstName->setText("TextLabel");
    mSrcUrl->setText("TextLabel");
}

inline QString DownloadWidget::WithUnit(qreal bytes)
{
    if (bytes < (1 << 10))
        return tr("%L1 B").arg(bytes);
    if (bytes < (1 << 20))
        return tr("%L1 KiB").arg(bytes / (1 << 10), 0, 'f', 2);
    if (bytes < (1 << 30))
        return tr("%L1 MiB").arg(bytes / (1 << 20), 0, 'f', 2);
    return tr("%L1 GiB").arg(bytes / (1 << 30), 0, 'f', 2);
}

void DownloadWidget::UpdateWidget()
{
    qreal totalBytes = mDownload->totalBytes();
    qreal receivedBytes = mDownload->receivedBytes();
    qreal bytesPerSecond = receivedBytes / mTimeAdded.elapsed() * 1000;

    auto state = mDownload->state();
    switch (state) {
    case QWebEngineDownloadRequest::DownloadRequested:
        Q_UNREACHABLE();
        break;
    case QWebEngineDownloadRequest::DownloadInProgress:
        if (totalBytes > 0) {
            mProgressBar->setValue(qRound(100 * receivedBytes / totalBytes));
            mProgressBar->setDisabled(false);
            mProgressBar->setFormat(
                tr("%p% - %1 of %2 downloaded - %3/s")
                .arg(WithUnit(receivedBytes), WithUnit(totalBytes),
                     WithUnit(bytesPerSecond)));
        } else {
            mProgressBar->setValue(0);
            mProgressBar->setDisabled(false);
            mProgressBar->setFormat(
                tr("unknown size - %1 downloaded - %2/s")
                .arg(WithUnit(receivedBytes), WithUnit(bytesPerSecond)));
        }
        break;
    case QWebEngineDownloadRequest::DownloadCompleted:
        mProgressBar->setValue(100);
        mProgressBar->setDisabled(true);
        mProgressBar->setFormat(
            tr("completed - %1 downloaded - %2/s")
            .arg(WithUnit(receivedBytes), WithUnit(bytesPerSecond)));
        break;
    case QWebEngineDownloadRequest::DownloadCancelled:
        mProgressBar->setValue(0);
        mProgressBar->setDisabled(true);
        mProgressBar->setFormat(
            tr("cancelled - %1 downloaded - %2/s")
            .arg(WithUnit(receivedBytes), WithUnit(bytesPerSecond)));
        break;
    case QWebEngineDownloadRequest::DownloadInterrupted:
        mProgressBar->setValue(0);
        mProgressBar->setDisabled(true);
        mProgressBar->setFormat(
            tr("interrupted: %1")
            .arg(mDownload->interruptReasonString()));
        break;
    }

    if (state == QWebEngineDownloadRequest::DownloadInProgress) {
        static QIcon cancelIcon(QIcon::fromTheme(QIcon::ThemeIcon::ProcessStop,
                                                 QIcon(":process-stop.png")));
        mCancelButton->setIcon(cancelIcon);
        mCancelButton->setToolTip(tr("Stop downloading"));
    } else {
        static QIcon removeIcon(QIcon::fromTheme(QIcon::ThemeIcon::EditClear,
                                                 QIcon(":edit-clear.png")));
        mCancelButton->setIcon(removeIcon);
        mCancelButton->setToolTip(tr("Remove from list"));
    }
}
