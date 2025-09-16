#include "salarywindow.h"
#include "monthlysalary.h"

// --- 包含所有需要的头文件 ---
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDebug>
#include <QDate>
#include <QRandomGenerator>
#include <QFileDialog>
#include <QTextStream>
#include <QInputDialog>
#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QRegularExpression>
#include <QLabel>


// calculateVisualWidth 辅助函数保持不变
int calculateVisualWidth(const QString &str) {
    int width = 0;
    for (const QChar &ch : str) {
        if (ch.unicode() > 255) { width += 2; } else { width += 1; }
    }
    return width;
}

SalaryWindow::SalaryWindow(Employee* emp, QWidget *parent)
    : QWidget(parent)
    , m_employee(emp)
    , m_isFiltered(false)
{
    // =================================================================
    // =========== 【核心】在 C++ 代码中构建整个界面 =====================
    // =================================================================

    // --- 1. 创建所有控件 ---
    tableWidget = new QTableWidget(this);
    btnAddSalary = new QPushButton("添加工资", this);
    btnDeleteSalary = new QPushButton("删除工资", this);
    queryButton = new QPushButton("查询", this);
    loadButton = new QPushButton("读取文件", this);
    exportButton = new QPushButton("导出文件", this);

    // --- 2. 创建布局 ---
    // 按钮的水平布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(queryButton);
    buttonLayout->addWidget(loadButton);
    buttonLayout->addWidget(exportButton);
    buttonLayout->addStretch(); // 弹性空间，将后面的按钮推到右边
    buttonLayout->addWidget(btnAddSalary);
    buttonLayout->addWidget(btnDeleteSalary);

    // 整体的垂直布局
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(tableWidget); // 将表格添加到布局中

    // --- 3. 将主布局应用到 SalaryWindow 窗口 ---
    this->setLayout(mainLayout);

    // --- 4. 设置窗口和控件的属性 ---
    setWindowTitle(QString("工资表 - %1").arg(emp->name));
    resize(1200, 600);

    // 设置表头
    tableWidget->setColumnCount(11);
    QStringList headers;
    headers << "月份" << "基本工资" << "岗位工资" << "工龄工资"<< "津贴" << "岗贴" << "补贴" << "房贴" << "交通补贴" << "个税" << "实发工资";
    tableWidget->setHorizontalHeaderLabels(headers);

    // 设置表格行为
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    // --- 5. 连接信号与槽 ---
    connect(queryButton, &QPushButton::clicked, this, &SalaryWindow::onQueryClicked);
    connect(loadButton, &QPushButton::clicked, this, &SalaryWindow::onLoadClicked);
    connect(exportButton, &QPushButton::clicked, this, &SalaryWindow::onExportClicked);
    connect(btnAddSalary, &QPushButton::clicked, this, &SalaryWindow::on_btnAddSalary_clicked);
    connect(btnDeleteSalary, &QPushButton::clicked, this, &SalaryWindow::on_btnDeleteSalary_clicked);
    connect(tableWidget, &QTableWidget::cellChanged, this, &SalaryWindow::onCellChanged);

    // --- 6. 初始数据填充 ---
    updateSalaryTable();
}

SalaryWindow::~SalaryWindow()
{
    // 由于控件都指定了 this 作为父对象，Qt会自动管理它们的内存，
    // 所以析构函数中无需手动 delete 它们。
}

// =================================================================
// ===========  其他函数中的 ui->... 全部修改为直接访问成员 ============
// =================================================================

void SalaryWindow::updateSalaryTable()
{
    if (!m_employee) return;

    tableWidget->setRowCount(m_employee->salaryCount); // 修改处

    for (int i = 0; i < m_employee->salaryCount; ++i) {
        MonthlySalary* salary = m_employee->getSalary(i);
        if (salary) {
            // 所有 ui->tableWidget 都改为 tableWidget
            tableWidget->setItem(i, 0, new QTableWidgetItem(salary->month));
            tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(salary->basicSalary)));
            tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(salary->postSalary)));
            tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(salary->senioritySalary)));
            tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(salary->allowance)));
            tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(salary->postAllowance)));
            tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(salary->subsidy)));
            tableWidget->setItem(i, 7, new QTableWidgetItem(QString::number(salary->housingAllowance)));
            tableWidget->setItem(i, 8, new QTableWidgetItem(QString::number(salary->transportAllowance)));
            tableWidget->setItem(i, 9, new QTableWidgetItem(QString::number(salary->getTax())));
            tableWidget->setItem(i, 10, new QTableWidgetItem(QString::number(salary->getNetSalary())));
        }
    }
}

