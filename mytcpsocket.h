#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include "player.h"

class MyTCPSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTCPSocket();
    ~MyTCPSocket();

    void emitReadyread();
    void emitdisconnect();

    Player *player;

    void addplayer(QString name);
    void deleteplayer();

signals:
    void readyReadClient(MyTCPSocket*);
    void Disconnect(MyTCPSocket*);
};

#endif // MYTCPSOCKET_H
