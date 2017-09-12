#include "battle.h"
#include "gameserver.h"
#include <algorithm>

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
    round = 0;
    lastwinner = -1;
    cur = breaking;

    connect(this, SIGNAL(startroundsignal()), this, SLOT(startround()));
    connect(this, SIGNAL(endroundsignal()), this, SLOT(endround()));
    connect(this, SIGNAL(nextroundsignal()), this, SLOT(nextround()));
    connect(this, SIGNAL(startturnsignal()), this, SLOT(startturn()));
    connect(this, SIGNAL(endmulligansignal()), this, SLOT(endmulligan()));
    connect(this, SIGNAL(switchturnsignal()), this, SLOT(switchturn()));
    connect(this, SIGNAL(endturnsignal()), this, SLOT(endturn()));
    connect(this, SIGNAL(oversignal(overstate)), this, SLOT(over(overstate)));

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(nextround()));
    timer->setSingleShot(true);
    timer->start(1000);
}

void Battle::shuffle(int index, QList<Card *> *src){
    deck[index] = new QList<Card*>;
    deck[index]->clear();
    int sz = src->size();
    int id[50];
    for (int i = 0;  i < sz; ++i)
        id[i] = i;
    QString msg;
    std::random_shuffle(id, id+sz);
    for (int i = 0; i < sz; ++i){
        int t = src->at(id[i])->get_id();
        deck[index]->append(new Card(t));
        deck[index]->back()->set_loc("mdeck");
        if (i == 0) msg = QString::number(t);
            else msg += " " + QString::number(t);
    }

    //qDebug() << tar->size();
    //send to players
    gs->Send_Data(client[index], "LoadDeck_m: " + msg);
    gs->Send_Data(client[index^1], "LoadDeck_o: " + msg);
}

void Battle::nextround(){
    qDebug() << "nextround";
    ++round;
    switch (round) {
    case 1:
        pumping(10, 0);
        pumping(10, 1);
        Mulligan(3, 0);
        Mulligan(3, 1);
        cur = mulligan;

        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(endmulligan()));
        timer->setSingleShot(true);
        timer->start(60000);
        break;
    case 2:
        pumping(2, 0);
        pumping(2, 1);
        Mulligan(1, 0);
        Mulligan(1, 1);
        cur = mulligan;

        timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(endmulligan()));
        timer->setSingleShot(true);
        timer->start(20000);
        break;
    case 3:
        pumping(1, 0);
        pumping(1, 1);
        Mulligan(1, 0);
        Mulligan(1, 1);
        cur = mulligan;

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
    if (!deck[id]->empty()){
        card[id]->push_back( deck[id]->front() );
        card[id]->back()->set_loc("mcard");
        deck[id]->pop_front();
        num_++;
    }
    //send data to players
   gs->Send_Data(client[id], "PUMP_m: " + QString::number(num_));
   gs->Send_Data(client[id^1], "PUMP_o: " + QString::number(num_));
}

void Battle::Mulligan(int num, int id){
    qDebug() << "mulligan";
    cnt[id] = num;
    //send data to players
    gs->Send_Data(client[id], "Mulligan" + QString::number(num));
}

