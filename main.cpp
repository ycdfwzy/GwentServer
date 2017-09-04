#include <QCoreApplication>
#include "gameserver.h"
#include "mytcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    //MyTCPServer *gs = new MyTCPServer;
    gameServer *gs = new gameServer;
    return a.exec();
}
