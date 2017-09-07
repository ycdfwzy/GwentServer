#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QString>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QVariantMap>
#include <QList>
#include "cards/deck.h"

class MyTCPSocket;

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(MyTCPSocket  *client_, QString Alldata, QObject *parent = nullptr);
    ~Player();

    MyTCPSocket *client;
    QString name;

signals:

public slots:
private:

    int totalgames, victorygames, drawgames,defeatgames;
    QList<Deck*> decks;
};

#endif // PLAYER_H
