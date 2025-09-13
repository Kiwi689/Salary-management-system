#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include"mainwindow.h"
namespace Ui {
class LoginWindow;
}

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

//设置信号
signals:
    void loginSuccess();
//按钮
private slots:

    void on_loginButton_clicked();

private:
    Ui::LoginWindow *ui;
    MainWindow mainWin;
};

#endif // LOGINWINDOW_H
