#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariant>
#include <QVariantMap>
#include <QVariantList>
#include <QFile>
#include "mytcpsocket.h"
#include "mytcpserver.h"

class gameServer : public QObject
{
    Q_OBJECT
public:
    gameServer();
    ~gameServer();

    MyTCPServer *server;
    QVector<MyTCPSocket*> clients;

    void server_New_Connect();
    void client_Disconnected();
    //void Read_Data(MyTCPSocket*);
    void checklogin(MyTCPSocket*, QString, QString);

};

#endif // GAMESERVER_H
