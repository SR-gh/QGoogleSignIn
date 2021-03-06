#ifndef QAUTHGSI_H
#define QAUTHGSI_H

#include <QGuiApplication>
#include <QAndroidActivityResultReceiver>
#include <memory>

class QAuthGSI : public QObject
{
    Q_OBJECT
public:
    typedef enum
    {
        QGSI_SIGN_IN = 1973,
        QGSI_GET_TOKEN = 1974
    } GSIJavaIntent;    // Made public because used in the interface now
private:
    typedef enum
    {
        ERROR_INVALID_ACCOUNT = 0x10001,
        ERROR_INVALID_TOKEN = 0x10002,
    } GSIJavaErrorCodes;
    typedef enum
    {
        ERROR_UNKNOWN = 0x20001,    // We do not get an informative error code in all cases.
    } GSICppErrorCodes;
    struct QGSIAARR : public QAndroidActivityResultReceiver
    {
        static constexpr int RESULT_OK = -1;
        QGSIAARR(QAuthGSI * p_caller) : caller(p_caller) {}
        QAuthGSI * caller = nullptr;
        void handleActivityResult(int receiverRequestCode, int resultCode, const QAndroidJniObject &data) override;
    };

    // Meta-type registering
    struct QAuthGSIRegisterer
    {
        QAuthGSIRegisterer();
    };
    static const QAuthGSIRegisterer registerer;

public:
    static const QString WAIT_FOR_ASYNC_ANSWER;
public:
    explicit QAuthGSI(QGuiApplication *parent);

    //Interface
    void signIn(bool silently);
    void signOut();
    void obtainToken();

    // Why is it public and here ? Because it is called by JNI handlers. NO MORE :)
private:
    void startGSIIntent(QAuthGSI::GSIJavaIntent reason);
    QString obtainTokenImpl(bool silently, QAuthGSI::GSIJavaIntent reason); // no reason to have a bool nor a QString on public interface right now
signals:
    // GSI
    void gsiTokenReceived(QString tokenId, QAuthGSI::GSIJavaIntent reason);
    void gsiTokenRequestFailed(int resultCode, QSharedPointer<const QAndroidJniObject> data);
    void failedRefresh(int statusCode, bool silently);
    void successfulSignOut();

public slots:
private:
    QGuiApplication * theApp = nullptr;
private:
    // GSI
    std::unique_ptr<QGSIAARR> activityReceiver;

};
Q_DECLARE_METATYPE(QSharedPointer<const QAndroidJniObject>)
Q_DECLARE_METATYPE(QAuthGSI::GSIJavaIntent) // After QAuthGSI declaration

#endif // QAUTHGSI_H
