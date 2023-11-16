/*
 * @Author: 陈俊健
 * @Date: 2023-10-29 13:18:19
 * @LastEditors: 陈俊健
 * @LastEditTime: 2023-10-31 22:16:01
 * @FilePath: \LabViewIniEditor\analysis_ini.cpp
 * @Description:
 *
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved.
 */
#include "analysis_ini.h"
#include <QDebug>
#include <QFile>
#include <QTextCodec>
#include <QTextStream>

const QStringList STR_TEST_TYPE = {
    "AT", "AT1", "68", "串口查询真", "串口查询假", "单按钮弹框", "双按钮弹框",
};

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
参数配置 = AT&AT:1:0:LH:HEX:0:0:1|0<_空白>&1<_空白>
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
            testItem.append(line); // 将测试项名称保存起来
            continue;
        }
        if (line.startsWith(";") || line == "") // 如果是注释、空行，则跳过
            continue;
        if (line.contains("="))
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

QVector<int> splitStringSquareBracketsToInt(const QString &input, char separator, int size)
{
    QVector<int> qvInt = {};
    QStringList qslParamList = splitStringSquareBrackets(input, separator);
    for (int i = 0; i < qslParamList.size(); i++) // 解析出 序列 到 qvInt 中
    {
        qvInt.append(qslParamList.at(i).trimmed().toInt());
    }
    for (int i = qslParamList.size(); i <= size; i++) // 省略写法，后面的参数会继承上一个的数值
    {
        int tempInt = qvInt.at(i - 1);
        qvInt.append(tempInt);
    }
    return qvInt;
}

QStringList splitStringSquareBracketsToQString(const QString &input, char separator, int size)
{
    QStringList qsl = {};
    QStringList qslParamList = splitStringSquareBrackets(input, separator);
    for (int i = 0; i < qslParamList.size(); i++) // 解析出 序列 到 qsl 中
    {
        qsl.append(qslParamList.at(i).trimmed());
    }
    for (int i = qslParamList.size(); i <= size; i++) // 省略写法，后面的参数会继承上一个的数值
    {
        qsl.append(qsl.at(i - 1));
    }
    return qsl;
}

void printTestCmd(const TestCmd &tc)
{
    qDebug() << tc.index << tc.brief << tc.comName;
    qDebug() << "发送" << tc.tx << "接收" << tc.rx;

    qDebug() << "解析方式" << tc.cmdType << "编码方式" << tc.encodeWay;
    qDebug() << "延时" << tc.cmdDelay << "超时" << tc.cmdTimeout;
}

void printTestResult(const TestResult &tr)
{
    qDebug() << tr.index << tr.show;

    qDebug() << "字节数" << tr.dataByteLen << "小数" << tr.decimal << "字节序" << tr.byteOrder << "符号" << tr.sign;

    qDebug() << "解析" << tr.analysisWay << tr.analysisContent;
}

void printTestItem(const TestItem &ti)
{
    qDebug() << "测试名：" << ti.name;
    qDebug() << "重复次数" << ti.repeat;
    for (const auto &cmd : ti.cmdList)
    {
        printTestCmd(cmd);
    }

    for (const auto &result : ti.resultList)
    {
        printTestResult(result);
    }
    qDebug() << "-----------------------";
}

