#ifndef ANDROIDMESSAGEHANDLER_H
#define ANDROIDMESSAGEHANDLER_H
#if defined(QT_DEBUG) && defined(Q_OS_ANDROID) && !defined(Q_OS_ANDROID_EMBEDDED)
#define QGSILOG
#endif
#ifdef QGSILOG
#include <android/log.h>
#include <thread>
#include <sstream>
#include <QDateTime>
void messageHandlerWithDateTime(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QDateTime now = QDateTime::currentDateTime();
    QString formattedMessage = qFormatLogMessage(type, context, msg);

    android_LogPriority priority = ANDROID_LOG_DEBUG;
    switch (type)
    {
        case QtDebugMsg: priority = ANDROID_LOG_DEBUG; break;
        case QtInfoMsg: priority = ANDROID_LOG_INFO; break;
        case QtWarningMsg: priority = ANDROID_LOG_WARN; break;
        case QtCriticalMsg: priority = ANDROID_LOG_ERROR; break;
        case QtFatalMsg: priority = ANDROID_LOG_FATAL; break;
        default:break;
    }

    std::ostringstream oss;
    oss << std::showbase << std::hex << std::this_thread::get_id();
    __android_log_print(priority, qPrintable(now.toString("[yyyy/MM/dd;HH:mm:ss.zzz] ")+QCoreApplication::applicationName()),
                        "[%s] %s:%d (%s): %s\n", oss.str().c_str(), context.file, context.line,
                        context.function, qPrintable(formattedMessage));
}
#endif
#endif // ANDROIDMESSAGEHANDLER_H
