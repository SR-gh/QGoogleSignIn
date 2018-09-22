#include "qfirebase.h"
#include <QAndroidJniObject>
#include <QtAndroid>
#include <qdebug.h>
#include <tuple>
#include <firebase/util.h>
#include <memory>
//#7#include <thread>
//#7#include <sstream>

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
    firebase::Future<firebase::auth::User*> result =
        m_firebaseAuth->SignInWithCredential(credential);

    void * context = this;
    result.OnCompletion(
        [](const firebase::Future<firebase::auth::User*>& result, void* user_data)
        {
            if (result.error() == firebase::auth::kAuthErrorNone)
            {
                firebase::auth::User* user = *result.result();
                qInfo() << " Google Sign In authenticated user : "  << user->uid().c_str() << " " << user->display_name().c_str() << "(" << user->email().c_str() << ")";

                emit static_cast<QFirebase*>(user_data)->firebaseAuthSucceed(user, int(QFirebase::AuthType::GSI));
            }
            else
            {
                qInfo() << " Google Sign In authentication failed with error : no=" << result.error() << " message=" << result.error_message();
                emit static_cast<QFirebase*>(user_data)->firebaseAuthFailed(result.error(), result.error_message());
            }
        },
    context);
}

void QFirebase::signInWithEmail(QString email, QString password)
{
    qInfo() << "Firebase Email Sign In";
    firebase::Future<firebase::auth::User*> result =
        m_firebaseAuth->SignInWithEmailAndPassword(email.toUtf8().data(), password.toUtf8().data());

    void * context = this;
    result.OnCompletion(
        [](const firebase::Future<firebase::auth::User*>& result, void* user_data)
        {
            if (result.error() == firebase::auth::kAuthErrorNone)
            {
                firebase::auth::User* user = *result.result();
                qInfo() << " Firebase Email authenticated user : "  << user->uid().c_str() << " " << user->display_name().c_str() << "(" << user->email().c_str() << ")";

                emit static_cast<QFirebase*>(user_data)->firebaseAuthSucceed(user, int(QFirebase::AuthType::PASSWORD));
            }
            else
            {
                qInfo() << " Firebase Email authentication failed with error : no=" << result.error() << " message=" << result.error_message();
                emit static_cast<QFirebase*>(user_data)->firebaseAuthFailed(result.error(), result.error_message());
            }
        },
    context);
}

void QFirebase::signUpWithEmail(QString email, QString password)
{
    qInfo() << "Firebase Email Sign Up";
    firebase::Future<firebase::auth::User*> result =
        m_firebaseAuth->CreateUserWithEmailAndPassword(email.toUtf8().data(), password.toUtf8().data());

    void * context = this;
    result.OnCompletion(
        [](const firebase::Future<firebase::auth::User*>& result, void* user_data)
        {
            if (result.error() == firebase::auth::kAuthErrorNone)
            {
                firebase::auth::User* user = *result.result();
                qInfo() << " Firebase Email Created authenticated user : "  << user->uid().c_str() << " " << user->display_name().c_str() << "(" << user->email().c_str() << ")";

                emit static_cast<QFirebase*>(user_data)->firebaseAuthSucceed(user, int(QFirebase::AuthType::PASSWORD));
            }
            else
            {
                qInfo() << " Firebase Email Created authentication failed with error : no=" << result.error() << " message=" << result.error_message();
                emit static_cast<QFirebase*>(user_data)->firebaseAuthFailed(result.error(), result.error_message());
            }
        },
    context);
}

