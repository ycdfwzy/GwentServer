#include <QCoreApplication>
#include "gameserver.h"
#include "mytcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MyTCPServer *gs = new MyTCPServer;
    return a.exec();
}
