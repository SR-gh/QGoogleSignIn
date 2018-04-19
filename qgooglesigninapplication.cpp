#include "qgooglesigninapplication.h"
#include <qdebug.h>
#include "qmaincontroller.h"
#include <QSettings>
#include <cassert>
#include <string>

#ifdef Q_QDOC
    QGoogleSignInApplication::QGoogleSignInApplication(int &argc, char **argv) : QGuiApplication(argc, argv)
#else
    QGoogleSignInApplication::QGoogleSignInApplication(int &argc, char **argv, int af) : QGuiApplication(argc, argv, af)
#endif
{

}
void QGoogleSignInApplication::init()
{
    qFirebase = std::unique_ptr<QFirebase>(new QFirebase(this));
    qAuthGSI = std::unique_ptr<QAuthGSI>(new QAuthGSI(this));
    // connections
    connect(this, &QGoogleSignInApplication::applicationStateChanged, this, &QGoogleSignInApplication::onApplicationStateChanged);
    // Firebase handling
    // auth
    connect(qFirebase.get(), &QFirebase::firebaseAuthSucceed, this, &QGoogleSignInApplication::onFirebaseAuthSucceed);
    connect(qFirebase.get(), &QFirebase::firebaseAuthFailed, this, &QGoogleSignInApplication::onFirebaseAuthFailed);
    connect(qFirebase.get(), &QFirebase::firebaseAuthLinkSucceed, this, &QGoogleSignInApplication::onFirebaseAuthLinkSucceed);

    // GSI handling
    connect(qAuthGSI.get(), &QAuthGSI::gsiTokenReceived, this, &QGoogleSignInApplication::onGsiTokenReceived);
    connect(qAuthGSI.get(), &QAuthGSI::gsiTokenRequestFailed, this, &QGoogleSignInApplication::onGsiTokenRequestFailed);
    connect(qAuthGSI.get(), &QAuthGSI::failedRefresh, this, &QGoogleSignInApplication::onFailedRefresh);
    connect(qAuthGSI.get(), &QAuthGSI::successfulSignOut, this, &QGoogleSignInApplication::onSuccessfulSignOut);

    // GUI controllers
    controllers.append(new QMainController("ctrlMain", this));

}

void QGoogleSignInApplication::signInWithGSI(bool silently)
{
    firebase::auth::User* user = qFirebase->getUser();
    if (user)
    {
        // TODO : check that the user is not logged with :
        // 1- any GSI account ? (if only one GSI account is allowed per user)
        // 2- this particular GSI account ? (what are the consequences if not checked ?)
        // if (user->)… not done

        // As we already are logged, we asked not to sign in, but to associate credentials
        qAuthGSI->obtainToken();
    }
    else
        qAuthGSI->signIn(silently);
}

void QGoogleSignInApplication::signInAnonymously()
{
    qFirebase->signInAnonymously();
}

void QGoogleSignInApplication::signOut()
{
    qFirebase->signOut();
    qAuthGSI->signOut();
    lastSuccessfulAuthType = QFirebase::AuthType::UNDEFINED;
}

void QGoogleSignInApplication::onApplicationStateChanged(Qt::ApplicationState state)
{
    switch (state)
    {
    case Qt::ApplicationActive:
    {
        qInfo() << "onApplicationStateChanged(ApplicationActive=" << state << "), handlingActivityResult=" << handlingActivityResult;
        if (!handlingActivityResult) // flag to differentiate activity respawn after a result is received or after creation
                                     // this is easier to treat in Java directly as the correct handler is called and not the other one (to check).
                                     // The Qt way of dealing with application state makes it difficult to treat in C++ these cases.
        {
            QSettings settings;
            lastSuccessfulAuthType = static_cast<QFirebase::AuthType>(settings.value("lastSuccessfulAuthType", QFirebase::AuthType::UNDEFINED).toInt());
            switch (lastSuccessfulAuthType)
            {
            case QFirebase::AuthType::GSI:
                // Sign In with GSI, silently
                qAuthGSI->signIn(true);
                break;
            case QFirebase::AuthType::UNDEFINED:
            case QFirebase::AuthType::ANONYMOUS:
            default:
                // Sign Anonymously
                signInAnonymously();
                break;
            }
        }
        handlingActivityResult = false; // reset for future respawns
        break;
    }
    case Qt::ApplicationSuspended:
    {
        qInfo() << "onApplicationStateChanged(ApplicationSuspended=" << state << ")";
        QSettings settings;
        settings.setValue("lastSuccessfulAuthType", lastSuccessfulAuthType);
        break;
    }
    default:
        qInfo() << "onApplicationStateChanged(unhandled state=" << state << ")";
        break;
    }

}

