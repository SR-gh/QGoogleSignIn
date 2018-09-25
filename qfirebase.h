#ifndef QFIREBASE_H
#define QFIREBASE_H

#include <QGuiApplication>
#include <memory>
// Firebase
#include <QAndroidJniEnvironment>
#include "firebase/app.h"
#include "firebase/auth.h"

#include "helper.h"

class QFirebase : public QObject
{
    Q_OBJECT

    class QFirebaseAuthListener : public firebase::auth::AuthStateListener, public firebase::auth::IdTokenListener
    {
        QFirebase * caller;
    public:
        explicit QFirebaseAuthListener(QFirebase* caller);
        ~QFirebaseAuthListener() override=default;
        void OnAuthStateChanged(firebase::auth::Auth* auth) override;
        void OnIdTokenChanged(firebase::auth::Auth* auth) override;
    };

    // Meta-type registering
    struct QFirebaseRegisterer
    {
        QFirebaseRegisterer();
    };
    static const QFirebaseRegisterer registerer;
public:
    explicit QFirebase(QObject *parent);
    ~QFirebase() override=default;

    enum AuthType
    {
        UNDEFINED,      // When there is no known authentication type
        ANONYMOUS,      // Firebase Anonymous authentication
        GSI,            // Google Sign In authentication
        PASSWORD,       // login+password authentication
        NO_SIGN_IN,    // When no authentication is voluntarily used
    };

    // Interface

    // Must be called before any other function call to this instance.
    // init is responsible of :
    // creating internal state
    // asking the underlying implementation (Firebase) to nicely initialize itself
    // It could had been inserted in the constructor, but :
    // this cumbersomly adds a constraint. Init emits a signal on completion, as
    // it is a possibly asynchronous task. If initialization finishes early, a signal could
    // be emitted synchronously. We can't tell. As no connection outside the constructor
    // can be done to these signals before construction is acheived, we would have to check
    // (using QObject::connectNotify() for instance) that no receiver has missed the
    // initializationComplete() signal. Another implementation would be to force the
    // creator to pass an object having an interface with a slot to connect the signal
    // to, before it may be emitted.
    // In all cases, as the initialization may be asynchronous and take a lot of time
    // for reasons beyond the scope of the application (for instance initialization asking the
    // user to install third party services), we have to be aware in the application
    // that this part may not be ready to be used.
    // We could add some defensive code to each function in order to check that initialization
    // is complete and successful, but it is useless, as the only thing we could do
    // is to inform the caller that we're not ready. So the application has, in any case,
    // to deal with this initialization state.
    // It is the responsibility of the application to choose its behaviour when this
    // part is not available. The application is informed either if the init is successful or
    // not.
    // We also could make this initialization blocking the application, but it is equivalent,
    // from the application point of view, to wait for a signal to be emitted, doing nothing
    // else waiting. It is possible for the application to implement such a behaviour.
    void init();  // This one may evolve in order to pass more parameters

    void signInWithGSI(QString tokenId);
    void signInWithEmail(QString email, QString password);
    void signUpWithEmail(QString email, QString password);
    void signInAnonymously();
    void signOut();
    void linkWithGSI(QString tokenId);
    void linkWithEmail(QString email, QString password);

    firebase::auth::User* getUser();

signals:
    // FB
    void firebaseAuthSucceed(firebase::auth::User* user, int authType);
    void firebaseAuthFailed(int errorCode, QString errorMessage);
    void firebaseAuthLinkSucceed(firebase::auth::User* user, int authType);
    void firebaseAuthLinkFailed(int errorCode, QString errorMessage);
    // emitted on initialization completion. Any call to a function of a QFirebase instance
    // before obtaining a successful result from this signal has undefined behaviour.
    void firebaseInitializationCompleted(firebase::InitResult result);
    // Reemitted from Firebase
    void authStateChanged(PointerContainer<firebase::auth::Auth>);
    void idTokenChanged(PointerContainer<firebase::auth::Auth>);

private:
    void linkWithCredentials(firebase::auth::Credential& credential, QFirebase::AuthType authType);
    void signInWithCredentials(firebase::auth::Credential& credential);
private:
    void whenFirebaseInitializationCompletes(firebase::InitResult result);
private:
    //Firebase
    QAndroidJniEnvironment m_qjniEnv;
    std::unique_ptr<firebase::App> m_firebaseApp; // must be deleted before m_qjniEnv, hence order of declaration matters.
    firebase::auth::Auth* m_firebaseAuth = nullptr; // non owning
    std::unique_ptr<QFirebaseAuthListener> firebaseAuthListener; // must be deleted before m_qjniEnv, hence order of declaration matters.
    std::unique_ptr<QFirebaseAuthListener> firebaseAuthTokenListener; // must be deleted before m_qjniEnv, hence order of declaration matters.
};
Q_DECLARE_METATYPE(PointerContainer<firebase::auth::Auth>)

#endif // QFIREBASE_H
