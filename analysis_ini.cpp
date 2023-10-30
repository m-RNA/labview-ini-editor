/*
 * @Author: 陈俊健
 * @Date: 2023-10-29 13:18:19
 * @LastEditors: 陈俊健
 * @LastEditTime: 2023-10-30 02:43:57
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
QVector<QStringList> analysis_ini_to_QStringList(const QString fileName)
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

/**
 * @brief 将字符串按照无视方括号[]内容分割
 * @param input 输入字符串
 * @param separator 分隔符
 * @return QStringList 分割后的字符串列表
 */
QStringList splitStringSquareBrackets(const QString &input, char separator)
{
    QStringList commandList;           // 每个命令
    QString cmd;                       // 当前命令
    bool insideSquareBrackets = false; // 是否在方括号中

    if (input.contains(separator) == false)
        return QStringList(input);

    for (const QChar &character : input)
    {
        if (character == separator && insideSquareBrackets == false) // 正常每次遇到 : 就是一个命令的结束
        {
            if (cmd.isEmpty())
                continue;
            // 当前命令不为空
            commandList.append(cmd); // 将当前命令添加到 commandList 中
            cmd.clear();             // 清空当前命令
        }
        else if (character == '[') // 取方括号中的内容，不包括 [ 和 ]
        {
            insideSquareBrackets = true;
        }
        else if (character == ']' && insideSquareBrackets) // 取方括号中的内容，不包括 [ 和 ]
        {
            insideSquareBrackets = false;
        }
        else
        {
            cmd.append(character); // 将字符添加到当前命令中
        }
    }

    if (cmd.isEmpty() == false)
    {
        commandList.append(cmd);
    }

    return commandList;
}

void printTestItem(const TestItem &ti)
{
    qDebug() << "测试名：" << ti.name;
    qDebug() << "重复次数" << ti.repeat;
    for (const auto &cmd : ti.cmdList)
    {
        qDebug() << cmd.index << cmd.brief << cmd.comName;
        qDebug() << "发送" << cmd.tx << "接收" << cmd.rx;

        qDebug() << "解析方式" << cmd.cmdType << "字节数" << cmd.dataByteLen << "小数" << cmd.decimal << "字节序"
                 << cmd.byteOrder << "编码方式" << cmd.encodeWay << "符号" << cmd.sign;
        qDebug() << "延时" << cmd.cmdDelay << "超时" << cmd.cmdTimeout << "显示结果" << cmd.resultShow;
        qDebug() << "解析" << cmd.rxAnalysis;
    }
    qDebug() << "-----------------------";
}

int qStringListIndexOf(const QStringList &list, const QString &str)
{
    for (int i = 0; i < list.size(); i++)
    {
        if (list.at(i).trimmed().startsWith(str))
            return i;
    }
    qDebug() << "未找到" << str << "这一行";
    return -1;
}

/**
 * @brief 解析单个测试项，将每个命令截取出来，放于 QVector<TestCmd> 中，并返回单个测试项的内容
 * @param testItem 单个测试项的内容
 * @return TestItem 单个测试项的内容
 */
