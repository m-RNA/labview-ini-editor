/*
 * @Author: 陈俊健
 * @Date: 2023-10-28 19:35:01
 * @LastEditors: 陈俊健
 * @LastEditTime: 2023-10-30 00:49:44
 * @FilePath: \LabViewIniEditer\mainwindow.cpp
 * @Description:
 *
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved.
 */
#include "mainwindow.h"
#include "test_item_interface.h"
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
    item->setIndex(testItemInterfaceList.size());
    testItemInterfaceList.append(item);
    ui->vloTestItem->insertWidget(ui->vloTestItem->count() - 2, item);
    // 滚动到底部
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
}

// 原文链接：https://blog.csdn.net/zhang_ze1234/article/details/107682660
void MainWindow::on_actOpenIni_triggered()
{
    QString pathName;
    //"."表示在当前工作路径下寻找，“Ini files(*.ini)”表示指定
    pathName = QFileDialog::getOpenFileName(this, "打开文件", "./", "Ini files(*.ini)");
    qDebug() << pathName;

    this->testItemList.clear();                  // 清空
    this->testItemList = analysis_ini(pathName); // 解析 ini 文件

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

    // 清空测试项
    for (int i = 0; i < testItemInterfaceList.size(); ++i)
    {
        ui->vloTestItem->removeWidget(testItemInterfaceList.at(i));
        testItemInterfaceList.at(i)->deleteLater();
    }
    testItemInterfaceList.clear();

    for (int i = 0; i < this->testItemList.at(testItemIndex).cmdList.size(); ++i)
    {
        TestItemInterface *item = new TestItemInterface(ui->scrollAreaWidgetContents);
        item->setTestItem(i, this->testItemList.at(testItemIndex).cmdList.at(i));
        testItemInterfaceList.append(item);
        ui->vloTestItem->insertWidget(ui->vloTestItem->count() - 2, item);
        // 滚动到底部
        ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
    }
}
