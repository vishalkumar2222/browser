cmake_minimum_required(VERSION 3.16)
project(Browser LANGUAGES CXX)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set (CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Core Gui WebEngineWidgets)


qt_add_executable(${PROJECT_NAME} Browser.cpp Browser.h
    BrowserWindow.cpp BrowserWindow.h
    CertificateDialog.cpp CertificateDialog.h
    DownloadManagerWidget.cpp DownloadManagerWidget.h
    DownloadWidget.cpp DownloadWidget.h
    main.cpp
    PasswordDialog.h PasswordDialog.cpp
    TabWidget.cpp TabWidget.h
    WebPage.cpp WebPage.h
    WebPopUpWindow.cpp WebPopUpWindow.h
    WebView.cpp WebView.h
    WebAuthDialog.cpp WebAuthDialog.h)

set_target_properties(Browser PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt::Core
    Qt::Gui
    Qt::WebEngineWidgets
)

set(browser_resource_files
	"data/AppLogoColor.png"
	"data/ninja.png"
)

qt_add_resources(Browser "Browser"
    PREFIX
        "/"
    BASE
        "data"
    FILES
        ${browser_resource_files}
)

set(browser1_resource_files
    "data/3rdparty/dialog-error.png"
    "data/3rdparty/edit-clear.png"
    "data/3rdparty/go-bottom.png"
    "data/3rdparty/go-next.png"
    "data/3rdparty/go-previous.png"
    "data/3rdparty/process-stop.png"
    "data/3rdparty/text-html.png"
    "data/3rdparty/view-refresh.png"
)

qt_add_resources(Browser "Browser1"
    PREFIX
        "/"
    BASE
        "data/3rdparty"
    FILES
        ${Browser1_resource_files}
)