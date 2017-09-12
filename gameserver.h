#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariant>
#include <QVariantMap>
#include <QVariantList>
#include <QFile>
#include "mytcpsocket.h"
#include "mytcpserver.h"
#include "player.h"
#include "battle.h"

class gameServer : public QObject
{
    Q_OBJECT
public:
    gameServer();
    ~gameServer();

    MyTCPServer *server;
    QList<MyTCPSocket*> clients;

    void client_Disconnected(MyTCPSocket*);
    void checklogin(MyTCPSocket*, QString, QString);
    void matched(MyTCPSocket*, QString, MyTCPSocket*, QString);
    void Send_Data(MyTCPSocket*, QString);
    void dealwithmsg(MyTCPSocket*, QString);

public slots:
    void onebattleover(Battle*);

private:
    QList<Battle*> battlelist;
};

#endif // GAMESERVER_H
