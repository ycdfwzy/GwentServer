#include "deck.h"
#include <QDebug>

Deck* Deck::ALL = nullptr;

Deck::Deck(const QString &name_, QJsonValue &info, QObject *parent) : QObject(parent)
{
    //qWarning() << "Deck";
    //qWarning() << info;
    name = name_;
    if (info.isObject()){
        QJsonObject inf = info.toObject();
        leader = new Card(inf.take("leader").toVariant().toInt());
        QJsonArray cards_ = inf.take("cards").toArray();
        int sz = cards_.size();
        for (int i = 0; i < sz; ++i){
            QJsonValue x = cards_.at(i);
            cards.append(new Card(x.toInt()));
        }
    }
}

Deck::Deck(const QString &name_, QString info, QObject *parent) : QObject(parent){
    name = name_;
    leader = nullptr;
    QStringList sl = info.split(',');
    int sz = sl.size();
    for (int i = 0; i < sz; ++i){
        QString tmp = sl.at(i);
        cards.append(new Card(tmp.toInt()));
    }
}

Deck::Deck(const QString &name_, Card* leader_, QList<Card*> *cardlist, QObject *parent) : QObject(parent){
    name = name_;
    leader = new Card(leader_->get_id());
    change_cardlist(cardlist);
}

Deck* Deck::get_ALL(){
    if (ALL == nullptr)
        ALL = new Deck("ALL", "16,8,18,41,19,17,10,7,101,57,58,66,75,169,188,181,182,256,225,236,281,294,63,167,171");
    return ALL;
}

void Deck::change_cardlist(QList<Card*> *cardlist){
    int sz = cards.size();
    for (int i = 0; i < sz; ++i)
        delete cards.at(i);
    cards.clear();

    if (cardlist != nullptr){
        sz = cardlist->size();
        for (int i = 0; i < sz; ++i){
            Card *tmp = new Card(cardlist->at(i)->get_id());
            cards.append(tmp);
        }
    }
}

QString Deck::get_name()const{return name;}
Card* Deck::get_leader()const{return leader;}
QList<Card*>& Deck::get_cards(){return cards;}
QList<Card*>* Deck::get_cards_pointer(){return &cards;}
