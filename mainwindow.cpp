/*
 * @Author: 陈俊健
 * @Date: 2023-10-28 19:35:01
 * @LastEditors: 陈俊健
 * @LastEditTime: 2024-06-11 17:06:32
 * @FilePath: \labview-ini-editor\mainwindow.cpp
 * @Description:
 *
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved.
 */
#include "mainwindow.h"
#include "labviewsetting.h"
#include "test_item_interface.h"
#include "test_result_interface.h"
#include "ui_mainwindow.h"
#include <QCheckBox>
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>

#if _MSC_VER >= 1600 // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

#define TEST_CMD_HEIGHT    105
#define TEST_RESULT_HEIGHT 52

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // ui->dwTestPool->setVisible(false);
    // ui->actSownExtTestItem->setChecked(false);
    ui->lwTestCmd->setGridSize(QSize(0, TEST_CMD_HEIGHT));

    connect(ui->lwTestCmd, &MyListWidget::itemsReordered, this, &MainWindow::onTestCmdReordered);
    connect(ui->lwTestResult, &MyListWidget::itemsReordered, this, &MainWindow::onTestResultReordered);
}

MainWindow::~MainWindow() { delete ui; }

QString FindFile(QString path, QString obj, QString target)
{
    // 查找配置文件
    QDir dir(path);
    QStringList filter;
    filter << "*.ini";
    dir.setNameFilters(filter);
    QStringList fileList = dir.entryList();
    QStringList targetFileList;
    qDebug() << "fileList: " << fileList;
    // 遍历 target 列表，移除版本号，与 obj
    for (int i = 0; i < fileList.size(); i++)
    {
        QString str = fileList.at(i);
        str = str.mid(0, str.indexOf(target));
        qDebug() << "str: " << str;
        if (str == obj)
        {
            targetFileList.append(fileList.at(i));
        }
    }
    int newIndex = 0;
    if (targetFileList.size() == 0)
    {
        qDebug() << "未找到" << target;
        return "";
    }
    else if (targetFileList.size() > 1)
    {
        // 查找有无最新版本
        qDebug() << "找到" << targetFileList.size() << "个" << target;
        int maxRev = 0;
        for (int i = 0; i < targetFileList.size(); i++)
        {
            int rev = 0;
            rev = targetFileList.at(i)
                      .mid(targetFileList.at(i).indexOf("_Rev") + 4,
                           targetFileList.at(i).indexOf(".ini") - targetFileList.at(i).indexOf("_Rev") - 4)
                      .toInt();
            if (rev > maxRev)
            {
                maxRev = rev;
                newIndex = i;
            }
        }
    }
    else
    {
        qDebug() << "找到一个" << target;
    }
    return targetFileList.at(newIndex);
}

