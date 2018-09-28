#include "quser.h"
#include <QQmlEngine>

const QUser::QUserRegisterer QUser::registerer;

QUser::QUser(QObject *parent) : QObject(parent)
{
}

bool QUser::getSignedIn() const
{
    return isSigned;
}

void QUser::setSignedIn(bool value)
{
    const bool changed = value != isSigned;
    if (changed)
    {
        isSigned = value;
        emit signedInChanged();
    }
}

QString QUser::getEmail() const
{
    return email;
}

void QUser::setEmail(const QString &value)
{
    const bool changed = value != email;
    if (changed)
    {
        email = value;
        emit emailChanged();
    }
}

QString QUser::getName() const
{
    return name;
}

void QUser::setName(const QString &value)
{
    const bool changed = value != name;
    if (changed)
    {
        name = value;
        emit nameChanged();
    }
}

QString QUser::getUrl() const
{
    return url;
}

void QUser::setUrl(const QString &value)
{
    const bool changed = value != url;
    if (changed)
    {
        url = value;
        emit urlChanged();
    }
}

void QUser::clear()
{
    setSignedIn(false);
    setEmail("");
    setName("");
    setUrl("");
}

QUser::QUserRegisterer::QUserRegisterer()
{
    qmlRegisterType<QUser>("GoogleSignIn.Entities", 1, 0, "User");
}
