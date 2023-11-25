﻿/*
 * @Author: m-RNA m-RNA@qq.com
 * @Date: 2023-11-16 23:26:05
 * @LastEditors: m-RNA m-RNA@qq.com
 * @LastEditTime: 2023-11-19 18:19:30
 * @FilePath: \LabViewIniEditor\inisettings.h
 * @Description:
 */
#ifndef INISETTINGS_H
#define INISETTINGS_H
#if _MSC_VER >= 1600 // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextCodec>

class IniSettings : public QObject
{
    Q_OBJECT
public:
    explicit IniSettings(const QString &fileName, QTextCodec *codec = QTextCodec::codecForName("UTF-8"),
                         QObject *parent = nullptr);
    ~IniSettings();
    bool isLoad(); // 是否加载成功

    void clear(); // 清空

    void beginGroup(const QString &prefix); // 设置组
    void endGroup();                        // 结束组
    QString group() const;                  // 获取当前组

    QStringList childGroups() const; // 获取子组
    QStringList childKeys() const;   // 获取子键

    QStringList allKeys() const;                      // 获取所有键
    void remove(const QString &key);                  // 删除键

    bool contains(const QString &key) const; // 是否包含键

    QString value(const QString &key, const QString &defaultValue = QString()) const; // 获取值
    void setValue(const QString &key, const QString &value);                          // 设置值

    QString fileName() const; // 获取文件名

private:
    bool loadFile();
    bool loadFile(const QString &fileName);

    bool saveFile();
    bool saveFile(const QString &fileName);

    bool m_isLoad = false;

    QString m_fileName = "";       // 文件名
    QString m_group = "";          // 当前组
    QTextCodec *m_codec = nullptr; // 编码方式

    QMap<QString, QString> m_mapKey;      // 键值对（不包含组）
    QStringList m_mapGroup;               // 组
    QStringList m_mapGroupNew;            // 组默认值
    QMap<QString, QString> m_mapGroupKey; // 组键
};

#endif // INISETTINGS_H
