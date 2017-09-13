#ifndef BATTLE_H
#define BATTLE_H

#include <QObject>
#include "player.h"
#include "cards/deck.h"
#include "cards/card.h"
#include "mytcpsocket.h"
#include <QTimer>
#include <QStringList>

class gameServer;

class Battle : public QObject
{
    Q_OBJECT
public:
    Battle(gameServer *gs, MyTCPSocket *client1, QString deck1, MyTCPSocket *client2, QString deck2, QObject *parent = nullptr);
    void readyPlay();
    void shuffle(int id, QList<Card *> *src);

    void pumping(int, int);
    void Mulligan(int, int);

    enum curState{turn0=0, turn1=1, mulligan, breaking, gameover};
    enum SKY{clean=0, fog=1, frost=2, rain=3};
    enum overstate{player0win = 0, player1win = 1, draw = 2};
    enum turnstate{getsrc, gettar, poxiao, chongzheng, gaier, dagang, zhangzhe0, zhangzhe1, zhangzhe2};


    void zhihuihaojiao(int, int);
    void mianyizengqiang(int, int);
    void silie(int);
    void birinongwu(int);
    void qingpengdayu(int);
    void ciguhanbing(int);
    void niuquzhijing();
    void qingkong();


signals:
    void startroundsignal();
    void endroundsignal();
    void nextroundsignal();
    void startturnsignal();
    void switchturnsignal();
    void endturnsignal();
    void endmulligansignal();
    void oversignal(overstate);
    void send_to_server(Battle*);

public slots:
    void startround();
    void endround();
    void nextround();
    void endmulligan();
    void startturn();
    void switchturn();
    void endturn();
    void over(overstate);
    void topassed(MyTCPSocket*);
    void tosurrender(MyTCPSocket*);

    void domulligan(int, int);
    void clicksomething(MyTCPSocket*, QString msg);
    void get_src(int, QString);
    void get_tar(int, QString);

private:
    gameServer *gs;

    MyTCPSocket *client[2];
    Player *player[2];
    Card *leader[2];
    QList<Card*> *cards[12];
    /*
    QList<Card*> *deck[2];
    QList<Card*> *graveyard[2];
    QList<Card*> *card[2];
    QList<Card*> *melee[2];
    QList<Card*> *ranged[2];
    QList<Card*> *siege[2];
    */
    /*
    SKY sky_melee[2];
    SKY sky_ranged[2];
    SKY sky_siege[2];
    */
    SKY sky[12];

    QStringList op;

    int round;
    bool passed[2];
    turnstate ts;
    int cnt[2];
    int lastwinner;
    int score[2];
    curState cur;
    QTimer *timer;
    Card *src;
};

#endif // BATTLE_H
