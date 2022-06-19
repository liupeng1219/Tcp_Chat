#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //远程连接云服务器上的MySQL
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("116.205.186.55");
    db.setPort(3306);
    db.setDatabaseName("ceshi");
    db.setUserName("root");
    db.setPassword("123456");
    if (!db.open()) {
        qDebug() << "connect failed";
        qDebug() << db.lastError(); // 输出数据库错误信息
    }
    else qDebug() << "database connect success";


    // 初始化服务器server对象
    myserver = new QTcpServer(this);
    // 启动监听
    myserver->listen(QHostAddress::Any,19999);
    // 当有客户端链接时候会发送newConnection信号，
    // 触发槽函数接受链接（得到一个与客户端通信的套接字QTcpSocket
    //connect(a,&b,c,d);
    connect(myserver,&QTcpServer::newConnection,this,[=](){
        qDebug() << "新客户端连接";
        // 与客户端通信的套接字
        QTcpSocket * mysocket = myserver->nextPendingConnection();
        // 有新连接进入，则将该套接字加入列表中
        client.append(mysocket);
        cli_name.append("");
        // 关联接收客户端数据信号readyRead信号（客户端有数据就会发readyRead信号）
        connect(mysocket,&QIODevice::readyRead,this,[=](){
            // 调用readall接收该客户端信息，存储在msg中
            msgType type = recvmsg(mysocket);
        });
        // 检测掉线信号
        connect(mysocket,&QTcpSocket::disconnected,this,[=](){
            // 首先找到断连套接字的编号，在队列中删除它
            int i = client.indexOf(mysocket);
            QString discliname = cli_name[i];
            client.removeAt(i);
            cli_name.removeAt(i);
            // 因为有两个同名的私聊窗口，所以还要检查
            i = cli_name.indexOf(discliname);
            if (i != -1) {
                QString msg = "Clo 1";
                client[i]->write(msg.toUtf8());
            }
            QTcpSocket *obj = (QTcpSocket*)sender();//掉线对象
            qDebug() << obj->peerAddress().toString();//打印出掉线对象的ip
            mysocket->close();
        });
    });


}

Widget::~Widget()
{
    db.close();
    delete ui;
}
void Widget::sndmsg(QTcpSocket *msocket, QString msg, msgType type){
    if (type == Msg) {
        // 解析信息，得到该私聊信息要发给的对象
        QStringList tmp = msg.split(" ");
        QString chat_name = tmp[0];
        // msg为聊天信息:格式：Msg 发言人 聊天信息
        msg = msg.mid(chat_name.size() + 1);
        msg = "Msg " + msg;

        for (int i = 0; i < cli_name.size(); i ++){
            if (cli_name[i] == chat_name){
                client[i]->write(msg.toUtf8());
            }
        }
    }
    else if (type == Iden) { // 用于身份认证，第一次连接服务器，上报自己名字
        int i = client.indexOf(msocket);
        cli_name[i] = msg;
    }
    else if (type == Req) {
        int i,j;
        QStringList tmp = msg.split(" ");
        QString cur_aname = tmp[0];
        QString cur_bname = tmp[1];
        // 找到自己私聊套接字的编号
        i = client.indexOf(msocket);
        // 判断ba是否在线，若在线，则表明自己是被提醒方，
        // 按照在线的名字进行私聊
        j = cli_name.indexOf(cur_bname+cur_aname);
        if (j != -1) {
            // 给自己也起名为ba
            cli_name[i] = cur_bname + cur_aname;
            // 找到对方的私聊窗口了
            msg = "Lok " + cli_name[i];
            msocket->write(msg.toUtf8());
            client[j]->write(msg.toUtf8());
            return;
        }
        // 若无在线，则判断b是否在线
        j = cli_name.indexOf(cur_bname);
        if (j == -1) { // b不在线,要告诉a
            msg = "Clo 0";
            qDebug() << msg;
            msocket->write(msg.toUtf8());
        }
        else { // b找到了，要告诉b有人私聊
            qDebug() << "找到朋友了";
            // 给自己起名ab
            cli_name[i] = cur_aname + cur_bname;
            msg = "Req " + cur_aname;
            client[j]->write(msg.toUtf8());
        }
    }
    else if (type == Addf) {
        int i = cli_name.indexOf(msg);
        int j = client.indexOf(msocket);
        if (i != -1) {
            msg = "Addf " + cli_name[j];

            client[i]->write(msg.toUtf8());
        }
    }
    else if (type == Delf) { //处理客户端删除好友的请求
        QStringList qlist = msg.split(" ");
        int i = cli_name.indexOf(qlist[0]);
        if (i != -1) {
            msg = "Delf " + qlist[1];
            client[i]->write(msg.toUtf8());
        }

    }
}
//type + name + m
msgType Widget::recvmsg(QTcpSocket *msocket){
    msgType type;
    QString msg;
    QByteArray message = msocket->readAll();
    QString tmp = QString(message);
    qDebug() << "recv: " << tmp;

    QStringList qlist = tmp.split(" ");
    QString stype = qlist.at(0);
    if (stype == "Msg") type = Msg;
    else if (stype == "Iden") type = Iden;
    else if (stype == "Req") type = Req;
    else if (stype == "Addf") type = Addf;
    else if (stype == "Delf") type = Delf;
    msg = tmp.mid(stype.size() + 1);

    sndmsg(msocket, msg, type);
    return type;
}

