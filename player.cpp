#include "player.h"
#include "mytcpsocket.h"

Player::Player(MyTCPSocket *client_, QString name_, QObject *parent) : QObject(parent){
    client = client_;
    name = name_;
}

Player::~Player(){}
