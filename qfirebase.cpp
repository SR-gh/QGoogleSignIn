#include "qfirebase.h"
#include <QAndroidJniObject>
#include <QtAndroid>
#include <qdebug.h>
#include <tuple>
#include <firebase/util.h>
#include <memory>

const QFirebase::QFirebaseRegisterer QFirebase::registerer;

QFirebase::QFirebase(QObject *parent) : QObject(parent),
    firebaseAuthListener(std::make_unique<QFirebaseAuthListener>(this)),
    firebaseAuthTokenListener(std::make_unique<QFirebaseAuthListener>(this))
{
    connect(this, &QFirebase::firebaseInitializationPartiallyCompleted, this, &QFirebase::onFirebaseInitializationPartiallyCompleted);
}

void QFirebase::init()
{
#if defined(__ANDROID__)
    // From : https://firebase.google.com/docs/reference/cpp/class/firebase/app
    // New App instance, the App should not be destroyed for the lifetime of the application.
    //
    // Maybe this is not a good idea, this unique_ptr. But the doc says should, not must, so…
    m_firebaseApp = std::unique_ptr<firebase::App>(firebase::App::Create(firebase::AppOptions(), m_qjniEnv.operator ->(), QtAndroid::androidActivity().object()));
#else
    // Unused code ! Left as a reminder for other platforms
    m_firebaseApp = std::unique_ptr<firebase::App>(firebase::App::Create(firebase::AppOptions()));
#endif  // defined(__ANDROID__)

    qInfo() << "Starting FB init";
    ::firebase::ModuleInitializer initializer;
    initializer.Initialize(m_firebaseApp.get(), this, [](::firebase::App* app, void* context)
                            {
                                (void) app;
                                auto myThis = static_cast<QFirebase*>(context);
                                ::firebase::InitResult init_result;
                                ::firebase::auth::Auth::GetAuth(myThis->m_firebaseApp.get(), &init_result);
                                qInfo() << "Finished FB init with result=" << init_result;
                                emit myThis->firebaseInitializationPartiallyCompleted(init_result, QPrivateSignal());
                                return init_result;
    });
}

void QFirebase::signInWithGSI(QString tokenId)
{
    qInfo() << "Firebase GSI Sign In";
    firebase::auth::Credential credential =
        firebase::auth::GoogleAuthProvider::GetCredential(tokenId.toUtf8().data(), nullptr);
    signInWithCredentials(credential);
}

void QFirebase::signInWithCredentials(firebase::auth::Credential &credential)
{
    qInfo() << "Before SignInAndRetrieveDataWithCredential()";
    firebase::Future<firebase::auth::SignInResult> result =
        m_firebaseAuth->SignInAndRetrieveDataWithCredential(credential);
    qInfo() << "After SignInAndRetrieveDataWithCredential(), before OnCompletion()";

    auto contextData = this;
    void * context = contextData;
    result.OnCompletion(
                [](const firebase::Future<firebase::auth::SignInResult>& result, void* user_data)
        {
            decltype(contextData) contextDataLambda = static_cast<decltype (contextDataLambda)>(user_data);
            if (result.error() == firebase::auth::kAuthErrorNone)
            {
                const firebase::auth::SignInResult& sir = *result.result();
                firebase::auth::User* user = sir.user;
                qInfo() << "Credentials Sign In. Authenticated user :"  << user->uid().c_str() << user->display_name().c_str() << "(" << user->email().c_str() << ") (provider,user_name)=(" << sir.info.provider_id.c_str() << "," << sir.info.user_name.c_str() << ")";
                // Deprecated : use user state changed signals instead.
                emit contextDataLambda->firebaseAuthSucceed();
            }
            else
            {
                const firebase::auth::SignInResult& sir = *result.result();
                firebase::auth::User* user = sir.user;
                QString uid;
                if (user)
                    uid = user->uid().c_str();
                qInfo() << "Credentials Sign In. Authentication failed for" << uid << "with error : no=" << result.error() << "message=" << result.error_message();
                emit contextDataLambda->firebaseAuthFailed(result.error(), result.error_message());
            }
        },
    context);
    qInfo() << "After OnCompletion()";
}

