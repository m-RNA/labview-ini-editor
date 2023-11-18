/*
 * @Author: 陈俊健
 * @Date: 2023-10-28 19:35:01
 * @LastEditors: m-RNA m-RNA@qq.com
 * @LastEditTime: 2023-11-17 01:53:55
 * @FilePath: \LabViewIniEditor\main.cpp
 * @Description:
 *
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved.
 */
#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QStringList>

#include "analysis_ini.h"
#include "inisettings.h"
void testIniSettings();

int main(int argc, char *argv[])
{
    testIniSettings();
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();

    // QString str = "68 [:] [&]74 11[|] 16 01 00 04 16<HEX>:AT[:]+ADB<\r\n>:68 68 68:AT[:]ABC";
    // QStringList qwe = splitStringSquareBrackets(str, ':');
    // for (int i = 0; i < qwe.size(); i++)
    // {
    //     qDebug() << qwe.at(i);
    // }
    // return 0;
}

void testIniSettings()
{
    IniSettings ini("test.ini", QTextCodec::codecForName("GB2312"));
    qDebug() << ini.fileName();
    qDebug() << ini.isLoad();

    QStringList groups = ini.childGroups();
    for (int i = 0; i < groups.size(); i++)
    {
        qDebug() << groups.at(i);
    }
    qDebug() << "---------------------";
    QStringList keys = ini.childKeys();
    for (int i = 0; i < keys.size(); i++)
    {
        qDebug() << keys.at(i);
    }
    qDebug() << "---------------------";
    QStringList allKeys = ini.allKeys();
    for (int i = 0; i < allKeys.size(); i++)
    {
        qDebug() << allKeys.at(i);
    }
    qDebug() << "---------------------";

    if (groups.isEmpty())
    {
        qDebug() << "groups is empty";
        return;
    }
    ini.beginGroup(groups.at(0));
    keys = ini.childKeys();
    for (int i = 0; i < keys.size(); i++)
    {
        qDebug() << keys.at(i);
    }
    qDebug() << "---------------------";
    allKeys = ini.allKeys();
    for (int i = 0; i < allKeys.size(); i++)
    {
        qDebug() << allKeys.at(i);
    }
    qDebug() << "---------------------";
}