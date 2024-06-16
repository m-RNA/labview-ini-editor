/*
 * @Author: 陈俊健
 * @Date: 2023-10-28 19:35:01
 * @LastEditors: m-RNA m-RNA@qq.com
 * @LastEditTime: 2023-11-19 17:39:03
 * @FilePath: \LabViewIniEditor\main.cpp
 * @Description:
 *
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved.
 */
#include "mainwindow.h"

#include <QApplication>
#include <QDebug>
#include <QStringList>
#include "labviewsetting.h"
#include "inisettings.h"
#include <QFontDatabase>

#if _MSC_VER >=1600    // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

void testReadIniSettings();
void testWriteIniSetting();

int main(int argc, char *argv[])
{
//    testWriteIniSetting();
//    testReadIniSettings();
    QApplication a(argc, argv);
    MainWindow w;
    QFontDatabase::addApplicationFont(":/Resource/alibabapuhtiBOLD.TTF");
    QFontDatabase::addApplicationFont(":/Resource/alibabapuhtiHEAVY.TTF");
    QFontDatabase::addApplicationFont(":/Resource/alibabapuhtiLIGHT.TTF");
    QFontDatabase::addApplicationFont(":/Resource/alibabapuhtiMEDIUM.TTF");
    QFontDatabase::addApplicationFont(":/Resource/alibabapuhtiREGULAR.TTF");

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

void testReadIniSettings()
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

void testWriteIniSetting()
{
    IniSettings ini("testWrite.ini", QTextCodec::codecForName("GB2312"));
    qDebug() << ini.fileName();
    qDebug() << ini.isLoad();
    ini.beginGroup("test");
    ini.setValue("test", "test");
    ini.setValue("test1", "test1");

    ini.beginGroup("中文");
    ini.setValue("中文", "中文");
    ini.setValue("中文1", "中文1");


    ini.endGroup();
}
