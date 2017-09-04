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

    QObject::connect(ts, &MyTCPSocket::readyReadClient, this, &MyTCPServer::Read_Data);
    QObject::connect(ts, &MyTCPSocket::Disconnect, this, &MyTCPServer::Disconnect);
    gs->clients.push_back(ts);
}

void MyTCPServer::Read_Data(MyTCPSocket* client){
    QByteArray buffer = client->readAll();
    QString str = buffer;
    qDebug() << str;
    if (str.startsWith('I')){
        QString str0 = str.mid(1);
        QStringList sl = str0.split(' ');
        if (sl.length() == 2)
            gs->checklogin(client, sl.at(0), sl.at(1));
        //qDebug() << sl.at(0);
        //qDebug() << sl.at(1);
    }
    //qDebug() << str;
}

void MyTCPServer::Send_Data(MyTCPSocket* client, QString msg){
    client->write(msg.toLatin1());
}

void MyTCPServer::Disconnect(MyTCPSocket* client){
    QObject::disconnect(client, &MyTCPSocket::readyReadClient, this, &MyTCPServer::Read_Data);
    QObject::disconnect(client, &MyTCPSocket::Disconnect, this, &MyTCPServer::Disconnect);
}
