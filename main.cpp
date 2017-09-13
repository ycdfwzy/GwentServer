/**
 *                             _ooOoo_
 *                            o8888888o
 *                            88" . "88
 *                            (| -_- |)
 *                            O\  =  /O
 *                         ____/`---'\____
 *                       .'  \\|     |//  `.
 *                      /  \\|||  :  |||//  \
 *                     /  _||||| -:- |||||-  \
 *                     |   | \\\  -  /// |   |
 *                     | \_|  ''\---/''  |   |
 *                     \  .-\__  `-`  ___/-. /
 *                   ___`. .'  /--.--\  `. . __
 *                ."" '<  `.___\_<|>_/___.'  >'"".
 *               | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 *               \  \ `-.   \_ __\ /__ _/   .-` /  /
 *          ======`-.____`-.___\_____/___.-`____.-'======
 *                             `=---='
 *          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
 *                     佛祖保佑        永无BUG
 *            佛曰:
 *                   写字楼里写字间，写字间里程序员；
 *                   程序人员写程序，又拿程序换酒钱。
 *                   酒醒只在网上坐，酒醉还来网下眠；
 *                   酒醉酒醒日复日，网上网下年复年。
 *                   但愿老死电脑间，不愿鞠躬老板前；
 *                   奔驰宝马贵者趣，公交自行程序员。
 *                   别人笑我忒疯癫，我笑自己命太贱；
 *                   不见满街漂亮妹，哪个归得程序员？
*/
#include <QCoreApplication>
#include "gameserver.h"
#include "mytcpserver.h"
#include <QTime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

void init(){
    QFile file("D:\\git\\GwentServer\\data\\users\\all.json");
    if (file.open(QFile::ReadOnly)){
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
                        us.insert("status", "Offline");
                        all << us;
                    }
                }
            }
        }
        file.close();

        QJsonDocument jd = QJsonDocument::fromVariant(all);
        if (!jd.isNull() && file.open(QFile::WriteOnly | QFile::Truncate)){
            QTextStream out(&file);
            out.setCodec("UTF-8");
            out << jd.toJson();
            file.close();
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    init();
    //MyTCPServer *gs = new MyTCPServer;
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    gameServer *gs = new gameServer;
    return a.exec();
}
