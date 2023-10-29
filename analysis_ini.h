/*
 * @Author: 陈俊健
 * @Date: 2023-10-29 13:35:55
 * @LastEditors: 陈俊健
 * @LastEditTime: 2023-10-29 22:39:51
 * @FilePath: \LabViewIniEditer\analysis_ini.h
 * @Description:
 *
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved.
 */
#ifndef __ANALYSIS_INI_H__
#define __ANALYSIS_INI_H__

#include <QString>
#include <QStringList>
#include <QVector>

class TestCmd
{
public:
    int index = 1;        // 命令序号
    QString brief = "";   // 命令简介
    QString comName = ""; // 端口选择（底板串口、产品串口、主机串口...）

    QString tx = ""; // 发送内容
    QString rx = ""; // 接收内容

    // 命令类型（AT、AT<\r\n>、AT<HEX>、68、串口查询真、串口查询假、单按钮弹框、双按钮弹框...)
    QString cmdType = "AT";
    int dataByteLen = 2;       // 每个数据项占用字节的长度
    int decimal = 0;           // 小数点位置
    QString byteOrder = "LH";  // 字节序 （LH、HL）
    QString encodeWay = "HEX"; // 编码方式（HEX、H2S）
    QString sign = "0";        // 符号

    int cmdDelay = 0;        // 命令延时
    int cmdTimeout = 5;      // 命令超时
    QString resultShow = ""; // 显示结果

    QString rxAnalysis = ""; // 接收数据解析(截取、单匹配、双匹配)

    // QString dataLimit; // 数据规格限制
    // QString dataUnit;  // 数据单位
};

class TestItem
{
public:
    QString name = ""; // 测试项名称
    int repeat = 0;    // 重复次数

    QStringList resultShow = {}; // 显示结果
    QStringList dataLimit = {};  // 数据规格限制
    QStringList dataUnit = {};   // 数据单位

    QVector<TestCmd> cmdList = {}; // 测试项命令列表
    int cmdNum = 0;                // 测试项命令数量
};

QVector<QStringList> analysis_ini(const QString fileName);
TestItem analysis_StringToTestItem(const QStringList testItem);
QStringList splitStringSquareBrackets(const QString &input, char separator);

#endif
