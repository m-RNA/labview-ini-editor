/*
 * @Author: 陈俊健
 * @Date: 2023-10-29 13:35:55
 * @LastEditors: 陈俊健
 * @LastEditTime: 2023-10-29 13:44:26
 * @FilePath: \LabViewIniEditer\analysis_ini.h
 * @Description:
 *
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved.
 */
#ifndef __ANALYSIS_INI_H__
#define __ANALYSIS_INI_H__

#include <QString>
#include <QStringList>
#include <QVector>



/**
 * @brief 解析ini文件，将每个测试项的内容保存到 QVector<QStringList> 中
 * @param fileName ini文件的路径
 * @return QVector<QStringList> 每个测试项的内容
 */
QVector<QStringList> analysis_ini(const QString fileName);

#endif
