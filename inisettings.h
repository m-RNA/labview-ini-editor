/*
 * @Author: m-RNA m-RNA@qq.com
 * @Date: 2023-11-16 23:26:05
 * @LastEditors: 陈俊健
 * @LastEditTime: 2024-06-17 10:16:24
 * @FilePath: \LabViewIniEditor2024\inisettings.h
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

    void removeGroup(const QString &group);                             // 删除组
    void renameGroup(const QString &oldGroup, const QString &newGroup); // 重命名组

    QStringList childGroups() const;    // 获取子组
    QStringList childKeys() const;      // 获取子键
    QStringList childKeysOrder() const; // 获取子键（按照文件顺序）
    QStringList allKeys() const;        // 获取所有键
    QStringList allKeysOrder() const;   // 获取所有键（按照文件顺序）

    void remove(const QString &key);         // 删除键
    bool contains(const QString &key) const; // 是否包含键

    QString value(const QString &key, const QString &defaultValue = QString()) const; // 获取值
    void setValue(const QString &key, const QString &value);                          // 设置值

    QString fileName() const; // 获取文件名

    bool backupFile(const QString &backupPath = ""); // 备份文件

    bool saveFile();
    bool saveFileOrderKey(const QStringList &keyOrder,
                          const QString &groupOrder = ""); // 按照一定顺序保存文件

private:
    bool loadFile();
    bool loadFile(const QString &fileName);

    bool m_isLoad = false;

    QString m_fileName = "";       // 文件名
    QString m_group = "";          // 当前组
    QTextCodec *m_codec = nullptr; // 编码方式

    QMap<QString, QString> m_mapKey;      // 键值对（不包含组）
    QStringList m_mapGroup;               // 组
    QStringList m_mapGroupNew;            // 新组
    QMap<QString, QString> m_mapGroupKey; // 组键
};

#endif // INISETTINGS_H
