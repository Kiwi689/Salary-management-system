#include "mainwindow.h"
#include "loginwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
    LoginWindow loginwindow;

//    //先设定好：单击登录按键，可关闭登录页面，进入展示表
//    QObject::connect(&loginwindow,&LoginWindow::loginSuccess,[&](){
//        w.show();
//        loginwindow.close();
//    });

    //打开登录窗口
    loginwindow.show();

    //启动事件循环
    return a.exec();
}
