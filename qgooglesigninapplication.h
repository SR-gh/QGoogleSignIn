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
    Q_PROPERTY(QUser* user READ getUser NOTIFY userChanged) // NOTIFY signal mandatory. See Issue #18.
    // Extended properties, not mandatory for the POC.
    Q_PROPERTY(QVariantList userInfo READ getUserInfo NOTIFY userInfoChanged)

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

    // extended property related
    const QVariantList& getUserInfo() const;

signals:
    void error(const QString errorMessage);
    void applicationInitializedChanged(bool);
    void userChanged();

    // extended property related
    void userInfoChanged();

private:
    Q_DISABLE_COPY(QGoogleSignInApplication)
    void onApplicationStateChanged(Qt::ApplicationState state);
    void onGsiTokenReceived(QString tokenId, QAuthGSI::GSIJavaIntent reason);
    void onGsiTokenRequestFailed(int resultCode, QSharedPointer<const QAndroidJniObject> jniObject);
    void onFailedRefresh(int statusCode, bool silently);
    void onSuccessfulSignOut();

    // @deprecated
    void onFirebaseAuthSucceed();
    void onFirebaseAuthFailed(int errorCode, QString errorMessage);
    // @deprecated
    void onFirebaseAuthLinkSucceed();
    void onFirebaseAuthLinkFailed(int errorCode, QString errorMessage);
    void onFirebaseInitializationComplete(firebase::InitResult result); // firebase namespace used in application : pros/cons. No need to duplicate Firebase models IMHO, so let's use em. App will encapsulate them anyway.

    void onAuthStateChanged(PointerContainer<firebase::auth::Auth>);
    void onIdTokenChanged(PointerContainer<firebase::auth::Auth>);

private:
    // Extension functions for the application.
    void onIdTokenChangedUserInfo(PointerContainer<firebase::auth::Auth>);

private:
    bool checkEmailAndPassword(const QString& email,const QString& password) const;

private:
    bool isFirebaseInitialized() const;
    void setFirebaseInitialized(bool b);

    QFirebase* qFirebase = nullptr;   // deleted by Qt as a child of this.
    QAuthGSI* qAuthGSI = nullptr;     // deleted by Qt as a child of this.

    bool firebaseInitialized = false;

    QList<Controller*> controllers;

    QUser m_user;
    QVariantList m_userInfo;
};

#endif // QGOOGLESIGNINAPPLICATION_H
