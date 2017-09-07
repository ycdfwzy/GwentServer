#include "deck.h"
#include <QDebug>

Deck::Deck(QJsonValue &info, QObject *parent) : QObject(parent)
{
    //qWarning() << "Deck";
    //qWarning() << info;
    if (info.isObject()){
        QJsonObject inf = info.toObject();
        name = inf.take("deckname").toVariant().toString();
        leader = inf.take("leader").toVariant().toInt();
        QJsonArray cards_ = inf.take("cards").toArray();
        int sz = cards_.size();
        for (int i = 0; i < sz; ++i){
            QJsonValue x = cards_.at(i);
            cards.append(x.toInt());

        }
    }
}
