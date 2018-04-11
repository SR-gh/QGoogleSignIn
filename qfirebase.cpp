#include "qfirebase.h"
#include <QAndroidJniObject>
#include <QtAndroid>
#include <qdebug.h>

QFirebase::QFirebase(QGuiApplication *parent) : QObject(parent)
{
    if (!parent)
        throw "I must be given an application";
    m_firebaseApp = std::unique_ptr<firebase::App>(firebase::App::Create(firebase::AppOptions(), m_qjniEnv.operator ->(), QtAndroid::androidActivity().object()));
    m_firebaseAuth = firebase::auth::Auth::GetAuth(m_firebaseApp.get());
}

void QFirebase::signInWithGSI(QString tokenId)
{
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
                qInfo() << " Authenticated email : " << user->email().c_str();

                emit static_cast<QFirebase*>(user_data)->firebaseAuthSucceed(user);
            }
            else
            {
                qInfo() << " Authentication failed with error : " << result.error_message();
                emit static_cast<QFirebase*>(user_data)->firebaseAuthFailed(result.error(), result.error_message());
            }
        },
    context);
}

void QFirebase::signOut()
{
    m_firebaseAuth->SignOut();
}
