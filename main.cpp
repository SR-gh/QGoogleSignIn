#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "qgooglesigninapplication.h"
#include "controller.h"
#include <QQmlContext>
#include "androidmessagehandler.h"
#ifdef I_DO_CARE_ABOUT_THESE_OPENSSL_WARNINGS
#include <QSslSocket>
#endif
int main(int argc, char *argv[])
{
#ifdef QGSILOG
    qInstallMessageHandler(messageHandlerWithDateTime);
#endif
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#ifdef I_DO_CARE_ABOUT_THESE_OPENSSL_WARNINGS
    qInfo()<<"SSL version use for build: "<<QSslSocket::sslLibraryBuildVersionString();
    qInfo()<<"SSL version use for run-time: "<<QSslSocket::sslLibraryVersionNumber();
    qInfo()<<QCoreApplication::libraryPaths();
#endif
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGoogleSignInApplication app(argc, argv);
    app.setOrganizationName("Idées avec 2 L");
    app.setOrganizationDomain("renan.org");
    app.setApplicationName("GoogleSignIn");
    app.init();

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("theApp", &app);
    for (Controller* controller : app.getControllers())
    {
        engine.rootContext()->setContextProperty(controller->getName(), controller->asQObject());
    }

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
