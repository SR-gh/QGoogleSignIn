#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QString>
#include <QObject>

class Controller
{
public:
    Controller(const QString& aName);
    QString getName() const
    {
        return name;
    }
    virtual const QObject* asQObject() const=0;
    virtual QObject* asQObject() =0;
private:
    QString name;
};

#endif // CONTROLLER_H