void SalaryWindow::on_btnDeleteSalary_clicked()
{
    int currentRow = tableWidget->currentRow(); // 修改处
    if (currentRow >= 0 && currentRow < m_employee->salaryCount) {
           for (int i = currentRow; i < m_employee->salaryCount - 1; ++i) {
               m_employee->salaries[i] = m_employee->salaries[i + 1];
           }
           m_employee->salaryCount--;
           updateSalaryTable();
           tableWidget->clearSelection(); // 修改处
       } else {
           QMessageBox::warning(this, "删除失败", "请先选中要删除的工资记录！");
       }
}

void SalaryWindow::onCellChanged(int row, int column)
{
    if (!m_employee || row < 0 || row >= m_employee->salaryCount) return;

    MonthlySalary* salary = m_employee->getSalary(row);
    if (!salary) return;

    QString text = tableWidget->item(row, column)->text(); // 修改处

    switch (column) {
    case 0: salary->month = text; break;
    case 1: salary->basicSalary = text.toDouble(); break;
    // ... (其他case不变) ...
    case 8: salary->transportAllowance = text.toDouble(); break;
    }

    tableWidget->blockSignals(true); // 修改处
    tableWidget->setItem(row, 9, new QTableWidgetItem(QString::number(salary->getTax())));
    tableWidget->setItem(row, 10, new QTableWidgetItem(QString::number(salary->getNetSalary())));
    tableWidget->blockSignals(false); // 修改处
}

void SalaryWindow::applyFilter()
{
    for (int i = 0; i < tableWidget->rowCount(); ++i) { // 修改处
        tableWidget->setRowHidden(i, false); // 修改处
    }

    if (m_isFiltered && !m_filteredRows.isEmpty()) {
        for (int i = 0; i < tableWidget->rowCount(); ++i) { // 修改处
            if (!m_filteredRows.contains(i)) {
                tableWidget->setRowHidden(i, true); // 修改处
            }
        }
    }

    tableWidget->clearSelection(); // 修改处
    for (int row : m_filteredRows) {
        tableWidget->selectRow(row); // 修改处
    }
}

void SalaryWindow::clearFilter()
{
    m_filteredRows.clear();
    m_isFiltered = false;

    for (int i = 0; i < tableWidget->rowCount(); ++i) { // 修改处
        tableWidget->setRowHidden(i, false); // 修改处
    }

    tableWidget->clearSelection(); // 修改处
    QMessageBox::information(this, "清除筛选", "已清除所有筛选条件");
}

// on_btnAddSalary_clicked, onQueryClicked, onLoadClicked, onExportClicked, exportToTextFile
// 这些函数内部不访问 ui->... 所以它们无需任何修改，保持原样即可
// 为保持完整性，我将它们也附在下方

void SalaryWindow::on_btnAddSalary_clicked(){
    MonthlySalary newSalary;
    newSalary.month = QDate::currentDate().toString("yyyy-MM");
    QRandomGenerator* gen = QRandomGenerator::global();
    newSalary.basicSalary = 5000 + gen->bounded(2000);
    newSalary.postSalary = 2000 + gen->bounded(1000);
    newSalary.senioritySalary = m_employee->age * 20;
    newSalary.allowance = gen->bounded(500);
    newSalary.postAllowance = gen->bounded(300);
    newSalary.subsidy = gen->bounded(200);
    newSalary.housingAllowance = gen->bounded(800);
    newSalary.transportAllowance = gen->bounded(300);
    m_employee->addSalary(newSalary);
    updateSalaryTable();
}

