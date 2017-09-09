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

class gameServer : public QObject
{
    Q_OBJECT
public:
    gameServer();
    ~gameServer();

    MyTCPServer *server;
    QList<MyTCPSocket*> clients;

    //void server_New_Connect();
    void client_Disconnected(MyTCPSocket*);
    //void Read_Data(MyTCPSocket*);
    void checklogin(MyTCPSocket*, QString, QString);

};

#endif // GAMESERVER_H
