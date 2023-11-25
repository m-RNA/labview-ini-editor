/*
 * @Author: 陈俊健
 * @Date: 2023-10-31 00:00:44
 * @LastEditors: 陈俊健
 * @LastEditTime: 2023-10-31 23:12:27
 * @FilePath: \LabViewIniEditor\test_result_interface.cpp
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
}

TestResultInterface::~TestResultInterface() { delete ui; }

void TestResultInterface::setIndex(int index) { ui->lbIndex->setText(QString::number(index)); }

void TestResultInterface::setUi_Result(int index, const TestResult &item)
{
    ui->lbIndex->setText(QString::number(index));

    ui->spinIndex->setValue(item.index);
    if (item.show != "")
        ui->cbShow->setCurrentText(item.show);

    ui->spbxDataSize->setValue(item.dataByteLen);
    ui->spbxDecPlace->setValue(item.decimal);
    ui->spbxByteOrder->setCurrentText(item.byteOrder);
    ui->cbSign->setCurrentText(item.sign);

    if (item.analysisWay != "")
    {
        ui->cbAnalysis->setCurrentText(item.analysisWay);
        ui->leAnalysis->setText(item.analysisContent);
    }
}

void TestResultInterface::setUi_Config(const ConfigContent &config)
{
    ui->leName->setText(config.name);
    ui->cbDataLimit->setCurrentText(config.value);
    ui->cbUnit->setCurrentText(config.unit);
}

// void TestResultInterface::on_cbDataLimit_currentTextChanged(const QString &arg1)
//{
//     if (arg1 == "OK" || arg1.contains("READY"))
//         ui->cbUnit->setVisible(false);
//     else
//         ui->cbUnit->setVisible(true);
// }
