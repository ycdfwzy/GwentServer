#include "mytcpserver.h"
#include "gameserver.h"
#include <QString>
#include <QDebug>

MyTCPServer::MyTCPServer(gameServer *gs_){
    gs = gs_;
    this->listen(QHostAddress::Any, 8888);
    gs->clients.clear();
}

MyTCPServer::~MyTCPServer(){}

void MyTCPServer::incomingConnection(qintptr socketDescriptor){
    qDebug() << "incomingConnection";
    MyTCPSocket *ts = new MyTCPSocket;
    ts->setSocketDescriptor(socketDescriptor);
    //ts->socketDescriptor()

    QObject::connect(ts, &MyTCPSocket::readyReadClient, this, &MyTCPServer::Read_Data);
    //QObject::connect(ts, &MyTCPSocket::Disconnect, gs, &gameServer::client_Disconnected);
    //gs->clients.push_back(ts);
}

void MyTCPServer::Read_Data(MyTCPSocket* client){
    QByteArray buffer = client->readAll();
    QString str = buffer;
    QStringList strlist = str.split(';');
    int sz = strlist.size();
    for (int i = 0; i < sz-1; ++i)
        gs->dealwithmsg(client, QString(strlist.at(i)));
}

void MyTCPServer::Send_Data(MyTCPSocket* client, QString msg){
    msg = msg + ";";
    client->write(msg.toLatin1());
    //client->flush();
    while (client->waitForBytesWritten());
}
/*
void MyTCPServer::Disconnect(MyTCPSocket* client){

}
*/
