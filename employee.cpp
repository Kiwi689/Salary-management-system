#include "employee.h"
#include <QDebug>

//构造
Employee::Employee()
    : salaries(nullptr)  // 初始化为空指针
    , salaryCount(0)     // 初始数量为0
    , salaryCapacity(0)  // 初始容量为0

{
    id="";
    name="";
    age=0;
    hireDate="";
    phone="";
    address="";
}

//析构
Employee::~Employee()
{
    //释放月工资数组的内存
    delete [] salaries;
}

//add monthly salary recording
void Employee::addSalary(const MonthlySalary &newSalary){

    if(salaryCount>=salaryCapacity){
        //1.calculate new capacity
        int newCapacity=(salaryCapacity==0)?2:(salaryCapacity*3/2+1);

        //2.create a new bigger array
        MonthlySalary* newArray=new MonthlySalary[newCapacity];

        //3.copy old data to new array
        for(int i=0;i<salaryCount;i++){
            newArray[i]=salaries[i];
        }

        //4.free old array
        delete[] salaries;

        //5.use new data
        salaries=newArray;
        salaryCapacity=newCapacity;
    }
    //6.add new recording
    salaries[salaryCount]=newSalary;
    salaryCount++;
}

//delete monthly salary recording
void Employee::removeSalary(int index) {
    if (index < 0 || index >= salaryCount) return;

    // 新数组（少一个元素）
    MonthlySalary* newArr = new MonthlySalary[salaryCount - 1];

    // 拷贝除 index 外的元素
    int k = 0;
    for (int i = 0; i < salaryCount; i++) {
        if (i == index) continue;
        newArr[k++] = salaries[i];
    }

    // 替换旧数组
    delete[] salaries;
    salaries = newArr;
    salaryCount--;
}


//get monthly recording searched
MonthlySalary* Employee::getSalary(int index) const{
    if(index >=0 && index < salaryCount){
        return &salaries[index];//return address of the recording
    }
    return nullptr;//search null pointer

}
