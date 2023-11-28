/*
 * @Author: 陈俊健
 * @Date: 2023-11-18 21:46:11
 * @LastEditors: 陈俊健
 * @LastEditTime: 2023-11-29 00:19:33
 * @FilePath: \LabViewIniEditor\labviewsetting.cpp
 * @Description: 
 * 
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved. 
 */
#include "labviewsetting.h"
#include <QDebug>
#if _MSC_VER >= 1600 // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif
/**
 * @brief 打印日志消息。
 * @param str 要打印的日志消息。
 */
void logPrint(const QString &str) { qDebug() << str; }

// 补齐长度函数
void fillLength(QStringList &list, int length)
{
    if (list.size() < length)
    {
        for (int i = list.size(); i < length; i++)
        {
            list.append(list.at(list.size() - 1));
        }
    }
}

/**
 * @brief 使用指定的文件名构造一个 LabViewSetting 对象。
 * @param fileNameProtocol 协议文件的文件名。
 * @param fileNameConfig 配置文件的文件名。
 */
LabViewSetting::LabViewSetting(QString fileNameProtocol, QString fileNameConfig)
{
    this->fileNameProtocol = fileNameProtocol;
    this->fileNameConfig = fileNameConfig;

    iniSettingsProtocol = new IniSettings(fileNameProtocol, QTextCodec::codecForName("GB2312"));
    iniSettingsConfig = new IniSettings(fileNameConfig, QTextCodec::codecForName("GB2312"));

    if (iniSettingsProtocol->isLoad() == false)
    {
        logPrint("协议文件加载失败");
    }
    if (iniSettingsConfig->isLoad() == false)
    {
        logPrint("配置文件加载失败");
    }

    analysisTestItem();
    analysisConfigItem();
}

/**
 * @brief 销毁 LabViewSetting 对象。
 */
LabViewSetting::~LabViewSetting()
{
    delete iniSettingsProtocol;
    delete iniSettingsConfig;
}

/**
 * @brief 清空测试项列表和配置项列表。
 */
void LabViewSetting::clear()
{
    testItemList.clear();
    configItemList.clear();
}

/**
 * @brief 获取测试项列表。
 * @return 测试项列表。
 */
QList<TestItem> LabViewSetting::getTestItemList() const { return testItemList; }

/**
 * @brief 获取配置项列表。
 * @return 配置项列表。
 */
QList<ConfigItem> LabViewSetting::getConfigItemList() const { return configItemList; }

/**
 * @brief 分析测试项。
 */
void LabViewSetting::analysisTestItem()
{
    testItemList.clear();
    QStringList testItemNameList = {};
    if (iniSettingsConfig->isLoad()) // 获取测试项名称列表
    {
        iniSettingsConfig->beginGroup("Test Item");
        testItemNameList = iniSettingsConfig->allKeys();
        iniSettingsConfig->endGroup();
        foreach (QString testItemName, testItemNameList)
        {
            TestItem testItem = getTestItem(testItemName);
            if (testItem.name != "")
            {
                testItemList.append(testItem);
            }
        }
    }
}

/**
 * @brief 分析配置项。
 */
void LabViewSetting::analysisConfigItem()
{
    const QString TEST_ITEM_GROUP = "Test Item";
    const QString UNIT_GROUP = "UNIT";

    configItemList.clear();
    if (iniSettingsConfig->isLoad()) // 获取配置项名称列表
    {
        iniSettingsConfig->beginGroup(TEST_ITEM_GROUP);
        QStringList testItemNameList = iniSettingsConfig->allKeys(); // 获取测试项名称列表

        iniSettingsConfig->beginGroup(UNIT_GROUP);
        QStringList unitList = iniSettingsConfig->allKeys(); // 获取单位列表

        foreach (QString testItemName, testItemNameList) // 遍历测试项名称列表
        {
            /**
            [Test Item]
            模组上电=1 ; 测试项名称 = 是否启用
            ADC0and1=0 ; 测试项名称 = 是否启用
            */
            iniSettingsConfig->beginGroup(TEST_ITEM_GROUP);
            ConfigItem configItem;
            configItem.name = testItemName;                                     // 测试项名称
            configItem.enable = iniSettingsConfig->value(testItemName).toInt(); // 是否启用
            iniSettingsConfig->endGroup();

            /**
            [模组上电] ;测试项名称
            模组上电=OK ; 限定名称 = 限定值

            [ADC0and1] ;测试项名称
            ADC0 = [800,1000] ; 限定名称 = [限定值1,限定值2]
            ADC1 = [800,1000] ; 限定名称 = [限定值1,限定值2]
            */
            iniSettingsConfig->beginGroup(testItemName);
            QStringList configContentList = iniSettingsConfig->allKeys(); // 获取配置项内容列表

            foreach (QString configContent, configContentList)
            {
                iniSettingsConfig->beginGroup(testItemName);
                ConfigContent cc;
                cc.name = configContent; // 限定名称
                cc.value = iniSettingsConfig->value(configContent);
                foreach (QString unit, unitList)
                {
                    iniSettingsConfig->beginGroup(UNIT_GROUP);
                    if (unit == cc.name)
                    {
                        cc.unit = iniSettingsConfig->value(unit);
                        break;
                    }
                }
                configItem.contentList.append(cc);
            }
            configItemList.append(configItem);
        }
    }
}

