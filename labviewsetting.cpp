/*
 * @Author: 陈俊健
 * @Date: 2023-11-18 21:46:11
 * @LastEditors: 陈俊健
 * @LastEditTime: 2024-06-21 03:27:42
 * @FilePath: \LabViewIniEditor2024\labviewsetting.cpp
 * @Description:
 *
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved.
 */
#include "labviewsetting.h"
#include <QDebug>
#if _MSC_VER >= 1600 // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif
const QString TEST_ITEM_GROUP = "Test Item";
const QString UNIT_GROUP = "UNIT";

const QStringList keyOrderProtocol = {
    "端口选择", "发送", "接收", "参数配置", "解析", "功能配置",
};

// [测试项名称] [MES链接] [测试开始] [测试结束]
const QStringList passTestItemList = {
    "测试项名称",
    "MES链接",
    "测试开始",
    "测试结束",
};

/**
 * @brief 打印日志消息。
 * @param str 要打印的日志消息。
 */
void logPrint(const QString &str) { qDebug() << str; }

/**
 * @brief 补齐长度函数。
 * @param list 字符串列表。
 * @param length 长度。
 */
void fillLength(QStringList &list, int length)
{
    int sizeOld = list.size();
    QString last = "";
    if (sizeOld < length)
    {
        if (sizeOld > 0)
            last = list.at(list.size() - 1);
        for (int i = list.size(); i < length; i++)
        {
            list.append(last);
        }
    }
}

/**
 * @brief 协议文件是否加载。
 * @return 加载成功返回 true，否则返回 false。
 */
bool LabViewSetting::isLoadProtocol(void) { return iniSettingsProtocol->isLoad(); }

/**
 * @brief 配置文件是否加载。
 * @return 加载成功返回 true，否则返回 false。
 */
bool LabViewSetting::isLoadConfig(void) { return iniSettingsConfig->isLoad(); }

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

    if (isLoadProtocol() == false)
        logPrint("协议文件加载失败");
    else
        iniToTestItemList();

    if (isLoadConfig() == false)
        logPrint("配置文件加载失败");
    else
        iniToConfigItemList();
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
 * @brief 保存文件。
 * @return 保存成功返回 true，否则返回 false。
 */
bool LabViewSetting::saveFile(void)
{
    qDebug() << "保存文件";
    testItemListToIni();
    iniSettingsProtocol->saveFileOrderKey(keyOrderProtocol);
    // iniSettingsConfig->saveFile();
    return true;
}

/**
 * @brief 获取测试项列表。
 * @return 测试项列表。
 */
QList<TestItem> LabViewSetting::getTestItemList() const { return testItemList; }
QList<TestItem> *LabViewSetting::getTestItemListAddr() { return &testItemList; }

/**
 * @brief 获取配置项列表。
 * @return 配置项列表。
 */
QList<ConfigItem> LabViewSetting::getConfigItemList() const { return configItemList; }

QStringList LabViewSetting::getConfigTestItemKey(const QString &name) const
{
    iniSettingsConfig->beginGroup(TEST_ITEM_GROUP);
    if (iniSettingsConfig->childGroups().contains(name) == false)
    {
        iniSettingsConfig->endGroup();
        return QStringList();
    }
    iniSettingsConfig->beginGroup(name);
    QStringList keyList = iniSettingsConfig->childKeysOrder();
    iniSettingsConfig->endGroup();
    return keyList;
}

void LabViewSetting::insertTestItemProtocol(int index, const TestItem &testItem)
{
    if (index < 0 || index > testItemList.size())
    {
        return;
    }
    QString lastName = testItemList.at(index).name;
    testItemList.insert(index, testItem);

    iniSettingsProtocol->endGroup();
    QStringList testItemNameList = iniSettingsProtocol->childGroups();
    for (int i = 0; i < testItemNameList.size(); i++)
    {
        if (testItemNameList.at(i) == lastName)
        {
            index = i;
            break;
        }
    }
    iniSettingsProtocol->insertGroup(index, testItem.name);
}

