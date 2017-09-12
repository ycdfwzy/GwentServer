#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include "mytcpsocket.h"
#include "waitqueue.h"
#include <QVector>

class gameServer;

class MyTCPServer : public QTcpServer
{
public:
    MyTCPServer(gameServer *gs_);
    ~MyTCPServer();

    void incomingConnection(qintptr);
    void Read_Data(MyTCPSocket*);
    void Send_Data(MyTCPSocket* client, QString msg);
    //void Disconnect(MyTCPSocket*);
    //QVector<MyTCPSocket*> clients;

    gameServer *gs;

signals:
    void readyReadClient(MyTCPSocket*);
};

#endif // MYTCPSERVER_H
