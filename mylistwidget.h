/*
 * @Author: 陈俊健
 * @Date: 2024-06-10 17:48:01
 * @LastEditors: 陈俊健
 * @LastEditTime: 2024-06-10 17:49:36
 * @FilePath: \LabViewIniEditor2024\myListWidget.hpp
 * @Description:
 *
 * Copyright (c) 2024 by Chenjunjian, All Rights Reserved.
 */
#pragma once
#include <QListWidget>

class MyListWidget : public QListWidget
{
    Q_OBJECT
public:
    MyListWidget(QWidget *parent = nullptr)
        : QListWidget(parent)
    {
    }

signals:
    void itemsReordered(); // 自定义信号，表示项已重新排序或数据变更

protected:
    void dropEvent(QDropEvent *event) override
    {
        QListWidget::dropEvent(event);

        // 在这里处理拖拽逻辑...

        // 数据处理完毕，发射信号
        emit itemsReordered();
    }
};