/**
 * @brief 使用指定的名称分析测试项。
 * @param testItemName 要分析的测试项的名称。
 * @return 分析后的测试项。
 */
TestItem LabViewSetting::getTestItem(const QString &testItemName)
{
    TestItem testItem;
    testItem.name = testItemName;
    testItem.cmdList = {};
    if (iniSettingsProtocol->childGroups().contains(testItemName) == false) // 测试项名称不存在
    {
        return testItem;
    }

    iniSettingsProtocol->beginGroup(testItemName);
    QString port = iniSettingsProtocol->value("端口选择");
    QString sent = iniSettingsProtocol->value("发送");
    QString receive = iniSettingsProtocol->value("接收");
    QString param = iniSettingsProtocol->value("参数配置");
    QString analysis = iniSettingsProtocol->value("解析");
    QString function = iniSettingsProtocol->value("功能配置");
    qDebug() << function;
    iniSettingsProtocol->endGroup();

    QStringList portList = splitStringSquareBrackets(port, ':');
    QStringList sentList = splitStringSquareBrackets(sent, ':');
    QStringList receiveList = splitStringSquareBrackets(receive, ':');
    QStringList paramList = splitStringSquareBrackets(param, ':');
    QStringList analysisContentList = splitStringSquareBrackets(analysis, ':');
    if (paramList.size() < 8)
    {
        logPrint(testItemName + " 参数配置:参数不全");
        return testItem;
    }
    QStringList paramListEnd = splitStringSquareBrackets(paramList.at(7), '|');
    if (paramListEnd.size() < 2)
    {
        logPrint(testItemName + " 超时时间|显示结果:参数不全");
        return testItem;
    }
    QStringList cmdTypeList = splitStringSquareBrackets(paramList.at(0), '&');
    QStringList dataByteLenList = splitStringSquareBrackets(paramList.at(1), '&');
    QStringList decimalList = splitStringSquareBrackets(paramList.at(2), '&');
    QStringList byteOrderList = splitStringSquareBrackets(paramList.at(3), '&');
    QStringList encodeWayList = splitStringSquareBrackets(paramList.at(4), '&');
    QStringList signList = splitStringSquareBrackets(paramList.at(5), '&');
    QStringList delayList = splitStringSquareBrackets(paramList.at(6), '&');
    QStringList timeoutList = splitStringSquareBrackets(paramListEnd.at(0), '&');
    QStringList resultShowList = splitStringSquareBrackets(paramListEnd.at(1), '&');

    int cmdNum = sentList.size();
    testItem.cmdNum = cmdNum;
    // 填充长度
    fillLength(portList, cmdNum);      // 端口选择
    fillLength(receiveList, cmdNum);   // 接收
    fillLength(cmdTypeList, cmdNum);   // 解析方式
    fillLength(encodeWayList, cmdNum); // 编码方式
    fillLength(delayList, cmdNum);     // 延时时间
    fillLength(timeoutList, cmdNum);   // 超时时间

    for (int i = 0; i < cmdNum; i++)
    {
        TestCmd testCmd;
        testCmd.index = i;
        testCmd.comName = portList.at(i).trimmed();               // 端口选择
        testCmd.tx = sentList.at(i).trimmed();                    // 发送
        testCmd.rx = receiveList.at(i).trimmed();                 // 接收
        testCmd.cmdType = cmdTypeList.at(i).trimmed();            // 解析方式
        testCmd.encodeWay = encodeWayList.at(i).trimmed();        // 编码方式
        testCmd.cmdDelay = delayList.at(i).trimmed().toInt();     // 延时时间
        testCmd.cmdTimeout = timeoutList.at(i).trimmed().toInt(); // 超时时间
        testItem.cmdList.append(testCmd);
    }

    int resultNum = resultShowList.size();
    testItem.resultNum = resultNum;
    for (int i = 0; i < resultNum; i++)
    {
        TestResult testResult;
        if (resultShowList.at(i).contains("<"))
        {
            // 截取 < 之前的字符串
            testResult.index = resultShowList.at(i).mid(0, resultShowList.at(i).indexOf("<")).toInt();
            // 截取<>之间的字符串,包含<>
            testResult.show = resultShowList.at(i).mid(resultShowList.at(i).indexOf("<")).trimmed();
        }
        else
        {
            testResult.index = resultShowList.at(i).toInt();
            if (testResult.index >= cmdNum)
            {
                logPrint(testItemName + " 解析:结果数量大于命令数量");
                testResult.index = cmdNum - 1;
            }
        }

        testItem.resultList.append(testResult);

        // 记录最大的 index
        if (testItem.resultIndexMax < testResult.index)
        {
            testItem.resultIndexMax = testResult.index;
        }
    }
    int resultIndexMax = testItem.resultIndexMax;
    qDebug() << "resultIndexMax:" << resultIndexMax << "resultNum:" << resultNum;
    fillLength(dataByteLenList, resultIndexMax + 1);
    fillLength(decimalList, resultIndexMax + 1);
    fillLength(byteOrderList, resultIndexMax + 1);
    fillLength(signList, resultIndexMax + 1);
    fillLength(analysisContentList, resultIndexMax + 1);

    for (int i = 0; i < resultNum; i++)
    {
        TestResult &testResult = testItem.resultList[i];
        int index = testResult.index;
        testResult.dataByteLen = dataByteLenList.at(index).trimmed().toInt(); // 每个数据项占用字节的长度
        testResult.decimal = decimalList.at(index).trimmed().toInt();         // 小数点位置
        testResult.byteOrder = byteOrderList.at(index).trimmed();             // 字节序 （LH、HL）
        testResult.sign = signList.at(index).trimmed();                       // 符号

        // 解析=双匹配&LADC[: ]1[,]&OK:双匹配&LADC[: ]1[,]&OK
        QStringList anaList = splitStringSquareBrackets(analysisContentList.at(i).trimmed(), '&');

        // 截取第一个 & 前的内容， 放到 analysisWay
        testResult.analysisWay
            = analysisContentList.at(i).trimmed().mid(0, analysisContentList.at(i).trimmed().indexOf("&"));
        // 截取第一个 & 后的内容， 放到 analysisContent
        testResult.analysisContent
            = analysisContentList.at(i).trimmed().mid(analysisContentList.at(i).trimmed().indexOf("&") + 1);
    }
    // 功能配置=重发次数(45)
    if (function.startsWith("重发次数"))
    {
        testItem.repeat
            = function.mid(function.indexOf("(") + 1, function.indexOf(")") - function.indexOf("(") - 1).toInt();
    }

    // testItem.print();

    return testItem;
}

