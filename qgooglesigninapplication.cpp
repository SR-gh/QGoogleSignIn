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
    qFirebase = new QFirebase(this);    // parent's lifetime
    qAuthGSI = new QAuthGSI(this);      // parent's lifetime
    // connections
    connect(this, &QGoogleSignInApplication::applicationStateChanged, this, &QGoogleSignInApplication::onApplicationStateChanged);
    // Firebase handling
    // auth
    connect(qFirebase, &QFirebase::firebaseInitializationCompleted, this, &QGoogleSignInApplication::onFirebaseInitializationComplete);
    connect(qFirebase, &QFirebase::firebaseAuthSucceed, this, &QGoogleSignInApplication::onFirebaseAuthSucceed);
    connect(qFirebase, &QFirebase::firebaseAuthFailed, this, &QGoogleSignInApplication::onFirebaseAuthFailed);
    connect(qFirebase, &QFirebase::firebaseAuthLinkSucceed, this, &QGoogleSignInApplication::onFirebaseAuthLinkSucceed);
    connect(qFirebase, &QFirebase::firebaseAuthLinkFailed, this, &QGoogleSignInApplication::onFirebaseAuthLinkFailed);
    connect(qFirebase, &QFirebase::authStateChanged, this, &QGoogleSignInApplication::onAuthStateChanged);
    connect(qFirebase, &QFirebase::idTokenChanged, this, &QGoogleSignInApplication::onIdTokenChanged);

    // GSI handling
    connect(qAuthGSI, &QAuthGSI::gsiTokenReceived, this, &QGoogleSignInApplication::onGsiTokenReceived);
    connect(qAuthGSI, &QAuthGSI::gsiTokenRequestFailed, this, &QGoogleSignInApplication::onGsiTokenRequestFailed);
    connect(qAuthGSI, &QAuthGSI::failedRefresh, this, &QGoogleSignInApplication::onFailedRefresh);
    connect(qAuthGSI, &QAuthGSI::successfulSignOut, this, &QGoogleSignInApplication::onSuccessfulSignOut);

    // GUI controllers
    controllers.append(new QMainController("ctrlMain", this));

    // sub-init with needed connections established.
    qFirebase->init();
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

void QGoogleSignInApplication::signInWithEmail(QString email, QString password)
{
    firebase::auth::User* user = qFirebase->getUser();
    if (user)
    {
        // TODO : check that the user is not logged with :
        // 1- any GSI account ? (if only one GSI account is allowed per user)
        // 2- this particular GSI account ? (what are the consequences if not checked ?)
        // if (user->)… not done

        // As we already are logged, we asked not to sign in, but to associate credentials
        qFirebase->linkWithEmail(email, password);
    }
    else
        qFirebase->signInWithEmail(email, password);
}

void QGoogleSignInApplication::signUpWithEmail(QString email, QString password)
{
    qFirebase->signUpWithEmail(email, password);
}

void QGoogleSignInApplication::signInAnonymously()
{
    qFirebase->signInAnonymously();
}

void QGoogleSignInApplication::signOut()
{
    qFirebase->signOut();
    qAuthGSI->signOut();
//    lastSuccessfulAuthType = QFirebase::AuthType::UNDEFINED;
}

void QGoogleSignInApplication::onApplicationStateChanged(Qt::ApplicationState state)
{
    switch (state)
    {
    case Qt::ApplicationActive:
    {
        qInfo() << "onApplicationStateChanged(ApplicationActive=" << state;// << "), handlingActivityResult=" << handlingActivityResult;
        // Issue#11 : we do nothing at startup.
//        if (!handlingActivityResult) // flag to differentiate activity respawn after a result is received or after creation
//                                     // this is easier to treat in Java directly as the correct handler is called and not the other one (to check).
//                                     // The Qt way of dealing with application state makes it difficult to treat in C++ these cases.
//        {
//            QSettings settings;
//            lastSuccessfulAuthType = static_cast<QFirebase::AuthType>(settings.value("lastSuccessfulAuthType", QFirebase::AuthType::UNDEFINED).toInt());
//            switch (lastSuccessfulAuthType)
//            {
//            case QFirebase::AuthType::NO_SIGN_IN:
//            case QFirebase::AuthType::UNDEFINED:
//                break;
//            case QFirebase::AuthType::GSI:
//                // Sign In with GSI, silently
//                qAuthGSI->signIn(true);
//                break;
//            case QFirebase::AuthType::PASSWORD:
//                //signInWithEmail(WTF)
//                break;
//            case QFirebase::AuthType::ANONYMOUS:
//                // Sign Anonymously
//                signInAnonymously();
//                break;
//            default:
//                break;
//            }
//        }
//        handlingActivityResult = false; // reset for future respawns
        break;
    }
    case Qt::ApplicationSuspended:
    {
        qInfo() << "onApplicationStateChanged(ApplicationSuspended=" << state << ")";
//        QSettings settings;
//        settings.setValue("lastSuccessfulAuthType", lastSuccessfulAuthType);
        break;
    }
    default:
        qInfo() << "onApplicationStateChanged(unhandled state=" << state << ")";
        break;
    }

}

