#include "mytcpserver.h"
#include <QString>

MyTCPServer::MyTCPServer(){
    this->listen(QHostAddress::Any, 8888);
    clients.clear();
}

MyTCPServer::~MyTCPServer(){}

void MyTCPServer::incomingConnection(qintptr socketDescriptor){
    MyTCPSocket *ts = new MyTCPSocket;
    ts->setSocketDescriptor(socketDescriptor);

    QObject::connect(ts, &MyTCPSocket::readyReadClient, this, &MyTCPServer::Read_Data);
    QObject::connect(ts, &MyTCPSocket::Disconnect, this, &MyTCPServer::Disconnect);
    clients.push_back(ts);
    Send_Data(ts, "welcome!");
}

void MyTCPServer::Read_Data(MyTCPSocket* client){
    QByteArray buffer = client->readAll();
    QString str = buffer;
    qDebug() << str;
}

void MyTCPServer::Send_Data(MyTCPSocket* client, QString msg){
    client->write(msg.toLatin1());
}

void MyTCPServer::Disconnect(MyTCPSocket* client){
    QObject::disconnect(client, &MyTCPSocket::readyReadClient, this, &MyTCPServer::Read_Data);
    QObject::disconnect(client, &MyTCPSocket::Disconnect, this, &MyTCPServer::Disconnect);
}
