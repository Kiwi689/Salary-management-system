#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include "monthlysalary.h"

class Employee
{
public:
    Employee();
    ~Employee();

    QString id;
    QString name;
    int age;
    QString hireDate;
    QString phone;
    QString address;

    MonthlySalary* salaries;//指针，指向月工资数组，0903完成
    int salaryCount;//当前已有工资月数记录
    int salaryCapacity;//数组当前最大容量

    // 添加月工资记录的功能函数
    void addSalary(const MonthlySalary& newSalary);
    void removeSalary(int index);
    // 可选：获取第i个月工资记录，能不能完成不知道
    MonthlySalary* getSalary(int index) const;

};


#endif // EMPLOYEE_H
