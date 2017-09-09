#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QString>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QVariantMap>
#include <QVariantList>
#include <QList>
#include <QFile>
#include "cards/card.h"
#include "cards/deck.h"

class MyTCPSocket;

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(MyTCPSocket *client_, const QString &Alldata, QObject *parent = nullptr);
    ~Player();

    MyTCPSocket *client;

    QString get_name()const;
    int get_totalgames()const;
    int get_victorygames()const;
    int get_drawgames()const;
    int get_defeatgames()const;
    int get_nextdeckname();
    bool update_deck(QString name, Card* leader, QList<Card*> *cardlist);
    QList<Deck*>& get_decks();
    QList<Deck*>* get_decks_pointer();
    void writetofile();

signals:

public slots:
private:
    QString name;
    int totalgames, victorygames, drawgames, defeatgames;
    QList<Deck*> decks;
    int name_helper;
};
/*
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
};*/

#endif // PLAYER_H