void SalaryWindow::onQueryClicked(){
    if (m_isFiltered) {
        int result = QMessageBox::question(this, "清除筛选","当前已有筛选条件，是否先清除筛选？",QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if (result == QMessageBox::Yes) { clearFilter(); return; }
        else if (result == QMessageBox::Cancel) { return; }
    }
    QDialog dialog(this);
    dialog.setWindowTitle("工资查询");
    QFormLayout form(&dialog);
    QComboBox *fieldCombo = new QComboBox(&dialog);
    fieldCombo->addItem("月份", 0);
    fieldCombo->addItem("基本工资", 1);
    fieldCombo->addItem("岗位工资", 2);
    fieldCombo->addItem("工龄工资", 3);
    fieldCombo->addItem("实发工资", 10);
    QLineEdit *valueEdit = new QLineEdit(&dialog);
    valueEdit->setPlaceholderText("输入查询值");
    QComboBox *operatorCombo = new QComboBox(&dialog);
    operatorCombo->addItem("等于", "==");
    operatorCombo->addItem("大于", ">");
    operatorCombo->addItem("大于等于", ">=");
    operatorCombo->addItem("小于", "<");
    operatorCombo->addItem("小于等于", "<=");
    operatorCombo->addItem("包含", "contains");
    QPushButton *clearButton = new QPushButton("清除筛选", &dialog);
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    form.addRow("查询字段:", fieldCombo);
    form.addRow("比较条件:", operatorCombo);
    form.addRow("查询值:", valueEdit);
    form.addRow(clearButton);
    form.addRow(&buttonBox);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    connect(clearButton, &QPushButton::clicked, [&]() { clearFilter(); dialog.reject(); });
    if (dialog.exec() == QDialog::Accepted) {
        int fieldIndex = fieldCombo->currentData().toInt();
        QString operatorStr = operatorCombo->currentData().toString();
        QString value = valueEdit->text().trimmed();
        if (value.isEmpty()) { QMessageBox::warning(this, "提示", "请输入查询值"); return; }
        m_filteredRows.clear();
        for (int i = 0; i < m_employee->salaryCount; ++i) {
            MonthlySalary* salary = m_employee->getSalary(i);
            if (!salary) continue;
            bool match = false;
            double numericValue = value.toDouble();
            switch (fieldIndex) {
            case 0:
                if (operatorStr == "==") match = (salary->month == value);
                else if (operatorStr == "contains") match = salary->month.contains(value);
                break;
            case 1: case 2: case 3: case 10:
                double compareValue;
                if (fieldIndex == 10) compareValue = salary->getNetSalary();
                else if (fieldIndex == 1) compareValue = salary->basicSalary;
                else if (fieldIndex == 2) compareValue = salary->postSalary;
                else compareValue = salary->senioritySalary;
                if (operatorStr == "==") match = (qFuzzyCompare(compareValue, numericValue));
                else if (operatorStr == ">") match = (compareValue > numericValue);
                else if (operatorStr == ">=") match = (compareValue >= numericValue);
                else if (operatorStr == "<") match = (compareValue < numericValue);
                else if (operatorStr == "<=") match = (compareValue <= numericValue);
                break;
            }
            if (match) { m_filteredRows.insert(i); }
        }
        if (m_filteredRows.isEmpty()) {
            m_isFiltered = false;
            QMessageBox::information(this, "查询结果", "未找到匹配的记录");
        } else {
            m_isFiltered = true;
            QMessageBox::information(this, "查询结果", QString("找到 %1 条匹配记录").arg(m_filteredRows.size()));
        }
        applyFilter();
    }
}

void SalaryWindow::onLoadClicked(){
    QString fileName = QFileDialog::getOpenFileName(this,"打开工资文件", "", "CSV文件 (*.csv);;文本文件 (*.txt);;所有文件 (*)");
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) { QMessageBox::warning(this, "错误", "无法打开文件: " + file.errorString()); return; }
    QTextStream in(&file);
    in.setAutoDetectUnicode(true);
    in.setCodec("UTF-8");
    int loadedCount = 0;
    int errorCount = 0;
    QStringList errorMessages;
    if (!in.atEnd()) {
        QString headerLine = in.readLine();
        if (!headerLine.contains("月份") && !headerLine.contains("基本工资")) { file.seek(0); in.seek(0); }
    }
    QRegularExpression dateRegex("^\\d{4}-\\d{2}$");
    int lineNumber = 1;
    while (!in.atEnd()) {
        QString line = in.readLine();
        lineNumber++;
        if (line.trimmed().isEmpty() || line.trimmed().startsWith("#") || line.trimmed().contains(QRegularExpression("^-+$"))) { continue; }
        QStringList fields;
        if (line.contains(',')) { fields = line.split(','); }
        else { fields = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts); }
        if (fields.size() < 9) { errorCount++; errorMessages.append(QString("第%1行: 字段数量不足 (%2个，需要至少9个)").arg(lineNumber).arg(fields.size())); continue; }
        if (!dateRegex.match(fields[0]).hasMatch()) { errorCount++; errorMessages.append(QString("第%1行: 月份格式错误 '%2'").arg(lineNumber).arg(fields[0])); continue; }
        bool conversionOk = true;
        QVector<double> numericValues;
        for (int i = 1; i <= 8 && i < fields.size(); ++i) {
            bool ok;
            double value = fields[i].toDouble(&ok);
            if (!ok) { conversionOk = false; errorCount++; errorMessages.append(QString("第%1行: 第%2列数字格式错误 '%3'").arg(lineNumber).arg(i+1).arg(fields[i])); break; }
            numericValues.append(value);
        }
        if (!conversionOk) continue;
        MonthlySalary newSalary;
        newSalary.month = fields[0];
        newSalary.basicSalary = numericValues[0];
        newSalary.postSalary = numericValues[1];
        newSalary.senioritySalary = numericValues[2];
        newSalary.allowance = numericValues[3];
        newSalary.postAllowance = numericValues[4];
        newSalary.subsidy = numericValues[5];
        newSalary.housingAllowance = numericValues[6];
        newSalary.transportAllowance = numericValues[7];
        m_employee->addSalary(newSalary);
        loadedCount++;
    }
    file.close();
    updateSalaryTable();
    QString message = QString("成功加载 %1 条记录").arg(loadedCount);
    if (errorCount > 0) {
        message += QString("\n发现 %1 个问题").arg(errorCount);
        if (errorCount <= 5) { message += "\n问题详情:\n" + errorMessages.join("\n"); }
        else { message += "\n（前5个问题）:\n" + errorMessages.mid(0, 5).join("\n"); }
    }
    QMessageBox::information(this, "读取完成", message);
}

