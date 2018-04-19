#ifndef QMAINCONTROLLER_H
#define QMAINCONTROLLER_H

#include <QObject>
#include "controller.h"

class QMainController : public QObject, public Controller
{
    Q_OBJECT
public:
    explicit QMainController(const QString& aName, QObject *parent = nullptr);
    const QObject* asQObject() const override { return this; }
    QObject* asQObject() override { return this; }

    Q_INVOKABLE void signInWithGSI();
    Q_INVOKABLE void signOut();
signals:

public slots:
};

#endif // QMAINCONTROLLER_H