TestItem analysis_StringToTestItem(const QStringList testItem)
{
    TestItem testItemObj;
    testItemObj.name = testItem.at(0).mid(1, testItem.at(0).size() - 2); // 测试项名称
    // 首先要判断单个测试项的命令数目
    // 发送= 这一行中的 : 的个数，再减去被方括号括起来的 : 的个数
    // 寻找 以 发送 开头的行
    int indexTemp = qStringListIndexOf(testItem, "发送");
    if (indexTemp == -1)
        return testItemObj;
    QString qsCmdListOrigin = testItem.at(indexTemp);
    qsCmdListOrigin = qsCmdListOrigin.mid(qsCmdListOrigin.indexOf("=") + 1).trimmed(); // 截取 = 之后的字符串

    QStringList qslCmdList = splitStringSquareBrackets(qsCmdListOrigin, ':');
    testItemObj.cmdNum = qslCmdList.size(); // 测试项命令数量
    qDebug() << "命令数量：" << testItemObj.cmdNum;

    // 将发送命令 写入 QVector<TestCmd> 中
    // 注意分类<HEX>、<\r\n>、68
    for (int i = 0; i < testItemObj.cmdNum; i++)
    {
        TestCmd testCmdObj;
        testCmdObj.index = i + 1;                      // 命令序号
        testCmdObj.comName = testItem.at(1).trimmed(); // 端口选择（底板串口、产品串口、主机串口...）
        testCmdObj.tx = qslCmdList.at(i).trimmed();    // 发送内容
        if (testCmdObj.tx.contains("<HEX>"))
        {
            // testCmdObj.cmdType = "AT<HEX>"; // 命令类型
            // testCmdObj.tx = testCmdObj.tx.mid(0, testCmdObj.tx.size() - 5); // 读取时，先不去掉 <HEX>
        }
        else if (testCmdObj.tx.contains("<\\r\\n>"))
        {
            // testCmdObj.cmdType = "AT<\r\n>"; // 命令类型
            // testCmdObj.tx = testCmdObj.tx.mid(0, testCmdObj.tx.size() - 6); // 读取时，先不去掉 <\r\n>
        }
        else if (testCmdObj.tx.contains("68"))
        {
            testCmdObj.cmdType = "68"; // 命令类型
        }
        testItemObj.cmdList.append(testCmdObj);
    }

    indexTemp = qStringListIndexOf(testItem, "接收");
    if (indexTemp == -1)
        return testItemObj;
    qsCmdListOrigin = testItem.at(indexTemp);
    qsCmdListOrigin = qsCmdListOrigin.mid(qsCmdListOrigin.indexOf("=") + 1).trimmed(); // 截取 = 之后的字符串
    qslCmdList = splitStringSquareBrackets(qsCmdListOrigin, ':');
    qDebug() << "接收数量：" << qslCmdList.size();
    for (int i = 0; i < qslCmdList.size() && i < testItemObj.cmdNum; i++)
    {
        testItemObj.cmdList[i].rx = qslCmdList.at(i).trimmed(); // 接收内容
    }
    if (qslCmdList.size() < testItemObj.cmdNum) // 省略写法，后面的参数会继承上一个的数值
    {
        for (int i = qslCmdList.size(); i < testItemObj.cmdNum; i++)
        {
            testItemObj.cmdList[i].rx = testItemObj.cmdList[i - 1].rx;
        }
    }

    indexTemp = qStringListIndexOf(testItem, "端口选择");
    if (indexTemp == -1)
        return testItemObj;
    qsCmdListOrigin = testItem.at(indexTemp);
    qsCmdListOrigin = qsCmdListOrigin.mid(qsCmdListOrigin.indexOf("=") + 1).trimmed(); // 截取 = 之后的字符串
    qslCmdList = splitStringSquareBrackets(qsCmdListOrigin, ':');
    qDebug() << "端口选择数量：" << qslCmdList.size();
    for (int i = 0; i < qslCmdList.size() && i < testItemObj.cmdNum; i++)
    {
        testItemObj.cmdList[i].comName = qslCmdList.at(i).trimmed();
    }
    if (qslCmdList.size() < testItemObj.cmdNum) // 省略写法，后面的参数会继承上一个的数值
    {
        for (int i = qslCmdList.size(); i < testItemObj.cmdNum; i++)
        {
            testItemObj.cmdList[i].comName = testItemObj.cmdList[i - 1].comName;
        }
    }

    // 参数配置 =解析方式 ：每个数据项占用字节的长度:小数位:字节序:编码方式:符号:延时时间:超时时间|显示结果
    indexTemp = qStringListIndexOf(testItem, "参数配置");
    if (indexTemp == -1)
        return testItemObj;
    qsCmdListOrigin = testItem.at(indexTemp);
    qsCmdListOrigin = qsCmdListOrigin.mid(qsCmdListOrigin.indexOf("=") + 1).trimmed(); // 截取 = 之后的字符串
    qslCmdList = splitStringSquareBrackets(qsCmdListOrigin, ':');

    if (qslCmdList.size() < 8) // 参数不全
    {
        qDebug() << "参数不全";
        return testItemObj;
    }

    // 解析方式 = AT1&AT1&AT1&68&AT1&AT1
    QStringList qslParamList = splitStringSquareBrackets(qslCmdList[0], '&');
    qDebug() << "解析方式数量：" << qslParamList.size();
    for (int i = 0; i < qslParamList.size() && i < testItemObj.cmdNum; i++)
    {
        if (qslParamList.at(i).trimmed() == "AT1")
            testItemObj.cmdList[i].cmdType = "AT<HEX>";
        else if (qslParamList.at(i).trimmed() == "AT" && testItemObj.cmdList[i].tx.endsWith("<\\r\\n>"))
            testItemObj.cmdList[i].cmdType = "AT<\\r\\n>";
        else
            testItemObj.cmdList[i].cmdType = qslParamList.at(i).trimmed(); // 解析方式
    }
    if (qslParamList.size() < testItemObj.cmdNum) // 省略写法，后面的参数会继承上一个的数值
    {
        for (int i = qslParamList.size(); i < testItemObj.cmdNum; i++)
        {
            testItemObj.cmdList[i].cmdType = testItemObj.cmdList[i - 1].cmdType;
        }
    }

    // 数据项占用字节 int
    qslParamList = splitStringSquareBrackets(qslCmdList[1], '&');
    qDebug() << "数据项占用字节数量：" << qslParamList.size();
    for (int i = 0; i < qslParamList.size() && i < testItemObj.cmdNum; i++)
    {
        testItemObj.cmdList[i].dataByteLen = qslParamList.at(i).trimmed().toInt();
    }
    if (qslParamList.size() < testItemObj.cmdNum) // 省略写法，后面的参数会继承上一个的数值
    {
        for (int i = qslParamList.size(); i < testItemObj.cmdNum; i++)
        {
            testItemObj.cmdList[i].dataByteLen = testItemObj.cmdList[i - 1].dataByteLen;
        }
    }

    // 小数位 int
    qslParamList = splitStringSquareBrackets(qslCmdList[2], '&');
    qDebug() << "小数位数量：" << qslParamList.size();
    for (int i = 0; i < qslParamList.size() && i < testItemObj.cmdNum; i++)
    {
        testItemObj.cmdList[i].decimal = qslParamList.at(i).trimmed().toInt();
    }
    if (qslParamList.size() < testItemObj.cmdNum) // 省略写法，后面的参数会继承上一个的数值
    {
        for (int i = qslParamList.size(); i < testItemObj.cmdNum; i++)
        {
            testItemObj.cmdList[i].decimal = testItemObj.cmdList[i - 1].decimal;
        }
    }

    // 字节序 = LH
    qslParamList = splitStringSquareBrackets(qslCmdList[3], '&');
    qDebug() << "字节序数量：" << qslParamList.size();
    for (int i = 0; i < qslParamList.size() && i < testItemObj.cmdNum; i++)
    {
        testItemObj.cmdList[i].byteOrder = qslParamList.at(i).trimmed();
    }
    if (qslParamList.size() < testItemObj.cmdNum) // 省略写法，后面的参数会继承上一个的数值
    {
        for (int i = qslParamList.size(); i < testItemObj.cmdNum; i++)
        {
            testItemObj.cmdList[i].byteOrder = testItemObj.cmdList[i - 1].byteOrder;
        }
    }

    // 编码方式 = HEX
    qslParamList = splitStringSquareBrackets(qslCmdList[4], '&');
    qDebug() << "编码方式数量：" << qslParamList.size();
    for (int i = 0; i < qslParamList.size() && i < testItemObj.cmdNum; i++)
    {
        testItemObj.cmdList[i].encodeWay = qslParamList.at(i).trimmed();
    }
    if (qslParamList.size() < testItemObj.cmdNum) // 省略写法，后面的参数会继承上一个的数值
    {
        for (int i = qslParamList.size(); i < testItemObj.cmdNum; i++)
        {
            testItemObj.cmdList[i].encodeWay = testItemObj.cmdList[i - 1].encodeWay;
        }
    }

    // 符号 = 0
    qslParamList = splitStringSquareBrackets(qslCmdList[5], '&');
    qDebug() << "符号数量：" << qslParamList.size();
    for (int i = 0; i < qslParamList.size() && i < testItemObj.cmdNum; i++)
    {
        testItemObj.cmdList[i].sign = qslParamList.at(i).trimmed();
    }
    if (qslParamList.size() < testItemObj.cmdNum) // 省略写法，后面的参数会继承上一个的数值
    {
        for (int i = qslParamList.size(); i < testItemObj.cmdNum; i++)
        {
            testItemObj.cmdList[i].sign = testItemObj.cmdList[i - 1].sign;
        }
    }

    // 延时时间 = 0 int
    qslParamList = splitStringSquareBrackets(qslCmdList[6], '&');
    qDebug() << "延时时间数量：" << qslParamList.size();
    for (int i = 0; i < qslParamList.size() && i < testItemObj.cmdNum; i++)
    {
        testItemObj.cmdList[i].cmdDelay = qslParamList.at(i).trimmed().toInt();
    }
    if (qslParamList.size() < testItemObj.cmdNum) // 省略写法，后面的参数会继承上一个的数值
    {
        for (int i = qslParamList.size(); i < testItemObj.cmdNum; i++)
        {
            testItemObj.cmdList[i].cmdDelay = testItemObj.cmdList[i - 1].cmdDelay;
        }
    }

    // qslCmdList[7] = 超时时间 | 显示结果
    qslParamList = splitStringSquareBrackets(qslCmdList[7], '|');

    if (qslParamList.size() != 2)
        return testItemObj;

    // qslParamList[0] = 超时时间
    QStringList qslParamList_End = splitStringSquareBrackets(qslParamList[0], '&');
    qDebug() << "超时时间数量：" << qslParamList_End.size();
    for (int i = 0; i < qslParamList_End.size() && i < testItemObj.cmdNum; i++)
    {
        testItemObj.cmdList[i].cmdTimeout = qslParamList_End.at(i).trimmed().toInt();
    }
    if (qslParamList_End.size() < testItemObj.cmdNum) // 省略写法，后面的参数会继承上一个的数值
    {
        for (int i = qslParamList_End.size(); i < testItemObj.cmdNum; i++)
        {
            testItemObj.cmdList[i].cmdTimeout = testItemObj.cmdList[i - 1].cmdTimeout;
        }
    }

    // qslParamList[1] = 显示结果
    qslParamList_End = splitStringSquareBrackets(qslParamList[1], '&');
    qDebug() << "显示结果数量：" << qslParamList_End.size();
    for (int i = 0; i < qslParamList_End.size() && i < testItemObj.cmdNum; i++)
    {
        int index = -1;
        // 截取 < 之前的字符串
        if (qslParamList_End.at(i).contains("<"))
        {
            index = qslParamList_End.at(i).mid(0, qslParamList_End.at(i).indexOf("<")).trimmed().toInt();
        }
        else
        {
            index = qslParamList_End.at(i).trimmed().toInt();
        }
        if (index != -1)
        {
            testItemObj.cmdList[index].resultShow = qslParamList_End.at(i).trimmed();
            index = -1;
        }
    }

    // 解析
    indexTemp = qStringListIndexOf(testItem, "解析");
    if (indexTemp != -1)
    {
        qsCmdListOrigin = testItem.at(indexTemp);
        qsCmdListOrigin = qsCmdListOrigin.mid(qsCmdListOrigin.indexOf("=") + 1).trimmed();
        qslCmdList = splitStringSquareBrackets(qsCmdListOrigin, ':');
        qDebug() << "显示结果数量：" << qslCmdList.size();
        for (int i = 0; i < qslCmdList.size() && i < testItemObj.cmdNum; i++)
        {
            testItemObj.cmdList[i].rxAnalysis = qslCmdList.at(i).trimmed();
        }
    }

    // 功能配置=重发次数(45)
    indexTemp = qStringListIndexOf(testItem, "功能配置");
    if (indexTemp != -1)
    {
        qsCmdListOrigin = testItem.at(indexTemp);
        qsCmdListOrigin = qsCmdListOrigin.mid(qsCmdListOrigin.indexOf("=") + 1).trimmed();

        if (qsCmdListOrigin.contains("重发次数"))
        {
            // 截取 () 中的数据
            qsCmdListOrigin = qsCmdListOrigin.mid(qsCmdListOrigin.indexOf("(") + 1);
            qsCmdListOrigin = qsCmdListOrigin.mid(0, qsCmdListOrigin.indexOf(")"));
            testItemObj.repeat = qsCmdListOrigin.toInt();
        }
    }

    printTestItem(testItemObj);

    return testItemObj;
}

QVector<TestItem> analysis_ini(const QString &pathFileName)
{
    QVector<QStringList> qslTestItemList = analysis_ini_to_QStringList(pathFileName); // 解析 ini 文件
    QVector<TestItem> testItemList = {};
    for (const auto &item : qslTestItemList)
    {
        testItemList.append(analysis_StringToTestItem(item));
    }
    return testItemList;
}