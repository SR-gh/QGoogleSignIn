#ifndef QAUTHGSI_H
#define QAUTHGSI_H

#include <QGuiApplication>
#include <QAndroidActivityResultReceiver>
#include <memory>

class QAuthGSI : public QObject
{
    Q_OBJECT
    enum
    {
        QGSI_SIGN_IN = 1973
    }GSIJavaIntent;
    enum
    {
        ERROR_INVALID_ACCOUNT = 0x10001,
        ERROR_INVALID_TOKEN = 0x10002,
    } GSIJavaErrorCodes;
    enum
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

public:
    static const QString WAIT_FOR_ASYNC_ANSWER;
public:
    explicit QAuthGSI(QGuiApplication *parent);

    //Interface
    void signIn(bool silently);
    void signOut();

    // Why is it public and here ? Because it is called by JNI handlers. NO MORE :)
private:
    void startGSIIntent();
signals:
    // GSI
    void gsiTokenReceived(QString tokenId);
    void failedRefresh(int statusCode, bool silently);
    void successfulSignOut();
private:
    void onGsiTokenReceived(QString tokenId);
    void onFailedRefresh(int statusCode, bool silently);
    void onSuccessfulSignOut();

public slots:
private:
    QGuiApplication * theApp = nullptr;
private:
    // GSI
    std::unique_ptr<QGSIAARR> activityReceiver;

};

#endif // QAUTHGSI_H
