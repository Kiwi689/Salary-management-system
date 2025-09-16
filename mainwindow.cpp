#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "employee.h"
#include "salarywindow.h"

#include <QToolBar>
#include <QMessageBox>
#include <QDebug>
#include <QTableWidgetItem>
#include <QInputDialog>
#include <QFileDialog>

// === 新增的头文件，用于JSON处理和查询对话框 ===
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QPushButton>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_employees(nullptr)
    , m_employeeCount(0)
    , m_employeeCapacity(0)
    , m_currentSelectedRow(-1)
{
    ui->setupUi(this);

    // 设置窗口标题和大小
    setWindowTitle("企业员工信息管理系统");
    resize(1000, 600);

    // ====================== 工具栏设置 ======================
    QToolBar *toolBar = addToolBar(tr("主工具栏"));
    toolBar->setFixedHeight(60);
    toolBar->setStyleSheet("QToolButton { font-size: 10pt; padding: 8px; }"); // 样式优化

    // --- 员工管理 ---
    addEmployeeAction = new QAction(tr("添加员工"), this);
    toolBar->addAction(addEmployeeAction);

    deleteEmployeeAction = new QAction(tr("删除员工"), this);
    toolBar->addAction(deleteEmployeeAction);

    toolBar->addSeparator();

    // --- 工资管理 ---
    salaryAction = new QAction(tr("工资管理"), this);
    toolBar->addAction(salaryAction);

    toolBar->addSeparator();

    // --- 【新增】查询与文件操作 ---
    queryAction = new QAction(tr("查询员工"), this);
    toolBar->addAction(queryAction);

    loadAction = new QAction(tr("导入数据"), this);
    toolBar->addAction(loadAction);

    exportAction = new QAction(tr("导出数据"), this);
    toolBar->addAction(exportAction);

    // ====================== 表格设置 ======================
    ui->tableWidget->setColumnCount(5);
    QStringList headers;
    headers << "工号" << "姓名" << "年龄" << "电话" << "住址";
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // 表格只读

    setCentralWidget(ui->tableWidget);

    // ====================== 连接信号与槽 ======================
    connect(addEmployeeAction, &QAction::triggered, this, &MainWindow::onAddEmployeeClicked);
    connect(deleteEmployeeAction, &QAction::triggered, this, &MainWindow::onDeleteEmployeeClicked);
    connect(salaryAction, &QAction::triggered, this, &MainWindow::on_SalaryButton_clicked);
    connect(ui->tableWidget, &QTableWidget::cellClicked, this, &MainWindow::onEmployeeSelected);

    // --- 【新增】连接新功能的信号 ---
    connect(queryAction, &QAction::triggered, this, &MainWindow::onQueryEmployeeClicked);
    connect(loadAction, &QAction::triggered, this, &MainWindow::onLoadDataClicked);
    connect(exportAction, &QAction::triggered, this, &MainWindow::onExportDataClicked);


    // 加载一个测试员工
    Employee* testEmp = new Employee();
    testEmp->id = "1001";
    testEmp->name = "张三";
    testEmp->age = 35;
    testEmp->phone = "13800138000";
    testEmp->address = "北京朝阳区";
    // 为测试员工添加几条工资
    MonthlySalary s1; s1.month = "2025-07"; s1.basicSalary = 8000; testEmp->addSalary(s1);
    MonthlySalary s2; s2.month = "2025-08"; s2.basicSalary = 8200; testEmp->addSalary(s2);
    addEmployeeToArray(testEmp);

    updateEmployeeTable();
}

MainWindow::~MainWindow()
{
    clearAllData(); // 使用封装的清理函数
    delete ui;
}

// 清理所有员工数据，释放内存
void MainWindow::clearAllData()
{
    for (int i = 0; i < m_employeeCount; ++i) {
        delete m_employees[i];
    }
    delete[] m_employees;

    m_employees = nullptr;
    m_employeeCount = 0;
    m_employeeCapacity = 0;
    m_currentSelectedRow = -1;
}


