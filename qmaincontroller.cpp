#include "qmaincontroller.h"
#include "qgooglesigninapplication.h"

QMainController::QMainController(const QString& aName, QObject *parent) : QObject(parent), Controller(aName)
{

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
