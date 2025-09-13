#ifndef MONTHLYSALARY_H
#define MONTHLYSALARY_H

#include <QString>
class MonthlySalary
{
public:
    MonthlySalary();
    ~MonthlySalary(){};

    QString month;
    double basicSalary;
    double postSalary;
    double senioritySalary;
    //其他工资待添加
};

#endif // MONTHLYSALARY_H