// 原文链接：https://blog.csdn.net/zhang_ze1234/article/details/107682660
void MainWindow::on_actOpenIni_triggered()
{
    QString pathName;
    QString pathName_Old = fileNameConfig.mid(0, fileNameConfig.lastIndexOf("/"));
    if (pathName_Old == "")
    {
        //"."表示在当前工作路径下寻找
        pathName_Old = "./";
    }
    // “Ini files(*.ini)”表示指定
    pathName = QFileDialog::getOpenFileName(this, "打开文件", pathName_Old, "Ini files(*.ini)");
    if (pathName == "")
    {
        qDebug() << "未选择文件";
        return;
    }
    if (!pathName.endsWith(".ini"))
    {
        qDebug() << "文件格式错误";
        return;
    }
    if (!QFile::exists(pathName))
    {
        qDebug() << "文件不存在";
        return;
    }
    // if (pathName == fileNameProtocol || pathName == fileNameConfig)
    // {
    //     qDebug() << "文件未修改";
    //     return;
    // }
    // 协议文件 和 配置文件 分别放在同目录下的 协议文件 和 配置文件 文件夹下，如下：
    // C:NR90HCNA00NNA_IO测试程序_Rev01\协议文件\L-CM5TR01-90HCW_IO测试程序协议文件_Rev01.ini
    // C:NR90HCNA00NNA_IO测试程序_Rev01\配置文件\L-CM5TR01-90HCW_IO测试程序配置文件_Rev01.ini
    // 其中 L-CM5TR01-90HCW 表示测试对象， IO测试程序 表示测试程序， Rev01 表示版本号（版本号不同时，使用最新版本）
    // 因此要匹配“协议”或“配置”前的字符串，以判断是否为同一测试对象
    if (pathName.contains("协议文件"))
    {
        qDebug() << "打开协议文件";
        fileNameProtocol = pathName;
        QStringList strQirList = pathName.split("/");
        // 获取协议文件名，移除版本号
        QString strTestObj = strQirList.last().mid(0, strQirList.last().lastIndexOf("协议文件"));
        qDebug() << "strTestObj: " << strTestObj;
        // 设置标题
        this->setWindowTitle(strTestObj);
        // QString strPath = pathName.mid(0, pathName.lastIndexOf("/"));
        // strPath = strPath.mid(0, strPath.lastIndexOf("/"));
        // strPath += "/配置文件/";
        // qDebug() << ": " << strPath;
        // fileNameConfig = strPath + FindFile(strPath, strTestObj, "配置文件");
    }
    // else if (pathName.contains("配置文件"))
    // {
    //     qDebug() << "打开配置文件";
    //     fileNameConfig = pathName;
    //     QStringList strQirList = pathName.split("/");
    //     // 获取配置文件 前缀，移除版本号
    //     QString strTestObj = strQirList.last().mid(0, strQirList.last().lastIndexOf("配置文件"));
    //     qDebug() << "strTestObj: " << strTestObj;
    //     QString strPath = pathName.mid(0, pathName.lastIndexOf("/"));
    //     strPath = strPath.mid(0, strPath.lastIndexOf("/"));
    //     strPath += "/协议文件/";
    //     qDebug() << ": " << strPath;
    //     fileNameProtocol = strPath + FindFile(strPath, strTestObj, "协议文件");
    // }
    else
    {
        qDebug() << "文件名需要包含“协议”或“配置”关键字";
        return;
    }
    qDebug() << "pathName: " << pathName;
    qDebug() << "fileNameProtocol: " << fileNameProtocol;
    // qDebug() << "fileNameConfig: " << fileNameConfig;

    if (fileNameProtocol == "")
    {
        qDebug() << "未找到协议文件";
        return;
    }
    // if (fileNameConfig == "")
    // {
    //     qDebug() << "未找到配置文件";
    //     return;
    // }
    labviewSetting = new LabViewSetting(fileNameProtocol, fileNameConfig);
    this->testItemList.clear();                             // 清空
    this->testItemList = labviewSetting->getTestItemList(); // 解析 协议文件

    // this->configItemList.clear(); // 清空
    // this->configItemList = analysis_config_ini(fileNameConfig); // 解析 配置文件
    // this->configItemList = labviewSetting->getConfigItemList(); // 解析 配置文件

    ui->actSave->setEnabled(true); // 保存按钮可用
    updateTestItemListUi();        // 更新测试项列表
}

// void MainWindow::on_btnAddResultItem_clicked()
//{
//     //    // 往 scrollArea_Result 添加一个 TestResult
//     //    TestResultInterface *item = new TestResultInterface(this);
//     //    item->setIndex(testResultInterfaceList.size());
//     //    testResultInterfaceList.append(item);
//     //    ui->vloResultItem->insertWidget(ui->vloResultItem->count() - 2, item);
//     //    // 滚动到底部
//     // ui->scrollArea_Result->verticalScrollBar()->setValue(ui->scrollArea_Result->verticalScrollBar()->maximum());
// }

/**
 * @brief 加载测试项界面
 *
 * @param item 被点击的列表项
 */
