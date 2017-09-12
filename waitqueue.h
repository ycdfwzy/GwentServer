#ifndef WAITQUEUE_H
#define WAITQUEUE_H

#include <QObject>
#include <QList>
#include <QStringList>
#include "cards/card.h"
#include "cards/deck.h"
#include "battle.h"
#include "mytcpsocket.h"
#include "mytcpserver.h"

class gameServer;

class WaitQueue : public QObject
{
    Q_OBJECT
public:
    explicit WaitQueue(QObject *parent = nullptr);

    static void readPlay(gameServer*, MyTCPSocket*, QString);
    static void stopwait(MyTCPSocket*);

signals:

public slots:

private:
    static QList<MyTCPSocket*> *waiting;
    static QStringList *deckname;
    static gameServer *gs;
};

#endif // WAITQUEUE_H
