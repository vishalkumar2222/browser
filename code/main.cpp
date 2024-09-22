#include <QApplication>
#include <QLoggingCategory>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

#include "Browser.h"
#include "BrowserWindow.h"
#include "TabWidget.h"

QUrl CommandLineUrlArgument() {
	const QStringList args = QCoreApplication::arguments();
	for (const QString& arg : args.mid(1)) {
		if (!arg.startsWith(QLatin1Char('-')))
			return QUrl::fromUserInput(arg);
	}
	return QUrl("chrome://qt");
}

int main(int argc, char** argv){
    QApplication app(argc, argv);
	QLoggingCategory::setFilterRules(QStringLiteral("qt.network.ssl.warning=false"));
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, true);

	QUrl url = CommandLineUrlArgument();

    Browser browser;
	BrowserWindow* window = browser.CreateHiddenWindow(false);
	window->GetTabWidget()->SetUrl(url);
    window->show();
    return app.exec();
}
