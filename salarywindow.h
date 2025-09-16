#ifndef SALARYWINDOW_H
#define SALARYWINDOW_H

#include <QWidget>
#include <QSet>
#include "employee.h"

// --- Forward declarations ---
class QTableWidget;
class QPushButton;

class SalaryWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SalaryWindow(Employee* emp, QWidget *parent = nullptr);
    ~SalaryWindow();

private slots:
    void on_btnAddSalary_clicked();
    void on_btnDeleteSalary_clicked();
    void onCellChanged(int row, int column);
    void onQueryClicked();
    void onLoadClicked();
    void onExportClicked();
    void clearFilter();

private:
    void updateSalaryTable();
    void applyFilter();

    // --- 【FIX】Add the missing function declaration here ---
    bool exportToTextFile(const QString &fileName, bool isCSV);

    // --- Data members ---
    Employee* m_employee;
    bool m_isFiltered;
    QSet<int> m_filteredRows;

    // --- UI widget members ---
    QTableWidget *tableWidget;
    QPushButton *btnAddSalary;
    QPushButton *btnDeleteSalary;
    QPushButton *queryButton;
    QPushButton *loadButton;
    QPushButton *exportButton;
};

#endif // SALARYWINDOW_H