void QGoogleSignInApplication::onGsiTokenReceived(QString tokenId, QAuthGSI::GSIJavaIntent reason)
{
    if (QAuthGSI::GSIJavaIntent::QGSI_SIGN_IN == reason)
        qFirebase->signInWithGSI(tokenId);
    else
        qFirebase->linkWithGSI(tokenId);
}

void QGoogleSignInApplication::onGsiTokenRequestFailed(int resultCode, QSharedPointer<const QAndroidJniObject> jniObject)
{
    qInfo() << "onGsiTokenRequestFailed(resultCode=" << resultCode << ",QSharedPointer<const QAndroidJniObject> jniObject=" << jniObject.data()->toString() << ")";
}

void QGoogleSignInApplication::onFailedRefresh(int statusCode, bool silently)
{
    (void) statusCode;
    // TODO : application state is in our Firebase user (tbd)
    // so, we do not have to transmit more state. I think. I believe. I guess. I don't know.
    if (!silently)
    {
        // avoid loops : should we pass a counter as well ? or anything that informs us that
        // we should not retry. A counter looks good to me. Must think.
        // A reference to a C++ thing that is sent back from Java looks dangerous (activities
        // die and our QtActivity state should be OK to handle any case).
        signInWithGSI(false);
    }
}

void QGoogleSignInApplication::onSuccessfulSignOut()
{
    // handle state, if necessary
    qInfo() << "User successfuly signed out of the application";
}

void QGoogleSignInApplication::onFirebaseAuthSucceed(firebase::auth::User *user, int p_authType)
{
    QFirebase::AuthType authType = static_cast<QFirebase::AuthType>(p_authType);
    qInfo() << "User successfuly signed in the application. You may want to add some extra code here. You may want to rely only on user change listener otherwise.";
    assert(user);
    const std::string providerId = user->provider_id();
    qInfo() << "provider=" << providerId.c_str();
    firebase::auth::User * authUser = qFirebase->getUser();
    const std::string authProviderId = authUser->provider_id();
    qInfo() << "authProviderId=" << authProviderId.c_str();
    // Where is firebase::auth::GoogleAuthProvider::PROVIDER_ID ??? Am I going to JNI a constant ?
//    if (providerId == "google.com")
//    {
//        lastSuccessfulAuthType = AuthType::GSI;
//    }
//    else if (providerId == "firebase")
//    {
//        lastSuccessfulAuthType = AuthType::ANONYMOUS;
//    }
//    else
//    {
//        lastSuccessfulAuthType = AuthType::UNDEFINED;
//    }
    lastSuccessfulAuthType = authType;
}

void QGoogleSignInApplication::onFirebaseAuthFailed(int errorCode, QString errorMessage)
{
    (void) errorCode;
    (void) errorMessage;
    qInfo() << "User could not log in. You may want to handle this case in a particular way.";
}

void QGoogleSignInApplication::onFirebaseAuthLinkSucceed(firebase::auth::User *user, int p_authType)
{
    (void) user;
    QFirebase::AuthType authType = static_cast<QFirebase::AuthType>(p_authType);
    qInfo() << "User successfuly linked an account to their Firebase account. You may want to add some extra code here. You may want to rely only on user change listener otherwise.";
    if (QFirebase::AuthType::ANONYMOUS == lastSuccessfulAuthType || QFirebase::AuthType::UNDEFINED == lastSuccessfulAuthType)
        lastSuccessfulAuthType = authType;
}

void QGoogleSignInApplication::setHandlingActivityResult(bool value)
{
    handlingActivityResult = value;
}
