#include "battle.h"
#include "gameserver.h"
#include <algorithm>

QString tostring(int x1, int x2, int x3){
    return QString::number(x1) + " " + QString::number(x2) + " " + QString::number(x3);
}

QString tostring(int x1, int x2, int x3, int x4){
    return QString::number(x1) + " " + QString::number(x2) + " " + QString::number(x3) + " " + QString::number(x4);
}

QString tostring(int x1, int x2, int x3, int x4, int x5){
    return QString::number(x1) + " " + QString::number(x2) + " " + QString::number(x3) + " " + QString::number(x4) + " " + QString::number(x5);
}


Battle::Battle(gameServer *gs, MyTCPSocket *client1, QString deck1, MyTCPSocket *client2, QString deck2, QObject *parent) : QObject(parent)
{
    this->gs = gs;
    this->client[0] = client1;
    this->client[1] = client2;
    player[0] = client[0]->get_player();
    player[1] = client[1]->get_player();
    player[0]->startbattle(this);
    player[1]->startbattle(this);

    shuffle(0, player[0]->get_cardlist(deck1));
    //qDebug() << deck[0]->size();
    shuffle(1, player[1]->get_cardlist(deck2));

    for (int i = 0; i < 10; ++i){
        cards[i] = new QList<Card*>;
        cards[i]->clear();
        sky[i] = clean;
    }
    /*
    for (int i = 0; i < 2; ++i){
        //deck[i] = new QList<Card*>;
        //deck[i]->clear();
        graveyard[i] = new QList<Card*>;
        graveyard[i]->clear();
        card[i] = new QList<Card*>;
        card[i]->clear();
        melee[i] = new QList<Card*>;
        melee[i]->clear();
        ranged[i] = new QList<Card*>;
        ranged[i]->clear();
        siege[i] = new QList<Card*>;
        siege[i]->clear();
        passed[i] = false;
        sky_melee[i] = clean;
        sky_ranged[i] = clean;
        sky_siege[i] = clean;
    }
    */
    round = 0;
    lastwinner = -1;
    cur = breaking;
    score[0] = score[1] = 0;

    connect(this, SIGNAL(startroundsignal()), this, SLOT(startround()));
    connect(this, SIGNAL(endroundsignal()), this, SLOT(endround()));
    connect(this, SIGNAL(nextroundsignal()), this, SLOT(nextround()));
    connect(this, SIGNAL(startturnsignal()), this, SLOT(startturn()));
    connect(this, SIGNAL(endmulligansignal()), this, SLOT(endmulligan()));
    connect(this, SIGNAL(switchturnsignal()), this, SLOT(switchturn()));
    connect(this, SIGNAL(endturnsignal()), this, SLOT(endturn()));
    connect(this, SIGNAL(oversignal(overstate)), this, SLOT(over(overstate)));

    timer = nullptr;

    QTimer *t = new QTimer(this);
    connect(t, SIGNAL(timeout()), this, SLOT(nextround()));
    t->setSingleShot(true);
    t->start(1000);
}

void Battle::shuffle(int index, QList<Card *> *src){
    cards[10^index] = new QList<Card*>;
    cards[10^index]->clear();
    //deck[index] = new QList<Card*>;
    //deck[index]->clear();
    int sz = src->size();
    int id[50];
    for (int i = 0; i < sz; ++i)
        id[i] = i;
    QString msg;
    std::random_shuffle(id, id+sz);
    for (int i = 0; i < sz; ++i){
        int t = src->at(id[i])->get_id();
        cards[10^index]->append(new Card(t));
        cards[10^index]->back()->set_loc(10^index);
        if (i == 0) msg = QString::number(t);
            else msg += " " + QString::number(t);
    }

    //qDebug() << tar->size();
    //send to players
    gs->Send_Data(client[index], "LoadDeck: 0 " + msg);
    gs->Send_Data(client[index^1], "LoadDeck: 1 " + msg);
}

void Battle::nextround(){
    qDebug() << "nextround";
    ++round;
    switch (round) {
    case 1:
        cur = mulligan;
        pumping(10, 0);
        pumping(10, 1);
        Mulligan(3, 0);
        Mulligan(3, 1);

        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(endmulligan()));
        timer->setSingleShot(true);
        timer->start(60000);
        break;
    case 2:
        cur = mulligan;
        pumping(2, 0);
        pumping(2, 1);
        Mulligan(1, 0);
        Mulligan(1, 1);

        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(endmulligan()));
        timer->setSingleShot(true);
        timer->start(20000);
        break;
    case 3:
        cur = mulligan;
        pumping(1, 0);
        pumping(1, 1);
        Mulligan(1, 0);
        Mulligan(1, 1);

        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(endmulligan()));
        timer->setSingleShot(true);
        timer->start(20000);
        break;
    default:
        break;
    }
}

