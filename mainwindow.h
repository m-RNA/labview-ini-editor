/*
 * @Author: 陈俊健
 * @Date: 2023-10-28 19:35:01
 * @LastEditors: m-RNA m-RNA@qq.com
 * @LastEditTime: 2023-11-27 01:11:39
 * @FilePath: \LabViewIniEditor\mainwindow.h
 * @Description:
 *
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "analysis_ini.h"
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

    void on_leTestItemName_editingFinished();

    void on_actSave_triggered();

    void on_lwlTestItemList_itemSelectionChanged();

    void on_lwlTestItemPool_itemSelectionChanged();

    void on_btnRemoveTestICmd_clicked();

private:
    Ui::MainWindow *ui;
    QString fileNameProtocol = "";
    QString fileNameConfig = "";

    QString leTestItemName_Old = "";

    QList<TestItem> testItemList = {};
    QList<ConfigItem> configItemList = {};
    QList<TestItemInterface *> testCmdInterfaceList = {};
    QList<QWidget *> testResultInterfaceList = {};

    LabViewSetting *labviewSetting;

    int getTestItemIndex(const QString &name);

    void updateTestItemUi();
    void updateTestItemListUi();

    void loadTestCmdUi(const QVector<TestCmd> &cmdList);
    void loadTestResultUi(const QVector<TestResult> &resultList);
    void loadTestItemUi(QListWidgetItem *item);

    void insertTestCmd(QVector<TestCmd> &cmdList, const TestCmd &cmd, int cmdIndex);

    void addTestCmdInterface(TestItemInterface *item);
    void insertTestCmdInterface(int index, TestItemInterface *item);

    void addResultInterface(TestResultInterface *item);
    void insertResultInterface(int index, TestResultInterface *item);
};
#endif // MAINWINDOW_H
