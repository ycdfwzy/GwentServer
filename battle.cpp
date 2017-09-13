#include "battle.h"
#include "gameserver.h"
#include <algorithm>

QString tostring(int x1, int x2, int x3){
    return QString::number(x1) + " " + QString::number(x2) + " " + QString::number(x3);
}

QString tostring(int x1, int x2, int x3, int x4){
    return QString::number(x1) + " " + QString::number(x2) + " " + QString::number(x3) + " " + QString::number(x4);
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

    //send to players

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
    //if (msglist.size()!=2) return;

    //QStringList msglist = msg.split(" ");
    //QString loc;

    //deploy
/*
    int t = src->get_id();
    switch (t) {
    case 10:
    case 17:
        if (msg.startsWith("mmelee ")){
            msg = msg.mid(7);
            if (t == 10) zhihuihaojiao(melee[id], msg.toInt(), id);
                else mianyizengqiang(melee[id], msg.toInt(), id);
        } else
        if (msg.startsWith("omelee ")){
            msg = msg.mid(7);
            if (t == 10) zhihuihaojiao(melee[id^1], msg.toInt(), id);
                else mianyizengqiang(melee[id^1], msg.toInt(), id);
        } else
        if (msg.startsWith("mranged ")){
            msg = msg.mid(8);
            if (t == 10) zhihuihaojiao(ranged[id], msg.toInt(), id);
                else mianyizengqiang(ranged[id], msg.toInt(), id);
        } else
        if (msg.startsWith("oranged ")){
            msg = msg.mid(8);
            if (t == 10) zhihuihaojiao(ranged[id^1], msg.toInt(), id);
                else mianyizengqiang(ranged[id^1], msg.toInt(), id);
        } else
        if (msg.startsWith("msiege ")){
            msg = msg.mid(7);
            if (t == 10) zhihuihaojiao(siege[id], msg.toInt(), id);
                else mianyizengqiang(siege[id], msg.toInt(), id);
        } else
        if (msg.startsWith("osiege ")){
            msg = msg.mid(7);
            if (t == 10) zhihuihaojiao(siege[id^1], msg.toInt(), id);
                else mianyizengqiang(siege[id^1], msg.toInt(), id);
        } else
        {
            ts = getsrc; //undo
            return;
        }
        break;
    case 8:
    case 18:
    case 19:
    case 41:

        break;
    default:
        break;
    }
*/
//    if (src->get_baseblood() > 0){
    int index1, index2;
        if (msg.startsWith("b ")){
            msg = msg.mid(2);
            QStringList msglist = msg.split(' ');
            if (msglist.at(0).toInt() > 1){
                index1 = msglist.at(0).toInt()^playerid;
                index2 = msglist.at(1).toInt();

                cards[index1]->insert(index2, src);
                src->set_loc(index1);

                int k = cards[playerid]->indexOf(src);
                cards[playerid]->removeAll(src);

                gs->Send_Data(client[0], "move " + tostring(playerid, k, index1, index2));
                gs->Send_Data(client[1], "move " + tostring(playerid^1, k, index1^1, index2));

                emit endturnsignal();
            }

            /*
            msglist = msg.split(" ");
            loc = msglist.at(0);
            index = msglist.at(1).toInt();
            */
            /*
            if (loc.compare("mmelee") == 0){
                melee[id]->insert(index, src);
                src->set_loc("mmelee");
            } else
            if (loc.compare("omelee") == 0){
                melee[id^1]->insert(index, src);
                src->set_loc("omelee");
            } else
            if (loc.compare("mranged") == 0){
                ranged[id]->insert(index, src);
                src->set_loc("mranged");
            } else
            if (loc.compare("oranged") == 0){
                ranged[id^1]->insert(index, src);
                src->set_loc("oranged");
            }else
            if (loc.compare("msiege") == 0){
                siege[id]->insert(index, src);
                src->set_loc("msiege");
            } else
            if (loc.compare("osiege") == 0){
                siege[id^1]->insert(index, src);
                src->set_loc("osiege");
            } else
            {
                ts = getsrc; //undo
                return;
            }
            */

        }
//    }

    //QString tmp = "mcard " + QString::number(k) + " " + loc + " "+ QString::number(index);
    //gs->Send_Data(client[id], "move_m " + tmp);
    //gs->Send_Data(client[id^1], "move_o " + tmp);
}
/*
void Battle::zhihuihaojiao(QList<Card *> *cardlist, int index, int playerid){
    int l = index-2, r = index+2;
    if (l < 0) l = 0;
    if (r >= cardlist->size())
        r = cardlist->size()-1;
    for (int i = l; i <= r; ++i){
        Card* card = cardlist->at(i);
        card->add_boost(4);

        QString msg =  " " + QString::number(i) + " 0 4 0";
        gs->Send_Data(client[playerid], "Update_m " + msg);
        gs->Send_Data(client[playerid^1], "Update_o " + msg);
    }
}

void Battle::mianyizengqiang(QList<Card *> *cardlist, int index, int playerid){
    int l = index-1, r = index+1;
    if (l < 0) l = 0;
    if (r >= cardlist->size())
        r = cardlist->size()-1;
    for (int i = l; i <= r; ++i){
        Card* card = cardlist->at(i);
        card->add_boost(3);
        card->add_armor(3);

        QString msg = QString::number(i) + " 0 3 3";
        gs->Send_Data(client[playerid], "Update_m " + msg);
        gs->Send_Data(client[playerid^1], "Update_o " + msg);
    }
}

void Battle::silie(QList<Card *> *cardlist, int index, int playerid){
    int sz = cardlist->size();
    for (int i = 0; i < sz; ++i){
        Card* card = cardlist->at(i);
        card->add_boost(-3);

        QString msg = " " + QString::number(i) + " 0 -3 0";
        gs->Send_Data(client[playerid], "Update_m " + msg);
        gs->Send_Data(client[playerid^1], "Update_o " + msg);
    }
}
*/