void Battle::pumping(int num, int id){
    qDebug() << "pumping";
    int num_ = 0;
    for (int i = 0; i < num; ++i)
    if (!cards[10^id]->empty()){
        cards[id]->push_back( cards[10^id]->front() );
        cards[id]->back()->set_loc(id);
        cards[10^id]->pop_front();
        num_++;
    }
    //send data to players
   gs->Send_Data(client[id], "PUMP: 0 " + QString::number(num_));
   gs->Send_Data(client[id^1], "PUMP: 1 " + QString::number(num_));
}

void Battle::Mulligan(int num, int id){
    qDebug() << "mulligan";
    cnt[id] = num;
    //send data to players
    gs->Send_Data(client[id], "Mulligan" + QString::number(num));
}

void Battle::endmulligan(){
    qDebug() << "endmulligan";
    //qDebug() << timer->isActive();
    if (timer->isActive()){
        timer->stop();
        //delete timer;
        //timer = nullptr;
        cnt[0] = cnt[1] = 0;
    }
    emit startroundsignal();
}

void Battle::switchturn(){
    qDebug() << "switchturn";
    if (cur == turn0) cur = turn1;
        else cur = turn0;
    emit startturnsignal();
}

void Battle::startround(){
    qDebug() << "startround";
    if (lastwinner == -1){
        int t = qrand()&1;
        if (t) cur = turn0;
            else cur = turn1;
    } else
    {
        if (lastwinner == 0) cur = turn0;
            else cur = turn1;
    }
    passed[0] = passed[1] = false;

    //clear the board
    for (int i = 2; i < 8; ++i){
        while (!cards[i]->empty()){
            Card* card = cards[i]->front();
            cards[8^(i&1)]->append(card);
            card->set_loc(8^(i&1));
            cards[i]->pop_front();


            gs->Send_Data(client[0], "move " +  tostring(i, 0, 8^(i&1)) );
            gs->Send_Data(client[1], "move " +  tostring(i^1, 0, 8^(i&1)^1) );
        }

        sky[i] = clean;
        gs->Send_Data(client[0], "cleansky " + QString::number(i));
        gs->Send_Data(client[1], "cleansky " + QString::number(i^1));
    }

    //send to players
    gs->Send_Data(client[0], "Round"+QString::number(round));
    gs->Send_Data(client[1], "Round"+QString::number(round));

    emit startturnsignal();
}

void Battle::startturn(){
    qDebug() << "start turn";
    if (cards[cur]->empty()){
        passed[cur] = true;
    }

    qDebug() << passed[0] << passed[1];
    if (passed[cur]){
        if (passed[cur^1]){
            emit endroundsignal();
        } else{
            emit endturn();
        }
        return;
    }

    ts = getsrc;
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(endturn()));
    timer->setSingleShot(true);
    timer->start(60000);

    //send to players
    gs->Send_Data(client[cur], "turn_m!");
    gs->Send_Data(client[cur^1], "turn_o!");

    //sky
    //Vran Warrior

}

void Battle::endround(){
    qDebug() << "end round";
    cur = breaking;
    int s[2];
    s[0] = s[1] = 0;
    for (int i = 2; i < 8; ++i){
        foreach (Card* card, *cards[i]){
            int blood = card->get_baseblood() + card->get_boostblood();
            s[i&1] += blood;
        }
    }

    if (s[0] > s[1]){
        score[0]++, lastwinner = 0;
        gs->Send_Data(client[0], "winthisround");
        gs->Send_Data(client[1], "losethisround");
    } else
    if (s[1] > s[0]){
        score[1]++, lastwinner = 1;
        gs->Send_Data(client[0], "losethisround");
        gs->Send_Data(client[1], "winthisround");
    } else
    {
        score[0]++; score[1]++;
        gs->Send_Data(client[0], "drawthisround");
        gs->Send_Data(client[1], "drawthisround");
    }

    if (score[0] == 2 && score[1] == 2){
        emit oversignal(draw);
    } else
    if (score[0] == 2){
        emit oversignal(player0win);
    } else
    if (score[1] == 2){
        emit oversignal(player1win);
    } else
    {
        //nextround();
        emit nextroundsignal();
    }
}

