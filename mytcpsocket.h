#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QObject>
#include <QtNetwork/QTcpSocket>

class MyTCPSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTCPSocket();
    ~MyTCPSocket();

    void emitReadyread();
    void emitdisconnect();

signals:
    void readyReadClient(MyTCPSocket*);
    void Disconnect(MyTCPSocket*);
};

#endif // MYTCPSOCKET_H
