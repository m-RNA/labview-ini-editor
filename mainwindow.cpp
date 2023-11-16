/*
 * @Author: 陈俊健
 * @Date: 2023-10-28 19:35:01
 * @LastEditors: 陈俊健
 * @LastEditTime: 2023-11-02 02:21:35
 * @FilePath: \LabViewIniEditor\mainwindow.cpp
 * @Description:
 *
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved.
 */
#include "mainwindow.h"
#include "test_item_interface.h"
#include "test_result_interface.h"
#include "ui_mainwindow.h"
#include <QCheckBox>
#include <QDateTime>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->dwTestPool->setVisible(false);
    ui->actSownExtTestItem->setChecked(false);
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
    //"."表示在当前工作路径下寻找，“Ini files(*.ini)”表示指定
    pathName = QFileDialog::getOpenFileName(this, "打开文件", "./", "Ini files(*.ini)");
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
        QString strPath = pathName.mid(0, pathName.lastIndexOf("/"));
        strPath = strPath.mid(0, strPath.lastIndexOf("/"));
        strPath += "/配置文件/";
        qDebug() << ": " << strPath;
        fileNameConfig = strPath + FindFile(strPath, strTestObj, "配置文件");
    }
    else if (pathName.contains("配置文件"))
    {
        qDebug() << "打开配置文件";
        fileNameConfig = pathName;
        QStringList strQirList = pathName.split("/");
        // 获取配置文件 前缀，移除版本号
        QString strTestObj = strQirList.last().mid(0, strQirList.last().lastIndexOf("配置文件"));
        qDebug() << "strTestObj: " << strTestObj;
        QString strPath = pathName.mid(0, pathName.lastIndexOf("/"));
        strPath = strPath.mid(0, strPath.lastIndexOf("/"));
        strPath += "/协议文件/";
        qDebug() << ": " << strPath;
        fileNameProtocol = strPath + FindFile(strPath, strTestObj, "协议文件");
    }
    else
    {
        qDebug() << "文件名需要包含“协议”或“配置”关键字";
        return;
    }
    qDebug() << "pathName: " << pathName;
    qDebug() << "fileNameProtocol: " << fileNameProtocol;
    qDebug() << "fileNameConfig: " << fileNameConfig;

    if (fileNameProtocol == "")
    {
        qDebug() << "未找到协议文件";
        return;
    }
    if (fileNameConfig == "")
    {
        qDebug() << "未找到配置文件";
        return;
    }
    this->configItemList.clear();                               // 清空
    this->configItemList = analysis_config_ini(fileNameConfig); // 解析 配置文件
    ui->lwlTestItemList->clear();
    QStringList strConfigList;
    for (int i = 0; i < this->configItemList.size(); i++) // 往 ListWidget 添加测试项
    {
        QListWidgetItem *item = new QListWidgetItem(ui->lwlTestItemList);
        QCheckBox *checkBox = new QCheckBox(ui->lwlTestItemList);
        checkBox->setChecked(this->configItemList.at(i).enable);
        // checkBox->setText(this->configItemList.at(i).name);
        ui->lwlTestItemList->setItemWidget(item, checkBox);

        QString str = this->configItemList.at(i).name;
        strConfigList << str;
        item->setText("   " + str);
    }

    this->testItemList.clear();                                   // 清空
    this->testItemList = analysis_protocol_ini(fileNameProtocol); // 解析 协议文件
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