void Battle::endturn(){
    qDebug() << "end turn";

    //qDebug() << timer->isActive();
    if (timer->isActive()){
        timer->stop();
        //delete timer;
        //timer = nullptr;
        //结算
        for (int i = 2; i < 8; ++i){
            int sz = cards[i]->size();
            for (int j = sz-1; j >= 0; --j){
                int blood = cards[i]->at(j)->get_baseblood();
                if (blood <= 0){
                    cards[8^(i&1)]->append(cards[i]->at(j));
                    cards[i]->removeAt(j);

                    gs->Send_Data(client[0], "move " + tostring(i, j, 8^(i&1)));
                    gs->Send_Data(client[1], "move " + tostring(i^1, j, 8^(i&1)^1));
                }
            }
        }

    } else
    if (!cards[cur]->empty()){ // punishment
        int t = qrand()%(cards[cur]->size());
        Card *tmp = cards[cur]->at(t);
        cards[cur]->removeAt(t);
        cards[8^cur]->append(tmp);
        //QString msg = "mcard " + QString::number(t) + " mgraveyard";

        gs->Send_Data(client[0], "move " + tostring(cur, t, 8^cur));
        gs->Send_Data(client[1], "move " + tostring(cur^1, t, 8^cur^1));
    }

    emit switchturnsignal();
}

void Battle::domulligan(int playerid, int index){
    qDebug() << "domulligan";
    if (cnt[playerid] > 0){
        --cnt[playerid];

        int t = qrand()%cards[10^playerid]->size();
        qDebug() << t << " " << cards[10^playerid]->size();
        Card* tmp1 = cards[playerid]->at(index);
        Card* tmp2 = cards[10^playerid]->at(t);
        cards[playerid]->removeAt(index);
        cards[10^playerid]->removeAt(t);
        cards[playerid]->append(tmp2);
        cards[10^playerid]->append(tmp1);

        // send to players

        gs->Send_Data(client[0], "move " + tostring(playerid, index, 10^playerid));
        gs->Send_Data(client[1], "move " + tostring(playerid^1, index, 10^playerid^1));

        gs->Send_Data(client[0], "move " + tostring(10^playerid, t, playerid));
        gs->Send_Data(client[1], "move " + tostring(10^playerid^1, t, playerid^1));

        //gs->Send_Data(client[playerid], "replace_m" + QString::number(index) + " " + QString::number(t));
        //gs->Send_Data(client[playerid^1], "replace_o" + QString::number(index) + " " + QString::number(t));

        if (cnt[1] == 0 && cnt[0] == 0){
            emit endmulligansignal();
        }
    }
}

void Battle::over(overstate state){
    qDebug() << "gameover" << state;
    cur = gameover;
    //send to palyer

    if (state == draw){
        player[0]->drawgame();
        gs->Send_Data(client[0], "YOUGETDRAW!");
        player[1]->drawgame();
        gs->Send_Data(client[1], "YOUGETDRAW!");
    } else
    {
        player[state]->wingame();
        gs->Send_Data(client[state], "YOUAREWINNER!");
        player[state^1]->losegame();
        gs->Send_Data(client[state^1], "YOUAERLOSER!");
    }
    gs->Send_Data(client[0], "Nothing!!!!!!!!!!");
    gs->Send_Data(client[1], "Nothing!!!!!!!!!!");
    emit send_to_server(this);
}

void Battle::topassed(MyTCPSocket *c){
    int playerid;
    if (c == client[0])
        playerid = 0;
    else playerid = 1;
    if (playerid == cur){
        passed[cur] = true;
        /*if (timer->isActive()){
            timer->stop();
        }*/
        //emit switchturnsignal();
        emit endturnsignal();
    }
}

void Battle::tosurrender(MyTCPSocket *c){
    int playerid;
    if (c == client[0])
        playerid = 0;
    else playerid = 1;
    if (timer->isActive()){
        timer->stop();
        //delete timer;
        //timer = nullptr;
    }
    if (playerid == 0) emit over(player1win);
        else emit over(player0win);
}

void Battle::clicksomething(MyTCPSocket *c, QString msg){
    int playerid;
    if (c == client[0]) playerid = 0;
        else playerid = 1;
    qDebug() << msg << ts;
    if (cur == mulligan){
        QStringList lt = msg.split(' ');
        //if (msg.startsWith('b') || msg.startsWith("others"))
        //    return;
        QString tmp = lt.at(0);
        if (tmp.compare("0") == 0){
            tmp = lt.at(1);
            domulligan(playerid, tmp.toInt());
        }
    } else
    if (cur == playerid) {
        //qDebug() << "inchoose";
        switch (ts) {
        case getsrc:
            get_src(playerid, msg);
            break;
        case gettar:
            get_tar(playerid, msg);
            break;
        case poxiao:
            break;
        case chongzheng:
            break;
        case gaier:
            break;
        case dagang:
            break;
        case zhangzhe0:
            break;
        case zhangzhe1:
            break;
        case zhangzhe2:
            break;
        default:
            break;
        }
    }
    //else other functions
}

