#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include "mytcpsocket.h"

class gameServer : public QObject
{
    Q_OBJECT
public:
    gameServer();
    ~gameServer();

    QTcpServer *server;
    QVector<MyTCPSocket*> clients;

    void server_New_Connect();
    void client_Disconnected();
    void Read_Data(MyTCPSocket*);
};

#endif // GAMESERVER_H
