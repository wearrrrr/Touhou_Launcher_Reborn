#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtLogging>
#include <QQmlContext>
#include <QThread>
#include <QPalette>
#include <QDebug>

#include "AppSettings.h"
#include "Downloader.h"
#include "Clipboard.h"
#include "FileIO.h"
#include "GameLauncher.h"
#include "Util.h"
#include "RPC.h"

int main(int argc, char *argv[])
{
    // For some reason, the stock file picker is partially broken :D, so it's GTK time until that gets fixed, or maybe forever.
    #ifdef Q_OS_LINUX
        qputenv("QT_QPA_PLATFORMTHEME", "gtk3");
    #endif
    qputenv("QML_XHR_ALLOW_FILE_READ", "1");
    QGuiApplication app(argc, argv);
    app.setOrganizationName("wearr");
    app.setOrganizationDomain("wearr.dev");
    app.setApplicationName("NineLauncher");
    // Set app version
    app.setApplicationVersion("0.0.1");
    QQmlApplicationEngine engine;
    static AppSettings settings("wearr", "NineLauncher");
    static Clipboard clipboard = Clipboard();
    static Downloader downloader = Downloader();
    static Util util = Util();
    static RPC rpc = RPC();
    engine.rootContext()->setContextProperty("AppSettings", &settings);
    engine.rootContext()->setContextProperty("Clipboard", &clipboard);
    engine.rootContext()->setContextProperty("Downloader", &downloader);
    engine.rootContext()->setContextProperty("QtVersion", QString(qVersion()));
    engine.rootContext()->setContextProperty("Util", &util);
    // RPC is initialized when the settings are loaded in the QML 
    engine.rootContext()->setContextProperty("RPC", &rpc);

    engine.addImportPath(":/MMaterial");
    engine.addImportPath("qrc:/");

    qmlRegisterType<Downloader>("Downloader", 1, 0, "Downloader");
    qmlRegisterType<FileIO>("FileIO", 1, 0, "FileIO");
    qmlRegisterType<GameLauncher>("GameLauncher", 1, 0, "GameLauncher");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection
    );
    
    engine.loadFromModule("nineLauncher", "Main");

    int ret = app.exec();

    settings.sync();

    return ret;
}
