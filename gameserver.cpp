#include "gameserver.h"
#include <QDebug>

gameServer::gameServer()
{
    server = new MyTCPServer(this);
    //server->listen(QHostAddress::Any, 8888);
    clients.clear();

    //QObject::connect(server, &QTcpServer::newConnection, this, &gameServer::server_New_Connect);
}

gameServer::~gameServer(){}

void gameServer::server_New_Connect(){
    //clients.push_back( server->nextPendingConnection() );
    //QObject::connect(clients.back(), &MyTCPSocket::readyReadClient, this, &gameServer::Read_Data);
    //QObject::connect(clients.back(), &MyTCPSocket::disconnected, this, &gameServer::client_Disconnected);
    //qDebug() << "A clinet is connected!";
    //clients.back()->write("Hi!");
}

void gameServer::client_Disconnected(){
    qDebug() << "A client is disconnected";
}

void gameServer::checklogin(MyTCPSocket *client, QString username, QString password){
    //qDebug() << "checklogin:" << username << password;
    QFile file("D:\\git\\GwentServer\\data\\users\\all.json");
    if (file.open(QFile::ReadOnly)){
        //qDebug() << "in file";
        bool flag = false;
        QString str = file.readAll();
        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(str.toUtf8(), &error);
        if (error.error == QJsonParseError::NoError){
            if (!(jsonDocument.isNull() || jsonDocument.isEmpty())){
                if (jsonDocument.isArray()){
                    QJsonArray usrarr = jsonDocument.array();
                    QVariantList all;
                    QVariantMap us;
                    int sz = usrarr.size();
                    for (int i = 0; i < sz; ++i){
                        us = usrarr.at(i).toVariant().toMap();
                        //qDebug() << "username" << us["username"].toString();
                        //qDebug() << "password" << us["password"].toString();
                        //qDebug() << username.compare(us["username"].toString());
                        //qDebug() << password.compare(us["password"].toString());
                        if (username.compare(us["username"].toString()) == 0 && password.compare(us["password"].toString()) == 0 &&
                                us["status"].toString().compare("Offline") == 0){
                            flag = true;
                            us.insert("status", QVariant("Online"));
                        }
                        all << us;
                    }

                    file.close();
                    //qDebug() << flag;
                    if (!flag){
                        server->Send_Data(client, "Sorry!");
                    } else
                    {
                        QJsonDocument jd = QJsonDocument::fromVariant(all);
                        if (!jd.isNull() && file.open(QFile::WriteOnly | QFile::Truncate)){
                            QTextStream out(&file);
                            out.setCodec("UTF-8");
                            out << jsonDocument.toJson();
                        }
                        server->Send_Data(client, "Welcome!");
                    }
                }
            }
        }
    }
}
