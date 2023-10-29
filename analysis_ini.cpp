/*
 * @Author: 陈俊健
 * @Date: 2023-10-29 13:18:19
 * @LastEditors: 陈俊健
 * @LastEditTime: 2023-10-29 13:45:21
 * @FilePath: \LabViewIniEditer\analysis_ini.cpp
 * @Description:
 *
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved.
 */
#include "analysis_ini.h"
#include <QDebug>
#include <QFile>
#include <QTextCodec>
#include <QTextStream>
/*
1、要在形如如下格式文件中截取出每一个测试项：如下所示，有3个测试项目：开机检测、关机漏电流、底板初始化
2、要在每个测试项中截取出每一行的内容，如下所示，每一行的内容用 QStringList 将中的测试内容保存起来
3、最后每个测试项的内容用 QVector<QStringList> 将中的测试内容保存起来

[开机检测]
端口选择 = 底板串口
发送 =NA
接收 = 10000ms
参数配置 = AT:1:0:LH:HEX:0:0:18|0
;参数配置 =解析方式：每个数据项占用字节的长度:小数位:字节序:编码方式:符号:延时时间:超时时间|显示结果


[关机漏电流]
端口选择 = 底板串口:底板串口:底板串口
发送 =68 74 11 0E 01 03 FF 16:68 74 11 0D 01 00 FB 16:68 74 11 0C 01 01 FB 16
接收 = 68 74 A1 0E 01 03 8F 16:68 74 A1 0D 02:68 74 A1 0C 01 01 8B 16
参数配置 = AT1&68&AT1:2:3&3:LH:HEX:0:5:5|1
;参数配置 =解析方式：每个数据项占用字节的长度:小数位:字节序:编码方式:符号:延时时间:超时时间|显示结果


[底板初始化]
端口选择 = 底板串口:底板串口
发送 =68 74 11 00 02 FF 00 EE 16<HEX>:68 74 11 00 02 00 00 EF 16<HEX>
接收 = 68 74 A1 00 02 FF 00 7E 16:68 74 A1 00 02 00 00 7F 16
参数配置 = AT1:0:0:LH:H2S:0:0:5|1
;参数配置 =解析方式：每个数据项占用字节的长度:小数位:字节序:编码方式:符号:延时时间:超时时间|显示结果
*/
/**
 * @brief 解析ini文件，将每个测试项的内容保存到 QVector<QStringList> 中
 * @param fileName ini文件的路径
 * @return QVector<QStringList> 每个测试项的内容
 */
QVector<QStringList> analysis_ini(const QString fileName)
{
    QVector<QStringList> testItemList; // 每个测试项的内容
    QFile file(fileName);              // 相对路径、绝对路径、资源路径都行
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "文件打开失败";
        return testItemList;
    }
    QTextStream in(&file);                        // 指定打开方式
    in.setCodec(QTextCodec::codecForName("GBK")); // 设置中文编码
    QStringList testItem;                         // 每个测试项的内容
    while (!in.atEnd())
    {
        QString line = in.readLine();
        line = line.trimmed();                          // 去掉两端的空白字符
        if (line.startsWith("[") && line.endsWith("]")) // 判断是否为测试项
        {
            if (testItem.size() > 0) // 如果不是第一个测试项，则将上一个测试项保存起来
            {
                testItemList.append(testItem);
                testItem.clear();
            }
        }
        if (line.startsWith(";") || line == "") // 如果是注释、空行，则跳过
            continue;
        testItem.append(line);
    }
    testItemList.append(testItem);
    file.close();

    // 测试输出
    for (int i = 0; i < testItemList.size(); i++)
    {
        QStringList testItem = testItemList.at(i);
        qDebug() << "第" << i + 1 << "个测试项：";
        for (int j = 0; j < testItem.size(); j++)
        {
            qDebug() << testItem.at(j);
        }
        qDebug() << "-----------------------";
    }
    return testItemList;
}
