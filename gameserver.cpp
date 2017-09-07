#include "gameserver.h"
#include <QDebug>

gameServer::gameServer()
{
    server = new MyTCPServer(this);
    clients.clear();
}

gameServer::~gameServer(){}

void gameServer::client_Disconnected(MyTCPSocket *client){
    qDebug() << "Client " << client->player->name << " is disconnected";
    //change status
    QFile file("D:\\git\\GwentServer\\data\\users\\all.json");
    if (file.open(QFile::ReadOnly)){
        //qDebug() << "in File";
        bool flag = false;
        QVariantList all;
        QString str = file.readAll();

        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(str.toUtf8(), &error);
        if (error.error == QJsonParseError::NoError){
            if (!(jsonDocument.isNull() || jsonDocument.isEmpty())){
                if (jsonDocument.isArray()){
                    QJsonArray usrarr = jsonDocument.array();

                    QVariantMap us;
                    int sz = usrarr.size();
                    for (int i = 0; i < sz; ++i){
                        us = usrarr.at(i).toVariant().toMap();
                        if (client->player->name.compare(us["username"].toString()) == 0){
                            flag = true;
                            us.insert("status", "Offline"); //change status
                        }
                        all << us;
                    }
                }
            }
        }
        file.close();

        //qDebug() << all;
        if (!flag){
            // what's the fuck?!
        } else
        {
            QJsonDocument jd = QJsonDocument::fromVariant(all);
            if (!jd.isNull() && file.open(QFile::WriteOnly | QFile::Truncate)){
                QTextStream out(&file);
                out.setCodec("UTF-8");
                //qDebug() << jd.toJson();
                out << jd.toJson();
                file.close();
            }
        }
    }


    client->deleteplayer();
    //delete client from clientlist
    for (QList<MyTCPSocket*>::iterator i = clients.begin(); i!=clients.end(); i++)
        if ((*i)->socketDescriptor() == client->socketDescriptor()){
            clients.erase(i);
        }

    QObject::disconnect(client, &MyTCPSocket::readyReadClient, server, &MyTCPServer::Read_Data);
    QObject::disconnect(client, &MyTCPSocket::Disconnect, this, &gameServer::client_Disconnected);
}

void gameServer::checklogin(MyTCPSocket *client, QString username, QString password){
    qDebug() << "checklogin:" << username << password;
    QFile file("D:\\git\\GwentServer\\data\\users\\all.json");
    if (file.open(QFile::ReadOnly)){
        bool flag = false;
        QVariantList all;
        QString str = file.readAll();

        QJsonParseError error;
        QJsonDocument jsonDocument = QJsonDocument::fromJson(str.toUtf8(), &error);
        if (error.error == QJsonParseError::NoError){
            if (!(jsonDocument.isNull() || jsonDocument.isEmpty())){
                if (jsonDocument.isArray()){
                    QJsonArray usrarr = jsonDocument.array();

                    QVariantMap us;
                    int sz = usrarr.size();
                    for (int i = 0; i < sz; ++i){
                        us = usrarr.at(i).toVariant().toMap();
                        if (username.compare(us["username"].toString()) == 0 && password.compare(us["password"].toString()) == 0 &&
                                us["status"].toString().compare("Offline") == 0){
                            flag = true;
                            us.insert("status", "Online");//change status
                        }
                        all << us;
                    }
                }
            }
        }
        file.close();

        if (!flag){
            server->Send_Data(client, "Sorry!");
        } else
        {
            QJsonDocument jd = QJsonDocument::fromVariant(all);
            if (!jd.isNull() && file.open(QFile::WriteOnly | QFile::Truncate)){
                QTextStream out(&file);
                out.setCodec("UTF-8");
                out << jd.toJson();
                file.close();
            }
            server->Send_Data(client, "Welcome!");
            //client->addplayer(username);

            //send user data
            QString dir_path = "D:\\git\\GwentServer\\data\\users\\" + username + ".json";
            QFile userinfo(dir_path);
            if (userinfo.open(QFile::ReadOnly)){
                QString msg = userinfo.readAll();
                //qDebug() << msg;
                client->addplayer(msg);
                msg = "Data:" + msg;
                server->Send_Data(client, msg);
            }
        }
    }
}
