#include <QApplication>
#include <QLoggingCategory>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

#include "MainWindow.h"


int main(int argc, char** argv){
    QApplication app(argc, argv);

    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, true);

    MainWindow window;
    window.show();
    return app.exec();
}
