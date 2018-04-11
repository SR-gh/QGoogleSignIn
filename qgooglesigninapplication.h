#ifndef QGOOGLESIGNINAPPLICATION_H
#define QGOOGLESIGNINAPPLICATION_H

#include <QObject>
#include <QGuiApplication>

#include "qfirebase.h"
#include "qauthgsi.h"

//#include <QAndroidActivityResultReceiver>

#include <memory>

//// Firebase
//#include <QAndroidJniEnvironment>
//#include "firebase/app.h"
//#include "firebase/auth.h"
// GUI
#include <QList>
#include "controller.h"

#if defined(qGoogleSignInApp)
#undef qGoogleSignInApp
#endif
#define qGoogleSignInApp (static_cast<QGoogleSignInApplication *>(QCoreApplication::instance()))

class QGoogleSignInApplication : public QGuiApplication
{
    Q_OBJECT

public:
#ifdef Q_QDOC
    QGoogleSignInApplication(int &argc, char **argv);
#else
    QGoogleSignInApplication(int &argc, char **argv, int af = ApplicationFlags);
#endif

    // public app init : ok
    void init();

    // From Java Stuff
    QAuthGSI * completelySuspiciousGetterOfAuthGSI() { return qAuthGSI.get(); }

    // Why is it public and here ? Because it is called by Controllers.
    // We need a Sign In delegate to provide to the Controller. Do we ?
    void signIn(bool silently);
    void signOut();

    // public GUI : y not
    const QList<Controller*>& getControllers() const { return controllers; }

signals:
//    // FB
//    void firebaseAuthSucceed(firebase::auth::User* user);
//    void firebaseAuthFailed(int errorCode, QString errorMessage);

private:
    Q_DISABLE_COPY(QGoogleSignInApplication)
    void onApplicationStateChanged(Qt::ApplicationState state);
    void onGsiTokenReceived(QString tokenId);   // pas trop de raison de le mettre ici, si ? Si l'appli veut le choper, pourquoi pas. Mais comment dérancher le comportement par défaut, dans ce cas-là ?
    void onFailedRefresh(int statusCode, bool silently);
    void onSuccessfulSignOut();
private:
    std::unique_ptr<QFirebase> qFirebase;
    std::unique_ptr<QAuthGSI> qAuthGSI;

    QList<Controller*> controllers;
};

#endif // QGOOGLESIGNINAPPLICATION_H
