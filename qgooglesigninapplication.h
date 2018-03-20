#ifndef QGOOGLESIGNINAPPLICATION_H
#define QGOOGLESIGNINAPPLICATION_H

#include <QObject>
#include <QGuiApplication>
#include <QAndroidActivityResultReceiver>

#include <memory>

// Firebase
#include <QAndroidJniEnvironment>
#include "firebase/app.h"
#include "firebase/auth.h"
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
    enum
    {
        QGSI_SIGN_IN = 1973
    }GSIJavaIntent;
    enum
    {
        ERROR_INVALID_ACCOUNT = 0x10001,
        ERROR_INVALID_TOKEN = 0x10002,
    } GSIJavaErrorCodes;
    enum
    {
        ERROR_UNKNOWN = 0x20001,    // We do not get an informative error code in all cases.
    } GSICppErrorCodes;
    struct QGSIAARR : public QAndroidActivityResultReceiver
    {
        static constexpr int RESULT_OK = -1;
        QGSIAARR(QGoogleSignInApplication * p_caller) : caller(p_caller) {}
        QGoogleSignInApplication * caller = nullptr;
        void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data) override;
    };

public:
    static const QString WAIT_FOR_ASYNC_ANSWER;
#ifdef Q_QDOC
    QGoogleSignInApplication(int &argc, char **argv);
#else
    QGoogleSignInApplication(int &argc, char **argv, int af = ApplicationFlags);
#endif

    // public app init : ok
    void init();

    // GSI or FB stuff
    // Why is it public and here ? Because it is called by JNI handlers.
    void startGSIIntent();
    // Why is it public and here ? Because it is called by Controllers.
    // We need a Sign In delegate to provide to the Controller. Do we ?
    void signIn(bool silently);
    void signOut();

    // public GUI : y not
    const QList<Controller*>& getControllers() const { return controllers; }

signals:
    // GSI
    void gsiTokenReceived(QString tokenId);
    void failedRefresh(int statusCode, bool silently);
    void successfulSignOut();
    // FB
    void firebaseAuthSucceed(firebase::auth::User* user);
    void firebaseAuthFailed(int errorCode, QString errorMessage);

private:
    Q_DISABLE_COPY(QGoogleSignInApplication)
    void onApplicationStateChanged(Qt::ApplicationState state);
    void onGsiTokenReceived(QString tokenId);
    void onFailedRefresh(int statusCode, bool silently);
    void onSuccessfulSignOut();
private:
    // GSI
    std::unique_ptr<QGSIAARR> activityReceiver;
    //Firebase
    QAndroidJniEnvironment m_qjniEnv;
    std::unique_ptr<firebase::App> m_firebaseApp; // must be deleted before m_qjniEnv, hence order of declaration matters.
    firebase::auth::Auth* m_firebaseAuth = nullptr; // non owning
    QList<Controller*> controllers;
};

#endif // QGOOGLESIGNINAPPLICATION_H
