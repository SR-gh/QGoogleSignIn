#include "quser.h"
#include <QQmlEngine>

const QUser::QUserRegisterer QUser::registerer;

QUser::QUser(QObject *parent) : QObject(parent)
{

}

//QUser::QUser(const QUser &u) : QObject (u)
//{
//    if (&u != this)
//    {
//        setSignedIn(u.getSignedIn());
//        setEmail(u.getEmail());
//        setName(u.getName());
//        setUrl(u.getUrl());
//    }
//    return *this;
//}

//QUser &QUser::operator=(const QUser &u)
//{
//    if (&u != this)
//    {
//        setSignedIn(u.getSignedIn());
//        setEmail(u.getEmail());
//        setName(u.getName());
//        setUrl(u.getUrl());
//    }
//    return *this;
//}

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
    isSigned = false;
    email.clear();
    name.clear();
    url.clear();
}

QUser::QUserRegisterer::QUserRegisterer()
{
    qmlRegisterType<QUser>("GoogleSignIn.Entities", 1, 0, "User");
}
