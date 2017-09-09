#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QObject>
#include <QtNetwork/QTcpSocket>
#include "player.h"
#include "cards/deck.h"
#include "cards/card.h"

class MyTCPSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTCPSocket();
    ~MyTCPSocket();

    void emitReadyread();
    void emitdisconnect();
    void update_deck(QStringList&);
    void addplayer(QString Alldata);
    void deleteplayer();
    Player* get_player();

signals:
    void readyReadClient(MyTCPSocket*);
    void Disconnect(MyTCPSocket*);
private:
    Player *player;
};

#endif // MYTCPSOCKET_H
