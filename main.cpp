#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "qgooglesigninapplication.h"
#include "controller.h"
#include <QQmlContext>
int main(int argc, char *argv[])
{
#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGoogleSignInApplication app(argc, argv);
    app.setOrganizationName("Idées avec 2 L");
    app.setOrganizationDomain("renan.org");
    app.setApplicationName("GoogleSignIn");
    app.init();

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    for (Controller* controller : app.getControllers())
    {
        engine.rootContext()->setContextProperty(controller->getName(), controller->asQObject());
    }

    return app.exec();
}
