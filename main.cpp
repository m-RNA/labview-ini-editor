/*
 * @Author: 陈俊健
 * @Date: 2023-10-28 19:35:01
 * @LastEditors: 陈俊健
 * @LastEditTime: 2023-10-29 14:21:42
 * @FilePath: \LabViewIniEditer\main.cpp
 * @Description: 
 * 
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved. 
 */
#include "mainwindow.h"

#include <QApplication>

#include "analysis_ini.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
