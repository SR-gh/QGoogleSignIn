#include "qgooglesigninapplication.h"
#include <qdebug.h>
#include "qmaincontroller.h"

//const QString QGoogleSignInApplication::WAIT_FOR_ASYNC_ANSWER = "wait_async";

#ifdef Q_QDOC
    QGoogleSignInApplication::QGoogleSignInApplication(int &argc, char **argv) : QGuiApplication(argc, argv)
#else
    QGoogleSignInApplication::QGoogleSignInApplication(int &argc, char **argv, int af) : QGuiApplication(argc, argv, af)
#endif
{

}
void QGoogleSignInApplication::init()
{
    qFirebase = std::unique_ptr<QFirebase>(new QFirebase(this));
    qAuthGSI = std::unique_ptr<QAuthGSI>(new QAuthGSI(this));
    // connections
    connect(this, &QGoogleSignInApplication::applicationStateChanged, this, &QGoogleSignInApplication::onApplicationStateChanged);
    // Firebase
    // auth
#if defined(__ANDROID__)
    // specific Android setup
    // Firebase should be considered available only on completion of this (possibly)
    // asynchronous initialization.
    // Either the application can do something while waiting or not.
    // IMO, it should not hang and should be able to display its waiting
    // status and be left if asked to.
#else
    // Unused code !
  firebase::App* app = firebase::App::Create(firebase::AppOptions());
#endif  // defined(__ANDROID__)
    connect(qAuthGSI.get(), &QAuthGSI::gsiTokenReceived, this, &QGoogleSignInApplication::onGsiTokenReceived);
    connect(qAuthGSI.get(), &QAuthGSI::failedRefresh, this, &QGoogleSignInApplication::onFailedRefresh);
    connect(qAuthGSI.get(), &QAuthGSI::successfulSignOut, this, &QGoogleSignInApplication::onSuccessfulSignOut);

    // GUI controllers
    controllers.append(new QMainController("ctrlMain", this));

}

void QGoogleSignInApplication::signIn(bool silently)
{
    qAuthGSI->signIn(silently);
}

void QGoogleSignInApplication::signOut()
{
    qFirebase->signOut();
    qAuthGSI->signOut();
}

void QGoogleSignInApplication::onApplicationStateChanged(Qt::ApplicationState state)
{
    switch (state)
    {
    case Qt::ApplicationActive:
        qInfo() << "onApplicationStateChanged(ApplicationActive=" << state << ")";
        // Sign In with GSI, silently
        signIn(true);
        break;
    case Qt::ApplicationSuspended:
        qInfo() << "onApplicationStateChanged(ApplicationSuspended=" << state << ")";
        break;
    default:
        qInfo() << "onApplicationStateChanged(unhandled state=" << state << ")";
        break;
    }

}

void QGoogleSignInApplication::onGsiTokenReceived(QString tokenId)
{
    qFirebase->signInWithGSI(tokenId);
}

void QGoogleSignInApplication::onFailedRefresh(int statusCode, bool silently)
{
    // TODO : application state is in our Firebase user (tbd)
    // so, we do not have to transmit more state. I think. I believe. I guess. I don't know.
    if (!silently)
    {
        // avoid loops : should we pass a counter as well ? or anything that informs us that
        // we should not retry. A counter looks good to me. Must think.
        // A reference to a C++ thing that is sent back from Java looks dangerous (activities
        // die and our QtActivity state should be OK to handle any case).
        signIn(false);
    }
}

void QGoogleSignInApplication::onSuccessfulSignOut()
{
    // handle state, if necessary
    qInfo() << "User successfuly signed out of the application";
}