void Battle::endmulligan(){
    qDebug() << "endmulligan";
    if (timer->isActive()){
        timer->stop();
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
    //send to players
    gs->Send_Data(client[0], "Round"+QString::number(round));
    gs->Send_Data(client[1], "Round"+QString::number(round));

    emit startturnsignal();
}

void Battle::startturn(){
    qDebug() << "start turn";
    if (card[cur]->empty()){
        passed[cur] = true;
    }
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
    for (int i = 0; i < 2; ++i){
        s[0] = 0;
        foreach (Card* card, *melee[i]){
            int blood = card->get_baseblood() + card->get_boostblood();
            score[i] += blood;
        }
        foreach (Card* card, *ranged[i]){
            int blood = card->get_baseblood() + card->get_boostblood();
            score[i] += blood;
        }
        foreach (Card* card, *siege[i]){
            int blood = card->get_baseblood() + card->get_boostblood();
            score[i] += blood;
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

    if (!timer->isActive()){ // punishment
        int t = qrand()%(card[cur]->size());
        card[cur]->removeAt(t);
        QString msg = "mcard " + QString::number(t) + " mgraveyard";
        gs->Send_Data(client[cur], "move_m" + msg);
        gs->Send_Data(client[cur^1], "move_o" + msg);
    } else
    {
        timer->stop();
    }

    emit switchturnsignal();
}

void Battle::domulligan(int playerid, int index){
    qDebug() << "domulligan";
    if (cnt[playerid] > 0){
        --cnt[playerid];

        int t = qrand()%deck[playerid]->size();
        qDebug() << t << " " << deck[playerid]->size();
        Card* tmp1 = card[playerid]->at(index);
        Card* tmp2 = deck[playerid]->at(t);
        card[playerid]->removeAt(index);
        deck[playerid]->removeAt(t);
        card[playerid]->append(tmp2);
        deck[playerid]->append(tmp1);

        // send to players
        gs->Send_Data(client[playerid], "replace_m" + QString::number(index) + " " + QString::number(t));
        gs->Send_Data(client[playerid^1], "replace_o" + QString::number(index) + " " + QString::number(t));

        if (cnt[1] == 0 && cnt[0] == 0){
            emit endmulligansignal();
        }
    }
}

void Battle::over(overstate state){
    qDebug() << "gameover";
    cur = gameover;
    //send to palyer

    if (state == draw){
        player[0]->drawgame();
        gs->Send_Data(client[0], "DRAW!");
        player[1]->drawgame();
        gs->Send_Data(client[1], "DRAW!");
    } else
    {
        player[state]->wingame();
        gs->Send_Data(client[state], "WIN!");
        player[state^1]->losegame();
        gs->Send_Data(client[state^1], "LOSE!");
    }
    emit send_to_server(this);
}

void Battle::topassed(MyTCPSocket *c){
    int playerid;
    if (c == client[0])
        playerid = 0;
    else playerid = 1;
    if (playerid == cur)
        passed[cur] = true;
}

void Battle::tosurrender(MyTCPSocket *c){
    int playerid;
    if (c == client[0])
        playerid = 0;
    else playerid = 1;
    if (playerid == 0) emit over(player1win);
        else emit over(player0win);
}

void Battle::clicksomething(MyTCPSocket *c, QString msg){
    int id;
    if (c == client[0]) id = 0;
        else id = 1;
    if (cur == mulligan){
        if (msg.startsWith("mcard ")){
            QString msg0 = msg.mid(6);
            domulligan(id, msg0.toInt());
        }
    } else
    if (cur == id) {
        //qDebug() << "inchoose";
        switch (ts) {
        case getsrc:
            get_src(id, msg);
            break;
        case gettar:
            get_tar(id, msg);
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

void Battle::get_src(int id, QString msg){
    qDebug() << "get_src" << id << " "<< msg;
    QStringList msglist = msg.split(" ");
    if (msglist.size()!=2) return;
    QString loc = msglist.at(0);
    int index = msglist.at(1).toInt();
    if (loc.compare("mcard") != 0) return;
    src = card[id]->at(index);
    ts = gettar;
}

void Battle::get_tar(int id, QString msg){
    qDebug() << "get_tar" << id << " "<< msg;
    //if (msglist.size()!=2) return;

    QStringList msglist;
    QString loc;
    int index;
    int k = card[id]->indexOf(src);
    card[id]->removeAll(src);

    //deploy
    if (msg.startsWith("blank ")){
    msg = msg.mid(6);
    msglist = msg.split(" ");
    loc = msglist.at(0);
    index = msglist.at(1).toInt();

    if (loc.compare("mmelee")){
        melee[id]->insert(index, src);
        src->set_loc("mmelee");
    } else
    if (loc.compare("omelee")){
        melee[id^1]->insert(index, src);
        src->set_loc("omelee");
    } else
    if (loc.compare("mranged")){
        ranged[id]->insert(index, src);
        src->set_loc("mranged");
    } else
    if (loc.compare("oranged")){
        ranged[id^1]->insert(index, src);
        src->set_loc("oranged");
    }else
    if (loc.compare("msiege")){
        siege[id]->insert(index, src);
        src->set_loc("msiege");
    } else
    if (loc.compare("osiege")){
        siege[id^1]->insert(index, src);
        src->set_loc("osiege");
    } else
    {
        ts = getsrc; //undo
        return;
    }
    }

    //send to players
    QString tmp = "mcard " + QString::number(k) + " " + loc + " "+ QString::number(index);
    gs->Send_Data(client[id], "move_m " + tmp);
    gs->Send_Data(client[id^1], "move_o " + tmp);

    emit switchturnsignal();
}
