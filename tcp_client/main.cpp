#include "widget.h"
#include "login.h"
#include "person.h"


#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    person *p = new person(nullptr,"刘鹏");
//    p->show();

//    Widget *p = new Widget(nullptr,"刘鹏","123");
//    p->show();

    login *w = new login();
    w->show();

    QObject::connect(w,&login::toper,[=](){
        QString tmp_name = w->getname();
        delete w;
        person *p = new person(nullptr,tmp_name);
        p->show();
    });

//    Widget b(nullptr, "刘鹏");
//    b.show();

    return a.exec();
}