void MainWindow::on_lwlTestItemPool_itemClicked(QListWidgetItem *item)
{
    QString str = item->text().trimmed();
    qDebug() << "点击 TestItemPool: " << str;

    // 获取当前点击的测试项的索引
    int testItemIndex = -1;
    for (int i = 0; i < this->testItemList.size(); i++)
    {
        if (str == this->testItemList.at(i).name)
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
    int configIndex = -1;
    for (int i = 0; i < this->configItemList.size(); i++)
    {
        if (str == this->configItemList.at(i).name)
        {
            configIndex = i;
            break;
        }
    }

    ui->leTestItemName->setText(testItemList.at(testItemIndex).name);
    ui->spbxRepeatTimes->setValue(testItemList.at(testItemIndex).repeat);

    // 清空命令项
    for (int i = 0; i < testCmdInterfaceList.size(); ++i)
    {
        // ui->vloCmdItem->removeWidget(testCmdInterfaceList.at(i));
        ui->lwTestCmd->removeItemWidget(ui->lwTestCmd->item(i)); // 从列表中移除
        testCmdInterfaceList.at(i)->deleteLater();
    }
    testCmdInterfaceList.clear();
    ui->lwTestCmd->clear();

    // 清空结果项
    for (int i = 0; i < testResultInterfaceList.size(); ++i)
    {
        //        ui->vloResultItem->removeWidget(testResultInterfaceList.at(i));
        ui->lwTestResult->removeItemWidget(ui->lwTestResult->item(i)); // 从列表中移除
        testResultInterfaceList.at(i)->deleteLater();
    }
    testResultInterfaceList.clear();
    ui->lwTestResult->clear();

    // 添加命令项
    for (int i = 0; i < this->testItemList.at(testItemIndex).cmdList.size(); ++i)
    {
        TestItemInterface *item = new TestItemInterface(this);
        item->setUi(i, this->testItemList.at(testItemIndex).cmdList.at(i));
        addTestCmdInterface(item);
    }

    // 添加结果项
    for (int i = 0; i < this->testItemList.at(testItemIndex).resultList.size(); ++i)
    {
        TestResultInterface *item = new TestResultInterface(this);
        auto result = this->testItemList.at(testItemIndex).resultList.at(i);
        item->setUi_Result(i, result);

        if (configIndex != -1)
        {
            auto config = this->configItemList.at(configIndex).contentList.at(i);
            item->setUi_Config(config);
        }

        testResultInterfaceList.append(item);
        //        ui->vloResultItem->insertWidget(ui->vloResultItem->count() - 2, item);

        QListWidgetItem *listItem = new QListWidgetItem(ui->lwTestResult);
        listItem->setSizeHint(QSize(0, 52));
        ui->lwTestResult->addItem(listItem);
        ui->lwTestResult->setItemWidget(listItem, item);

        // 滚动到底部
        // ui->scrollArea_Result->verticalScrollBar()->setValue(ui->scrollArea_Result->verticalScrollBar()->maximum());
    }
}

void MainWindow::on_lwlTestItemList_itemClicked(QListWidgetItem *item) { on_lwlTestItemPool_itemClicked(item); }

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
                       this->windowTitle() + "\n"
                       "\n"
                        "Author: "+ author+"\n"
                        "Version: " + version + "\n"
                        "Build Data: " + strData + "\n"
                        "Build Time: " + time + "\n"
                       "\n"
                        "CopyRight © "+ year + " by " + author + ", All Rights Reserved.");
}

void MainWindow::on_leTestItemName_editingFinished()
{
    ui->leTestItemName->setText(ui->leTestItemName->text().trimmed());
    //  do something

    leTestItemName_Old = ui->leTestItemName->text();
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
    insertTestCmd(testItemIndex, testCmdIndex + 1, cmd);
    insertTestCmdInterface(testCmdIndex + 1, item);
    ui->lwTestCmd->setCurrentRow(testCmdIndex + 1); // 滚动到复制的命令项

    // 更新命令项的序号
    for (int i = testCmdIndex; i < ui->lwTestCmd->count(); i++)
    {
        ((TestItemInterface *) ui->lwTestCmd->itemWidget(ui->lwTestCmd->item(i)))->setIndex(i);
    }
}

void MainWindow::on_btnAddTestICmd_clicked()
{
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
    TestCmd cmd;
    insertTestCmd(testItemIndex, testCmdIndex + 1, cmd);
    TestItemInterface *item = new TestItemInterface(this);
    insertTestCmdInterface(testCmdIndex + 1, item);

    // 更新命令项的序号
    for (int i = testCmdIndex; i < ui->lwTestCmd->count(); i++)
    {
        ((TestItemInterface *) ui->lwTestCmd->itemWidget(ui->lwTestCmd->item(i)))->setIndex(i);
    }
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
    listItem->setSizeHint(QSize(0, 52));
    ui->lwTestCmd->insertItem(index, listItem);
    ui->lwTestCmd->setItemWidget(listItem, item);
    ui->lwTestCmd->setCurrentRow(index); // 滚动到该项
}

void MainWindow::insertTestCmd(int itemListIndex, int cmdIndex, const TestCmd &cmd)
{
    if (itemListIndex < 0 || itemListIndex >= this->testItemList.size())
    {
        qDebug() << "itemListIndex 越界";
        return;
    }
    if (cmdIndex < 0 || cmdIndex > this->testItemList[itemListIndex].cmdList.size())
    {
        qDebug() << "cmdIndex 越界";
        return;
    }
    this->testItemList[itemListIndex].cmdList.insert(cmdIndex, cmd);

    for (const auto &cmd : this->testItemList[itemListIndex].cmdList)
    {
        printTestCmd(cmd);
    }
}


void MainWindow::on_lwTestCmd_indexesMoved(const QModelIndexList &indexes)
{
    qDebug() << "on_lwTestCmd_indexesMoved" << indexes;
}