void LabViewSetting::renameTestItemProtocol(const QString &oldName, const QString &newName)
{
    iniSettingsProtocol->renameGroup(oldName, newName);
    iniToTestItemList();
}

void LabViewSetting::removeTestItemProtocol(const QString &name)
{
    iniSettingsProtocol->removeGroup(name);
    iniToTestItemList();
}

void LabViewSetting::testItemListToIni()
{
    // iniSettingsProtocol->clear();
    for (const auto &testItem : testItemList)
    {
        QString port = "";     // 端口选择
        QString sent = "";     // 发送
        QString receive = "";  // 接收
        QString param = "";    // 参数配置
        QString analysis = ""; // 解析
        QString function = "重发次数(" + QString::number(testItem.repeat) + ")";

        for (auto testCmd : testItem.cmdList)
        {
            port += testCmd.comName;
            if (testCmd.brief.isEmpty() == false)
            {
                port += "<" + testCmd.brief + ">";
            }
            port += ":";
            sent += testCmd.tx + ":";
            receive += testCmd.rx + ":";
            param += testCmd.cmdType + "&"; // 解析方式
        }
        param.chop(1);
        param += ":";
        param += QString::number(testItem.dataByteLen); // 每个数据项占用字节的长度
        param += ":";
        param += QString::number(testItem.decimal); // 小数点位置
        param += ":";
        param += testItem.byteOrder; // 字节序 （LH、HL）
        param += ":";
        param += testItem.encodeWay; // 编码方式
        param += ":";
        param += testItem.sign; // 符号
        param += ":";
        for (auto testCmd : testItem.cmdList)
        {
            param += QString::number(testCmd.cmdDelay) + "&"; // 延时时间
        }
        param.chop(1);
        param += ":";
        for (auto testCmd : testItem.cmdList)
        {
            param += QString::number(testCmd.cmdTimeout) + "&"; // 超时时间
        }
        param.chop(1);
        param += "|";
        for (auto testResult : testItem.resultList)
        {
            param += QString::number(testResult.index) + testResult.show + "&"; // 显示结果
        }

        for (auto testResult : testItem.resultList)
        {
            if (testResult.analysisWay.isEmpty() || testResult.analysisContent.isEmpty())
                analysis += "NA:"; // 解析
            else
                analysis += testResult.analysisWay + "&" + testResult.analysisContent + ":"; // 解析
        }

        // 去掉最后一个符号
        port.chop(1);
        sent.chop(1);
        receive.chop(1);
        param.chop(1);
        analysis.chop(1);

        iniSettingsProtocol->beginGroup(testItem.name);
        iniSettingsProtocol->setValue("端口选择", port);
        iniSettingsProtocol->setValue("发送", sent);
        iniSettingsProtocol->setValue("接收", receive);
        iniSettingsProtocol->setValue("参数配置", param);
        if (analysis != "NA" && analysis != "NA:NA")
            iniSettingsProtocol->setValue("解析", analysis);
        else
            iniSettingsProtocol->remove("解析");
        if (testItem.repeat > 0)
            iniSettingsProtocol->setValue("功能配置", function);
        else
            iniSettingsProtocol->remove("功能配置");
        iniSettingsProtocol->endGroup();
    }
}

void LabViewSetting::setConfigItemList(const QList<ConfigItem> &configItemList) {}

/**
 * @brief 分析测试项。
 */
