#include "card.h"
#include <QDebug>

Card::Card(int id, QObject *parent) : QObject(parent)
{
    this->id = id;
    boostblood = 0;
    armor = 0;

    QFile file("D:\\git\\GwentServer\\cards\\cardinfo.json");
    if (file.open(QFile::ReadOnly)){
        QString str = file.readAll();
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(str.toUtf8(), &error);
        if (error.error == QJsonParseError::NoError){
            if (!(jsonDocument.isNull() || jsonDocument.isEmpty())){
                if (jsonDocument.isObject()){
                    QJsonObject info = jsonDocument.object();

                    QJsonValue all = info.take(QString::number(id));
                    if (all.isObject()){
                        //qDebug() << "isObject";
                        QJsonObject tmp = all.toObject();
                        name = tmp.take("name_cn").toVariant().toString();
                        picpath = tmp.take("picpath").toVariant().toString();
                        rule = tmp.take("rule").toVariant().toString();
                        type = tmp.take("type").toVariant().toString();
                        color = tmp.take("color").toVariant().toString();
                        faction = tmp.take("faction").toVariant().toString();
                        baseblood = tmp.take("blood").toVariant().toInt();

                        for (int i = 0; i < 12; ++i)
                            flag[i] = true;
                        if (baseblood > 0){
                            lane = tmp.take("location").toVariant().toString();
                            if (lane.contains("disloyal")){
                                for (int i = 2; i < 8; i += 2)
                                    flag[i] = false;
                            } else {
                                for (int i = 3; i < 8; i += 2)
                                    flag[i] = false;
                            }
                            if (lane.contains("siege")){
                                flag[4] = flag[5] = flag[6] = flag[7] = false;
                            } else
                            if (lane.contains("ranged")){
                                flag[2] = flag[3] = flag[6] = flag[7] = false;
                            } else
                            if (lane.contains("melee")){
                                flag[2] = flag[3] = flag[4] = flag[5] = false;
                            }
                        }
                        ARMOR = false;
                        //qDebug() << name;
                    }
                }
            }
        }
    }
}

Card::Card(const Card &c){
    name = c.name;
    faction = c.name;
    picpath = c.picpath;
    color = c.color;
    rule = c.rule;
    type = c.type;
    id = c.id;
    baseblood = c.baseblood;
    boostblood = c.boostblood;
    armor = c.armor;
}

void Card::emitdeathwish(){
    emit deathwish(curloc&1);
}

void Card::set_loc(int loc){
    curloc = loc;
    if (curloc == 8 || curloc == 9)
        emit deathwish(curloc&1);
}

int Card::get_loc(){return curloc;}
QString Card::get_name() const{return name;}
QString Card::get_faction() const{return faction;}
QString Card::get_picpath() const{return picpath;}
QString Card::get_color() const{return color;}
QString Card::get_rule() const{return rule;}
QString Card::get_type() const{return type;}
int Card::get_id() const{return id;}
int Card::get_baseblood() const{return baseblood;}
int Card::get_boostblood() const{return boostblood;}
int Card::get_armor() const{return armor;}
bool Card::checkvalid(int loc){
    qDebug() << flag[2] << flag[3] << flag[4] << flag[5] << flag[6] << flag[7];
    return flag[loc];
}
bool Card::get_ARMOR(){return ARMOR;}
void Card::set_ARMOR(bool f){ARMOR = f;}
void Card::add_armor(int dlt){
    if (dlt < 0 && ARMOR){
        set_ARMOR(false);
        return;
    }
    if (armor + dlt < 0){
        dlt += armor;
        armor = 0;
        add_boost(dlt);
        return;
    }
    armor += dlt;
}
void Card::add_boost(int dlt){
    if (boostblood + dlt < 0){
        dlt += boostblood;
        boostblood = 0;
        add_base(dlt);
        return;
    }
    boostblood += dlt;
}
void Card::add_base(int dlt){
    baseblood += dlt;
    if (baseblood < 0)
        baseblood = 0;
}

void Card::reset(){
    Card *tmp = new Card(id);
    baseblood = tmp->get_baseblood();
    boostblood = 0;
    armor = 0;
    ARMOR = false;
}
