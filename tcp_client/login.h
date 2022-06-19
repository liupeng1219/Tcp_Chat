#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include "widget.h"

namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();
    void sndmsg(msgType type);
    QString getname();
    void paintEvent(QPaintEvent *event);


signals:
    void toper();

private:
    Ui::login *ui;
    QTcpSocket *denglu_socket;
    QString myname;
    QSqlDatabase db;
};

#endif // LOGIN_H
