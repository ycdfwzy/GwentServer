#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QString>

class MyTCPSocket;

class Player : public QObject
{
    Q_OBJECT
public:
    explicit Player(MyTCPSocket  *client_, QString name_, QObject *parent = nullptr);
    ~Player();

    MyTCPSocket *client;
    QString name;

signals:

public slots:
};

#endif // PLAYER_H
