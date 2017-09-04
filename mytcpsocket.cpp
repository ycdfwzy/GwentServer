#include "mytcpsocket.h"

MyTCPSocket::MyTCPSocket(){
    player = nullptr;
    QObject::connect(this, &MyTCPSocket::readyRead, this, &MyTCPSocket::emitReadyread);
    QObject::connect(this, &MyTCPSocket::disconnected, this, &MyTCPSocket::emitdisconnect);
}

MyTCPSocket::~MyTCPSocket(){}

void MyTCPSocket::emitReadyread(){
    emit readyReadClient(this);
}

void MyTCPSocket::emitdisconnect(){
    emit Disconnect(this);
}

void MyTCPSocket::addplayer(QString name){
    player = new Player(this, name);
}

void MyTCPSocket::deleteplayer(){
    delete player;
}
