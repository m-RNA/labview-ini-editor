﻿/*
 * @Author: m-RNA m-RNA@qq.com
 * @Date: 2023-11-18 21:46:10
 * @LastEditors: 陈俊健
 * @LastEditTime: 2023-11-29 00:20:44
 * @FilePath: \LabViewIniEditor\labviewsetting.h
 * @Description:
 *
 */
#ifndef LABVIEWSETTING_H
#define LABVIEWSETTING_H

#include "inisettings.h"
#include <QList>

QStringList splitStringSquareBrackets(const QString &input, char separator);

class TestCmd
{
public:
    int index = 0;        // 命令序号
    QString brief = "";   // 命令简介
    QString comName = ""; // 端口选择（底板串口、产品串口、主机串口...）

    QString tx = ""; // 发送内容
    QString rx = ""; // 接收内容

    // 命令类型（AT、AT1、68、串口查询真、串口查询假、单按钮弹框、双按钮弹框...)
    QString cmdType = "AT";
    QString encodeWay = "HEX"; // 编码方式（HEX、H2S）

    int cmdDelay = 0;   // 命令延时
    int cmdTimeout = 5; // 命令超时

    void print() const;
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

    QString analysisWay = "";     // 截取、单匹配、双匹配
    QString analysisContent = ""; // 数据截取区间、单匹配内容、双匹配内容

    void print() const;
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

    void print() const;
};

class ConfigContent
{
public:
    QString name = "";  // 限定名称
    QString value = ""; // 限定值
    QString unit = "";  // 限定单位
};

class ConfigItem
{
public:
    QString name = ""; // 测试项名称
    int index = 0;     // 测试项序号
    bool enable = 0;   // 测试项是否启用

    QVector<ConfigContent> contentList = {}; // 测试项内容列表,包含名称、值、单位

    void print() const;
};

class LabViewSetting
{
public:
    LabViewSetting(QString fileNameProtocol, QString fileNameConfig);
    ~LabViewSetting();
    void clear();

    QList<TestItem> getTestItemList() const;
    QList<ConfigItem> getConfigItemList() const;

private:
    void analysisTestItem();
    void analysisConfigItem();

    TestItem getTestItem(const QString &testItemName);

    QString fileNameProtocol = "";
    QString fileNameConfig = "";

    IniSettings *iniSettingsProtocol;
    IniSettings *iniSettingsConfig;

    QList<TestItem> testItemList = {};
    QList<ConfigItem> configItemList = {};
};

#endif // LABVIEWSETTING_H
