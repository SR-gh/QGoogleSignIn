#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QString>
#include <QObject>

class Controller
{
public:
    explicit Controller(const QString& aName);
    virtual ~Controller() = default;
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
