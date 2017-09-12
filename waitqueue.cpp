#include "waitqueue.h"
#include "gameserver.h"

gameServer* WaitQueue::gs = nullptr;
QList<MyTCPSocket*>* WaitQueue::waiting = nullptr;
QStringList* WaitQueue::deckname = nullptr;

WaitQueue::WaitQueue(QObject *parent) : QObject(parent)
{

}

void WaitQueue::readPlay(gameServer *gs_, MyTCPSocket *client, QString name){
    // do more valid matching algorithm

    if (gs == nullptr){
        gs = gs_;
    }
    if (waiting == nullptr){
        waiting = new QList<MyTCPSocket*>;
        waiting->clear();
    }
    if (deckname == nullptr){
        deckname = new QStringList;
        deckname->clear();
    }

    if (!waiting->empty()){
        MyTCPSocket *client0 = waiting->front();
        QString name0 = deckname->front();
        waiting->pop_front();
        deckname->pop_front();
        gs->matched(client0, name0, client, name);
    } else
    {
        waiting->append(client);
        deckname->append(name);
    }
}

void WaitQueue::stopwait(MyTCPSocket *client){
    int index = waiting->indexOf(client);
    if (index != -1){
        waiting->removeAt(index);
        deckname->removeAt(index);
    }
}
