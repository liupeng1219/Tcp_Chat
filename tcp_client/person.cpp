#include "person.h"
#include "ui_person.h"

person::person(QWidget *parent, QString name) :
    QWidget(parent),
    ui(new Ui::person)
{
    ui->setupUi(this);
    myname = name;
    // 设置个人界面的基本信息
    this->setWindowIcon(QIcon(":/images/bird.png"));
    this->setWindowTitle(name);

    ui->toolBox->setStyleSheet("background:rgba(255,255,255,0%);border-style:outset");
    ui->addf_btn->setFlat(true);
    ui->hymEdit->setStyleSheet("background:rgba(255,255,255,40%);border-style:outset");

    // 图标资源
    iconNameList << "toux1" << "toux2" << "toux3" << "toux4" << "toux5" << "toux6"
                    << "toux7" << "toux8" << "toux9" << "toux10" << "toux11" << "toux12"
                       << "toux13" << "toux14" << "toux15";

    // 连接服务器，告知己方身份
    mysocket = new QTcpSocket(this);
    mysocket->connectToHost(server_IP,19999);
    connect(mysocket,&QTcpSocket::connected,[=](){
        QMessageBox::about(this,"服务器","成功连接");
        QString tmp = "Iden " + myname;
        mysocket->write(tmp.toUtf8());
    });

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

    // 查询好友信息
    QStringList ceshi;
    QString usern = myname;
    QString cmd = QString("select * from friend where user1 = '%1'").arg(usern);
    QSqlQuery db_cursor(db);
    db_cursor.exec(cmd);
    if(db_cursor.size() == -1){
        QMessageBox::warning(this,"警告","初始化出错，请关闭重启");
    }
    else{
        QString col1, col2;
        while(db_cursor.next()){
            col1= db_cursor.value(0).toString();
            col2= db_cursor.value(1).toString();
            ceshi << col2;
        }
    }

    // 根据传入的好友参数，创建好友列表，建立信号槽
    friendnum = ceshi.size();
    for (int i = 0; i < ceshi.size(); i ++) {
        QToolButton *btn = new QToolButton(this);
        btn->setStyleSheet("background-color: rgba(0, 0, 0, 0)");
        int k = rand() % 15;
        btn->setIcon(QPixmap(QString(":/images/%1.jpg").arg(iconNameList[k])));
        btn->setIconSize(QPixmap(":/images/qmnn.png").size()*0.8);
        btn->setText(ceshi[i]);
        btn->setFont(QFont("宋体",14));
        btn->setAutoRaise(true);
        btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        ui->vlayout->addWidget(btn);
        vector.push_back(btn);
        IsShow.push_back(false);
        connect(btn,&QToolButton::clicked,[=](){
//            if (IsShow[i]) {
//                QMessageBox::warning(this,"警告","1该聊天框已被打开"+QString(friendnum));
//                return;
//            }
            IsShow[i] = true;
            Widget *widget = new Widget(nullptr,btn->text(),myname);
            widget->setWindowIcon(btn->icon());
            widget->setWindowTitle(btn->text());
            widget->show();
            connect(widget,&Widget::closeWidget,this,[=](){
               IsShow[i] = false;
            });
        });
        // 这里是右键点击好友，可以删除的功能
        btn->setContextMenuPolicy(Qt::CustomContextMenu);  //添加右键点击事件
        // 当产生鼠标右键点击事件时，触发menu()函数，创建菜单
        connect(btn,&QToolButton::customContextMenuRequested,this,[=](){
            QMenu *cmenu = new QMenu(btn);
            QAction *action1 = new QAction(tr("删除好友"), this);
            action1->setData(1);
            cmenu->addAction(action1);
            // 触发删除信号
            connect(action1,&QAction::triggered,this,[=](){
                qDebug() << "删除了";
                // 数据库删除动作
                QString user1 = myname;
                QString user2 = btn->text();
                QString cmd1 = QString("delete from friend where user1 = '%1' and user2 = '%2'").arg(user1).arg(user2);
                QString cmd2 = QString("delete from friend where user1 = '%1' and user2 = '%2'").arg(user2).arg(user1);
                QSqlQuery cursor(db);
                bool re = cursor.exec(cmd1);
                cursor.exec(cmd2);
                qDebug() << re;
                // 发送删除信息给服务器
                QString msg = "Delf " + btn->text() + " " + myname;
                // 格式为类型名+好友名+我的名字
                mysocket->write(msg.toUtf8());
                // 好友列表删除动作
                hydel(btn->text());
            });
            //将下方对话框的数据传到上方对话框内
            cmenu->exec(QCursor::pos());
        });

    }

    // 点击添加好友，发送加好友信息
    connect(ui->addf_btn,&QPushButton::clicked,this,[=](){
        QString user1 = myname;
        QString user2 = ui->hymEdit->text();
        ui->hymEdit->clear();
        ui->hymEdit->setFocus();
        QSqlQuery db_cursor(db);
        // 首先查询是否有user2
        QString cmd = QString("select * from user where username = '%1'").arg(user2);
        db_cursor.exec(cmd);
        if(db_cursor.size() == 0){
            QMessageBox::warning(this,"警告","未找到该用户");
        }else{ // user2存在
            // 插入user2和user1的朋友关系
            QString cmd1 = QString("insert into friend(user1, user2) values('%1','%2');").arg(user1).arg(user2);
            QString cmd2 = QString("insert into friend(user1, user2) values('%1','%2');").arg(user2).arg(user1);
            db_cursor.exec(cmd1);
            db_cursor.exec(cmd2);

            // 尝试在这直接添加好友,步骤同上面的创建好友列表一样
            QToolButton *btn = new QToolButton(this);
            btn->setStyleSheet("background-color: rgba(0, 0, 0, 0)");
            int k = rand() % 15;
            btn->setIcon(QPixmap(QString(":/images/%1.jpg").arg(iconNameList[k])));
            btn->setIconSize(QPixmap(":/images/qmnn.png").size()*0.8);
            btn->setText(user2);
            btn->setFont(QFont("宋体",14));
            btn->setAutoRaise(true);
            btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            ui->vlayout->addWidget(btn);
            vector.push_back(btn);
            IsShow.push_back(false);
            connect(btn,&QToolButton::clicked,[=](){
//                if (IsShow[friendnum]) {
//                    QMessageBox::warning(this,"警告","2该聊天框已被打开"+QString(friendnum));
//                    return;
//                }
                IsShow[friendnum] = true;
                Widget *widget = new Widget(nullptr,btn->text(),myname);
                widget->setWindowIcon(btn->icon());
                widget->setWindowTitle(btn->text());
                widget->show();
                connect(widget,&Widget::closeWidget,this,[=](){
                   IsShow[friendnum] = false;
                });
            });
            friendnum ++;
            // 发送添加好友请求给服务器
            QString msg = "Addf " + user2;
            mysocket->write(msg.toUtf8());
            // 这里是右键点击好友，可以删除的功能
            btn->setContextMenuPolicy(Qt::CustomContextMenu);  //添加右键点击事件
            // 当产生鼠标右键点击事件时，触发menu()函数，创建菜单
            connect(btn,&QToolButton::customContextMenuRequested,this,[=](){
                QMenu *cmenu = new QMenu(btn);
                QAction *action1 = new QAction(tr("删除好友"), this);
                action1->setData(1);
                cmenu->addAction(action1);
                // 触发删除信号
                connect(action1,&QAction::triggered,this,[=](){
                    qDebug() << "删除了";
                    // 数据库删除动作
                    QString user1 = myname;
                    QString user2 = btn->text();
                    QString cmd1 = QString("delete from friend where user1 = '%1' and user2 = '%2'").arg(user1).arg(user2);
                    QString cmd2 = QString("delete from friend where user1 = '%1' and user2 = '%2'").arg(user2).arg(user1);
                    QSqlQuery cursor(db);
                    bool re = cursor.exec(cmd1);
                    cursor.exec(cmd2);
                    qDebug() << re;
                    // 发送删除信息给服务器
                    QString msg = "Delf " + btn->text() + " " + myname;
                    // 格式为类型名+好友名+我的名字
                    mysocket->write(msg.toUtf8());
                    // 好友列表删除动作
                    hydel(btn->text());
                });
                //将下方对话框的数据传到上方对话框内
                cmenu->exec(QCursor::pos());
            });
        }
    });

    // 有消息到达，触发
    connect(mysocket,&QIODevice::readyRead,this,[=](){
        QByteArray message = mysocket->readAll();
        QString tmp = QString(message);
        QStringList qlist = tmp.split(" ");
        if (qlist[0] == "Req")
            QMessageBox::about(this,"通知",qlist[1]+"请求与你私聊,请点击头像进行私聊");
        else if (qlist[0] == "Addf"){
            QMessageBox::about(this,"通知",qlist[1]+"请求成为你的好友");

            QToolButton *btn = new QToolButton(this);
            btn->setStyleSheet("background-color: rgba(0, 0, 0, 0)");
            int k = rand() % 15;
            btn->setIcon(QPixmap(QString(":/images/%1.jpg").arg(iconNameList[k])));
            btn->setIconSize(QPixmap(":/images/qmnn.png").size()*0.8);
            btn->setText(qlist[1]);
            btn->setFont(QFont("宋体",14));
            btn->setAutoRaise(true);
            btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            ui->vlayout->addWidget(btn);
            vector.push_back(btn);
            IsShow.push_back(false);
            connect(btn,&QToolButton::clicked,[=](){
//                if (IsShow[friendnum]) {
//                    QMessageBox::warning(this,"警告","3该聊天框已被打开"+QString(friendnum));
//                    return;
//                }
                IsShow[friendnum] = true;
                Widget *widget = new Widget(nullptr,btn->text(),myname);
                widget->setWindowIcon(btn->icon());
                widget->setWindowTitle(btn->text());
                widget->show();
                connect(widget,&Widget::closeWidget,this,[=](){
                   IsShow[friendnum] = false;
                });
            });
            friendnum ++;
            // 这里是右键点击好友，可以删除的功能
            btn->setContextMenuPolicy(Qt::CustomContextMenu);  //添加右键点击事件
            // 当产生鼠标右键点击事件时，触发menu()函数，创建菜单
            connect(btn,&QToolButton::customContextMenuRequested,this,[=](){
                QMenu *cmenu = new QMenu(btn);
                QAction *action1 = new QAction(tr("删除好友"), this);
                action1->setData(1);
                cmenu->addAction(action1);
                // 触发删除信号
                connect(action1,&QAction::triggered,this,[=](){
                    qDebug() << "删除了";
                    // 数据库删除动作
                    QString user1 = myname;
                    QString user2 = btn->text();
                    QString cmd1 = QString("delete from friend where user1 = '%1' and user2 = '%2'").arg(user1).arg(user2);
                    QString cmd2 = QString("delete from friend where user1 = '%1' and user2 = '%2'").arg(user2).arg(user1);
                    QSqlQuery cursor(db);
                    bool re = cursor.exec(cmd1);
                    cursor.exec(cmd2);
                    qDebug() << re;
                    // 发送删除信息给服务器
                    QString msg = "Delf " + btn->text() + " " + myname;
                    // 格式为类型名+好友名+我的名字
                    mysocket->write(msg.toUtf8());
                    // 好友列表删除动作
                    hydel(btn->text());
                });
                //将下方对话框的数据传到上方对话框内
                cmenu->exec(QCursor::pos());
            });

        }
        else if (qlist[0] == "Delf") {
            QMessageBox::about(this,"通知",qlist[1]+"与你解除了好友关系");
            hydel(qlist[1]);
        }
    });

}

void person::hydel(QString name){
    for (int i = 0; i < friendnum; i ++){
        if (vector[i]->text() == name) {
            delete vector[i];
            vector.remove(i);
            IsShow.remove(i);
        }
    }
    friendnum --;
}

person::~person()
{
    delete ui;
}

void person::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/images/chunse3");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);
}

