#include <QApplication>
#include <QLoggingCategory>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

#include "BrowserCore.h"
#include "MainWindow.h"


int main(int argc, char** argv){
    QApplication app(argc, argv);

    return app.exec();
}