void Battle::get_src(int playerid, QString msg){
    qDebug() << "get_src" << playerid << " "<< msg;
    QStringList msglist = msg.split(" ");
    if (msglist.size()!=2) return;
    QString loc = msglist.at(0);
    int index = msglist.at(1).toInt();
    if (loc.compare("0") != 0) return;
    src = cards[playerid]->at(index);
    ts = gettar;
}

void Battle::get_tar(int playerid, QString msg){
    qDebug() << "get_tar" << playerid << " "<< msg;

    if (msg.contains("others")){
        ts = getsrc; //undo
        return;
    }

    int k = cards[playerid]->indexOf(src);
    int index1, index2;
    int t = src->get_id();
    QStringList msglist = msg.split(' ');

    switch (t) {
    case 19:
    case 18:
    case 41:
    case 8:
        if (msg.startsWith("b ")) msg = msg.mid(2);
        msglist = msg.split(' ');
        index1 = msglist.at(0).toInt()^playerid;
        if (t == 19) silie(index1);
        else if (t == 18) birinongwu(index1);
        else if (t == 41) qingpengdayu(index1);
        else if (t == 8) ciguhanbing(index1);

        gs->Send_Data(client[0], "move " + tostring(playerid, k, 8^playerid));
        gs->Send_Data(client[1], "move " + tostring(playerid^1, k, 8^playerid^1));
        emit endturnsignal();
        break;
    case 10:
    case 17:
        if (!msg.startsWith("b ")){
            index1 = msglist.at(0).toInt()^playerid;
            index2 = msglist.at(1).toInt();
            if (t == 10){
                zhihuihaojiao(index1, index2);
            } else
            {
                mianyizengqiang(index1, index2);
            }

            gs->Send_Data(client[0], "move " + tostring(playerid, k, 8^playerid));
            gs->Send_Data(client[1], "move " + tostring(playerid^1, k, 8^playerid^1));

            emit endturnsignal();
        }
        break;
    default:
        if (msg.startsWith("b ")){
            msg = msg.mid(2);
            msglist = msg.split(' ');
            if (msglist.at(0).toInt() > 1){
                index1 = msglist.at(0).toInt()^playerid;
                index2 = msglist.at(1).toInt();

                cards[index1]->insert(index2, src);
                src->set_loc(index1);

                cards[playerid]->removeAll(src);

                gs->Send_Data(client[0], "move " + tostring(playerid, k, index1, index2));
                gs->Send_Data(client[1], "move " + tostring(playerid^1, k, index1^1, index2));

                emit endturnsignal();
            }
        }
        break;
    }

}

void Battle::zhihuihaojiao(int x, int y){
    int sz = cards[x]->size();
    int l = y-2, r = y+2;
    if (l < 0) l = 0;
    if (r >= sz) r = sz-1;
    for (int i = l; i <= r; ++i){
        Card *card = cards[x]->at(i);
        card->add_boost(4);

        gs->Send_Data(client[0], "bloodchange " + tostring(x, i, 0, 4, 0));
        gs->Send_Data(client[1], "bloodchange " + tostring(x^1, i, 0, 4, 0));
    }
}

void Battle::mianyizengqiang(int x, int y){
    int sz = cards[x]->size();
    int l = y-1, r = y+1;
    if (l < 0) l = 0;
    if (r >= sz) r = sz-1;
    for (int i = l; i <= r; ++i){
        Card *card = cards[x]->at(i);
        card->add_boost(3);
        card->add_armor(3);

        gs->Send_Data(client[0], "bloodchange " + tostring(x, i, 0, 3, 3));
        gs->Send_Data(client[1], "bloodchange " + tostring(x^1, i, 0, 3, 3));
    }
}

void Battle::ciguhanbing(int x){

}

void Battle::birinongwu(int x){

}

void Battle::qingpengdayu(int x){

}

void Battle::silie(int x){
    int sz = cards[x]->size();
    for (int i = 0; i < sz; ++i){
        Card *card = cards[x]->at(i);
        card->add_boost(-3);

        gs->Send_Data(client[0], "bloodchange " + tostring(x, i, 0, 0, -3));
        gs->Send_Data(client[1], "bloodchange " + tostring(x^1, i, 0, 0, -3));
    }
}