void LabViewSetting::iniToTestItemList()
{
    testItemList.clear();
    if (iniSettingsProtocol->isLoad()) // 获取测试项名称列表
    {
        foreach (QString testItemName, iniSettingsProtocol->childGroups())
        {
            if (passTestItemList.contains(testItemName)) // 跳过测试项名称
                continue;
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
void LabViewSetting::iniToConfigItemList()
{
    configItemList.clear();
    if (iniSettingsConfig->isLoad()) // 获取配置项名称列表
    {
        iniSettingsConfig->beginGroup(TEST_ITEM_GROUP);
        QStringList testItemNameList = iniSettingsConfig->allKeysOrder(); // 获取测试项名称顺序列表

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
[Test Item]
key = value
*/

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
    QString port = iniSettingsProtocol->value("端口选择");     // 端口选择
    QString sent = iniSettingsProtocol->value("发送");         // 发送
    QString receive = iniSettingsProtocol->value("接收");      // 接收
    QString param = iniSettingsProtocol->value("参数配置");    // 参数配置
    QString analysis = iniSettingsProtocol->value("解析");     // 解析
    QString function = iniSettingsProtocol->value("功能配置"); // 功能配置
    iniSettingsProtocol->endGroup();

    // 分割字符串，去掉方括号中的内容，返回字符串列表
    QStringList portList = splitStringSquareBrackets(port, ':');                // 端口列表
    QStringList sentList = splitStringSquareBrackets(sent, ':');                // 发送列表
    QStringList receiveList = splitStringSquareBrackets(receive, ':');          // 接收列表
    QStringList paramList = splitStringSquareBrackets(param, ':');              // 参数配置列表
    QStringList analysisContentList = splitStringSquareBrackets(analysis, ':'); // 解析内容列表

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
    int cmdNum = sentList.size(); // 测试项命令数量
    // testItem.cmdNum = cmdNum;
    testItem.dataByteLen = paramList.at(1).toInt(); // 每个数据项占用字节的长度
    testItem.decimal = paramList.at(2).toInt();     // 小数点位置
    testItem.byteOrder = paramList.at(3);           // 字节序 （LH、HL）
    testItem.encodeWay = paramList.at(4);           // 编码方式（HEX、H2S）
    testItem.sign = paramList.at(5);                // 符号

    QStringList cmdTypeList = splitStringSquareBrackets(paramList.at(0), '&');       // 解析方式列表（AT、68
    QStringList delayList = splitStringSquareBrackets(paramList.at(6), '&');         // 延时列表
    QStringList timeoutList = splitStringSquareBrackets(paramListEnd.at(0), '&');    // 超时列表
    QStringList resultShowList = splitStringSquareBrackets(paramListEnd.at(1), '&'); // 显示结果列表

    // 填充长度
    fillLength(portList, cmdNum);    // 端口选择
    fillLength(receiveList, cmdNum); // 接收
    fillLength(cmdTypeList, cmdNum); // 解析方式
    fillLength(delayList, cmdNum);   // 延时时间
    fillLength(timeoutList, cmdNum); // 超时时间

    for (int i = 0; i < cmdNum; i++)
    {
        TestCmd testCmd;
        testCmd.index = i;
        testCmd.comName = portList.at(i).trimmed(); // 端口选择
        if (testCmd.comName.contains("<"))
        {
            // 截取<>之间的字符串,不包含<>
            if (testCmd.comName.contains(">"))
            {
                testCmd.brief = testCmd.comName.mid(testCmd.comName.indexOf("<") + 1,
                                                    testCmd.comName.indexOf(">") - testCmd.comName.indexOf("<") - 1);
            }
            // 截取 < 之前的字符串
            testCmd.comName = testCmd.comName.mid(0, testCmd.comName.indexOf("<")).trimmed();
        }

        testCmd.tx = sentList.at(i).trimmed();                       // 发送
        testCmd.rx = receiveList.at(i).trimmed();                    // 接收
        testCmd.cmdType = cmdTypeList.at(i).trimmed();               // 解析方式
        testCmd.cmdDelay = delayList.at(i).trimmed().toDouble();     // 延时时间
        testCmd.cmdTimeout = timeoutList.at(i).trimmed().toDouble(); // 超时时间
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
    fillLength(analysisContentList, resultIndexMax + 1);

    for (int i = 0; i < resultNum; i++)
    {
        TestResult &testResult = testItem.resultList[i];
        // int index = testResult.index;

        // 解析=双匹配&LADC[: ]1[,]&OK:双匹配&LADC[: ]1[,]&OK
        // QStringList anaList = splitStringSquareBrackets(analysisContentList.at(i).trimmed(), '&');

        int indexSplit = analysisContentList.at(i).trimmed().indexOf("&");
        // 截取第一个 & 前的内容， 放到 analysisWay
        testResult.analysisWay = analysisContentList.at(i).trimmed().mid(0, indexSplit);
        // 截取第一个 & 后的内容， 放到 analysisContent
        if (indexSplit != -1)
        {
            testResult.analysisContent = analysisContentList.at(i).trimmed().mid(indexSplit + 1);
            // 如果解析内容中包含 :  ，则需要用[]把":"括起来，即xxx:xxx -> xxx[:]xxx
            if (testResult.analysisContent.contains(":"))
                testResult.analysisContent.replace(":", "[:]"); // 替换冒号
        }
        else
            testResult.analysisContent = "";
    }

    // 功能配置=重发次数(45)
    if (function.startsWith("重发次数"))
    {
        if (function.contains("(") && function.contains(")"))
        {
            testItem.repeat
                = function.mid(function.indexOf("(") + 1, function.indexOf(")") - function.indexOf("(") - 1).toInt();
        }
        else if (function.contains("（") && function.contains("）"))
        {
            testItem.repeat
                = function.mid(function.indexOf("（") + 1, function.indexOf("）") - function.indexOf("（") - 1).toInt();
        }
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
    qDebug() << "命令序号" << index;
    qDebug() << "命令注释" << brief;

    qDebug() << "端口选择" << comName;
    qDebug() << "发送" << tx;
    qDebug() << "接收" << rx;
    qDebug() << "命令类型" << cmdType;
    qDebug() << "延时时间" << cmdDelay;
    qDebug() << "超时时间" << cmdTimeout;
}

void TestResult::print() const
{
    qDebug() << "显示结果" << index << show;
    qDebug() << "解析" << analysisWay << analysisContent;
}

void TestItem::print() const
{
    qDebug() << "\n\n测试名：" << name;

    for (const auto &cmd : cmdList)
    {
        cmd.print();
        qDebug();
    }
    qDebug() << "字节数" << dataByteLen;
    qDebug() << "小数" << decimal;
    qDebug() << "字节序" << byteOrder;
    qDebug() << "编码方式" << encodeWay;
    qDebug() << "符号" << sign;

    for (const auto &result : resultList)
    {
        result.print();
    }

    qDebug() << "重复次数" << repeat;
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

    // 当[]的内容是 separator 时，需要将[]移去
    QString temp = "";
    for (int i = 0; i < input.size(); i++)
    {
        if (input.at(i) == separator && insideSquareBrackets == false) // 正常每次遇到 : 就是一个命令的结束
        {
            if (cmd.isEmpty())
                continue;
            // 当前命令不为空
            commandList.append(cmd); // 将当前命令添加到 commandList 中
            cmd.clear();             // 清空当前命令
        }
        else if (input.at(i) == '[') // 取方括号中的内容，不包括 [ 和 ]
        {
            insideSquareBrackets = true;
        }
        else if (input.at(i) == ']' && insideSquareBrackets) // 取方括号中的内容，不包括 [ 和 ]
        {
            if (temp.contains(separator))
            {
                cmd.append(temp);
            }
            else
            {
                cmd.append("[" + temp + "]");
            }
            temp.clear();
            insideSquareBrackets = false;
        }
        else
        {
            if (insideSquareBrackets == false)
            {
                cmd.append(input.at(i));
            }
            else
            {
                temp.append(input.at(i));
            }
        }
    }

    if (cmd.isEmpty() == false)
    {
        commandList.append(cmd);
    }

    return commandList;
}