void MainWindow::loadTestItemUi(QListWidgetItem *item)
{
    if (leTestItemName_Old != "")
    {
        // 获取当前点击的测试项的索引
        int testItemIndex = getTestItemIndex(leTestItemName_Old);
        if (testItemIndex == -1)
            qDebug() << "未找到上一个测试项";
        else
            updateTestCmdListFromUi(this->testItemList[testItemIndex].cmdList);
    }

    leTestItemName_Old = item->text().trimmed();
    qDebug() << "点击: " << leTestItemName_Old;

    // 获取当前点击的测试项的索引
    int testItemIndex = -1;
    for (int i = 0; i < this->testItemList.size(); i++)
    {
        if (leTestItemName_Old == this->testItemList.at(i).name)
        {
            testItemIndex = i;
            break;
        }
    }
    if (testItemIndex == -1)
    {
        qDebug() << "未找到测试项";
        return;
    }

    ui->leTestItemName->setText(testItemList.at(testItemIndex).name);
    ui->spbxDataSize->setValue(testItemList.at(testItemIndex).dataByteLen);
    ui->spbxDecPlace->setValue(testItemList.at(testItemIndex).decimal);
    ui->spbxByteOrder->setCurrentText(testItemList.at(testItemIndex).byteOrder);
    ui->cbSign->setCurrentText(testItemList.at(testItemIndex).sign);
    ui->spbxRepeatTimes->setValue(testItemList.at(testItemIndex).repeat);

    // 清空命令项
    testCmdInterfaceList.clear();

    // 获取当前点击的配置项的索引
    // int configIndex = -1;
    // for (int i = 0; i < this->configItemList.size(); i++)
    // {
    //     if (leTestItemName_Old == this->configItemList.at(i).name)
    //     {
    //         configIndex = i;
    //         break;
    //     }
    // }

    // 清空结果项
    testResultInterfaceList.clear();
    ui->lwTestResult->clear();

    // 添加结果项
    for (int i = 0; i < this->testItemList.at(testItemIndex).resultList.size(); ++i)
    {
        TestResultInterface *item = new TestResultInterface(this);
        TestResult result = this->testItemList.at(testItemIndex).resultList.at(i);
        item->setUi_Result(i, result);

        // if (this->configItemList.at(configIndex).contentList.size() <= i)
        // {
        //     qDebug() << "Warning: " << this->testItemList.at(testItemIndex).name << "可能没有配置显示结果";
        //     break;
        // }

        // if (configIndex != -1)
        // {
        //     ConfigContent config = this->configItemList.at(configIndex).contentList.at(i);
        //     item->setUi_Config(config);
        // }

        testResultInterfaceList.append(item);
        // ui->vloResultItem->insertWidget(ui->vloResultItem->count() - 2, item);

        QListWidgetItem *listItem = new QListWidgetItem(ui->lwTestResult);
        listItem->setSizeHint(QSize(0, TEST_RESULT_HEIGHT));
        ui->lwTestResult->addItem(listItem);
        ui->lwTestResult->setItemWidget(listItem, item);
    }
    updateTestItemUi();
}

void MainWindow::on_lwlTestItemList_itemSelectionChanged()
{
    QListWidgetItem *item = ui->lwlTestItemList->currentItem();
    loadTestItemUi(item);
}

void MainWindow::on_lwlTestItemPool_itemSelectionChanged()
{
    QListWidgetItem *item = ui->lwlTestItemPool->currentItem();
    loadTestItemUi(item);
}

void MainWindow::on_dwTestPool_visibilityChanged(bool visible)
{
    qDebug() << "on_dwTestPool_visibilityChanged" << visible;
    ui->actSownExtTestItem->setChecked(visible);
}

void MainWindow::on_actSownExtTestItem_triggered(bool checked) { ui->dwTestPool->setVisible(checked); }

void MainWindow::on_actAbout_triggered()
{
    QString data = __DATE__;
    QString time = __TIME__;
    QString year = data.mid(7, 4);
    QString month = data.mid(0, 3);
    QString day = data.mid(4, 2);
    QString author = "ChenJunJian";
    QString version = "0.0.0";
    QString postscript = "Just For Fun! No Other Ideas."; // PS：只是为了好玩方便，没有其他想法
    QStringList monthList = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    for (int i = 0; i < monthList.size(); i++)
    {
        if (month == monthList.at(i))
        {
            month = QString::number(i + 1);
            break;
        }
    }

    QString strData = year + "/" + month + "/" + day;

    QMessageBox::about(this, "About",
                        "LabViewIniEditor-Demo\n"
                        "\n"
                        "Author: "+ author+"\n"
                        "Postscript: "+ postscript+"\n"
                        "\n"
                        "Version: " + version + "\n"
                        "Build Data: " + strData + "\n"
                        "Build Time: " + time + "\n"
                        "\n"
                        "CopyRight © "+ year + " by " + author + ", All Rights Reserved.");
}

void MainWindow::on_leTestItemName_editingFinished()
{
    // 获取当前点击的测试项的名称
    QString str = ui->leTestItemName->text().trimmed();
    if (str == "")
    {
        ui->leTestItemName->setText(leTestItemName_Old);
        return;
    }
    ui->leTestItemName->setText(str);
    // 获取当前点击的测试项的索引
    int testItemIndex = getTestItemIndex(leTestItemName_Old);
    if (testItemIndex == -1)
    {
        qDebug() << "未找到测试项";
        return;
    }
    // 修改测试项名称
    this->testItemList[testItemIndex].name = str;
    // this->configItemList[testItemIndex].name = str;
    // 更新测试项界面
    updateTestItemUi();
    updateTestItemListUi();
    leTestItemName_Old = str;
}