void QFirebase::signInWithEmail(QString email, QString password)
{
    qInfo() << "Firebase Email Sign In";
    firebase::auth::Credential credential =
        firebase::auth::EmailAuthProvider::GetCredential(email.toUtf8().data(), password.toUtf8().data());
    signInWithCredentials(credential);
}

void QFirebase::signUpWithEmail(QString email, QString password)
{
    qInfo() << "Firebase Email Sign Up";
    firebase::Future<firebase::auth::User*> result =
        m_firebaseAuth->CreateUserWithEmailAndPassword(email.toUtf8().data(), password.toUtf8().data());

    auto contextData = this;
    void * context = contextData;
    result.OnCompletion(
        [](const firebase::Future<firebase::auth::User*>& result, void* user_data)
        {
            decltype(contextData) contextDataLambda = static_cast<decltype (contextDataLambda)>(user_data);
            if (result.error() == firebase::auth::kAuthErrorNone)
            {
                firebase::auth::User* user = *result.result();
                qInfo() << "Firebase Email Created authenticated user :"  << user->uid().c_str() << user->display_name().c_str() << "(" << user->email().c_str() << ")";
                // Deprecated : use user state changed signals instead.
                emit contextDataLambda->firebaseAuthSucceed();
            }
            else
            {
                qInfo() << "Firebase Email Created authentication failed with error : no=" << result.error() << "message=" << result.error_message();
                emit contextDataLambda->firebaseAuthFailed(result.error(), result.error_message());
            }
        },
    context);
}

void QFirebase::signInAnonymously()
{
    qInfo() << "Firebase Anonymous Sign In";
    firebase::Future<firebase::auth::User*> result = m_firebaseAuth->SignInAnonymously();
    auto contextData = this;
    void * context = contextData;
    result.OnCompletion(
        [](const firebase::Future<firebase::auth::User*>& result, void* user_data)
        {
            decltype(contextData) contextDataLambda = static_cast<decltype (contextDataLambda)>(user_data);
            if (result.error() == firebase::auth::kAuthErrorNone)
            {
                firebase::auth::User* user = *result.result();
                qInfo() << "Anonymous authenticated user :" << user->uid().c_str() << user->display_name().c_str();
                // Deprecated : use user state changed signals instead.
                emit contextDataLambda->firebaseAuthSucceed();
            }
            else
            {
                qInfo() << "Anonymous authentication failed with error : no=" << result.error() << "message=" << result.error_message();
                emit contextDataLambda->firebaseAuthFailed(result.error(), result.error_message());
            }
        },
    context);
}

void QFirebase::signOut()
{
    qInfo() << "Firebase Sign Out";
    m_firebaseAuth->SignOut();
}

void QFirebase::linkWithGSI(QString tokenId)
{
    qInfo() << "Firebase GSI account linking to User";

    firebase::auth::Credential credential =
        firebase::auth::GoogleAuthProvider::GetCredential(tokenId.toUtf8().data(), nullptr);
    linkWithCredentials(credential);
}

void QFirebase::linkWithEmail(QString email, QString password)
{
    qInfo() << "Firebase Email account linking to User";

    firebase::auth::Credential credential =
        firebase::auth::EmailAuthProvider::GetCredential(email.toUtf8().data(), password.toUtf8().data());
    linkWithCredentials(credential);
}

firebase::auth::User *QFirebase::getUser()
{
    return m_firebaseAuth->current_user();
}