// ====================================================================
// =========================  新增功能：查询员工  =========================
// ====================================================================

void MainWindow::onQueryEmployeeClicked()
{
    if (m_isEmployeeFiltered) {
        int result = QMessageBox::question(this, "清除筛选", "当前已有筛选，是否清除？", QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::Yes) {
            clearEmployeeFilter();
        }
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("查询员工");
    QFormLayout form(&dialog);

    QComboBox *fieldCombo = new QComboBox(&dialog);
    fieldCombo->addItem("姓名", "name");
    fieldCombo->addItem("工号", "id");
    fieldCombo->addItem("电话", "phone");
    fieldCombo->addItem("住址", "address");

    QLineEdit *valueEdit = new QLineEdit(&dialog);
    valueEdit->setPlaceholderText("输入查询关键词");

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow("查询字段:", fieldCombo);
    form.addRow("查询值:", valueEdit);
    form.addRow(&buttonBox);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString field = fieldCombo->currentData().toString();
        QString value = valueEdit->text().trimmed();
        if (value.isEmpty()) {
            QMessageBox::warning(this, "提示", "请输入查询值");
            return;
        }

        m_filteredEmployeeRows.clear();
        for (int i = 0; i < m_employeeCount; ++i) {
            Employee* emp = m_employees[i];
            bool match = false;
            if (field == "name") {
                match = emp->name.contains(value);
            } else if (field == "id") {
                match = emp->id.contains(value);
            } else if (field == "phone") {
                match = emp->phone.contains(value);
            } else if (field == "address") {
                match = emp->address.contains(value);
            }
            if (match) {
                m_filteredEmployeeRows.insert(i);
            }
        }

        if (m_filteredEmployeeRows.isEmpty()) {
            QMessageBox::information(this, "查询结果", "未找到匹配的员工。");
        } else {
            m_isEmployeeFiltered = true;
            applyEmployeeFilter();
            QMessageBox::information(this, "查询结果", QString("找到 %1 名匹配的员工。").arg(m_filteredEmployeeRows.size()));
        }
    }
}

void MainWindow::applyEmployeeFilter()
{
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        if (m_isEmployeeFiltered && !m_filteredEmployeeRows.contains(i)) {
            ui->tableWidget->setRowHidden(i, true);
        } else {
            ui->tableWidget->setRowHidden(i, false);
        }
    }
}

void MainWindow::clearEmployeeFilter()
{
    m_filteredEmployeeRows.clear();
    m_isEmployeeFiltered = false;
    applyEmployeeFilter(); // 传入false状态，会自动显示所有行
}


// ====================================================================
// ======================  新增功能：导入/导出  ========================
// ====================================================================