int MainWindow::getTestItemIndex(const QString &name)
{
    for (int i = 0; i < this->testItemList.size(); i++)
    {
        if (name == this->testItemList.at(i).name)
        {
            return i;
        }
    }
    return -1;
}

TestItem *MainWindow::getTestItemCurrent(void)
{
    // 获取当前点击的测试项的名称
    QString str = ui->leTestItemName->text().trimmed();
    // 获取当前点击的测试项的索引
    int testItemIndex = getTestItemIndex(str);
    if (testItemIndex == -1)
    {
        qDebug() << "未找到测试项";
        return nullptr;
    }
    return &this->testItemList[testItemIndex];
}

void MainWindow::on_btnCopyTestICmd_clicked()
{
    // 获取当前点击的测试项的名称
    QString str = ui->leTestItemName->text().trimmed();
    // 获取当前点击的测试项的索引
    int testItemIndex = getTestItemIndex(str);
    if (testItemIndex == -1)
    {
        qDebug() << "未找到测试项";
        return;
    }
    // 获取当前点击的 lwTestCmd 的索引
    int testCmdIndex = ui->lwTestCmd->currentRow();
    if (testCmdIndex == -1)
    {
        qDebug() << "未选择命令项";
        return;
    }
    qDebug() << "点击 TestCmd: " << str << " " << testCmdIndex;

    // 在当前点击的命令项下方添加一个命令项
    TestItemInterface *item = new TestItemInterface(this);
    TestCmd cmd = ((TestItemInterface *) ui->lwTestCmd->itemWidget(ui->lwTestCmd->item(testCmdIndex)))->getTestCmd();

    item->setUi(testCmdIndex + 1, this->testItemList.at(testItemIndex).cmdList.at(testCmdIndex));

    insertTestCmd(this->testItemList[testItemIndex].cmdList, cmd, testCmdIndex + 1);
    insertTestCmdInterface(testCmdIndex + 1, item);

    // 更新命令项
    loadTestCmdUi(this->testItemList.at(testItemIndex).cmdList);
    ui->lwTestCmd->setCurrentRow(testCmdIndex + 1);
}

void MainWindow::on_btnAddTestICmd_clicked()
{
    QString str = ui->leTestItemName->text().trimmed();
    // 获取当前点击的测试项的索引
    // int testItemIndex = getTestItemIndex(str);
    // if (testItemIndex == -1)
    // {
    //     qDebug() << "未找到测试项";
    //     return;
    // }
    // 获取当前点击的 lwTestCmd 的索引
    int testCmdIndex = ui->lwTestCmd->currentRow();
    qDebug() << "点击 TestCmd: " << str << " " << testCmdIndex;
    if (testCmdIndex == -1)
        testCmdIndex = 0;
    else
        testCmdIndex++;
    // 在当前点击的命令项下方添加一个命令项
    TestCmd cmd;
    // insertTestCmd(this->testItemList[testItemIndex].cmdList, cmd, testCmdIndex);

    // 更新命令项
    // loadTestCmdUi(this->testItemList.at(testItemIndex).cmdList);
    ui->lwTestCmd->setCurrentRow(testCmdIndex);
}

void MainWindow::loadTestCmdUi(const QVector<TestCmd> &cmdList)
{
    ui->lwTestCmd->clear();
    for (int i = 0; i < cmdList.size(); ++i)
    {
        TestItemInterface *item = new TestItemInterface(this);
        item->setUi(i, cmdList.at(i));
        addTestCmdInterface(item);
    }
}

