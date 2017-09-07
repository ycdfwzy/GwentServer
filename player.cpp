#include "player.h"
#include "mytcpsocket.h"
#include <QDebug>

Player::Player(MyTCPSocket *client_, QString Alldata, QObject *parent) : QObject(parent){
    client = client_;

    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(Alldata.toUtf8(), &error);
    if (error.error == QJsonParseError::NoError){
        if (!(jsonDocument.isNull() || jsonDocument.isEmpty())){
            if (jsonDocument.isObject()){
                QJsonObject info = jsonDocument.object();

                name = info.take("name").toVariant().toString();
                totalgames = info.take("totalgames").toVariant().toInt();
                victorygames = info.take("victorygames").toVariant().toInt();
                drawgames = info.take("drawgames").toInt();
                defeatgames = info.take("defeatgames").toInt();

                //QJsonValue decksJsonValue = info.take("decks");
                QJsonArray decks_ = info.take("decks").toArray();
                int sz = decks_.size();
                Deck *dk;
                for (int i = 0; i < sz; ++i){
                    //new deck
                    dk = new Deck(decks_.at(i));
                    decks.append(dk);
                }
            }
        }
    }
}

Player::~Player(){}
