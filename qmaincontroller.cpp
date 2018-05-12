#include "qmaincontroller.h"
#include "qgooglesigninapplication.h"
#include "androidhelper.h"

QMainController::QMainController(const QString& aName, QObject *parent) : QObject(parent), Controller(aName)
{
    connect(qGoogleSignInApp, &QGoogleSignInApplication::error, this, &QMainController::onError);
}

void QMainController::signInWithGSI()
{
    // TODO : use a delegate with an Interface, not a global ?
    qGoogleSignInApp->signInWithGSI(false);
}

void QMainController::signOut()
{
    // TODO : use a delegate with an Interface, not a global ?
    qGoogleSignInApp->signOut();
}

void QMainController::signInWithEmail(QString email, QString password)
{
    qGoogleSignInApp->signInWithEmail(email, password);
}

void QMainController::signUpWithEmail(QString email, QString password)
{
    qGoogleSignInApp->signUpWithEmail(email, password);
}

void QMainController::onError(const QString &errorMessage)
{
    AndroidHelper::showToast(errorMessage);
}