void QFirebase::linkWithCredentials(firebase::auth::Credential& credential)
{
    firebase::auth::User* user = getUser();
    if (!user)
        return;

    firebase::Future<firebase::auth::SignInResult> result =
        user->LinkAndRetrieveDataWithCredential(credential);
    auto contextData = this;
    void * context = contextData;
    result.OnCompletion(
                [](const firebase::Future<firebase::auth::SignInResult>& result, void* user_data)
        {
            decltype(contextData) contextDataLambda = static_cast<decltype (contextDataLambda)>(user_data);
            if (result.error() == firebase::auth::kAuthErrorNone)
            {
                const firebase::auth::SignInResult& sir = *result.result();
                firebase::auth::User* user = sir.user;
                qInfo() << "Linked user :" << user->uid().c_str() << user->display_name().c_str() << sir.info.provider_id.c_str();
                // Deprecated : use user state changed signals instead.
                emit contextDataLambda->firebaseAuthLinkSucceed();
            }
            else
            {
                qInfo() << "Failed to link user with error : no=" << result.error() << "message=" << result.error_message();
                emit contextDataLambda->firebaseAuthLinkFailed(result.error(), result.error_message());
            }
        },
    context);
}

void QFirebase::onFirebaseInitializationPartiallyCompleted(firebase::InitResult result)
{
    qInfo() << "Firebase initialization partially completed. Registering Firebase auth listeners.";
    m_firebaseAuth = std::unique_ptr<firebase::auth::Auth>(firebase::auth::Auth::GetAuth(m_firebaseApp.get()));
    m_firebaseAuth->AddAuthStateListener(firebaseAuthListener.get());
    m_firebaseAuth->AddIdTokenListener(firebaseAuthTokenListener.get());
    emit firebaseInitializationCompleted(result);
}


QFirebase::QFirebaseAuthListener::QFirebaseAuthListener(QFirebase *caller) : caller(caller)
{
}

void QFirebase::QFirebaseAuthListener::OnAuthStateChanged(firebase::auth::Auth *auth)
{
    // See comments in void QFirebase::QFirebaseAuthListener::OnIdTokenChanged(firebase::auth::Auth *auth)
    firebase::auth::User* user = auth->current_user();
    if (user != nullptr)
    {
        qInfo() << "Auth state changed for" << user->uid().c_str() << user->display_name().c_str() << user->email().c_str() << user->provider_id().c_str();
    }
    else
    {
        qInfo() << "Auth state : logout.";
    }
    emit caller->authStateChanged(PointerContainer<firebase::auth::Auth>(auth));
}

// This is the preferred callback to handle user sign in state change.
// Sadly, it does not inform about which provider was used to log in.
// This information can be obtained from the Sign In Future callback.
// The Sign In Future callback is generally called after the IdTokenChanged listener.
// It also may not be called at all, for instance at application startup.
// So, if we do want to know about the provider used, we should get this
// information when processing Sign In, and then persisting it.
// As we can determine, this is independent from Firebase.
// The application user is something different from information obtained
// from authentication provider accounts. An application may want to get
// them or not. Right now
void QFirebase::QFirebaseAuthListener::OnIdTokenChanged(firebase::auth::Auth *auth)
{
    // We're in a Firebase thread. Use only const function on auth, as we're not
    // aware that non const functions are thread safe. Const ones are supposed to be.
    firebase::auth::User* user = auth->current_user();
    if (user != nullptr)
    {
        qInfo() << "IdToken changed for" << user->uid().c_str() << user->display_name().c_str() << user->email().c_str() << user->provider_id().c_str();
        for (firebase::auth::UserInfoInterface* v : user->provider_data())
            qInfo() << v->provider_id().c_str() << v->uid().c_str();
    }
    else
    {
        qInfo() << "IdToken : logout.";
    }
    // We can pass auth to slots.
    // Application thread can take non const action on it.
    emit caller->idTokenChanged(PointerContainer<firebase::auth::Auth>(auth));
}

QFirebase::QFirebaseRegisterer::QFirebaseRegisterer()
{
    qRegisterMetaType<PointerContainer<firebase::auth::Auth>>();
}
