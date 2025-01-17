﻿/*
 * @Author: 陈俊健
 * @Date: 2023-10-28 19:35:01
 * @LastEditors: 陈俊健
 * @LastEditTime: 2024-07-03 04:11:38
 * @FilePath: \LabViewIniEditor2024\mainwindow.h
 * @Description:
 *
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "labviewsetting.h"
#include "test_item_interface.h"
#include "test_result_interface.h"
#include <QDockWidget>
#include <QListWidgetItem>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_actOpenIni_triggered();
    void on_actLoadIni_triggered();
    void on_actSave_triggered();

    void on_actSSCOM_triggered();
    void on_actBSP_triggered();

    void on_actAbout_triggered();
    void on_actDocOnline_triggered();
    void on_actUpdateVersion_triggered();

    void on_actSownAllTestItem_triggered(bool checked);

    void on_dwlTestItemAll_visibilityChanged(bool visible);

    void on_lwTestItemConfig_itemSelectionChanged();
    void on_lwTestItemConfigKey_itemSelectionChanged();
    void on_lwTestItemAll_itemSelectionChanged();

    void on_leTestItemName_editingFinished();
    void on_spbxRepeatTimes_valueChanged(int arg1);
    void on_spbxDataSize_valueChanged(int arg1);
    void on_spbxDecPlace_valueChanged(int arg1);
    void on_spbxByteOrder_currentTextChanged(const QString &arg1);
    void on_cbSign_currentTextChanged(const QString &arg1);

    void on_btnAddTestICmd_clicked();
    void on_btnCopyTestICmd_clicked();
    void on_btnRemoveTestICmd_clicked();

    void on_btnAddTestResult_clicked();
    void on_btnCopyTestIResult_clicked();
    void on_btnRemoveTestIResult_clicked();

    void on_btnAddTestItemAll_clicked();
    void on_btnCopyTestItemAll_clicked();
    void on_btnRemoveTestItemAll_clicked();

    void onTestCmdReordered(void);     // 测试项命令重排序
    void onTestResultReordered(void);  // 测试项结果重排序
    void onTestItemAllReordered(void); // 测试项额外项重排序

    // void on_lwlTestItemConfig_itemClicked(QListWidgetItem *item);
    // void on_lwlTestItemAll_itemClicked(QListWidgetItem *item);
    // void on_lwlTestItemConfigKey_itemClicked(QListWidgetItem *item);

    void on_lwTestResultItem_IndexChanged(int index);

    void on_actNextTestItem_triggered();

    void on_actPreviousTestItem_triggered();

private:
    Ui::MainWindow *ui;
    QString title = ""; // 窗口标题

    QString filePathProtocol = ""; // 协议文件名
    QString filePathConfig = "";   // 配置文件名
    QString fileNameProtocol = ""; // 协议文件路径
    QString fileNameConfig = "";   // 配置文件路径
    bool isNeedConfigFile = false; // 是否需要导入配置文件

    QString uiLwTestItemName = ""; // 旧的测试项名称

    QList<TestItem> *testItemListAddr = nullptr; // 测试项列表
    QList<ConfigItem> configItemList = {};       // 配置项列表

    LabViewSetting *labviewSetting = nullptr; // LabView设置

    int uiLwTestItemAllRow = -1; // 全部测试项列表行号

    TestItem *getTestItemCurrent(void); // 获取当前测试项

    void uiUpdateTestCmd(const QVector<TestCmd> &cmdList);          // 更新测试命令界面
    void uiUpdateTestResult(const QVector<TestResult> &resultList); // 更新测试结果界面
    void uiUpdateTestItem(QString testItemName);                    // 更新测试项界面
    void uiUpdateTestItemList();                                    // 更新测试项列表界面
    void uiUpdateConfigKey(QString name, bool needConfigKeyTestItemUpdate);

    void uiClearAll(); // 清空界面

    void uiAddTestCmd(TestItemInterface *item);  // 界面添加测试命令
    void uiAddResult(TestResultInterface *item); // 界面添加测试结果

    void uiInsertTestCmd(int index, TestItemInterface *item);  // 界面插入测试命令
    void uiInsertResult(int index, TestResultInterface *item); // 界面插入测试结果

    void uiRemoveTestCmd(int index); // 界面删除测试命令
    void uiRemoveResult(int index);  // 界面删除测试结果

    void insertTestCmd(QVector<TestCmd> &cmdList, const TestCmd &cmd, int cmdIndex); // 插入测试命令

    void updateTestCmdListFromUi(QVector<TestCmd> &cmdList);          // 从界面更新测试命令列表
    void updateTestResultListFromUi(QVector<TestResult> &resultList); // 从界面更新测试结果列表
    void updateTestItemFromUi(TestItem *testItem);                    // 从界面更新测试项
    void updateTestItemListFromUi();                                  // 从界面更新测试项列表
};
#endif // MAINWINDOW_H
