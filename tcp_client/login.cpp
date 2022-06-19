#include "login.h"
#include "ui_login.h"

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    // 使窗口透明
    //setWindowFlags(Qt::FramelessWindowHint);

    // 设置输入框透明无边框
    ui->dl_btn->setFlat(true);
    ui->zc_btn->setFlat(true);
    ui->zh_btn->setStyleSheet("background:rgba(255,255,255,40%);border-style:outset");
    ui->mm_btn->setStyleSheet("background:rgba(255,255,255,40%);border-style:outset");
    ui->mm_btn->setEchoMode(QLineEdit::Password);//输入时显示数字，失去焦点时显示圆点

    // 测试使用
//    ui->zh_btn->setText("刘鹏");
//    ui->mm_btn->setText("123");

    // 因为要登陆验证和注册，所以连接数据库
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


    connect(ui->dl_btn,&QPushButton::clicked,this,[=](){
        // 点一下登陆，将输入框内的内容发送给数据库，清空输入框
        sndmsg(dlu);
    });
    connect(ui->zc_btn,&QPushButton::clicked,this,[=](){
        // 点一下注册，将输入框内的内容发送给数据库，清空输入框
        sndmsg(zce);
    });
    //创建一个快捷键"Key_Return"键
    QShortcut *key=new QShortcut(QKeySequence(Qt::Key_Return),this);
    connect(key,&QShortcut::activated,this,[=](){
        sndmsg(dlu);
    });
}


login::~login()
{
    delete ui;
}
void login::sndmsg(msgType type){
    QString usern = ui->zh_btn->text();
    QString pwd = ui->mm_btn->text();
    ui->zh_btn->clear();
    ui->zh_btn->setFocus();
    ui->mm_btn->clear();
    ui->mm_btn->setFocus();
    // 保证myname的值为最新
    myname = usern;
    if (type == dlu){//发送登陆信息
        QString cmd = QString("select * from user where username = '%1' and passwd = '%2'").arg(usern).arg(pwd);
        QSqlQuery db_cursor(db);
        // 执行
        db_cursor.exec(cmd);
        QString res;
        if(db_cursor.size() == 0){
            QMessageBox::warning(this,"警告","用户未注册");
        }else{
            emit toper();
        }
    }
    else if (type == zce) { // 发送注册信息
        QSqlQuery query(db);
        QString cmd = QString("insert into user(username, passwd) values('%1','%2')").arg(usern).arg(pwd);
        bool re = query.exec(cmd);
        QString res;
        if(!re){
            QMessageBox::warning(this,"警告","用户已注册");
        }else{
            QMessageBox::about(this,"注册","注册成功，请登录");
        }
    }
}
QString login::getname(){
    return myname;
}
// 设置背景
void login::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/images/chunse2");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);
}

