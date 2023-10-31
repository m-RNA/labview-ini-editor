/*
 * @Author: 陈俊健
 * @Date: 2023-10-28 19:35:01
 * @LastEditors: 陈俊健
 * @LastEditTime: 2023-10-29 20:43:40
 * @FilePath: \LabViewIniEditer\main.cpp
 * @Description:
 *
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved.
 */
#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QStringList>

#include "analysis_ini.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();

    QString str = "68 [:] [&]74 11[|] 16 01 00 04 16<HEX>:AT[:]+ADB<\r\n>:68 68 68:AT[:]ABC";
    QStringList qwe = splitStringSquareBrackets(str, ':');
    for (int i = 0; i < qwe.size(); i++)
    {
        qDebug() << qwe.at(i);
    }
    return 0;
}
