/*
 * @Author: 陈俊健
 * @Date: 2023-10-28 19:35:01
 * @LastEditors: 陈俊健
 * @LastEditTime: 2023-10-31 22:36:50
 * @FilePath: \LabViewIniEditor\mainwindow.h
 * @Description: 
 * 
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved. 
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "analysis_ini.h"
#include "test_item_interface.h"
#include <QMainWindow>
#include <QListWidgetItem>
#include <QDockWidget>


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

    void addTestCmdInterface(TestItemInterface *item);

    void insertTestCmd(int itemListIndex, int cmdIndex, const TestCmd &cmd);

    void insertTestCmdInterface(int index, TestItemInterface *item);

private slots:

    void on_actOpenIni_triggered();

    void on_lwlTestItemPool_itemClicked(QListWidgetItem *item);

    void on_lwlTestItemList_itemClicked(QListWidgetItem *item);

    void on_dwTestPool_visibilityChanged(bool visible);

    void on_actSownExtTestItem_triggered(bool checked);

    void on_actAbout_triggered();


    void on_btnAddTestICmd_clicked();

    void on_btnCopyTestICmd_clicked();

    void on_leTestItemName_editingFinished();

    void on_lwTestCmd_indexesMoved(const QModelIndexList &indexes);

private:
    Ui::MainWindow *ui;
    QString fileNameProtocol = "";
    QString fileNameConfig = "";

    QString leTestItemName_Old = "";

    QVector<TestItem> testItemList = {};
    QVector<ConfigItem> configItemList = {};
    QVector<TestItemInterface *> testCmdInterfaceList = {};
    QVector<QWidget *> testResultInterfaceList = {};

    int getTestItemIndex(const QString &name);
};
#endif // MAINWINDOW_H
