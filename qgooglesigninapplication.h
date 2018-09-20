#ifndef QGOOGLESIGNINAPPLICATION_H
#define QGOOGLESIGNINAPPLICATION_H

#include <QObject>
#include <QGuiApplication>

#include "qfirebase.h"
#include "qauthgsi.h"
#include "quser.h"

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
    Q_PROPERTY(bool applicationInitialized READ isFirebaseInitialized WRITE setFirebaseInitialized NOTIFY applicationInitializedChanged)
    Q_PROPERTY(QUser* user READ getUser)

public:
#ifdef Q_QDOC
    QGoogleSignInApplication(int &argc, char **argv);
#else
    QGoogleSignInApplication(int &argc, char **argv, int af = ApplicationFlags);
#endif
    // public app init : ok
    void init();

    // From Java Stuff
    QAuthGSI * completelySuspiciousGetterOfAuthGSI() { return qAuthGSI; }

    // Why is it public and here ? Because it is called by Controllers.
    // We need a Sign In delegate to provide to the Controller. Do we ?
    void signInWithGSI(bool silently);
    void signInWithEmail(QString email, QString password);
    void signUpWithEmail(QString email, QString password);
    void signInAnonymously();
    void signOut();

    // public GUI : y not
    const QList<Controller*>& getControllers() const { return controllers; }

    void setHandlingActivityResult(bool value);

    // proprerty-related
    const QUser *getUser() const;
    QUser *getUser();
    void setUser(QUser *user);

signals:
    //    // FB
    //    void firebaseAuthSucceed(firebase::auth::User* user);
//    void firebaseAuthFailed(int errorCode, QString errorMessage);
    void error(const QString errorMessage);
    void applicationInitializedChanged(bool);
    void userChanged();
private:
    Q_DISABLE_COPY(QGoogleSignInApplication)
    void onApplicationStateChanged(Qt::ApplicationState state);
    void onGsiTokenReceived(QString tokenId, QAuthGSI::GSIJavaIntent reason);   // pas trop de raison de le mettre ici, si ? Si l'appli veut le choper, pourquoi pas. Mais comment dérancher le comportement par défaut, dans ce cas-là ?
    void onGsiTokenRequestFailed(int resultCode, QSharedPointer<const QAndroidJniObject> jniObject);
    void onFailedRefresh(int statusCode, bool silently);
    void onSuccessfulSignOut();

    void onFirebaseAuthSucceed(firebase::auth::User* user, int authType); // firebase namespace used in application : pros/cons. No need to duplicate Firebase models IMHO, so let's use em. App will encapsulate them anyway.
    void onFirebaseAuthFailed(int errorCode, QString errorMessage);
    void onFirebaseAuthLinkSucceed(firebase::auth::User* user, int authType);
    void onFirebaseAuthLinkFailed(int errorCode, QString errorMessage);
    void onFirebaseInitializationComplete(firebase::InitResult result);

    void onAuthStateChanged(PointerContainer<firebase::auth::Auth>);
    void onIdTokenChanged(PointerContainer<firebase::auth::Auth>);

private:
    bool isFirebaseInitialized() const;
    void setFirebaseInitialized(bool b);

    QFirebase* qFirebase = nullptr;   // child deletion.
    QAuthGSI* qAuthGSI = nullptr;     // child deletion.

    bool firebaseInitialized = false;
    QFirebase::AuthType lastSuccessfulAuthType = QFirebase::AuthType::UNDEFINED;
    bool handlingActivityResult = false;    // this flag to handle the case when app respawns and handles both onActivityResult AND onApplicationStateChanged. Only one signin should be attempted.

    QList<Controller*> controllers;

    QUser m_user;
};

#endif // QGOOGLESIGNINAPPLICATION_H
