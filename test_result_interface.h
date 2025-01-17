﻿/*
 * @Author: 陈俊健
 * @Date: 2023-10-31 00:00:44
 * @LastEditors: 陈俊健
 * @LastEditTime: 2024-06-30 19:35:18
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

    int getShowIndex() const;

    void setIndex(int index);
    void setUi(int index, const TestResult &item);

    // 从界面获取数据
    TestResult getTestResult() const;

signals:
    void indexChanged(int index);

private slots:
    void on_spinIndex_valueChanged(int arg1);

private:
    Ui::TestResultInterface *ui;
};

#endif // TEST_RESULT_INTERFACE_H
