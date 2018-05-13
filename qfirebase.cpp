#include "qfirebase.h"
#include <QAndroidJniObject>
#include <QtAndroid>
#include <qdebug.h>
#include <tuple>
#include <firebase/util.h>
#include <memory>

const QFirebase::QFirebaseRegisterer QFirebase::registerer;

QFirebase::QFirebase(QObject *parent) : QObject(parent),
    firebaseAuthListener(std::make_unique<QFirebaseAuthListener>(this))
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
                                myThis->onFirebaseInitializationComplete(init_result);
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

    firebase::Future<firebase::auth::User*> result =
        user->LinkWithCredential(credential);
    auto contextData = new std::tuple<decltype(this), decltype(authType)> {this, authType};
    void * context = contextData;
    result.OnCompletion(
        [](const firebase::Future<firebase::auth::User*>& result, void* user_data)
        {
            //std::tuple<typeof(this), typeof(authType)> * contextDataLambda = static_cast<decltype (contextDataLambda)>(user_data);
            decltype(contextData) contextDataLambda = static_cast<decltype (contextDataLambda)>(user_data);
            if (result.error() == firebase::auth::kAuthErrorNone)
            {
                firebase::auth::User* user = *result.result();
                qInfo() << " Linked user : " << user->uid().c_str() << " " << user->display_name().c_str();

                emit static_cast<QFirebase*>(std::get<0>(*contextDataLambda))->firebaseAuthLinkSucceed(user, int(std::get<1>(*contextDataLambda)));
            }
            else
            {
                qInfo() << " Failed to link user with error : no=" << result.error() << " message=" << result.error_message();
                emit static_cast<QFirebase*>(std::get<0>(*contextDataLambda))->firebaseAuthLinkFailed(result.error(), result.error_message());
            }
            delete contextDataLambda;
        },
    context);
}

void QFirebase::onFirebaseInitializationComplete(firebase::InitResult result)
{
    qInfo() << "Registering Firebase auth listeners";
    m_firebaseAuth->AddAuthStateListener(firebaseAuthListener.get());
    m_firebaseAuth->AddIdTokenListener(firebaseAuthListener.get());
    emit firebaseInitializationComplete(result);
}


QFirebase::QFirebaseAuthListener::QFirebaseAuthListener(QFirebase *caller) : caller(caller)
{
}

void QFirebase::QFirebaseAuthListener::OnAuthStateChanged(firebase::auth::Auth *auth)
{
    emit caller->authStateChanged(PointerContainer<firebase::auth::Auth>(auth));
}

void QFirebase::QFirebaseAuthListener::OnIdTokenChanged(firebase::auth::Auth *auth)
{
    emit caller->idTokenChanged(PointerContainer<firebase::auth::Auth>(auth));
}

QFirebase::QFirebaseRegisterer::QFirebaseRegisterer()
{
    qRegisterMetaType<PointerContainer<firebase::auth::Auth>>();
}
