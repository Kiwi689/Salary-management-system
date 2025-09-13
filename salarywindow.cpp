#include "salarywindow.h"
#include "ui_salarywindow.h"
#include "monthlysalary.h"
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDate>   // 添加这行，包含QDate头文件
#include <QRandomGenerator> // 同时添加这个，用于生成随机数

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


    //确保仅能选中单行
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); // 整行选中
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection); // 只能单选

    //关闭工资表
//    connect(ui->btnBack, &QPushButton::clicked, this, [=](){
//        emit backToMain();
//        this->close();   // 关闭工资表窗口
//    });

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

void SalaryWindow::on_btnAddSalary_clicked()
{
    MonthlySalary newSalary;
       newSalary.month = QDate::currentDate().toString("yyyy-MM");
       QRandomGenerator* gen = QRandomGenerator::global();
       newSalary.basicSalary = 5000 + gen->bounded(2000);
       newSalary.postSalary = 2000 + gen->bounded(1000);
       newSalary.senioritySalary = m_employee->age * 20;
       m_employee->addSalary(newSalary);
       updateSalaryTable();
}



void SalaryWindow::on_btnDeleteSalary_clicked()
{
    int currentRow = ui->tableWidget->currentRow();
       if (currentRow >= 0) {
           // 同时删除数据和表格行
           m_employee->removeSalary(currentRow);   // 从 Employee 数据结构删除
           updateSalaryTable();                    // 重新刷新表格
           ui->tableWidget->clearSelection();      // 清除选中
       } else {
           QMessageBox::warning(this, "删除失败", "请先选中要删除的工资记录！");
       }
}
