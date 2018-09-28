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
                                myThis->m_firebaseAuth = firebase::auth::Auth::GetAuth(myThis->m_firebaseApp.get());
                                ::firebase::auth::Auth::GetAuth(myThis->m_firebaseApp.get(), &init_result);
                                qInfo() << "Finished FB init with result=" << init_result;
                                myThis->whenFirebaseInitializationCompletes(init_result);
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
                auto profile = sir.info.profile;
                for (const auto& p : profile)
                {
                    qInfo() << p.first.type() << p.second.type();
                    qInfo() << p.first.AsString().string_value() << p.second.AsString().string_value();
                }

                emit contextDataLambda->firebaseAuthSucceed(user);
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

                emit contextDataLambda->firebaseAuthSucceed(user);
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

                emit contextDataLambda->firebaseAuthSucceed(user);
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

                emit contextDataLambda->firebaseAuthLinkSucceed(user);
            }
            else
            {
                const firebase::auth::SignInResult& sir = *result.result();
                firebase::auth::User* user = contextDataLambda->m_firebaseAuth->current_user();//sir.user;
                QString uid;
                if (user)
                    uid = user->uid().c_str();
                qInfo() << "Failed to link user" << uid.toUtf8() << "with error : no=" << result.error() << "message=" << result.error_message();
                emit contextDataLambda->firebaseAuthLinkFailed(result.error(), result.error_message());
            }
            delete contextDataLambda;
        },
    context);
}

void QFirebase::whenFirebaseInitializationCompletes(firebase::InitResult result)
{
    qInfo() << "Firebase initialization completed. Registering Firebase auth listeners.";
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
