#include "monthlysalary.h"

MonthlySalary::MonthlySalary()
{
    basicSalary = 0;
    postSalary = 0;
    senioritySalary = 0;
    allowance = 0;
    postAllowance = 0;
    subsidy = 0;
    housingAllowance = 0;
    transportAllowance = 0;
}
double MonthlySalary::getGrossSalary() const
{
    return basicSalary + postSalary + senioritySalary
           + allowance + postAllowance + subsidy
           + housingAllowance + transportAllowance;
}

double MonthlySalary::getTax() const
{
    double gross = getGrossSalary();
    double tax = 0;

    if (gross <= 800) {
        tax = 0;
    } else if (gross <= 1000) {
        tax = (gross - 800) * 0.05;
    } else if (gross <= 5000) {
        tax = 200 * 0.05 + (gross - 1000) * 0.10;
    } else {
        tax = 200 * 0.05 + 4000 * 0.10 + (gross - 5000) * 0.20;
    }

    return tax;
}

double MonthlySalary::getNetSalary() const
{
    return getGrossSalary() - getTax();
}
