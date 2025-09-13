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
    double postSalary;//岗位工资
    double senioritySalary;//工龄工资
    double allowance;         // 津贴
    double postAllowance;     // 岗贴
    double subsidy;           // 补贴
    double housingAllowance;  // 房贴
    double transportAllowance;// 交通补贴

    // 自动计算
    double getGrossSalary() const;    // 应发工资（税前）
    double getTax() const;            // 个税
    double getNetSalary() const;      // 实发工资
};

#endif // MONTHLYSALARY_H