void printConfigItem(const ConfigItem &ci)
{
    qDebug() << "配置名：" << ci.name;
    qDebug() << "序号" << ci.index << "启用" << ci.enable;
    for (auto cc : ci.contentList)
    {
        qDebug() << cc.name << "规格" << cc.value << "单位" << cc.unit;
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
    qDebug() << "未找到" << str << "行";
    return -1;
}

int qStringListVectorHeadIndexOf(const QVector<QStringList> &list, const QString &str)
{
    for (int i = 0; i < list.size(); i++)
    {
        if (list.at(i).at(0).trimmed().startsWith(str))
            return i;
    }
    qDebug() << "未找到" << str << "项";
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
            // testCmdObj.cmdType = "68"; // 命令类型
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
        testItemObj.cmdList[i].cmdType = qslParamList.at(i).trimmed(); // 解析方式
    }
    if (qslParamList.size() < testItemObj.cmdNum) // 省略写法，后面的参数会继承上一个的数值
    {
        for (int i = qslParamList.size(); i < testItemObj.cmdNum; i++)
        {
            testItemObj.cmdList[i].cmdType = testItemObj.cmdList[i - 1].cmdType;
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
    testItemObj.resultNum = qslParamList_End.size(); // 测试项结果数量
    qDebug() << "显示结果数量：" << testItemObj.resultNum;
    for (int i = 0; i < testItemObj.resultNum; i++)
    {
        TestResult testResultObj;
        if (qslParamList_End.at(i).contains("<"))
        {
            // 截取 < 之前的字符串
            testResultObj.index = qslParamList_End.at(i).mid(0, qslParamList_End.at(i).indexOf("<")).toInt();
            // 截取<>之间的字符串,包含<>
            testResultObj.show = qslParamList_End.at(i).mid(qslParamList_End.at(i).indexOf("<")).trimmed();
        }
        else
        {
            testResultObj.index = qslParamList_End.at(i).toInt();
        }

        testItemObj.resultList.append(testResultObj);

        // 记录最大的 index
        if (testItemObj.resultIndexMax < testResultObj.index)
        {
            testItemObj.resultIndexMax = testResultObj.index;
        }
    }

    // 数据项占用字节 int
    QVector<int> qvInt = splitStringSquareBracketsToInt(qslCmdList[1], '&', testItemObj.resultIndexMax);
    qDebug() << "数据项占用字节";
    for (int i = 0; i < testItemObj.resultNum; i++)
    {
        testItemObj.resultList[i].dataByteLen = qvInt.at(testItemObj.resultList[i].index);
        qDebug() << testItemObj.resultList[i].index << testItemObj.resultList[i].dataByteLen; // 调试打印
    }

    // 小数位 int
    qvInt.clear();
    qvInt = splitStringSquareBracketsToInt(qslCmdList[2], '&', testItemObj.resultIndexMax);
    qDebug() << "小数位";
    for (int i = 0; i < testItemObj.resultNum; i++)
    {
        testItemObj.resultList[i].decimal = qvInt.at(testItemObj.resultList[i].index);
        qDebug() << testItemObj.resultList[i].index << testItemObj.resultList[i].decimal; // 调试打印
    }

    // // 字节序 = LH
    qslParamList = splitStringSquareBracketsToQString(qslCmdList[3], '&', testItemObj.resultIndexMax);
    qDebug() << "字节序数量";
    for (int i = 0; i < testItemObj.resultNum; i++)
    {
        testItemObj.resultList[i].byteOrder = qslParamList.at(testItemObj.resultList[i].index);
        qDebug() << testItemObj.resultList[i].index << testItemObj.resultList[i].byteOrder; // 调试打印
    }

    // // 符号 = 0
    qslParamList = splitStringSquareBracketsToQString(qslCmdList[5], '&', testItemObj.resultIndexMax);
    qDebug() << "符号";
    for (int i = 0; i < testItemObj.resultNum; i++)
    {
        testItemObj.resultList[i].sign = qslParamList.at(testItemObj.resultList[i].index);
        qDebug() << testItemObj.resultList[i].index << testItemObj.resultList[i].sign; // 调试打印
    }

    // 解析=双匹配&LADC[:]1[,]&OK:双匹配&LADC[: ]1[,]&OK
    indexTemp = qStringListIndexOf(testItem, "解析");
    if (indexTemp != -1)
    {
        qsCmdListOrigin = testItem.at(indexTemp);
        qsCmdListOrigin = qsCmdListOrigin.mid(qsCmdListOrigin.indexOf("=") + 1).trimmed();

        // qsCmdListOrigin =
        // 双匹配&LADC[:]1[,]&OK:双匹配&LADC[: ]1[,]&OK
        qslCmdList = splitStringSquareBrackets(qsCmdListOrigin, ':');
        qDebug() << "显示结果";
        // qslCmdList =
        // 双匹配&LADC[:]1[,]&OK
        // 双匹配&LADC[:]1[,]&OK
        for (int i = 0; i < testItemObj.resultNum; i++)
        {
            // 截取第一个 & 前的内容， 放到 analysisWay
            testItemObj.resultList[i].analysisWay = qslCmdList.at(i).mid(0, qslCmdList.at(i).indexOf("&")).trimmed();
            // 截取第一个 & 后的内容， 放到 analysisContent
            testItemObj.resultList[i].analysisContent
                = qslCmdList.at(i).mid(qslCmdList.at(i).indexOf("&") + 1).trimmed();
            qDebug() << testItemObj.resultList[i].index << testItemObj.resultList[i].analysisWay
                     << testItemObj.resultList[i].analysisContent; // 调试打印
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

QVector<TestItem> analysis_protocol_ini(const QString &pathFileProtocol)
{
    QVector<QStringList> qslTestItemList = analysis_ini_to_QStringList(pathFileProtocol); // 解析 ini 文件
    QVector<TestItem> testItemPool = {};
    for (const auto &item : qslTestItemList)
    {
        testItemPool.append(analysis_StringToTestItem(item));
    }
    return testItemPool;
}
// 获取 = 之后的字符串
QString getAfterEqual(const QString &input) { return input.mid(input.indexOf("=") + 1).trimmed(); }
// 获取 = 之前的字符串
QString getBeforeEqual(const QString &input) { return input.mid(0, input.indexOf("=")).trimmed(); }
// 是否包含 = 字符
bool isContainEqual(const QString &input) { return input.contains("="); }

QVector<ConfigItem> analysis_config_ini(const QString &pathFile)
{
    QVector<QStringList> vqslList = analysis_ini_to_QStringList(pathFile); // 解析 ini 文件
    QVector<ConfigItem> configItemList = {};

    /*
    [UNIT] ;限定单位列表
    休眠电流 = mA ; 限定名称 = 限定单位
    ADC0 = mV ; 限定名称 = 限定单位
    ADC1 = mV ; 限定名称 = 限定单位
    */
    int index = qStringListVectorHeadIndexOf(vqslList, "[UNIT]");
    if (index == -1)
        return configItemList;
    QStringList qslUnit = vqslList.at(index);
    qslUnit.removeFirst(); // 移除第一行
    qDebug() << "限定单位列表" << qslUnit;

    /**
    [Test Item]
    底板初始化=0 ; 测试项名称 = 是否启用
    */
    index = qStringListVectorHeadIndexOf(vqslList, "[Test Item]");
    if (index == -1)
        return configItemList;
    QStringList qslTestItem = vqslList.at(index);
    qslTestItem.removeFirst(); // 移除第一行

    for (int i = 0; i < qslTestItem.size(); i++)
    {
        if (qslTestItem.at(i).contains("=") == false)
            continue;

        /**
        [模组上电] ;测试项名称
        模组上电=OK ; 限定名称 = 限定值

        [ADC0and1] ;测试项名称
        ADC0 = [800,1000] ; 限定名称 = [限定值1,限定值2]
        ADC1 = [800,1000] ; 限定名称 = [限定值1,限定值2]
        */
        ConfigItem configItem;
        configItem.index = i;
        configItem.name = getBeforeEqual(qslTestItem.at(i));
        configItem.enable = getAfterEqual(qslTestItem.at(i)).toInt();

        index = qStringListVectorHeadIndexOf(vqslList, "[" + configItem.name + "]");
        if (index != -1)
        {
            for (int k = 1; k < vqslList.at(index).size(); ++k)
            {
                ConfigContent cc;
                cc.name = getBeforeEqual(vqslList.at(index).at(k));
                cc.value = getAfterEqual(vqslList.at(index).at(k));
                for (QString unit : qslUnit)
                {
                    QString temp = getBeforeEqual(unit);
                    if (temp == cc.name)
                    {
                        cc.unit = getAfterEqual(unit);
                        break;
                    }
                }
                configItem.contentList.append(cc);
            }
        }
        printConfigItem(configItem);
        configItemList.append(configItem);
    }
    return configItemList;
}
