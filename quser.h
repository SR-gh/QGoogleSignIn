#ifndef QUSER_H
#define QUSER_H

#include <QObject>

class QUser : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(bool signedIn READ getSignedIn WRITE setSignedIn NOTIFY signedInChanged)
    Q_PROPERTY(bool anonymous READ isAnonymous WRITE setAnonymous NOTIFY anonymousChanged)
    Q_PROPERTY(QString email READ getEmail WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString url READ getUrl WRITE setUrl NOTIFY urlChanged)

    explicit QUser(QObject *parent = nullptr);
//    QUser(const QUser &);   // WTF : why is this default and not operator=() ? It should be, using QObject(const QObject&) constructor. Hum not possible : what am I doing wrong, again ?
//    QUser& operator=(const QUser &); // not default
    ~QUser() override=default;

    bool getSignedIn() const;
    void setSignedIn(bool value);
    bool isAnonymous() const;
    void setAnonymous(bool value);
    QString getEmail() const;
    void setEmail(const QString &value);
    QString getName() const;
    void setName(const QString &value);
    QString getUrl() const;
    void setUrl(const QString &value);

    void clear();

private:
    // Meta-type registering
    struct QUserRegisterer
    {
        QUserRegisterer();
    };
    static const QUserRegisterer registerer;

signals:
    void signedInChanged();
    void anonymousChanged();
    void emailChanged();
    void nameChanged();
    void urlChanged();

public slots:
private:
    bool isSigned = false;
    bool anonymous = false;
    QString email;
    QString name;
    QString url;
};

#endif // QUSER_H
