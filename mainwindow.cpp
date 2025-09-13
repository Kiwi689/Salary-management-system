#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loginwindow.h"
#include "employee.h"
#include "monthlysalary.h"
#include "salarywindow.h"
#include <QToolBar>
#include <QMessageBox>
#include <QDebug>
#include <QTableWidgetItem> // 需要这个来操作表格项
#include <QDate>   // 添加这行，包含QDate头文件
#include <QRandomGenerator> // 同时添加这个，用于生成随机数



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_employees(nullptr)    // 初始化指针为空
    , m_employeeCount(0)      // 初始数量为0
    , m_employeeCapacity(0)   // 初始容量为0
    , m_currentSelectedRow(-1) // 初始化为-1，表示没有选中
{
    ui->setupUi(this);


    // 工具栏
    QToolBar *toolBar = addToolBar(tr("主工具栏"));
    toolBar->setFixedHeight(60);
    toolBar->setStyleSheet("QToolButton { font-size: 10pt; }");

    // 表格
    ui->tableWidget->setColumnCount(5);
    QStringList headers;
    headers << "工号" << "姓名" << "年龄" << "电话" << "住址";
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // **关键：把 tableWidget 设置为中心窗口**
    setCentralWidget(ui->tableWidget);
    resize(1000, 600);
    // 添加员工
    addEmployeeAction = new QAction(tr("添加员工"), this);
    connect(addEmployeeAction, &QAction::triggered, this, &MainWindow::onAddEmployeeClicked);
    toolBar->addAction(addEmployeeAction);
    toolBar->addSeparator();

    // 删除员工
    deleteEmployeeAction = new QAction(tr("删除员工"), this);
    connect(deleteEmployeeAction, &QAction::triggered, this, &MainWindow::onDeleteEmployeeClicked);
    toolBar->addAction(deleteEmployeeAction);
    toolBar->addSeparator();

    // 工资
    salaryAction = new QAction(tr("工资管理"), this);
    connect(salaryAction, &QAction::triggered, this, &MainWindow::on_SalaryButton_clicked);
    toolBar->addAction(salaryAction);


    // 连接表格的选择信号
        connect(ui->tableWidget, &QTableWidget::cellClicked,
                this, &MainWindow::onEmployeeSelected);

    // +++ 添加测试员工 +++
    Employee* testEmp = new Employee();
    testEmp->id = "1001";
    testEmp->name = "测试员工";
    testEmp->age = 25;
    testEmp->phone = "13800138000";
    testEmp->address = "测试地址";
    addEmployeeToArray(testEmp); // 用我们写的函数来添加

    // 刷新显示
    updateEmployeeTable();
}


MainWindow::~MainWindow()
{
    // 1. 删除每一个Employee对象（公寓里的每个人）
    for (int i = 0; i < m_employeeCount; ++i) {
        delete m_employees[i]; // 删除Employee对象
    }

    // 2. 删除存储指针的数组本身（拆除公寓楼）
    delete[] m_employees;

    delete ui;
}


void MainWindow::onEmployeeSelected(int row, int column)
{
    Q_UNUSED(column);
    m_currentSelectedRow = row;

    if (row >= 0 && row < m_employeeCount) {
        Employee* selectedEmployee = m_employees[row];
        qDebug() << "选中了职工:" << selectedEmployee->name;

    }
}


// 向数组添加一个新职工，并处理数组扩容
void MainWindow::addEmployeeToArray(Employee* newEmployee)
{
    // 1. 检查是否需要扩容（公寓楼是否住满了）
    if (m_employeeCount >= m_employeeCapacity) {
        // 计算新的容量。如果当前是0，就扩容到2，否则扩容到当前的1.5倍+1
        int newCapacity = (m_employeeCapacity == 0) ? 2 : (m_employeeCapacity * 3 / 2 + 1);

        // 2. 申请新的、更大的“公寓楼”（数组）
        Employee** newArray = new Employee*[newCapacity];

        // 3. 把原来“公寓楼”里的人全部搬到新楼（拷贝数据）
        for (int i = 0; i < m_employeeCount; ++i) {
            newArray[i] = m_employees[i];
        }

        // 4. 拆掉旧的公寓楼，释放内存
        delete[] m_employees;

        // 5. 让m_employees指向新的公寓楼
        m_employees = newArray;
        m_employeeCapacity = newCapacity;
    }

    // 6. 把新职工安排进新房间
    m_employees[m_employeeCount] = newEmployee;
    m_employeeCount++;
}

// 更新表格显示
void MainWindow::updateEmployeeTable()
{
    // 1. 先清空表格所有行
    ui->tableWidget->setRowCount(0);

    // 2. 设置表格的行数为当前职工数量
    ui->tableWidget->setRowCount(m_employeeCount);

    // 3. 遍历数组，填充表格
    for (int i = 0; i < m_employeeCount; ++i) {
        Employee* emp = m_employees[i]; // 拿到第i个职工的指针

        // 创建表格项并填入数据
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(emp->id));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(emp->name));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(emp->age))); // 数字转字符串
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(emp->phone));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(emp->address));
    }
}



// 添加职工按钮点击
void MainWindow::onAddEmployeeClicked()
{
    // 1. 创建一个新的职工对象
    Employee* newEmployee = new Employee();

    // 2. 这里应该弹出一个对话框让用户输入信息
    // 暂时先用测试数据
    newEmployee->id = QString::number(1000 + m_employeeCount + 1);
    newEmployee->name = "新员工" + QString::number(m_employeeCount + 1);
    newEmployee->age = 20 + m_employeeCount;
    newEmployee->phone = "未知";
    newEmployee->address = "未知";

    // 3. 添加到数组
    addEmployeeToArray(newEmployee);

    // 4. 更新显示
    updateEmployeeTable();

    qDebug() << "添加职工成功，当前总数：" << m_employeeCount;
}

// 删除职工按钮点击
void MainWindow::onDeleteEmployeeClicked()
{
    if (m_currentSelectedRow < 0 || m_currentSelectedRow >= m_employeeCount) {
        QMessageBox::warning(this, "警告", "请先选择一个职工");
        return;
    }

    // 1. 删除职工对象
    delete m_employees[m_currentSelectedRow];

    // 2. 将后面的元素前移
    for (int i = m_currentSelectedRow; i < m_employeeCount - 1; ++i) {
        m_employees[i] = m_employees[i + 1];
    }

    // 3. 更新计数
    m_employeeCount--;

    // 4. 重置选中状态
    m_currentSelectedRow = -1;

    // 5. 更新显示
    updateEmployeeTable();

    qDebug() << "删除职工成功，当前总数：" << m_employeeCount;
}



//工资按钮
void MainWindow::on_SalaryButton_clicked()
{
    if (m_currentSelectedRow < 0 || m_currentSelectedRow >= m_employeeCount) {
            QMessageBox::warning(this, "警告", "请先选择一个职工");
            return;
        }

        Employee* selectedEmployee = m_employees[m_currentSelectedRow];

        if (!salaryWin) {  // 只在第一次创建
            salaryWin = new SalaryWindow(selectedEmployee);
            salaryWin->setAttribute(Qt::WA_DeleteOnClose);
            salaryWin->setWindowModality(Qt::ApplicationModal);

            // 窗口关闭时，把指针设为 nullptr
            connect(salaryWin, &QObject::destroyed, this, [this]() {
                salaryWin = nullptr;
            });
        }
        salaryWin->show();
        salaryWin->raise();
        salaryWin->activateWindow();
}

void MainWindow::on_addEmployee_triggered()
{

}
