#include "widget.h"
#include "ui_widget.h"
#include <QColorDialog>

Widget::Widget(QWidget *parent, QString fname, QString name)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    myname = name;
    fri_name = fname;

    ui->txtBrs->setStyleSheet("background:rgba(255,255,255,40%);border-style:outset");
    ui->txtEdit->setStyleSheet("background:rgba(255,255,255,40%);border-style:outset");

    ui->fontCbx->setStyleSheet("background:rgba(255,255,255,40%);border-style:outset");
    ui->sizeCbx->setStyleSheet("background:rgba(255,255,255,40%);border-style:outset");
    ui->boldTBtn->setStyleSheet("background:rgba(255,255,255,40%);border-style:outset");
    ui->clearTBtn->setStyleSheet("background:rgba(255,255,255,40%);border-style:outset");
    ui->colorTBtn->setStyleSheet("background:rgba(255,255,255,40%);border-style:outset");
    ui->italicTBtn->setStyleSheet("background:rgba(255,255,255,40%);border-style:outset");
    ui->underlineTBtn->setStyleSheet("background:rgba(255,255,255,40%);border-style:outset");

    ui->quit_btn->setStyleSheet("background:rgba(255,255,255,0%);border-style:outset");
    ui->deliver_btn->setStyleSheet("background:rgba(255,255,255,0%);border-style:outset");

    //字体类型
    connect(ui->fontCbx,&QFontComboBox::currentFontChanged,[=](const QFont &font){
        ui->txtEdit->setFontFamily(font.toString());
        ui->txtEdit->setFocus();
    });
    //字体大小
    void (QComboBox:: * sizebtn)(const QString &text)=&QComboBox::currentTextChanged;
    connect(ui->sizeCbx,sizebtn,[=](const QString &text){
        ui->txtEdit->setFontPointSize(text.toDouble());
        ui->txtEdit->setFocus();
    });
    //字体的加粗
    connect(ui->boldTBtn,&QToolButton::clicked,[=](bool checked){
        if(checked)
        {
            ui->txtEdit->setFontWeight(QFont::Bold);
        }
        else {
            ui->txtEdit->setFontWeight(QFont::Normal);
        }
    });
    //字体倾斜
    connect(ui->italicTBtn,&QToolButton::clicked,[=](bool checked){
        ui->txtEdit->setFontItalic(checked);
        ui->txtEdit->setFocus();
    });
    //字体下划线
    connect(ui->underlineTBtn,&QToolButton::clicked,[=](bool checked){
        ui->txtEdit->setFontUnderline(checked);
        ui->txtEdit->setFocus();
    });
    //清空功能
    connect(ui->clearTBtn,&QToolButton::clicked,[=](){
        ui->txtBrs->clear();
    });
    // 颜色
    connect(ui->colorTBtn,&QToolButton::clicked,[=](){
        QColor color=QColorDialog::getColor(color,this);
        ui->txtEdit->setTextColor(color);
    });

    // 测试
    mysocket = new QTcpSocket(this);
    mysocket->connectToHost(server_IP,19999);
    connect(mysocket,&QTcpSocket::connected,[=](){
        QMessageBox::about(this,"服务器","成功连接");
        // 发送Req消息请求给服务器
        QString tmp = "Req " + myname + " " + fri_name;
        mysocket->write(tmp.toUtf8());
    });

    // 点击发送按钮
    connect(ui->deliver_btn,&QPushButton::clicked,[=](){
        sndmsg(Msg);
    });

    // 有消息到达，触发
    connect(mysocket,&QIODevice::readyRead,this,[=](){
        recvmsg();
    });
    // 退出按钮
    connect(ui->quit_btn,&QPushButton::clicked,this,[=](){
        this->close();
    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::sndmsg(msgType type){
    if (type == Msg){//发送聊天信息
        QString msg = "";
        msg = QString("Msg %1 %2 %3").arg(chat_name).arg(myname).arg(ui->txtEdit->toHtml());
        ui->txtEdit->clear();
        ui->txtEdit->setFocus();
        mysocket->write(msg.toUtf8());
    }
}
void Widget::recvmsg(){
    QByteArray message = mysocket->readAll();
    QString msg = QString(message);
    QStringList qlist = msg.split(" ");
    msgType type;
    if (qlist[0] == "Msg") type = Msg;
    else if (qlist[0] == "Clo") type = Clo;
    else if (qlist[0] == "Lok") type = Lok;
    if (type == Msg){//接收聊天信息
        QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        ui->txtBrs->setTextColor(QColor(Qt::blue));
        ui->txtBrs->append("["+qlist.at(1)+"]"+time);
        msg = msg.mid(qlist.at(1).size()+5);
        ui->txtBrs->append(msg);
    }
    else if (type == Clo) {
        if (qlist[1] == "0")
            QMessageBox::warning(this,"警告","对方未上线");
        else if (qlist[1] == "1")
            QMessageBox::warning(this,"警告","对方离开");
        this->close();
        mysocket->close();
    }
    else if (type == Lok) {
        QMessageBox::about(this,"通知","对方已进入私聊窗口");
        chat_name = qlist[1];
    }
}
QString Widget::getname(){
    return this->myname;
}
void Widget::closeEvent(QCloseEvent *e)
{
    emit this->closeWidget();
    mysocket->close();
    QWidget::closeEvent(e);
}
void Widget::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/images/beijing4");
    painter.drawPixmap(0,0,this->width(),this->height(),pix);
}
