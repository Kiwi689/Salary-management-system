QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    employee.cpp \
    employeeinfotable.cpp \
    loginwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    monthlysalary.cpp \
    salarywindow.cpp

HEADERS += \
    employee.h \
    employeeinfotable.h \
    loginwindow.h \
    mainwindow.h \
    monthlysalary.h \
    salarywindow.h

FORMS += \
    loginwindow.ui \
    mainwindow.ui \
    salarywindow.ui

TRANSLATIONS += \
    EmployeeSalary_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
