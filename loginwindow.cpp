#include "loginwindow.h"
#include "ui_loginwindow.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QLineEdit>
#include <QPushButton>

LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
//    //设置个标题
//    setWindowTitle("员工工资管理系统-登录");

//    //连接下登录按钮和槽
//    connect(ui->loginButton,&QPushButton::clicked,this,&LoginWindow::on_loginButton_clicked);


       // 保证密码隐藏
       ui->passwordLineEdit->setEchoMode(QLineEdit::Password);

       // 让按钮成为默认按钮（有时有帮助）并连接信号
       ui->loginButton->setDefault(true);
       connect(ui->loginButton, &QPushButton::clicked, this, &LoginWindow::on_loginButton_clicked);

       // 按回车也触发登录（可靠做法）
       connect(ui->userNameLineEdit, &QLineEdit::returnPressed, this, &LoginWindow::on_loginButton_clicked);
       connect(ui->passwordLineEdit, &QLineEdit::returnPressed, this, &LoginWindow::on_loginButton_clicked);



}

LoginWindow::~LoginWindow()
{
    delete ui;
}

void LoginWindow::on_loginButton_clicked()
{
        const QString username = ui->userNameLineEdit->text().trimmed();
        const QString password = ui->passwordLineEdit->text();

        // 简单演示验证（把它换成你真正的验证逻辑）
        if (username == "admin" && password == "123456") {
//            if (!mainWin) {
//                mainWin = new MainWindow(); // 如果你的 w 有接受用户名的构造函数可以改成 new w(username)
//            }
            mainWin.show();
            this->close(); // 关闭登录窗口（如果想保留可用 hide()）
        } else {
            QMessageBox::warning(this, tr("登录失败"), tr("用户名或密码错误"));
            ui->passwordLineEdit->clear();
            ui->passwordLineEdit->setFocus();
        }

}
