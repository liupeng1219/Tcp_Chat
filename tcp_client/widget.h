#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QDateTime>
#include <QMessageBox>
#include <QIcon>
#include <QToolButton>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlDatabase>
#include <QSqlQuery>//sql查询相关
#include <QSqlError>//sql输出错误
#include <QShortcut>
#include <QPainter>
#include <QAction>
#include <QMenu>

#define server_IP "116.205.186.55"
//#define server_IP "127.0.0.1"

QT_BEGIN_NAMESPACE

namespace Ui { class Widget; }
QT_END_NAMESPACE

enum msgType {Msg,Iden,dlu,zce,Req,Clo,Lok,Addf,Delf};

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent, QString, QString);
    ~Widget();
    void closeEvent(QCloseEvent *);
    void paintEvent(QPaintEvent *event);
    void sndmsg(msgType);
    void recvmsg();
    QString getname();

signals:
    void closeWidget();

private:
    Ui::Widget *ui;
    QTcpSocket *mysocket;
    QString myname;
    QString fri_name;// 私聊朋友的名字
    QString chat_name; // 私聊窗口的名字
};
#endif // WIDGET_H
