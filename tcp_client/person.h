#ifndef PERSON_H
#define PERSON_H

#include <QWidget>
#include "widget.h"


namespace Ui {
class person;
}

class person : public QWidget
{
    Q_OBJECT

public:
    explicit person(QWidget *parent, QString name);
    ~person();
    //QString recvmsg();
    void paintEvent(QPaintEvent *event);
    void hydel(QString);


signals:


private:
    Ui::person *ui;
    QString myname;
    QTcpSocket *mysocket;
    QVector<bool> IsShow;
    QVector< QToolButton *> vector;
    QStringList iconNameList; //图标资源列表
    int friendnum;

    QSqlDatabase db;
};

#endif // PERSON_H
