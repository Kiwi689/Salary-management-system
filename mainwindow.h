#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <salarywindow.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Employee;

namespace Ui{
class MainWindow;
}

class MainWindow:public QMainWindow{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent=nullptr);
    ~MainWindow();


    // 按钮点击的槽函数
    void onAddEmployeeClicked();
    void onDeleteEmployeeClicked();

    //员工被单击选择的槽函数
    void onEmployeeSelected(int row,int column);

private slots:
    void on_SalaryButton_clicked();

    void on_addEmployee_triggered();

private:
    Ui::MainWindow *ui;

    Employee** m_employees;//二级指针 意为Employee* m_employees[]

    int m_employeeCount;
    int m_employeeCapacity;
    int m_currentSelectedRow; // 记录当前选中的职工行号

    SalaryWindow* salaryWin = nullptr;  // 保存指针

    void updateEmployeeTable();
    void addEmployeeToArray(Employee* newEmployee);

    QAction *addEmployeeAction;
    QAction *deleteEmployeeAction;
    QAction *salaryAction;

};

#endif // MAINWINDOW_H
