#ifndef QFIREBASE_H
#define QFIREBASE_H

#include <QGuiApplication>
#include <memory>
// Firebase
#include <QAndroidJniEnvironment>
#include "firebase/app.h"
#include "firebase/auth.h"

class QFirebase : public QObject
{
    Q_OBJECT
public:
    explicit QFirebase(QGuiApplication *parent);

    // Interface
    void signInWithGSI(QString tokenId);
    void signOut();

signals:
    // FB
    void firebaseAuthSucceed(firebase::auth::User* user);
    void firebaseAuthFailed(int errorCode, QString errorMessage);

public slots:
private:
    QGuiApplication * theApp = nullptr;
private:
    //Firebase
    QAndroidJniEnvironment m_qjniEnv;
    std::unique_ptr<firebase::App> m_firebaseApp; // must be deleted before m_qjniEnv, hence order of declaration matters.
    firebase::auth::Auth* m_firebaseAuth = nullptr; // non owning
};

#endif // QFIREBASE_H