void MainWindow::onLoadDataClicked()
{
    int result = QMessageBox::question(this, "导入数据",
                                       "导入数据将覆盖当前所有员工信息，确定要继续吗？",
                                       QMessageBox::Yes | QMessageBox::No);
    if (result == QMessageBox::No) {
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(this, "选择导入文件", "", "JSON文件 (*.json)");
    if (filePath.isEmpty()) {
        return;
    }

    loadDataFromJson(filePath);
}

void MainWindow::onExportDataClicked()
{
    if (m_employeeCount == 0) {
        QMessageBox::information(this, "提示", "当前没有员工数据可以导出。");
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this, "选择导出位置", "employees_data.json", "JSON文件 (*.json)");
    if (filePath.isEmpty()) {
        return;
    }

    saveDataToJson(filePath);
}

void MainWindow::saveDataToJson(const QString &filePath)
{
    QJsonArray employeesArray;
    for (int i = 0; i < m_employeeCount; ++i) {
        Employee* emp = m_employees[i];
        QJsonObject employeeObject;
        employeeObject["id"] = emp->id;
        employeeObject["name"] = emp->name;
        employeeObject["age"] = emp->age;
        employeeObject["phone"] = emp->phone;
        employeeObject["address"] = emp->address;

        // --- 核心：嵌入工资数据 ---
        QJsonArray salariesArray;
        for (int j = 0; j < emp->salaryCount; ++j) {
            MonthlySalary* s = emp->getSalary(j);
            QJsonObject salaryObject;
            salaryObject["month"] = s->month;
            salaryObject["basicSalary"] = s->basicSalary;
            salaryObject["postSalary"] = s->postSalary;
            salaryObject["senioritySalary"] = s->senioritySalary;
            salaryObject["allowance"] = s->allowance;
            salaryObject["postAllowance"] = s->postAllowance;
            salaryObject["subsidy"] = s->subsidy;
            salaryObject["housingAllowance"] = s->housingAllowance;
            salaryObject["transportAllowance"] = s->transportAllowance;
            salariesArray.append(salaryObject);
        }
        employeeObject["salaries"] = salariesArray;
        employeesArray.append(employeeObject);
    }

    QJsonDocument doc(employeesArray);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Indented)); // Indented使JSON文件格式化，更易读
        file.close();
        QMessageBox::information(this, "成功", "数据已成功导出到:\n" + filePath);
    } else {
        QMessageBox::critical(this, "错误", "无法写入文件:\n" + filePath);
    }
}

void MainWindow::loadDataFromJson(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "错误", "无法读取文件:\n" + filePath);
        return;
    }

    QByteArray data = file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (!doc.isArray()) {
        QMessageBox::critical(this, "错误", "文件格式无效，根节点必须是JSON数组。");
        return;
    }

    // --- 安全地加载数据 ---
    clearAllData(); // 先清空现有数据
    clearEmployeeFilter(); // 清除筛选状态

    QJsonArray employeesArray = doc.array();
    for (const QJsonValue &value : employeesArray) {
        QJsonObject employeeObject = value.toObject();
        Employee* newEmployee = new Employee();

        newEmployee->id = employeeObject["id"].toString();
        newEmployee->name = employeeObject["name"].toString();
        newEmployee->age = employeeObject["age"].toInt();
        newEmployee->phone = employeeObject["phone"].toString();
        newEmployee->address = employeeObject["address"].toString();

        // --- 核心：读取工资数据 ---
        if (employeeObject.contains("salaries") && employeeObject["salaries"].isArray()) {
            QJsonArray salariesArray = employeeObject["salaries"].toArray();
            for (const QJsonValue &s_value : salariesArray) {
                QJsonObject salaryObject = s_value.toObject();
                MonthlySalary s;
                s.month = salaryObject["month"].toString();
                s.basicSalary = salaryObject["basicSalary"].toDouble();
                s.postSalary = salaryObject["postSalary"].toDouble();
                s.senioritySalary = salaryObject["senioritySalary"].toDouble();
                s.allowance = salaryObject["allowance"].toDouble();
                s.postAllowance = salaryObject["postAllowance"].toDouble();
                s.subsidy = salaryObject["subsidy"].toDouble();
                s.housingAllowance = salaryObject["housingAllowance"].toDouble();
                s.transportAllowance = salaryObject["transportAllowance"].toDouble();
                newEmployee->addSalary(s);
            }
        }
        addEmployeeToArray(newEmployee);
    }

    updateEmployeeTable();
    QMessageBox::information(this, "成功", QString("成功从文件导入 %1 名员工的数据。").arg(m_employeeCount));
}


// ====================================================================
// ======================  以下为原有功能函数  =========================
// ====================================================================

void MainWindow::onEmployeeSelected(int row, int column)
{
    Q_UNUSED(column);
    m_currentSelectedRow = row;
    if (row >= 0 && row < m_employeeCount) {
        Employee* selectedEmployee = m_employees[row];
        qDebug() << "选中了职工:" << selectedEmployee->name;
    }
}

