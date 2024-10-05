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
	return QUrl("www.google.com");
}

int main(int argc, char** argv){
    QApplication app(argc, argv);
	app.setWindowIcon(QIcon(QStringLiteral(":AppLogoColor.png")));
	QLoggingCategory::setFilterRules(QStringLiteral("qt.webenginecontext.debug=false"));

    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
    QWebEngineProfile::defaultProfile()->settings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, true);

	QUrl url = CommandLineUrlArgument();

    Browser browser;
	BrowserWindow* window = browser.CreateHiddenWindow();
	window->GetTabWidget()->SetUrl(url);
    window->show();
    return app.exec();
}
