#include "salarywindow.h"
#include "ui_salarywindow.h"
#include "monthlysalary.h"
#include <QTableWidgetItem>

SalaryWindow::SalaryWindow(Employee* emp, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SalaryWindow)
    , m_employee(emp)
{
    ui->setupUi(this);

    // 设置窗口标题
    setWindowTitle(QString("工资表 - %1").arg(emp->name));

    // 设置表头
    ui->tableWidget->setColumnCount(4);
    QStringList headers;
    headers << "月份" << "基本工资" << "岗位工资" << "工龄工资";
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    //关闭工资表
    connect(ui->btnBack, &QPushButton::clicked, this, [=](){
        emit backToMain();
        this->close();   // 关闭工资表窗口
    });

    updateSalaryTable();
}

SalaryWindow::~SalaryWindow()
{
    delete ui;
}

void SalaryWindow::updateSalaryTable()
{
    if (!m_employee) return;

    ui->tableWidget->setRowCount(m_employee->salaryCount);

    for (int i = 0; i < m_employee->salaryCount; ++i) {
        MonthlySalary* salary = m_employee->getSalary(i);
        if (salary) {
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem(salary->month));
            ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(salary->basicSalary)));
            ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(salary->postSalary)));
            ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(salary->senioritySalary)));
        }
    }
}
