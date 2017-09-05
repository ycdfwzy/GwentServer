#ifndef CARDS_CARD_H
#define CARDS_CARD_H

#include <QString>
#include <QObject>

class card : public QObject{
    Q_OBJECT
public:
    card(int num, QObject *parent = nullptr);
    QString name;
    int BaseBlood;
    int BoostBlood;

};

#endif //CARDS_CARD_H
