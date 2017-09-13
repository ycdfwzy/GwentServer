#include "gameserver.h"
#include <QDebug>

gameServer::gameServer()
{
    server = new MyTCPServer(this);
    clients.clear();
}

gameServer::~gameServer(){}

void gameServer::Send_Data(MyTCPSocket *client, QString msg){
    server->Send_Data(client, msg);
}

void gameServer::client_Disconnected(MyTCPSocket *client){
    QObject::disconnect(client, &MyTCPSocket::readyReadClient, server, &MyTCPServer::Read_Data);
    QObject::disconnect(client, &MyTCPSocket::Disconnect, this, &gameServer::client_Disconnected);

    qDebug() << "Client " << client->get_player()->get_name() << " is disconnected";
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
                        if (client->get_player()->get_name().compare(us["username"].toString()) == 0){
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
            clients.append(client);
            QObject::connect(client, &MyTCPSocket::Disconnect, this, &gameServer::client_Disconnected);

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
    //qDebug() << "Something happened to database!";
}

void gameServer::matched(MyTCPSocket *client1, QString name1, MyTCPSocket *client2, QString name2){
    Send_Data(client1, "FOUND " + client2->get_player()->get_name());
    Send_Data(client2, "FOUND " + client1->get_player()->get_name());
    Battle *x = new Battle(this, client1, name1, client2, name2);
    battlelist.append(x);
    connect(x, SIGNAL(send_to_server(Battle*)), this, SLOT(onebattleover(Battle*)));
}

void gameServer::onebattleover(Battle* b){
    battlelist.removeAll(b);
    delete b;
}

void gameServer::dealwithmsg(MyTCPSocket *client, QString str){
    qDebug() << str;
    if (str.startsWith('I')){
        QString str0 = str.mid(1);
        QStringList sl = str0.split(' ');
        if (sl.length() == 2)
            checklogin(client, sl.at(0), sl.at(1));
        qDebug() << clients.size();
        //qDebug() << sl.at(1);
    } else
    if (str.startsWith("AddDeck: ")){
        QString str0 = str.mid(9);
        QStringList sl = str0.split(' ');
        client->update_deck(sl);
    } else
    if (str.startsWith("ChooseforBattle: ")){
        QString str0 = str.mid(17);
        WaitQueue::readPlay(this, client, str0);
    } else
    if (str.startsWith("STOPMATCH!")){
        WaitQueue::stopwait(client);
    } else
    if (str.startsWith("click: ")){
        QString str0 = str.mid(7);
        if (client->get_player()->get_battle() != nullptr){
           client->get_player()->get_battle()->clicksomething(client, str0);
        }
    } else
    if (str.startsWith("pass")){
        if (client->get_player()->get_battle() != nullptr){
            client->get_player()->get_battle()->topassed(client);
        }
    } else
    if (str.startsWith("surrender")){
        if (client->get_player()->get_battle() != nullptr){
            client->get_player()->get_battle()->tosurrender(client);
        }
    }
}