void MainWindow::addEmployeeToArray(Employee* newEmployee)
{
    if (m_employeeCount >= m_employeeCapacity) {
        int newCapacity = (m_employeeCapacity == 0) ? 2 : (m_employeeCapacity * 3 / 2 + 1);
        Employee** newArray = new Employee*[newCapacity];
        for (int i = 0; i < m_employeeCount; ++i) {
            newArray[i] = m_employees[i];
        }
        delete[] m_employees;
        m_employees = newArray;
        m_employeeCapacity = newCapacity;
    }
    m_employees[m_employeeCount] = newEmployee;
    m_employeeCount++;
}

void MainWindow::updateEmployeeTable()
{
    qDebug() << "[诊断] 正在更新 MainWindow 的【员工】表格...";
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setRowCount(m_employeeCount);
    for (int i = 0; i < m_employeeCount; ++i) {
        Employee* emp = m_employees[i];
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(emp->id));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(emp->name));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(emp->age)));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(emp->phone));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(emp->address));
    }
}

void MainWindow::onAddEmployeeClicked()
{
    // 使用 QInputDialog 获取用户信息，比硬编码更好
    bool ok;
    QString name = QInputDialog::getText(this, "添加新员工", "姓名:", QLineEdit::Normal, "", &ok);
    if (!ok || name.isEmpty()) return;

    QString id = QInputDialog::getText(this, "添加新员工", "工号:", QLineEdit::Normal, "", &ok);
    if (!ok || id.isEmpty()) return;

    int age = QInputDialog::getInt(this, "添加新员工", "年龄:", 25, 18, 100, 1, &ok);
    if (!ok) return;

    QString phone = QInputDialog::getText(this, "添加新员工", "电话:", QLineEdit::Normal, "", &ok);
    if (!ok) return; // 电话可以为空

    QString address = QInputDialog::getText(this, "添加新员工", "住址:", QLineEdit::Normal, "", &ok);
    if (!ok) return; // 地址可以为空


    Employee* newEmployee = new Employee();
    newEmployee->id = id;
    newEmployee->name = name;
    newEmployee->age = age;
    newEmployee->phone = phone;
    newEmployee->address = address;

    addEmployeeToArray(newEmployee);
    updateEmployeeTable();
    qDebug() << "添加职工成功，当前总数：" << m_employeeCount;
}

void MainWindow::onDeleteEmployeeClicked()
{
    if (m_currentSelectedRow < 0 || m_currentSelectedRow >= m_employeeCount) {
        QMessageBox::warning(this, "警告", "请先选择一个要删除的职工");
        return;
    }

    int result = QMessageBox::question(this, "确认删除",
                                       QString("确定要删除员工【%1】吗？此操作不可恢复。")
                                       .arg(m_employees[m_currentSelectedRow]->name),
                                       QMessageBox::Yes | QMessageBox::No);

    if (result == QMessageBox::Yes) {
        delete m_employees[m_currentSelectedRow];
        for (int i = m_currentSelectedRow; i < m_employeeCount - 1; ++i) {
            m_employees[i] = m_employees[i + 1];
        }
        m_employeeCount--;
        m_currentSelectedRow = -1;
        updateEmployeeTable();
        qDebug() << "删除职工成功，当前总数：" << m_employeeCount;
    }
}


void MainWindow::on_SalaryButton_clicked()
{
    if (m_currentSelectedRow < 0 || m_currentSelectedRow >= m_employeeCount) {
        QMessageBox::warning(this, "警告", "请先选择一个职工以管理其工资");
        return;
    }

    Employee* selectedEmployee = m_employees[m_currentSelectedRow];

    // 创建一个【全新的】SalaryWindow实例
    SalaryWindow *salaryWindow = new SalaryWindow(selectedEmployee, this);
    salaryWindow->setAttribute(Qt::WA_DeleteOnClose);

    // 设置为模态，弹出后会显示在MainWindow前面，并且MainWindow不能动
    salaryWindow->setWindowModality(Qt::ApplicationModal);
    salaryWindow->show();
}
