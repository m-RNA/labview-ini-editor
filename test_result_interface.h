/*
 * @Author: 陈俊健
 * @Date: 2023-10-31 00:00:44
 * @LastEditors: 陈俊健
 * @LastEditTime: 2023-10-31 23:02:40
 * @FilePath: \LabViewIniEditer\test_result_interface.h
 * @Description: 
 * 
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved. 
 */
#ifndef TEST_RESULT_INTERFACE_H
#define TEST_RESULT_INTERFACE_H

#include <QWidget>
#include "analysis_ini.h"

namespace Ui {
class TestResultInterface;
}

class TestResultInterface : public QWidget
{
    Q_OBJECT

public:
    explicit TestResultInterface(QWidget *parent = nullptr);
    ~TestResultInterface();

    void setIndex(int index);
    void setUi(int index, const TestResult &item);

private:
    Ui::TestResultInterface *ui;
};

#endif // TEST_RESULT_INTERFACE_H
