#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSqlDatabase> //sql驱动基础
#include <QSqlQuery>//sql查询相关
#include <QSqlError>//sql输出错误

enum msgType {Msg, Iden, Req, Clo, Lok, Addf, Delf};

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void sndmsg(QTcpSocket *msocket,QString msg, msgType);
    msgType recvmsg(QTcpSocket *);

private:
    Ui::Widget *ui;
    QTcpServer *myserver;
    QList<QTcpSocket*> client;
    QList<QString> cli_name;
    QSqlDatabase db;
};
#endif // WIDGET_H
