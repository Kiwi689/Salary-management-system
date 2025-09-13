#ifndef SALARYWINDOW_H
#define SALARYWINDOW_H

#include <QWidget>
#include "employee.h"   // 需要员工类

namespace Ui {
class SalaryWindow;
}

class SalaryWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SalaryWindow(Employee* emp, QWidget *parent = nullptr);
    ~SalaryWindow();
    Employee* m_employee;  // 当前要显示工资的员工
    void updateSalaryTable();  // 刷新工资表
    void onCellChanged(int row, int column);
//signals:
//    void backToMain();  // 通知主窗口返回

private slots:
    void on_btnAddSalary_clicked();

    void on_btnDeleteSalary_clicked();

private:
    Ui::SalaryWindow *ui;

};

#endif // SALARYWINDOW_H
