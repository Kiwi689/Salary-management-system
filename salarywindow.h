#ifndef SALARYWINDOW_H
#define SALARYWINDOW_H

#include <QWidget>
#include "employee.h"   // 需要员工类
#include <QSet>

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
    void onQueryClicked();
    void onLoadClicked();
    void onExportClicked();
    void clearFilter(); // 添加这行

private:
    Ui::SalaryWindow *ui;
    bool exportToTextFile(const QString &fileName, bool isCSV);
    bool exportToExcel(const QString &fileName);
    QSet<int> m_filteredRows; // 存储当前筛选的行
    bool m_isFiltered;        // 是否处于筛选状态
    void applyFilter(); // 也需要声明
};

#endif // SALARYWINDOW_H