void MainWindow::loadTestResultUi(const QVector<TestResult> &resultList)
{
    ui->lwTestResult->clear();
    // 获取当前点击的测试项的索引
    int testItemIndex = -1;
    for (int i = 0; i < this->testItemList.size(); i++)
    {
        if (leTestItemName_Old == this->testItemList.at(i).name)
        {
            testItemIndex = i;
            break;
        }
    }
    if (testItemIndex == -1)
    {
        qDebug() << "未找到测试项";
        return;
    }
    // 获取当前点击的配置项的索引
    // int configIndex = -1;
    // for (int i = 0; i < this->configItemList.size(); i++)
    // {
    //     if (leTestItemName_Old == this->configItemList.at(i).name)
    //     {
    //         configIndex = i;
    //         break;
    //     }
    // }

    for (int i = 0; i < resultList.size(); ++i)
    {
        TestResultInterface *item = new TestResultInterface(this);
        item->setUi_Result(i, resultList.at(i));
        // if (this->configItemList.at(configIndex).contentList.size() <= i)
        // {
        //     qDebug() << "Warning: " << this->testItemList.at(testItemIndex).name << "可能没有配置显示结果";
        //     break;
        // }

        // if (configIndex != -1)
        // {
        //     ConfigContent config = this->configItemList.at(configIndex).contentList.at(i);
        //     item->setUi_Config(config);
        // }

        addResultInterface(item);
    }
}

void MainWindow::updateTestItemUi()
{
    // 获取当前点击的测试项的名称
    QString str = ui->leTestItemName->text().trimmed();
    // 获取当前点击的测试项的索引
    int testItemIndex = getTestItemIndex(str);
    if (testItemIndex == -1)
    {
        qDebug() << "未找到测试项";
        return;
    }
    // 更新命令项
    loadTestCmdUi(this->testItemList.at(testItemIndex).cmdList);

    // 更新结果项
    loadTestResultUi(this->testItemList.at(testItemIndex).resultList);
}

void MainWindow::addTestCmdInterface(TestItemInterface *item)
{
    insertTestCmdInterface(testCmdInterfaceList.size(), item);
}

void MainWindow::insertTestCmdInterface(int index, TestItemInterface *item)
{
    if (index < 0 || index > testCmdInterfaceList.size())
    {
        qDebug() << "index 越界";
        return;
    }
    testCmdInterfaceList.insert(index, item);

    QListWidgetItem *listItem = new QListWidgetItem(ui->lwTestCmd);
    listItem->setSizeHint(QSize(0, TEST_CMD_HEIGHT));
    ui->lwTestCmd->insertItem(index, listItem);   // 插入到列表中
    ui->lwTestCmd->setItemWidget(listItem, item); // 设置为该项的 Widget
    ui->lwTestCmd->setCurrentRow(index);          // 滚动到该项
}

void MainWindow::addResultInterface(TestResultInterface *item)
{
    insertResultInterface(testResultInterfaceList.size(), item);
}

void MainWindow::insertResultInterface(int index, TestResultInterface *item)
{
    if (index < 0 || index > testResultInterfaceList.size())
    {
        qDebug() << "index 越界";
        return;
    }
    testResultInterfaceList.insert(index, item);

    QListWidgetItem *listItem = new QListWidgetItem(ui->lwTestResult);
    listItem->setSizeHint(QSize(0, TEST_RESULT_HEIGHT));
    ui->lwTestResult->insertItem(index, listItem);   // 插入到列表中
    ui->lwTestResult->setItemWidget(listItem, item); // 设置为该项的 Widget
    ui->lwTestResult->setCurrentRow(index);          // 滚动到该项
}

void MainWindow::insertTestCmd(QVector<TestCmd> &cmdList, const TestCmd &cmd, int cmdIndex)
{
    if (cmdIndex < 0 || cmdIndex > cmdList.size())
    {
        qDebug() << "cmdIndex 越界";
        return;
    }
    cmdList.insert(cmdIndex, cmd);

    for (const TestCmd &cmd : cmdList)
    {
        cmd.print();
    }
}

void MainWindow::updateTestCmdListFromUi(QVector<TestCmd> &cmdList)
{
    cmdList.clear();
    for (int i = 0; i < ui->lwTestCmd->count(); i++)
    {
        TestItemInterface *item = (TestItemInterface *) ui->lwTestCmd->itemWidget(ui->lwTestCmd->item(i));
        cmdList.append(item->getTestCmd());
    }
}

void MainWindow::on_actSave_triggered()
{
    if (labviewSetting == nullptr)
    {
        qDebug() << "未打开文件";
        return;
    }

    // 获取当前的测试项的索引
    int testItemIndex = getTestItemIndex(leTestItemName_Old);
    if (testItemIndex == -1)
        qDebug() << "未找到测试项";
    else
        updateTestCmdListFromUi(this->testItemList[testItemIndex].cmdList);

    // 将界面testItem保存到 testItemList
    // updateTestItemListFromUi();
    labviewSetting->setTestItemList(testItemList);
    // 将界面configItem保存到 configItemList
    // labviewSetting->setConfigItemList(configItemList);

    // 保存配置文件 & 协议文件
    labviewSetting->saveFile();
}

