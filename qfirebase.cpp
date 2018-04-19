#include "qfirebase.h"
#include <QAndroidJniObject>
#include <QtAndroid>
#include <qdebug.h>

QFirebase::QFirebase(QGuiApplication *parent) : QObject(parent)
{
    if (!parent)
        throw "I must be given an application";
#if defined(__ANDROID__)
    // specific Android setup
    // Firebase should be considered available only on completion of this (possibly)
    // asynchronous initialization.
    // Either the application can do something while waiting or not.
    // IMO, it should not hang and should be able to display its waiting
    // status and be left if asked to.
    m_firebaseApp = std::unique_ptr<firebase::App>(firebase::App::Create(firebase::AppOptions(), m_qjniEnv.operator ->(), QtAndroid::androidActivity().object()));
#else
    // Unused code ! Left as a reminder
  firebase::App* app = firebase::App::Create(firebase::AppOptions());
#endif  // defined(__ANDROID__)
    m_firebaseAuth = firebase::auth::Auth::GetAuth(m_firebaseApp.get());
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
                qInfo() << " Google Sign In authentication failed with error : " << result.error_message();
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
                qInfo() << " Anonymous authentication failed with error : " << result.error_message();
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
    firebase::auth::User* user = getUser();
    if (!user)
        return;
    firebase::Future<firebase::auth::User*> result =
        user->LinkWithCredential(credential);
    void * context = this;
    result.OnCompletion(
        [](const firebase::Future<firebase::auth::User*>& result, void* user_data)
        {
            if (result.error() == firebase::auth::kAuthErrorNone)
            {
                firebase::auth::User* user = *result.result();
                qInfo() << " Linked GSI user : " << user->uid().c_str() << " " << user->display_name().c_str();

                emit static_cast<QFirebase*>(user_data)->firebaseAuthLinkSucceed(user, int(QFirebase::AuthType::GSI));
            }
            else
            {
                qInfo() << " Failed to link GSI user with error : " << result.error_message();
                emit static_cast<QFirebase*>(user_data)->firebaseAuthLinkFailed(result.error(), result.error_message());
            }
        },
    context);
}

firebase::auth::User *QFirebase::getUser()
{
    return m_firebaseAuth->current_user();
}
