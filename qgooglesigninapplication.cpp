#include "qgooglesigninapplication.h"
#include <qdebug.h>
#include <QAndroidJniObject>
#include <QtAndroid>
#include "qmaincontroller.h"

const QString QGoogleSignInApplication::WAIT_FOR_ASYNC_ANSWER = "wait_async";

#ifdef Q_QDOC
    QGoogleSignInApplication::QGoogleSignInApplication(int &argc, char **argv) : QGuiApplication(argc, argv)
#else
    QGoogleSignInApplication::QGoogleSignInApplication(int &argc, char **argv, int af) : QGuiApplication(argc, argv, af)
#endif
{

}
void QGoogleSignInApplication::init()
{
    activityReceiver = std::unique_ptr<QGSIAARR>(new QGSIAARR(this));
    // connections
    connect(this, &QGoogleSignInApplication::applicationStateChanged, this, &QGoogleSignInApplication::onApplicationStateChanged);
    // Firebase
    // auth
#if defined(__ANDROID__)
    m_firebaseApp = std::unique_ptr<firebase::App>(firebase::App::Create(firebase::AppOptions(), m_qjniEnv.operator ->(), QtAndroid::androidActivity().object()));
#else
    // Unused code !
  firebase::App* app = firebase::App::Create(firebase::AppOptions());
#endif  // defined(__ANDROID__)
    m_firebaseAuth = firebase::auth::Auth::GetAuth(m_firebaseApp.get());
    connect(this, &QGoogleSignInApplication::gsiTokenReceived, this, &QGoogleSignInApplication::onGsiTokenReceived);
    connect(this, &QGoogleSignInApplication::failedRefresh, this, &QGoogleSignInApplication::onFailedRefresh);
    connect(this, &QGoogleSignInApplication::successfulSignOut, this, &QGoogleSignInApplication::onSuccessfulSignOut);

    // GUI controllers
    controllers.append(new QMainController("ctrlMain", this));

}

void QGoogleSignInApplication::startGSIIntent()
{
    QAndroidJniObject jItent = QAndroidJniObject::callStaticObjectMethod("org.renan.android.firebase.auth.QGoogleSignIn", "getGSIIntent", "(Landroid/content/Context;)Landroid/content/Intent;", QtAndroid::androidActivity().object());
    QtAndroid::startActivity(jItent, QGSI_SIGN_IN, activityReceiver.get());
}

void QGoogleSignInApplication::signIn(bool silently)
{
    QAndroidJniObject result = QAndroidJniObject::callStaticObjectMethod("org.renan.android.firebase.auth.QGoogleSignIn", "getTokenIdFromSignedAccount", "(Landroid/content/Context;)Ljava/lang/String;", QtAndroid::androidActivity().object());
    QString tokenId = result.toString();
    qInfo() << tokenId;
    // no user previously signed in
    if (0 == tokenId.size())
    {
        if (!silently)
            startGSIIntent();
    }
    else
    {
        // Is the token a valid one ?
        QAndroidJniObject result = QAndroidJniObject::callStaticObjectMethod("org.renan.android.firebase.auth.QGoogleSignIn", "refreshToken", "(Landroid/content/Context;Z)Ljava/lang/String;", QtAndroid::androidActivity().object(), silently);
        tokenId = result.toString();
        if (0 == tokenId.size()/* && !silently*/)
        {
//            // error case
//            // we choose to try again with Intent
//            startGSIIntent();
            emit failedRefresh(ERROR_UNKNOWN, silently);   // implement any behaviour
        }
        else if (WAIT_FOR_ASYNC_ANSWER.compare(tokenId))
        {
            // must do nothing : treated in a callback
        }
        else
            emit gsiTokenReceived(tokenId);
    }
}

void QGoogleSignInApplication::signOut()
{
    m_firebaseAuth->SignOut();
    auto result = QAndroidJniObject::callStaticMethod<jboolean>("org.renan.android.firebase.auth.QGoogleSignIn", "signOut", "(Landroid/content/Context;)Z", QtAndroid::androidActivity().object());
    if (result)
        emit successfulSignOut();
}

void QGoogleSignInApplication::onApplicationStateChanged(Qt::ApplicationState state)
{
    switch (state)
    {
    case Qt::ApplicationActive:
        qInfo() << "onApplicationStateChanged(ApplicationActive=" << state << ")";
        // Sign In with GSI, silently
        signIn(true);
        break;
    case Qt::ApplicationSuspended:
        qInfo() << "onApplicationStateChanged(ApplicationSuspended=" << state << ")";
        break;
    default:
        qInfo() << "onApplicationStateChanged(unhandled state=" << state << ")";
        break;
    }

}

void QGoogleSignInApplication::QGSIAARR::handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data)
{
    qInfo() << "handleActivityResult(receiverRequestCode=" << receiverRequestCode << ", resultCode=" << resultCode << ",data=" << reinterpret_cast<const void*>(&data) << ")";
    if (QGSI_SIGN_IN == receiverRequestCode && RESULT_OK == resultCode)
    {
        QAndroidJniObject token = QAndroidJniObject::callStaticObjectMethod("org.renan.android.firebase.auth.QGoogleSignIn", "handleSignInResult", "(Landroid/content/Intent;)Ljava/lang/String;", data.object());
        QString tokenId = token.toString();
        qInfo() << tokenId;
        if (tokenId.size() > 0)
            emit caller->gsiTokenReceived(tokenId);
    }
}

void QGoogleSignInApplication::onGsiTokenReceived(QString tokenId)
{
    firebase::auth::Credential credential =
        firebase::auth::GoogleAuthProvider::GetCredential(tokenId.toUtf8().data(), nullptr);
    firebase::Future<firebase::auth::User*> result =
        m_firebaseAuth->SignInWithCredential(credential);

    void * context = nullptr;
    result.OnCompletion(
        [](const firebase::Future<firebase::auth::User*>& result, void* user_data)
        {
//            MyProgramContext* program_context = static_cast<MyProgramContext*>(user_data);
            (void)user_data;

            if (result.error() == firebase::auth::kAuthErrorNone)
            {
                firebase::auth::User* user = *result.result();
                qInfo() << " Authenticated email : " << user->email().c_str();

//                // Perform other actions on User, if you like.
//                firebase::auth::User::UserProfile profile;
//                profile.display_name = program_context->display_name;
//                user->UpdateUserProfile(profile);
                emit qGoogleSignInApp->firebaseAuthSucceed(user);
            }
            else
            {
                qInfo() << " Authentication failed with error : " << result.error_message();
                emit qGoogleSignInApp->firebaseAuthFailed(result.error(), result.error_message());
            }
        },
    context);
}

void QGoogleSignInApplication::onFailedRefresh(int statusCode, bool silently)
{
    // TODO : application state is in our Firebase user (tbd)
    // so, we do not have to transmit more state. I think. I beleive. I guess. I don't know.
    if (!silently)
    {
        // avoid loops : should we pass a counter as well ? or anything that informs us that
        // we should not retry. A counter looks good to me. Must think.
        // A reference to a C++ thing that is sent back from Java looks dangerous (activities
        // die and our QtActivity state should be OK to handle any case).
        signIn(false);
    }
}

void QGoogleSignInApplication::onSuccessfulSignOut()
{
    // handle state, if necessary
    qInfo() << "User successfuly signed out the application";
}
