#include "qauthgsi.h"
#include <qdebug.h>
#include <QAndroidJniObject>
#include <QtAndroid>
#include "qgooglesigninapplication.h"


const QString QAuthGSI::WAIT_FOR_ASYNC_ANSWER = "wait_async";
const QAuthGSI::QAuthGSIRegisterer QAuthGSI::registerer;

QAuthGSI::QAuthGSI(QGuiApplication *parent) : QObject(parent)
{
    if (!parent)
        throw "I must be given an application";
    activityReceiver = std::unique_ptr<QGSIAARR>(new QGSIAARR(this));
}

void QAuthGSI::QGSIAARR::handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data)
{
    qInfo() << "handleActivityResult(receiverRequestCode=" << receiverRequestCode << ", resultCode=" << resultCode << ",data=" << reinterpret_cast<const void*>(&data) << ")";
    if (QGSI_SIGN_IN == receiverRequestCode || QGSI_GET_TOKEN == receiverRequestCode)
    {
        if(RESULT_OK == resultCode)
        {
            QAndroidJniObject token = QAndroidJniObject::callStaticObjectMethod("org.renan.android.firebase.auth.QGoogleSignIn", "handleSignInResult", "(Landroid/content/Intent;)Ljava/lang/String;", data.object());
            QString tokenId = token.toString();
            qInfo() << tokenId;
            if (tokenId.size() > 0)
            {
                // If we asked for a sign in, then we should not try
                // to sign in concurrently when resuming the activity.
                // If we just asked for a token, then the previous
                // sign in mode may be attempted when resuming the activity.
                if (QGSI_SIGN_IN == receiverRequestCode)
                    qGoogleSignInApp->setHandlingActivityResult(true);
                emit caller->gsiTokenReceived(tokenId, static_cast<GSIJavaIntent>(receiverRequestCode));
            }
        }
        else
        {
            // We SHOULD NOT rely on getting an error to stop waiting for logging. Maybe an answer never
            // comes back from a sign-in request. That's life. Nevertheless, IF we get an error, it
            // would be nice to be able to handle it in a better way than doing nothing, if appropriate.
            qInfo() << "handleActivityResult() failed : " << data.toString();
            QSharedPointer<const QAndroidJniObject> jniObject(new QAndroidJniObject(data.object()));
            emit caller->gsiTokenRequestFailed(resultCode, jniObject);
        }
    }
}

void QAuthGSI::startGSIIntent(QAuthGSI::GSIJavaIntent reason)
{
    QAndroidJniObject jItent = QAndroidJniObject::callStaticObjectMethod("org.renan.android.firebase.auth.QGoogleSignIn", "getGSIIntent", "(Landroid/content/Context;)Landroid/content/Intent;", QtAndroid::androidActivity().object());
    QtAndroid::startActivity(jItent, reason, activityReceiver.get());
}

QString QAuthGSI::obtainTokenImpl(bool silently, QAuthGSI::GSIJavaIntent reason)
{
    QAndroidJniObject result = QAndroidJniObject::callStaticObjectMethod("org.renan.android.firebase.auth.QGoogleSignIn", "getTokenIdFromSignedAccount", "(Landroid/content/Context;)Ljava/lang/String;", QtAndroid::androidActivity().object());
    QString tokenId = result.toString();
    qInfo() << tokenId;
    bool resetToken = false;
    bool forceIntent = false;
    if (resetToken)
        tokenId = "";
    // no user previously signed in
    if (forceIntent || 0 == tokenId.size())
    {
        if (!silently)
            startGSIIntent(reason);
    }
    return tokenId;
}

void QAuthGSI::signIn(bool silently)
{
    QString tokenId = obtainTokenImpl(silently, GSIJavaIntent::QGSI_SIGN_IN);
    if (0 < tokenId.size())
    {
        // Is the token a valid one ?
        QAndroidJniObject result = QAndroidJniObject::callStaticObjectMethod("org.renan.android.firebase.auth.QGoogleSignIn", "refreshToken", "(Landroid/content/Context;Z)Ljava/lang/String;", QtAndroid::androidActivity().object(), silently);
        tokenId = result.toString();
        if (0 == tokenId.size())
        {
            emit failedRefresh(ERROR_UNKNOWN, silently);   // implement any behaviour
        }
        else if (0 == WAIT_FOR_ASYNC_ANSWER.compare(tokenId))
        {
            // must do nothing : treated in a callback
        }
        else
            emit gsiTokenReceived(tokenId, GSIJavaIntent::QGSI_SIGN_IN);
    }
}

void QAuthGSI::signOut()
{
    auto result = QAndroidJniObject::callStaticMethod<jboolean>("org.renan.android.firebase.auth.QGoogleSignIn", "signOut", "(Landroid/content/Context;)Z", QtAndroid::androidActivity().object());
    if (result)
        emit successfulSignOut();
}

void QAuthGSI::obtainToken()
{
    QString tokenId = obtainTokenImpl(false, GSIJavaIntent::QGSI_GET_TOKEN);
    if (0 < tokenId.size())
        emit gsiTokenReceived(tokenId, GSIJavaIntent::QGSI_GET_TOKEN);
}

QAuthGSI::QAuthGSIRegisterer::QAuthGSIRegisterer()
{
    qRegisterMetaType<QSharedPointer<const QAndroidJniObject>>();
    qRegisterMetaType<QAuthGSI::GSIJavaIntent>();
}
