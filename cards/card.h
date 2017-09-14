#ifndef CARD_H
#define CARD_H

#include <QObject>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

class Card : public QObject
{
    Q_OBJECT
public:
    explicit Card(int id, QObject *parent = nullptr);
    Card(const Card &c);
    QString get_name() const;
    QString get_faction() const;
    QString get_picpath() const;
    QString get_color() const;
    QString get_rule() const;
    QString get_type() const;
    int get_id() const;
    int get_baseblood() const;
    int get_boostblood() const;
    int get_armor() const;
    void set_loc(int);
    int get_loc();
    void add_base(int);
    void add_boost(int);
    void add_armor(int);
    void set_ARMOR(bool);
    bool get_ARMOR();
    bool checkvalid(int loc);
    void reset();
    void emitdeathwish();

signals:
    void deathwish(int);

public slots:

private:
    bool flag[12];
    int curloc;
    bool ARMOR;
    QString lane;
    QString name, faction;
    QString picpath, color;
    QString rule, type;
    int id, baseblood, boostblood, armor;
};

#endif // CARD_H
