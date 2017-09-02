#include "gameserver.h"
#include <QDebug>

gameServer::gameServer()
{
    server = new QTcpServer;
    server->listen(QHostAddress::Any, 8888);
    clients.clear();

    QObject::connect(server, &QTcpServer::newConnection, this, &gameServer::server_New_Connect);
}

gameServer::~gameServer(){}

void gameServer::server_New_Connect(){
    //clients.push_back( server->nextPendingConnection() );
    //QObject::connect(clients.back(), &MyTCPSocket::readyReadClient, this, &gameServer::Read_Data);
    //QObject::connect(clients.back(), &MyTCPSocket::disconnected, this, &gameServer::client_Disconnected);
    //qDebug() << "A clinet is connected!";
    //clients.back()->write("Hi!");
}



void gameServer::client_Disconnected(){
    qDebug() << "A client is disconnected";
}
