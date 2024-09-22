#pragma once

#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QFrame>
#include <QElapsedTimer>

QT_BEGIN_NAMESPACE
class QWebEngineDownloadRequest;
QT_END_NAMESPACE

class DownloadWidget final : public QFrame
{
    Q_OBJECT
public:
    explicit DownloadWidget(QWebEngineDownloadRequest *download, QWidget *parent = nullptr);

signals:
    void RemoveClicked(DownloadWidget *self);

private slots:
    void UpdateWidget();

private:
    QGridLayout* mTopLevelLayout;
    QLabel* mDstName;
    QPushButton* mCancelButton;
    QLabel* mSrcUrl;
    QProgressBar* mProgressBar;
	void Initialize();
    QString WithUnit(qreal bytes);
    QWebEngineDownloadRequest *mDownload;
    QElapsedTimer mTimeAdded;

};
