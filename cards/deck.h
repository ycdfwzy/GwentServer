#ifndef DECK_H
#define DECK_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include "cards/card.h"

class Deck : public QObject
{
    Q_OBJECT
public:
    explicit Deck(QJsonValue &info, QObject *parent = nullptr);

signals:

public slots:

private:
    QString name;
    int leader;
    QList<int> cards;
};

#endif // DECK_H
