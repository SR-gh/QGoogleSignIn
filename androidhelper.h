#ifndef ANDROIDHELPER_H
#define ANDROIDHELPER_H
#include <QString>

namespace AndroidHelper
{

typedef enum
{
    SHORT = 0,
    LONG = 1
} Duration;

void showToast(const QString &message, AndroidHelper::Duration duration = AndroidHelper::Duration::LONG);

}
#endif // ANDROIDHELPER_H
