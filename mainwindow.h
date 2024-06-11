/*
 * @Author: 陈俊健
 * @Date: 2023-10-28 19:35:01
 * @LastEditors: 陈俊健
 * @LastEditTime: 2024-06-12 01:45:18
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

    void on_dwTestPool_visibilityChanged(bool visible);

    void on_actSownExtTestItem_triggered(bool checked);

    void on_actAbout_triggered();

    void on_btnAddTestICmd_clicked();

    void on_btnCopyTestICmd_clicked();

    void on_btnRemoveTestICmd_clicked();

    void on_leTestItemName_editingFinished();

    void on_actSave_triggered();

    void on_lwlTestItemList_itemSelectionChanged();

    void on_lwlTestItemPool_itemSelectionChanged();

    void on_spbxRepeatTimes_valueChanged(int arg1);

    void on_spbxDataSize_valueChanged(int arg1);

    void on_spbxDecPlace_valueChanged(int arg1);

    void on_spbxByteOrder_currentTextChanged(const QString &arg1);

    void on_cbSign_currentTextChanged(const QString &arg1);

    void onTestCmdReordered(void);

    void onTestResultReordered(void);

private:
    Ui::MainWindow *ui;
    QString fileNameProtocol = ""; // 协议文件名
    QString fileNameConfig = "";   // 配置文件名

    QString leTestItemName_Old = ""; // 旧的测试项名称

    QList<TestItem> testItemList = {}; // 测试项列表
    // QList<ConfigItem> configItemList = {};                // 配置项列表
    QList<TestItemInterface *> testCmdInterfaceList = {}; // 测试项命令界面列表
    QList<QWidget *> testResultInterfaceList = {};        // 测试项结果界面列表

    LabViewSetting *labviewSetting; // LabView设置

    int getTestItemIndex(const QString &name); // 获取测试项序号
    TestItem *getTestItemCurrent(void);        // 获取当前测试项

    void updateTestItemUi();     // 更新测试项界面
    void updateTestItemListUi(); // 更新测试项列表界面

    void updateTestItemFromUi(TestItem *testItem);
    void updateTestItemListFromUi(); // 从界面更新测试项列表

    void loadTestCmdUi(const QVector<TestCmd> &cmdList);
    void loadTestResultUi(const QVector<TestResult> &resultList);
    void loadTestItemUi(QListWidgetItem *item);

    void insertTestCmd(QVector<TestCmd> &cmdList, const TestCmd &cmd, int cmdIndex);
    void updateTestCmdListFromUi(QVector<TestCmd> &cmdList);
    void updateTestResultListFromUi(QVector<TestResult> &resultList);

    void addTestCmdInterface(TestItemInterface *item);
    void insertTestCmdInterface(int index, TestItemInterface *item);

    void addResultInterface(TestResultInterface *item);
    void insertResultInterface(int index, TestResultInterface *item);
};
#endif // MAINWINDOW_H
