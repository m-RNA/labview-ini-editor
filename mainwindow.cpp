/*
 * @Author: 陈俊健
 * @Date: 2023-10-28 19:35:01
 * @LastEditors: 陈俊健
 * @LastEditTime: 2023-11-02 02:21:35
 * @FilePath: \LabViewIniEditer\mainwindow.cpp
 * @Description:
 *
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved.
 */
#include "mainwindow.h"
#include "test_item_interface.h"
#include "test_result_interface.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_btnAddTestItem_clicked()
{
    // 往 scrollArea 添加一个 TestItem
    TestItemInterface *item = new TestItemInterface(ui->scrollAreaWidgetContents);
    item->setIndex(testCmdInterfaceList.size());
    testCmdInterfaceList.append(item);
    ui->vloCmdItem->insertWidget(ui->vloCmdItem->count() - 2, item);
    // 滚动到底部
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
}

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
    this->testItemList.clear();                          // 清空
    this->testItemList = analysis_ini(fileNameProtocol); // 解析 ini 文件

    // 往 TreeWidget 添加测试项
    ui->treeWidget->clear();
    for (int i = 0; i < this->testItemList.size(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->treeWidget);
        item->setText(0, this->testItemList.at(i).name);
        // 添加子测试项
        for (int j = 0; j < this->testItemList.at(i).cmdList.size(); j++)
        {
            QTreeWidgetItem *subItem = new QTreeWidgetItem(item);
            QString subStr = this->testItemList.at(i).cmdList.at(j).brief;
            if (subStr == "")
                subStr = this->testItemList.at(i).cmdList.at(j).tx;
            subItem->setText(0, subStr);
        }
    }
    // 展开树
    // ui->treeWidget->expandAll();
}

void MainWindow::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    // 获取当前点击的测试项
    QString str = item->text(column);
    qDebug() << "点击 TreeWidget: " << str;

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
    ui->leTestItemName->setText(testItemList.at(testItemIndex).name);
    ui->spbxCmdNum->setValue(testItemList.at(testItemIndex).cmdList.size());
    ui->spbxResultNum->setValue(testItemList.at(testItemIndex).resultList.size());
    ui->spbxRepeatTimes->setValue(testItemList.at(testItemIndex).repeat);

    // 清空命令项
    for (int i = 0; i < testCmdInterfaceList.size(); ++i)
    {
        ui->vloCmdItem->removeWidget(testCmdInterfaceList.at(i));
        testCmdInterfaceList.at(i)->deleteLater();
    }
    testCmdInterfaceList.clear();

    // 清空结果项
    for (int i = 0; i < testResultInterfaceList.size(); ++i)
    {
        ui->vloResultItem->removeWidget(testResultInterfaceList.at(i));
        testResultInterfaceList.at(i)->deleteLater();
    }
    testResultInterfaceList.clear();

    // 添加命令项
    for (int i = 0; i < this->testItemList.at(testItemIndex).cmdList.size(); ++i)
    {
        TestItemInterface *item = new TestItemInterface(ui->scrollAreaWidgetContents);
        item->setUi(i, this->testItemList.at(testItemIndex).cmdList.at(i));
        testCmdInterfaceList.append(item);
        ui->vloCmdItem->insertWidget(ui->vloCmdItem->count() - 2, item);
        // 滚动到底部
        ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
    }

    // 添加结果项
    for (int i = 0; i < this->testItemList.at(testItemIndex).resultList.size(); ++i)
    {
        TestResultInterface *item = new TestResultInterface(ui->scrollAreaWidgetContents_Result);
        item->setUi(i, this->testItemList.at(testItemIndex).resultList.at(i));
        testResultInterfaceList.append(item);
        ui->vloResultItem->insertWidget(ui->vloResultItem->count() - 2, item);
        // 滚动到底部
        ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
    }
}

void MainWindow::on_btnAddResultItem_clicked()
{
    // 往 scrollArea_Result 添加一个 TestResult
    TestResultInterface *item = new TestResultInterface(ui->scrollAreaWidgetContents_Result);
    item->setIndex(testResultInterfaceList.size());
    testResultInterfaceList.append(item);
    ui->vloResultItem->insertWidget(ui->vloResultItem->count() - 2, item);
    // 滚动到底部
    ui->scrollArea_Result->verticalScrollBar()->setValue(ui->scrollArea_Result->verticalScrollBar()->maximum());
}