void QFirebase::signInAnonymously()
{
    qInfo() << "Firebase Anonymous Sign In";
    firebase::Future<firebase::auth::User*> result = m_firebaseAuth->SignInAnonymously();
    void * context = this;
    result.OnCompletion(
        [](const firebase::Future<firebase::auth::User*>& result, void* user_data)
        {
            if (result.error() == firebase::auth::kAuthErrorNone)
            {
                firebase::auth::User* user = *result.result();
                qInfo() << " Anonymous authenticated user : " << user->uid().c_str() << " " << user->display_name().c_str();

                emit static_cast<QFirebase*>(user_data)->firebaseAuthSucceed(user, int(QFirebase::AuthType::ANONYMOUS));
            }
            else
            {
                qInfo() << " Anonymous authentication failed with error : no=" << result.error() << " message=" << result.error_message();
                emit static_cast<QFirebase*>(user_data)->firebaseAuthFailed(result.error(), result.error_message());
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
    linkWithCredentials(credential, QFirebase::AuthType::GSI);
}

void QFirebase::linkWithEmail(QString email, QString password)
{
    qInfo() << "Firebase Email account linking to User";

    firebase::auth::Credential credential =
        firebase::auth::EmailAuthProvider::GetCredential(email.toUtf8().data(), password.toUtf8().data());
    linkWithCredentials(credential, QFirebase::AuthType::PASSWORD);
}

firebase::auth::User *QFirebase::getUser()
{
    return m_firebaseAuth->current_user();
}

void QFirebase::linkWithCredentials(firebase::auth::Credential& credential, QFirebase::AuthType authType)
{
    firebase::auth::User* user = getUser();
    if (!user)
        return;
    // Issue #7 : check thread-safety
//#7    std::ostringstream oss;
//#7    oss << std::this_thread::get_id();
//#7    qInfo() << " Before call, thread_id=" << oss.str().c_str();

    // Issue #8 : replace LinkWithCredential() with ReauthenticateAndRetrieveData() causes a crash !
//    firebase::Future<firebase::auth::User*> result =
//        user->LinkWithCredential(credential);
    firebase::Future<firebase::auth::SignInResult> result =   //#8
        user->ReauthenticateAndRetrieveData(credential);      //#8
    auto contextData = new std::tuple<decltype(this), decltype(authType)> {this, authType};
    void * context = contextData;
    result.OnCompletion(
//                [](const firebase::Future<firebase::auth::User *>& result, void* user_data) //#8
                [](const firebase::Future<firebase::auth::SignInResult>& result, void* user_data)
        {
            //std::tuple<typeof(this), typeof(authType)> * contextDataLambda = static_cast<decltype (contextDataLambda)>(user_data);
            decltype(contextData) contextDataLambda = static_cast<decltype (contextDataLambda)>(user_data);
            if (result.error() == firebase::auth::kAuthErrorNone)
            {
//                firebase::auth::User* user = *result.result();    //#8
                const firebase::auth::SignInResult& sir = *result.result();
                firebase::auth::User* user = sir.user;
                qInfo() << " Linked user : " << user->uid().c_str() << " " << user->display_name().c_str();

                emit static_cast<QFirebase*>(std::get<0>(*contextDataLambda))->firebaseAuthLinkSucceed(user, int(std::get<1>(*contextDataLambda)));
            }
            else
            {
//#7                std::ostringstream oss;
//#7                oss << std::this_thread::get_id();
                qInfo() << " Failed to link user with error : no=" << result.error() << " message=" << result.error_message() << " thread_id=" /*#7 << oss.str().c_str()*/;
                emit static_cast<QFirebase*>(std::get<0>(*contextDataLambda))->firebaseAuthLinkFailed(result.error(), result.error_message());
            }
            delete contextDataLambda;
        },
    context);
}

void QFirebase::whenFirebaseInitializationCompletes(firebase::InitResult result)
{
    qInfo() << "Registering Firebase auth listeners";
    m_firebaseAuth->AddAuthStateListener(firebaseAuthListener.get());
    m_firebaseAuth->AddIdTokenListener(firebaseAuthTokenListener.get());
    emit firebaseInitializationCompleted(result);
}


QFirebase::QFirebaseAuthListener::QFirebaseAuthListener(QFirebase *caller) : caller(caller)
{
}

void QFirebase::QFirebaseAuthListener::OnAuthStateChanged(firebase::auth::Auth *auth)
{

    qInfo() << "QFirebaseAuthListener::OnAuthStateChanged";
    firebase::auth::User* user = auth->current_user();
    if (user != nullptr)
    {
        qInfo() << "Auth state changed for " << user->uid().c_str() << user->display_name().c_str() << user->email().c_str() << user->provider_id().c_str();
    }
    else
    {
        qInfo() << "Auth state : logout.";
    }
    emit caller->authStateChanged(PointerContainer<firebase::auth::Auth>(auth));
    // Warning : cannot use auth asynchonously in a signal.
    // either the caller constructs a state change handler and give it to QFirebase or
}

void QFirebase::QFirebaseAuthListener::OnIdTokenChanged(firebase::auth::Auth *auth)
{
    qInfo() << "QFirebaseAuthListener::OnIdTokenChanged";
    firebase::auth::User* user = auth->current_user();
    if (user != nullptr)
    {
        qInfo() << "IdToken changed for " << user->uid().c_str() << user->display_name().c_str() << user->email().c_str() << user->provider_id().c_str();
        for (firebase::auth::UserInfoInterface* v : user->provider_data())
            qInfo() << v->provider_id().c_str() << v->uid().c_str();
    }
    else
    {
        qInfo() << "IdToken : logout.";
    }
    emit caller->idTokenChanged(PointerContainer<firebase::auth::Auth>(auth));
}

QFirebase::QFirebaseRegisterer::QFirebaseRegisterer()
{
    qRegisterMetaType<PointerContainer<firebase::auth::Auth>>();
}
