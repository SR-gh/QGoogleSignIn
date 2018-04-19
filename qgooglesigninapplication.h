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
    void signInWithGSI(bool silently);
    void signInAnonymously();
    void signOut();

    // public GUI : y not
    const QList<Controller*>& getControllers() const { return controllers; }

    void setHandlingActivityResult(bool value);

signals:
    //    // FB
    //    void firebaseAuthSucceed(firebase::auth::User* user);
//    void firebaseAuthFailed(int errorCode, QString errorMessage);

private:
    Q_DISABLE_COPY(QGoogleSignInApplication)
    void onApplicationStateChanged(Qt::ApplicationState state);
    void onGsiTokenReceived(QString tokenId);   // pas trop de raison de le mettre ici, si ? Si l'appli veut le choper, pourquoi pas. Mais comment dérancher le comportement par défaut, dans ce cas-là ?
    void onGsiTokenRequestFailed(int resultCode, QSharedPointer<const QAndroidJniObject> jniObject);   // pas trop de raison de le mettre ici, si ? Si l'appli veut le choper, pourquoi pas. Mais comment dérancher le comportement par défaut, dans ce cas-là ?
    void onFailedRefresh(int statusCode, bool silently);
    void onSuccessfulSignOut();

    void onFirebaseAuthSucceed(firebase::auth::User* user, int authType); // firebase namespace used in application : pros/cons. No need to duplicate Firebase models IMHO, so let's use em. App will encapsulate them anyway.
    void onFirebaseAuthFailed(int errorCode, QString errorMessage);

private:
    std::unique_ptr<QFirebase> qFirebase;
    std::unique_ptr<QAuthGSI> qAuthGSI;

    QList<Controller*> controllers;
    QFirebase::AuthType lastSuccessfulAuthType = QFirebase::AuthType::UNDEFINED;
    bool handlingActivityResult = false;    // this flag to handle the case when app respawns and handles both onActivityResult AND onApplicationStateChanged. Only one signin should be attempted.
};

#endif // QGOOGLESIGNINAPPLICATION_H
