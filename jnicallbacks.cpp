#include <qdebug.h>
#include "qgooglesigninapplication.h"
#include <jni.h>

extern "C" JNIEXPORT void JNICALL Java_org_renan_android_firebase_auth_QGoogleSignIn_onSuccessfulRefresh(JNIEnv *env, jclass cls,  jstring token)
{
    qInfo() << "native onSuccessfulRefresh";

    // http://doc.qt.io/qt-5/qandroidjniobject.html#toString
    QAndroidJniObject javaString = token;
    QString tokenId = javaString.toString();
    // TODO: Think. Do we have another context than app that may launch
    // concurrent refreshes ? Shall we pass a ref through JNI to get this
    // context back ?
    emit qGoogleSignInApp->gsiTokenReceived(tokenId);
}

extern "C" JNIEXPORT void JNICALL Java_org_renan_android_firebase_auth_QGoogleSignIn_onFailedRefresh(JNIEnv *env, jclass cls, jint statusCode, jboolean silently)
{
    qInfo() << "native onFailedRefresh, code=" << statusCode;
    // TODO : is this catch-all behaviour OK ? I don't think so.
    // At least we should not loop !
    emit qGoogleSignInApp->failedRefresh(statusCode, silently);// TODO : think ! I'm adding this one to handle my app case
}

extern "C" JNIEXPORT void JNICALL Java_org_renan_android_firebase_auth_QGoogleSignIn_onSuccessfulSignOut(JNIEnv *env, jclass cls)
{
    qInfo() << "native onSuccessfulSignOut";
    emit qGoogleSignInApp->successfulSignOut();
}