void MainWindow::on_btnRemoveTestICmd_clicked()
{
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    // 获取当前点击的 lwTestCmd 的索引
    int testCmdIndex = ui->lwTestCmd->currentRow();
    if (testCmdIndex == -1)
    {
        qDebug() << "未选择命令项";
        return;
    }

    // 删除当前点击的命令项
    testItem->cmdList.remove(testCmdIndex);

    // 更新命令项
    loadTestCmdUi(testItem->cmdList);
    if (testCmdIndex > 0)
        testCmdIndex--;
    ui->lwTestCmd->setCurrentRow(testCmdIndex);
}

void MainWindow::updateTestItemListUi()
{
    ui->lwlTestItemList->clear();
    QStringList strConfigList;
    // for (int i = 0; i < this->configItemList.size(); i++) // 往 ListWidget 添加测试项
    // {
    //     QListWidgetItem *item = new QListWidgetItem(ui->lwlTestItemList);
    //     QCheckBox *checkBox = new QCheckBox(ui->lwlTestItemList);
    //     checkBox->setChecked(this->configItemList.at(i).enable);
    //     // checkBox->setText(this->configItemList.at(i).name);
    //     ui->lwlTestItemList->setItemWidget(item, checkBox);

    //     QString str = this->configItemList.at(i).name;
    //     strConfigList << str;
    //     item->setText("   " + str);
    // }

    ui->lwlTestItemPool->clear();
    for (int i = 0; i < this->testItemList.size(); i++) // 往 ListWidget 添加测试项
    {
        if (strConfigList.contains(this->testItemList.at(i).name))
        {
            continue;
        }
        QListWidgetItem *item = new QListWidgetItem(ui->lwlTestItemPool);
        item->setText(this->testItemList.at(i).name);
    }
}

void MainWindow::updateTestItemListFromUi()
{
    // testItemList.clear();
    // for (int i = 0; i < ui->lwlTestItemPool->count(); i++)
    // {
    //     QListWidgetItem *itemUi = ui->lwlTestItemPool->item(i);
    //     TestItem testItem;
    //     testItem.name = itemUi->text().trimmed();
    //     testItem.byteOrder = ui->spbxByteOrder->currentText();
    //     testItem.dataByteLen = ui->spbxDataSize->value();
    //     testItem.decimal = ui->spbxDecPlace->value();
    //     testItem.repeat = ui->spbxRepeatTimes->value();

    //     QVector<TestCmd> cmdList;
    //     for (int j = 0; j < ui->lwTestCmd->count(); j++)
    //     {
    //         TestItemInterface *item = (TestItemInterface *) ui->lwTestCmd->itemWidget(ui->lwTestCmd->item(j));
    //         cmdList.append(item->getTestCmd());
    //     }
    //     testItemList.append(testItem);

    // }
}

void MainWindow::on_spbxRepeatTimes_valueChanged(int arg1)
{
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    testItem->repeat = arg1;
}

void MainWindow::on_spbxDataSize_valueChanged(int arg1)
{
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    testItem->dataByteLen = arg1;
}

void MainWindow::on_spbxDecPlace_valueChanged(int arg1)
{
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    testItem->decimal = arg1;
}

void MainWindow::on_spbxByteOrder_currentTextChanged(const QString &arg1)
{
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    testItem->byteOrder = arg1;
}

void MainWindow::on_cbSign_currentTextChanged(const QString &arg1)
{
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    testItem->sign = arg1;
}

void MainWindow::onTestCmdReordered(void)
{
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    QVector<TestCmd> cmdList;
    for (int i = 0; i < ui->lwTestCmd->count(); i++)
    {
        TestItemInterface *item = (TestItemInterface *) ui->lwTestCmd->itemWidget(ui->lwTestCmd->item(i));
        cmdList.append(item->getTestCmd());
    }
    testItem->cmdList = cmdList;
}

void MainWindow::onTestResultReordered(void)
{
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    QVector<TestResult> resultList;
    for (int i = 0; i < ui->lwTestResult->count(); i++)
    {
        TestResultInterface *item = (TestResultInterface *) ui->lwTestResult->itemWidget(ui->lwTestResult->item(i));
        resultList.append(item->getTestResult());
    }
    testItem->resultList = resultList;
}
