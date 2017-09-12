#include "player.h"
#include "mytcpsocket.h"
#include "Battle.h"
#include <QDebug>

Player::Player(MyTCPSocket *client_, const QString &Alldata, QObject *parent) : QObject(parent){
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
                QJsonObject decks_ = info.take("decks").toObject();
                Deck *dk;
                QStringList ks = decks_.keys();
                int sz = ks.size();
                for (int i = 0; i < sz; ++i){
                    QString tmp = ks.at(i);
                    dk = new Deck(tmp , decks_.value(tmp));
                    decks.append(dk);
                }
            }
        }
    }
    name_helper = 0;
    battle = nullptr;
}

Player::~Player(){}

bool Player::update_deck(QString name, Card* leader, QList<Card*> *cardlist){

    bool flag = false;
    int sz = decks.size();
    for (int i = 0; i < sz; ++i)
    if (name.compare(decks.at(i)->get_name()) == 0){
        flag = true;
        decks.at(i)->change_cardlist(cardlist);
        break;
    }
    if (!flag){
        Deck *tmp = new Deck(name, leader, cardlist);
        decks.append(tmp);
        name_helper++;
    }

    writetofile();
    return flag;
}

void Player::writetofile(){
    QJsonObject usr;
    usr.insert("name", name);
    usr.insert("totalgames", totalgames);
    usr.insert("victorygames", victorygames);
    usr.insert("drawgames", drawgames);
    usr.insert("defeatgames", defeatgames);

    QJsonObject DECKS;
    foreach (Deck* x, decks) {
        QString deckname = x->get_name();

        QJsonObject DECK;
        DECK.insert("leader", x->get_leader()->get_id());

        QJsonArray cards;
        QList<Card*> *tmp = x->get_cards_pointer();
        int sz = tmp->size();
        for (int i = 0; i < sz; ++i)
            cards.insert(0, tmp->at(i)->get_id());

        DECK.insert("cards", cards);

        DECKS.insert(deckname, DECK);
    }
    usr.insert("decks", DECKS);
    //qWarning() << usr;
    //qDebug() << usr;

    QString path = "D:\\git\\GwentServer\\data\\users\\" + name + ".json";
    qDebug() << path;
    QFile file(path);
    if (file.open(QFile::WriteOnly | QFile::Truncate)){
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << QJsonDocument(usr).toJson();
        file.close();
    }

}

QList<Card*>* Player::get_cardlist(QString name){
    QList<Card*>* ret = nullptr;
    foreach (Deck* deck, decks)
        if (name.compare(deck->get_name()) == 0){
            ret = deck->get_cards_pointer();
        }
    return ret;
}

void Player::wingame(){
    totalgames++;
    victorygames++;
}

void Player::drawgame(){
    totalgames++;
    drawgames++;
}

void Player::losegame(){
    defeatgames++;
    drawgames++;
}

void Player::battleover(){
    battle = nullptr;
    writetofile();
}

void Player::startbattle(Battle *b){
    battle = b;
}

QString Player::get_name()const{return name;}
int Player::get_totalgames()const{return totalgames;}
int Player::get_victorygames()const{return victorygames;}
int Player::get_drawgames()const{return drawgames;}
int Player::get_defeatgames()const{return defeatgames;}
QList<Deck*>& Player::get_decks(){return decks;}
QList<Deck*>* Player::get_decks_pointer(){return &decks;}
int Player::get_nextdeckname(){return name_helper+1;}
Battle* Player::get_battle(){ return battle;}
