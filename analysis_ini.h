/*
 * @Author: 陈俊健
 * @Date: 2023-10-29 13:35:55
 * @LastEditors: 陈俊健
 * @LastEditTime: 2023-10-31 12:18:03
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
    int index = 0;        // 命令序号
    QString brief = "";   // 命令简介
    QString comName = ""; // 端口选择（底板串口、产品串口、主机串口...）

    QString tx = ""; // 发送内容
    QString rx = ""; // 接收内容

    // 命令类型（AT、AT<\r\n>、AT<HEX>、68、串口查询真、串口查询假、单按钮弹框、双按钮弹框...)
    QString cmdType = "AT";
    QString encodeWay = "HEX"; // 编码方式（HEX、H2S）

    int cmdDelay = 0;   // 命令延时
    int cmdTimeout = 5; // 命令超时

    // QString resultShow = ""; // 显示结果
    // QString dataLimit; // 数据规格限制
    // QString dataUnit;  // 数据单位
    // QString rxAnalysis = ""; // 接收数据解析(截取、单匹配、双匹配)
};

class TestResult
{
public:
    int index = 0;     // CMD显示序号
    QString show = ""; // 显示结果 <_空白>

    int dataByteLen = 2;      // 每个数据项占用字节的长度
    int decimal = 0;          // 小数点位置
    QString byteOrder = "LH"; // 字节序 （LH、HL）
    QString sign = "0";       // 符号

    QString dataLimit = ""; // 数据规格限制
    QString dataUnit = "";  // 数据单位

    QString analysisWay = "";     // 截取、单匹配、双匹配
    QString analysisContent = ""; // 数据截取区间、单匹配内容、双匹配内容
};

class TestItem
{
public:
    QString name = ""; // 测试项名称
    int repeat = 0;    // 重复次数

    QVector<TestCmd> cmdList = {}; // 测试项命令列表
    int cmdNum = 0;                // 测试项命令数量

    QVector<TestResult> resultList = {}; // 测试项结果列表
    int resultNum = 0;                   // 测试项结果数量
    int resultIndexMax = 0;              // 测试项结果数量
};

QVector<TestItem> analysis_ini(const QString &pathFileName);
TestItem analysis_StringToTestItem(const QStringList testItem);
QStringList splitStringSquareBrackets(const QString &input, char separator);

#endif

/**
1、analysis_ini()将每个测试项截取出来 放于 QVector<QStringList> 中
2、对单个测试项进行解析，首先要判断单个测试项的命令数目，命令数目 可以如下判断 发送= 这一行中的 :
的个数，但是要注意，有的 ：被方括号括起来，这不表示把命令隔开，而是命令中有 ：，所以要注意
3、对单个测试项进行解析，将每个命令截取出来，放于 QVector<TestCmd> 中。

[飞行休眠电流测试]
// 端口选择 = 底板串口:底板串口:底板串口:底板串口:底板串口:底板串口
// 发送 =68 74 11 16 01 00 04 16<HEX>:68 74 11 08 01 00 F6 16<HEX>:68 74 11 0E 01 02 FE 16<HEX>:68 74 11 0D 01 00 FB
16:68 74 11 0C 01 01 FB 16<HEX>:68 74 11 C0 03 01 05 00 B6 16<HEX>:68 74 11 08 01 01 F7 16<HEX>
// 接收 = 68 74 A1 16 01 00 94 16:68 74 A1 08 01 00 86 16:68 74 A1 0E 01 02 8E 16:68 74 A1 0D 02:68 74 A1 0C 01 01 8B
16:68 74 A1 C0 03 01 05 00 46 16:68 74 A1 08 01 01 87 16
// 参数配置 =AT1&AT1&AT1&68&AT1&AT1:2:1:LH:HEX:0:0:5|3
// ;参数配置=解析方式：每个数据项占用字节的长度:小数位:字节序:编码方式:符号:延时时间:超时时间|显示结果
// 解析=双匹配&LADC[:]1[,]&OK:双匹配&LADC[: ]1[,]&OK
// 功能配置=重发次数(45)
*/