void QGoogleSignInApplication::onGsiTokenReceived(QString tokenId, QAuthGSI::GSIJavaIntent reason)
{
    qInfo() << "onGsiTokenReceived(reason=" << (QAuthGSI::GSIJavaIntent::QGSI_SIGN_IN == reason? "QGSI_SIGN_IN" : "QGSI_GET_TOKEN") << ")";
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
    qInfo() << "onFailedRefresh(statusCode=" << statusCode << ")";
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

//void QGoogleSignInApplication::onFirebaseAuthSucceed(firebase::auth::User *user, int p_authType)
void QGoogleSignInApplication::onFirebaseAuthSucceed(firebase::auth::User *user)
{
//    QFirebase::AuthType authType = static_cast<QFirebase::AuthType>(p_authType);
    qInfo() << "User successfuly signed in the application. You may want to add some extra code here. You may want to rely only on user change listener otherwise.";
    assert(user);
    const std::string providerId = user->provider_id();
    qInfo() << "provider=" << providerId.c_str();
    firebase::auth::User * authUser = qFirebase->getUser();
    const std::string authProviderId = authUser->provider_id();
    qInfo() << "authProviderId=" << authProviderId.c_str();
    for (firebase::auth::UserInfoInterface* v : authUser->provider_data())
        qInfo() << v->provider_id().c_str() << v->uid().c_str();
    qInfo() << "The user is" << (authUser->is_anonymous() ? "" : "not") << "anonymously signed in.";
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
//    lastSuccessfulAuthType = authType;
}

void QGoogleSignInApplication::onFirebaseAuthFailed(int errorCode, QString errorMessage)
{
    (void) errorCode;
    qInfo() << "User could not log in. You may want to handle this case in a particular way.";
    // Example of error reporting
    emit error(errorMessage);
}

//void QGoogleSignInApplication::onFirebaseAuthLinkSucceed(firebase::auth::User *user, int p_authType)
void QGoogleSignInApplication::onFirebaseAuthLinkSucceed(firebase::auth::User *user)
{
    (void) user;
//    (void) p_authType
//    QFirebase::AuthType authType = static_cast<QFirebase::AuthType>(p_authType);
    qInfo() << "User successfuly linked an account to their Firebase account. You may want to add some extra code here. You may want to rely only on user change listener otherwise.";
//    if (QFirebase::AuthType::ANONYMOUS == lastSuccessfulAuthType || QFirebase::AuthType::UNDEFINED == lastSuccessfulAuthType)
//        lastSuccessfulAuthType = authType;
}

void QGoogleSignInApplication::onFirebaseAuthLinkFailed(int errorCode, QString errorMessage)
{
    (void) errorCode;
    qInfo() << "User could not link an account to their Firebase account. You should handle this signal.";
    // Example of error reporting
    emit error(errorMessage);
}

void QGoogleSignInApplication::onFirebaseInitializationComplete(firebase::InitResult result)
{
    setFirebaseInitialized(0 == result);
}

void QGoogleSignInApplication::onAuthStateChanged(PointerContainer<firebase::auth::Auth> pca)
{
    qInfo() << "Auth state changed.";
    firebase::auth::Auth* auth = pca.getPtr();
    firebase::auth::User* user = auth->current_user();
    if (user != nullptr)
    {
        qInfo() << "Auth state changed for " << user->uid().c_str() << user->display_name().c_str() << user->email().c_str();
        m_user.setSignedIn(true);
        m_user.setEmail(user->email().c_str());
        m_user.setName(user->display_name().c_str());
        m_user.setUrl(user->photo_url().c_str());
    }
    else
    {
        qInfo() << "Auth state : logout.";
//        m_user = QUser();
        m_user.clear();
    }
}

void QGoogleSignInApplication::onIdTokenChanged(PointerContainer<firebase::auth::Auth> pca)
{
    qInfo() << "IdToken changed.";
    firebase::auth::Auth* auth = pca.getPtr();
    firebase::auth::User* user = auth->current_user();
    if (user != nullptr)
    {
        qInfo() << "IdToken changed for " << user->uid().c_str() << user->display_name().c_str() << user->email().c_str();
        m_user.setSignedIn(true);
        m_user.setEmail(user->email().c_str());
        m_user.setName(user->display_name().c_str());
        m_user.setUrl(user->photo_url().c_str());
    }
    else
    {
        qInfo() << "IdToken : logout.";
//        m_user = QUser();
        m_user.clear();
    }
}

bool QGoogleSignInApplication::isFirebaseInitialized() const
{
    return firebaseInitialized;
}

void QGoogleSignInApplication::setFirebaseInitialized(bool b)
{
    const bool changed = b != firebaseInitialized;
    firebaseInitialized = b;
    if (changed)
        emit applicationInitializedChanged(isFirebaseInitialized());
}

const QUser *QGoogleSignInApplication::getUser() const
{
    return &m_user;
}

QUser *QGoogleSignInApplication::getUser()
{
    return &m_user;
}

void QGoogleSignInApplication::setUser(QUser *user)
{
//    const QUser& tmpUser = ((nullptr == user) ? QUser() : *user);
//    if
//    {

//        if (tmpUser != m_user)
//        m_user = QUser();
//    }
//    else
//        m_user = *user;
    if (nullptr == user)
        m_user.clear();
    else
    {
        m_user.setSignedIn(user->getSignedIn());
        m_user.setEmail(user->getEmail());
        m_user.setName(user->getName());
        m_user.setUrl(user->getUrl());
    }
}

//void QGoogleSignInApplication::setHandlingActivityResult(bool value)
//{
//    handlingActivityResult = value;
//}
