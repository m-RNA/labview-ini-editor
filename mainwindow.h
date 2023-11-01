/*
 * @Author: 陈俊健
 * @Date: 2023-10-28 19:35:01
 * @LastEditors: 陈俊健
 * @LastEditTime: 2023-10-31 22:36:50
 * @FilePath: \LabViewIniEditer\mainwindow.h
 * @Description: 
 * 
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved. 
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "analysis_ini.h"
#include <QMainWindow>
#include <QTreeWidgetItem>


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

    void on_btnAddTestItem_clicked();

    void on_actOpenIni_triggered();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_btnAddResultItem_clicked();

private:
    Ui::MainWindow *ui;
    QString fileNameProtocol = "";
    QString fileNameConfig = "";

    QVector<TestItem> testItemList = {};
    QVector<QWidget *> testCmdInterfaceList = {};
    QVector<QWidget *> testResultInterfaceList = {};
};
#endif // MAINWINDOW_H
