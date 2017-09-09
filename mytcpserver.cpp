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
    QObject::connect(ts, &MyTCPSocket::Disconnect, gs, &gameServer::client_Disconnected);
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
        qDebug() << gs->clients.size();
        //qDebug() << sl.at(1);
    } else
    if (str.startsWith("AddDeck: ")){
        QString str0 = str.mid(9);
        QStringList sl = str0.split(' ');
        client->update_deck(sl);
    }
    //qDebug() << str;
}

void MyTCPServer::Send_Data(MyTCPSocket* client, QString msg){
    client->write(msg.toLatin1());
    client->waitForBytesWritten();
}
/*
void MyTCPServer::Disconnect(MyTCPSocket* client){

}
*/
