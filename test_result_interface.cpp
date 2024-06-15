/*
 * @Author: 陈俊健
 * @Date: 2023-10-31 00:00:44
 * @LastEditors: 陈俊健
 * @LastEditTime: 2024-06-16 06:26:55
 * @FilePath: \LabViewIniEditor2024\test_result_interface.cpp
 * @Description:
 *
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved.
 */
#if _MSC_VER >= 1600 // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

#include "test_result_interface.h"
#include "ui_test_result_interface.h"

TestResultInterface::TestResultInterface(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TestResultInterface)
{
    ui->setupUi(this);

    // 结果项相关，移至MainWindow中，不再显示
    ui->spbxByteOrder->setVisible(false);
    ui->spbxDecPlace->setVisible(false);
    ui->spbxDataSize->setVisible(false);
    ui->cbSign->setVisible(false);
    ui->lbSign->setVisible(false);
    ui->lbDataSize->setVisible(false);
    ui->lbDecPlace->setVisible(false);
    ui->lbUnit->setVisible(false);

    // 仅打开配置文件时显示
    ui->cbUnit->setVisible(false);
    ui->cbDataLimit->setVisible(false);
    ui->leName->setVisible(false);
}

TestResultInterface::~TestResultInterface() { delete ui; }

void TestResultInterface::setIndex(int index) { ui->lbIndex->setText(QString::number(index)); }

void TestResultInterface::setUi(int index, const TestResult &item)
{
    ui->lbIndex->setText(QString::number(index));

    ui->spinIndex->setValue(item.index);
    if (item.show != "")
        ui->cbShow->setCurrentText(item.show);

    // ui->spbxDataSize->setValue(item.dataByteLen);
    // ui->spbxDecPlace->setValue(item.decimal);
    // ui->spbxByteOrder->setCurrentText(item.byteOrder);
    // ui->cbSign->setCurrentText(item.sign);

    if (item.analysisWay != "")
    {
        ui->cbAnalysis->setCurrentText(item.analysisWay);
        QString content = item.analysisContent;
        // if (content.contains("[:]"))
        //     content.replace("[:]", ":");
        ui->leAnalysis->setText(content);
    }
}

TestResult TestResultInterface::getTestResult() const
{
    TestResult result;
    result.index = ui->spinIndex->value();              // 结果序号
    result.show = ui->cbShow->currentText();            // 结果显示
    result.analysisWay = ui->cbAnalysis->currentText(); // 截取、单匹配、双匹配
    result.analysisContent = ui->leAnalysis->text();    // 数据截取区间、单匹配内容、双匹配内容

    // 如果解析内容中包含 : | ，则需要用[]把":"括起来，即xxx:xxx -> xxx[:]xxx
    // if (result.analysisContent.contains(":"))
    //     result.analysisContent.replace(":", "[:]"); // 替换冒号
    // if (result.analysisContent.contains("|"))
    //     result.analysisContent.replace("|", "[|]"); // 替换竖线

    if (result.show == "不移除")
        result.show = "";
    if (result.analysisWay == "不截取")
        result.analysisWay = "NA";
    return result;
}

// void TestResultInterface::on_cbDataLimit_currentTextChanged(const QString &arg1)
//{
//     if (arg1 == "OK" || arg1.contains("READY"))
//         ui->cbUnit->setVisible(false);
//     else
//         ui->cbUnit->setVisible(true);
// }
