/*
 * @Author: 陈俊健
 * @Date: 2024-05-20 23:00:24
 * @LastEditors: 陈俊健
 * @LastEditTime: 2024-06-30 19:20:27
 * @FilePath: \LabViewIniEditor2024\test_item_interface.h
 * @Description: 
 * 
 * Copyright (c) 2024 by Chenjunjian, All Rights Reserved. 
 */
#ifndef TEST_ITEM_INTERFACE_H
#define TEST_ITEM_INTERFACE_H
#if _MSC_VER >= 1600 // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif
#include "labviewsetting.h"
#include <QWidget>

namespace Ui
{
class TestItemInterface;
}

class TestItemInterface : public QWidget
{
    Q_OBJECT

public:
    explicit TestItemInterface(QWidget *parent = nullptr);
    ~TestItemInterface();

    void setIndex(int index);
    void setHighlight(bool isSet);
    void setUi(int index, const TestCmd &item);
    void uiSetTx(QString strTx);

    // 从界面获取数据
    TestCmd getTestCmd() const;

private slots:

    void on_leTx_textChanged(const QString &arg1);

    void on_leTx_editingFinished();

    void on_leRx_editingFinished();

    void on_cbTestType_currentIndexChanged(int index);

    void on_cbTestType_currentTextChanged(const QString &arg1);

private:
    Ui::TestItemInterface *ui;
};

#endif // TEST_ITEM_INTERFACE_H
