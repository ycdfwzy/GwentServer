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

void MyTCPSocket::addplayer(QString Alldata){
    player = new Player(this, Alldata);
}

void MyTCPSocket::deleteplayer(){
    delete player;
}

void MyTCPSocket::update_deck(QStringList &deck){
    QString name = deck.at(0);
    int sz = deck.size();
    Card *leadercard = new Card(deck.at(1).toInt());
    QList<Card*> cardlist;
    for (int i = 2; i < sz; ++i){
        QString s = deck.at(i);
        Card* tmp = new Card(s.toInt());
        cardlist.append(tmp);
    }
    player->update_deck(name, leadercard, &cardlist);
}

Player* MyTCPSocket::get_player(){ return player;}
