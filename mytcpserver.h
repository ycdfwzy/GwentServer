#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QtNetwork/QTcpServer>
#include "mytcpsocket.h"
#include <QVector>

class MyTCPServer : public QTcpServer
{
public:
    MyTCPServer();
    ~MyTCPServer();

    void incomingConnection(qintptr);
    void Read_Data(MyTCPSocket*);
    void Send_Data(MyTCPSocket* client, QString msg);
    void Disconnect(MyTCPSocket*);
    QVector<MyTCPSocket*> clients;

signals:
    void readyReadClient(MyTCPSocket*);
};

#endif // MYTCPSERVER_H
