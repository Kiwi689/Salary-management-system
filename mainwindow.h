#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSet> // 需要包含QSet
#include "employee.h"
#include "salarywindow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // --- 原有槽函数 ---
    void onAddEmployeeClicked();
    void onDeleteEmployeeClicked();
    void onEmployeeSelected(int row, int column);
    void on_SalaryButton_clicked();

    // --- 新增槽函数 ---
    void onQueryEmployeeClicked();
    void onLoadDataClicked();
    void onExportDataClicked();


private:
    // --- UI 和数据 ---
    Ui::MainWindow *ui;
    Employee** m_employees;
    int m_employeeCount;
    int m_employeeCapacity;
    int m_currentSelectedRow;
    SalaryWindow *salaryWin = nullptr;

    // --- 新增的查询相关成员 ---
    bool m_isEmployeeFiltered = false;
    QSet<int> m_filteredEmployeeRows;


    // --- 核心功能函数 ---
    void addEmployeeToArray(Employee* newEmployee);
    void updateEmployeeTable();
    void clearAllData(); // 新增：用于安全加载新数据前清空内存

    // --- 新增的查询相关函数 ---
    void applyEmployeeFilter();
    void clearEmployeeFilter();

    // --- 新增的导入导出函数 ---
    void saveDataToJson(const QString& filePath);
    void loadDataFromJson(const QString& filePath);


    // --- 界面元素 ---
    QAction *addEmployeeAction;
    QAction *deleteEmployeeAction;
    QAction *salaryAction;
    // --- 新增的界面Action ---
    QAction *queryAction;
    QAction *loadAction;
    QAction *exportAction;

};
#endif // MAINWINDOW_H