void SalaryWindow::onExportClicked(){
    QDialog dialog(this);
    dialog.setWindowTitle("选择导出格式");
    QVBoxLayout layout(&dialog);
    QComboBox formatCombo(&dialog);
    formatCombo.addItem("CSV (Excel兼容)", "csv");
    formatCombo.addItem("对齐的文本文件 (*.txt)", "txt");
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    layout.addWidget(new QLabel("选择导出格式:"));
    layout.addWidget(&formatCombo);
    layout.addWidget(&buttonBox);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted) return;
    QString format = formatCombo.currentData().toString();
    QString filter;
    QString defaultSuffix;
    if (format == "csv") { filter = "CSV文件 (*.csv)"; defaultSuffix = "csv"; }
    else if (format == "txt") { filter = "文本文件 (*.txt)"; defaultSuffix = "txt"; }
    QString fileName = QFileDialog::getSaveFileName(this, "导出工资文件", "工资数据." + defaultSuffix, filter);
    if (fileName.isEmpty()) return;
    bool success = exportToTextFile(fileName, format == "csv");
    if (success) { QMessageBox::information(this, "导出完成", QString("工资数据已成功导出到: %1").arg(fileName)); }
    else { QMessageBox::warning(this, "导出失败", "导出过程中发生错误"); }
}

bool SalaryWindow::exportToTextFile(const QString &fileName, bool isCSV){
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) { return false; }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    if (isCSV) { out.setGenerateByteOrderMark(true); }
    QStringList headers = {"月份", "基本工资", "岗位工资", "工龄工资", "津贴", "岗贴", "补贴", "房贴", "交通补贴", "个税", "实发工资"};
    if (isCSV) {
        out << headers.join(',') << "\n";
    } else {
        QVector<int> columnWidths = {12, 12, 12, 12, 10, 10, 10, 10, 12, 10, 12};
        for (int i = 0; i < headers.size(); ++i) {
            QString header = headers[i];
            int padding = columnWidths[i] - calculateVisualWidth(header);
            if (padding < 0) padding = 0;
            out << header << QString(padding, ' ');
        }
        out << "\n";
        for (int i = 0; i < headers.size(); ++i) { out << QString(columnWidths[i], '-'); }
        out << "\n";
    }
    for (int i = 0; i < m_employee->salaryCount; ++i) {
        MonthlySalary* salary = m_employee->getSalary(i);
        if (salary) {
            QStringList values;
            values << salary->month
                   << QString::number(salary->basicSalary, 'f', 2)
                   << QString::number(salary->postSalary, 'f', 2)
                   << QString::number(salary->senioritySalary, 'f', 2)
                   << QString::number(salary->allowance, 'f', 2)
                   << QString::number(salary->postAllowance, 'f', 2)
                   << QString::number(salary->subsidy, 'f', 2)
                   << QString::number(salary->housingAllowance, 'f', 2)
                   << QString::number(salary->transportAllowance, 'f', 2)
                   << QString::number(salary->getTax(), 'f', 2)
                   << QString::number(salary->getNetSalary(), 'f', 2);
            if (isCSV) {
                out << values.join(',') << "\n";
            } else {
                QVector<int> columnWidths = {12, 12, 12, 12, 10, 10, 10, 10, 12, 10, 12};
                for (int j = 0; j < values.size(); ++j) {
                    QString value = values[j];
                    int padding = columnWidths[j] - calculateVisualWidth(value);
                    if (padding < 0) padding = 0;
                    out << value << QString(padding, ' ');
                }
                out << "\n";
            }
        }
    }
    file.close();
    return true;
}
