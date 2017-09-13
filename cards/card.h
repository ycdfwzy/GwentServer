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
    void add_boost(int);
    void add_armor(int);

signals:

public slots:

private:
    int curloc;
    QString name, faction;
    QString picpath, color;
    QString rule, type;
    int id, baseblood, boostblood, armor;
};

#endif // CARD_H
