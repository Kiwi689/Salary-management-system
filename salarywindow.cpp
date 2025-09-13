#include "salarywindow.h"
#include "ui_salarywindow.h"
#include "monthlysalary.h"
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QMainWindow>
#include <QVBoxLayout>
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
    ui->tableWidget->setColumnCount(11);
    QStringList headers;
    headers << "月份" << "基本工资" << "岗位工资" << "工龄工资"<< "津贴" << "岗贴" << "补贴" << "房贴" << "交通补贴"
            << "个税" << "实发工资";
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // 布局：让 tableWidget 填满整个窗口
       QVBoxLayout *layout = new QVBoxLayout(this);
       layout->addWidget(ui->tableWidget);
       layout->setContentsMargins(0, 0, 0, 0); // 去掉边距
       setLayout(layout);

    //表格大小设置
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // 表格列均分，不会太窄
    resize(2000, 600);  // 默认窗口大小大一点

    //确保仅能选中单行
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); // 整行选中
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection); // 只能单选

    // 在构造函数里加：
    connect(ui->tableWidget, &QTableWidget::cellChanged,
            this, &SalaryWindow::onCellChanged);

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
            ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(salary->allowance)));
            ui->tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(salary->postAllowance)));
            ui->tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(salary->subsidy)));
            ui->tableWidget->setItem(i, 7, new QTableWidgetItem(QString::number(salary->housingAllowance)));
            ui->tableWidget->setItem(i, 8, new QTableWidgetItem(QString::number(salary->transportAllowance)));
            ui->tableWidget->setItem(i, 9, new QTableWidgetItem(QString::number(salary->getTax())));
            ui->tableWidget->setItem(i, 10, new QTableWidgetItem(QString::number(salary->getNetSalary())));
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

    // 新增补贴（可以先设为 0，或者随机给一些数）
    newSalary.allowance = gen->bounded(500);
    newSalary.postAllowance = gen->bounded(300);
    newSalary.subsidy = gen->bounded(200);
    newSalary.housingAllowance = gen->bounded(800);
    newSalary.transportAllowance = gen->bounded(300);

    m_employee->addSalary(newSalary);
    updateSalaryTable();
}



void SalaryWindow::on_btnDeleteSalary_clicked()
{
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow >= 0 && currentRow < m_employee->salaryCount) {
    // 1. 删除内存中的工资记录（把后面的往前挪一位）
           for (int i = currentRow; i < m_employee->salaryCount - 1; ++i) {
               m_employee->salaries[i] = m_employee->salaries[i + 1];
           }
           m_employee->salaryCount--;

           // 2. 刷新表格显示
           updateSalaryTable();

           // 3. 清除选中状态
           ui->tableWidget->clearSelection();
       } else {
           QMessageBox::warning(this, "删除失败", "请先选中要删除的工资记录！");
       }
}

void SalaryWindow::onCellChanged(int row, int column)
{
    if (!m_employee || row < 0 || row >= m_employee->salaryCount) return;

    MonthlySalary* salary = m_employee->getSalary(row);
    if (!salary) return;

    QString text = ui->tableWidget->item(row, column)->text();

    // 把用户修改同步到数据
    switch (column) {
    case 0: salary->month = text; break;
    case 1: salary->basicSalary = text.toDouble(); break;
    case 2: salary->postSalary = text.toDouble(); break;
    case 3: salary->senioritySalary = text.toDouble(); break;
    case 4: salary->allowance = text.toDouble(); break;
    case 5: salary->postAllowance = text.toDouble(); break;
    case 6: salary->subsidy = text.toDouble(); break;
    case 7: salary->housingAllowance = text.toDouble(); break;
    case 8: salary->transportAllowance = text.toDouble(); break;
    }

    // 临时阻断信号，避免死循环
    ui->tableWidget->blockSignals(true);

    // 重新计算并刷新这一行的税收和实发工资
    ui->tableWidget->setItem(row, 9, new QTableWidgetItem(QString::number(salary->getTax())));
    ui->tableWidget->setItem(row, 10, new QTableWidgetItem(QString::number(salary->getNetSalary())));

    // 恢复信号
    ui->tableWidget->blockSignals(false);
}

