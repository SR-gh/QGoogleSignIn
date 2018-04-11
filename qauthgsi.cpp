#include "qauthgsi.h"
#include <qdebug.h>
#include <QAndroidJniObject>
#include <QtAndroid>

const QString QAuthGSI::WAIT_FOR_ASYNC_ANSWER = "wait_async";

QAuthGSI::QAuthGSI(QGuiApplication *parent) : QObject(parent)
{
    if (!parent)
        throw "I must be given an application";
    activityReceiver = std::unique_ptr<QGSIAARR>(new QGSIAARR(this));

}

void QAuthGSI::QGSIAARR::handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data)
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

void QAuthGSI::startGSIIntent()
{
    QAndroidJniObject jItent = QAndroidJniObject::callStaticObjectMethod("org.renan.android.firebase.auth.QGoogleSignIn", "getGSIIntent", "(Landroid/content/Context;)Landroid/content/Intent;", QtAndroid::androidActivity().object());
    QtAndroid::startActivity(jItent, QGSI_SIGN_IN, activityReceiver.get());
}

void QAuthGSI::signIn(bool silently)
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
        if (0 == tokenId.size())
        {
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

void QAuthGSI::signOut()
{
    auto result = QAndroidJniObject::callStaticMethod<jboolean>("org.renan.android.firebase.auth.QGoogleSignIn", "signOut", "(Landroid/content/Context;)Z", QtAndroid::androidActivity().object());
    if (result)
        emit successfulSignOut();
}
