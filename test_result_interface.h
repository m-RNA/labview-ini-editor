/*
 * @Author: 陈俊健
 * @Date: 2023-10-31 00:00:44
 * @LastEditors: 陈俊健
 * @LastEditTime: 2024-06-10 18:07:16
 * @FilePath: \LabViewIniEditor2024\test_result_interface.h
 * @Description:
 *
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved.
 */
#ifndef TEST_RESULT_INTERFACE_H
#define TEST_RESULT_INTERFACE_H
#if _MSC_VER >= 1600 // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

#include "labviewsetting.h"
#include <QWidget>

namespace Ui
{
class TestResultInterface;
}

class TestResultInterface : public QWidget
{
    Q_OBJECT

public:
    explicit TestResultInterface(QWidget *parent = nullptr);
    ~TestResultInterface();

    void setIndex(int index);
    void setUi_Result(int index, const TestResult &item);
    void setUi_Config(const ConfigContent &config);

    // 从界面获取数据
    TestResult getTestResult() const;

private:
    Ui::TestResultInterface *ui;
};

#endif // TEST_RESULT_INTERFACE_H