void ConfigItem::print() const
{
    qDebug() << "配置名：" << name;
    qDebug() << "序号" << index << "启用" << enable;
    for (auto cc : contentList)
    {
        qDebug() << cc.name << "规格" << cc.value << "单位" << cc.unit;
    }
}

void TestCmd::print() const
{
    qDebug() << "端口选择" << comName;
    qDebug() << "发送" << tx;
    qDebug() << "接收" << rx;
    qDebug() << "解析方式" << cmdType;
    qDebug() << "编码方式" << encodeWay;
    qDebug() << "延时时间" << cmdDelay;
    qDebug() << "超时时间" << cmdTimeout;
}

void TestResult::print() const
{
    qDebug() << "显示结果" << show;
    qDebug() << "字节数" << dataByteLen;
    qDebug() << "小数" << decimal;
    qDebug() << "字节序" << byteOrder;
    qDebug() << "符号" << sign;
    qDebug() << "解析" << analysisWay << analysisContent;
}

void TestItem::print() const
{
    qDebug() << "测试名：" << name;
    qDebug() << "重复次数" << repeat;
    for (const auto &cmd : cmdList)
    {
        cmd.print();
    }

    for (const auto &result : resultList)
    {
        result.print();
    }
}

/**
 * @brief 将字符串按照无视方括号[]内容分割
 * @param input 输入字符串
 * @param separator 分隔符
 * @return QStringList 分割后的字符串列表
 * @note 算法思路：
 * @note 1、遍历字符串中的每一个字符
 * @note 2、如果遇到分隔符，且不在方括号中，则将当前命令添加到 commandList 中
 * @note 3、如果遇到 [，则 insideSquareBrackets 置为 true
 * @note 4、如果遇到 ]，则 insideSquareBrackets 置为 false
 * @note 5、如果遇到其他字符，则将字符添加到当前命令中
 * @note 6、如果当前命令不为空，则将当前命令添加到 commandList 中
 * @note 7、返回 commandList
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
