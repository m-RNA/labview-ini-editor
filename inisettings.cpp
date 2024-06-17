#include "inisettings.h"
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>

#define SUBGROUP_SEPARATOR "/"
#if _MSC_VER >= 1600 // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

IniSettings::IniSettings(const QString &fileName, QTextCodec *codec, QObject *parent)
    : QObject(parent)
{
    m_fileName = fileName;         // 保存文件名
    m_codec = codec;               // 保存编码方式
    m_isLoad = loadFile(fileName); // 加载文件
}

IniSettings::~IniSettings()
{
    // saveFile();
}

bool IniSettings::isLoad() { return m_isLoad; }

bool IniSettings::loadFile() { return loadFile(m_fileName); }

bool IniSettings::loadFile(const QString &fileName)
{
    QFile file(fileName); // 新建文件对象
    if (file.exists() == false)
    {
        // 文件不存在，创建文件
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            return false;
        }
        file.close();
        return true;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) // 只读、文本方式打开
    {
        return false;
    }

    m_mapKey.clear();
    m_mapGroup.clear();
    m_mapGroupKey.clear();

    QString line;  // 读取的每一行数据
    QString group; // 当前组
    QString key;   // 等号前面的键
    QString value; // 等号后面的值
    int index;     // 等号索引

    QTextStream in(&file); // 新建文本流对象
    in.setCodec(m_codec);  // 设置编码方式

    while (!in.atEnd()) // 按行读取文件内容
    {
        line = in.readLine().trimmed(); // 读取一行并去除两端的空白

        if (line.isEmpty() || line.startsWith(";")) // 跳过注释、空行
        {
            continue;
        }

        if (line.startsWith("[") && line.endsWith("]")) // 组
        {
            group = line.mid(1, line.length() - 2);
            m_mapGroup.append(group);
            continue;
        }

        index = line.indexOf("="); // 等号索引
        if (index < 0)
        {
            continue;
        }

        key = line.left(index).trimmed();      // 键
        value = line.mid(index + 1).trimmed(); // 值

        if (group.isEmpty())
        {
            m_mapKey.insert(key, value); // 键值对
        }
        else
        {
            m_mapGroupKey.insert(group + SUBGROUP_SEPARATOR + key, value); // 组键值对
        }
    }
    file.close(); // 关闭文件
    return true;
}

/**
 * @brief 清空
 */
void IniSettings::clear()
{
    m_mapKey.clear();
    m_mapGroup.clear();
    m_mapGroupKey.clear();
    m_mapGroupNew.clear();
}

/**
 * @brief 设置组
 */
void IniSettings::beginGroup(const QString &prefix)
{
    m_group = prefix;
    if (!m_mapGroup.contains(prefix))
    {
        m_mapGroupNew.append(prefix);
    }
}

/**
 * @brief 结束组
 */
void IniSettings::endGroup() { m_group = ""; }

/**
 * @brief 获取组
 */
QString IniSettings::group() const { return m_group; }

/**
 * @brief 删除组
 * @param group 组名
 */
void IniSettings::removeGroup(const QString &group)
{
    if (m_mapGroup.contains(group))
    {
        m_mapGroup.removeOne(group);
    }
    else if (m_mapGroupNew.contains(group))
    {
        m_mapGroupNew.removeOne(group);
    }
    else
        return;
    foreach (QString key, m_mapGroupKey.keys())
    {
        if (key.startsWith(group + SUBGROUP_SEPARATOR))
        {
            m_mapGroupKey.remove(key);
        }
    }
}

/**
 * @brief 重命名组
 * @param oldGroup 原来的组
 * @param newGroup 新的组
 */
void IniSettings::renameGroup(const QString &oldGroup, const QString &newGroup)
{
    if (m_mapGroup.contains(oldGroup))
    {
        m_mapGroup.removeOne(oldGroup);
        m_mapGroup.append(newGroup);
    }
    else if (m_mapGroupNew.contains(oldGroup))
    {
        m_mapGroupNew.removeOne(oldGroup);
        m_mapGroupNew.append(newGroup);
    }
    else
        return;
    foreach (QString key, m_mapGroupKey.keys())
    {
        if (key.startsWith(oldGroup + SUBGROUP_SEPARATOR))
        {
            QString childKey = key.mid(oldGroup.length() + 1);
            QString value = m_mapGroupKey.value(key);
            m_mapGroupKey.remove(key);
            m_mapGroupKey.insert(newGroup + SUBGROUP_SEPARATOR + childKey, value);
        }
    }
}

/**
 * @brief 获取子组
 */
QStringList IniSettings::childGroups() const
{
    if (m_group.isEmpty())
    {
        return m_mapGroup;
    }

    QStringList list; // 子组列表
    foreach (QString key, m_mapGroupKey.keys())
    {
        if (key.startsWith(m_group + SUBGROUP_SEPARATOR))
        {
            QString group = key.mid(m_group.length() + 1);
            group = group.left(group.indexOf(SUBGROUP_SEPARATOR));
            if (!list.contains(group))
            {
                list.append(group);
            }
        }
    }
    return list;
}

/**
 * @brief 获取子键
 */
QStringList IniSettings::childKeys() const
{
    QStringList list; // 子键列表
    foreach (QString key, m_mapGroupKey.keys())
    {
        if (key.startsWith(m_group + SUBGROUP_SEPARATOR))
        {
            QString childKey = key.mid(m_group.length() + 1);
            childKey = childKey.mid(childKey.indexOf(SUBGROUP_SEPARATOR) + 1);
            if (!list.contains(childKey))
            {
                list.append(childKey);
            }
        }
    }
    return list;
}

/**
 * @brief 获取所有键
 */
QStringList IniSettings::allKeys() const
{
    QStringList list = {};
    foreach (QString key, m_mapGroupKey.keys())
    {
        if (key.startsWith(m_group + SUBGROUP_SEPARATOR))
        {
            QString childKey = key.mid(m_group.length() + 1);
            childKey = childKey.mid(childKey.indexOf(SUBGROUP_SEPARATOR) + 1);
            if (!list.contains(childKey))
            {
                list.append(childKey);
            }
        }
    }
    return list;
}

/**
 * @brief 删除键
 */
void IniSettings::remove(const QString &key)
{
    if (m_group.isEmpty())
    {
        m_mapKey.remove(key);
    }
    else
    {
        m_mapGroupKey.remove(m_group + SUBGROUP_SEPARATOR + key);
    }
}

/**
 * @brief 是否包含键
 */
bool IniSettings::contains(const QString &key) const
{
    if (m_group.isEmpty())
    {
        return m_mapKey.contains(key);
    }
    else
    {
        return m_mapGroupKey.contains(m_group + SUBGROUP_SEPARATOR + key);
    }
}

/**
 * @brief 获取值
 */
QString IniSettings::value(const QString &key, const QString &defaultValue) const
{
    if (m_group.isEmpty())
    {
        return m_mapKey.value(key, defaultValue);
    }
    else
    {
        return m_mapGroupKey.value(m_group + SUBGROUP_SEPARATOR + key, defaultValue);
    }
}

/**
 * @brief 设置值
 */
void IniSettings::setValue(const QString &key, const QString &value)
{
    if (m_group.isEmpty())
    {
        m_mapKey.insert(key, value);
    }
    else
    {
        m_mapGroupKey.insert(m_group + SUBGROUP_SEPARATOR + key, value);
    }
}

/**
 * @brief 获取文件名
 */
QString IniSettings::fileName() const { return m_fileName; }

bool IniSettings::backupFile(const QString &filePathBackup)
{
    if (m_fileName.isEmpty())
    {
        return false;
    }
    QFile file(m_fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }
    QString backupPath = filePathBackup.mid(0, filePathBackup.lastIndexOf("/"));
    QDir dir;
    if (!dir.exists(backupPath))
    {
        dir.mkpath(backupPath);
    }

    // QFile fileBackup
    QFile fileBackup(filePathBackup);
    if (!fileBackup.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "打开文件失败";
        return false;
    }

    // 复制原文件内容到新文件
    QTextStream in(&file);
    in.setCodec(m_codec); // 设置编码方式

    QTextStream out(&fileBackup);
    out.setCodec(m_codec);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        out << line << "\n";
    }
    fileBackup.flush();
    fileBackup.close();
    file.close(); // 关闭文件
    return true;
}

/**
 * @brief 保存文件
 */
bool IniSettings::saveFile()
{
    if (m_fileName.isEmpty())
    {
        return false;
    }
    // 创建新文件，来备份原文件，时间
    QString backupPath = m_fileName.mid(0, m_fileName.lastIndexOf("/")) + "/Backup/";
    backupPath = backupPath + m_fileName.right(m_fileName.length() - m_fileName.lastIndexOf("/") - 1);
    backupPath = backupPath + QDateTime::currentDateTime().toString(".yyyyMMddhhmmss") + ".ini";
    if (!backupFile(backupPath))
    {
        return false;
    }

    // 写入新文件
    QFile file(m_fileName);
    QFile fileBackup(backupPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }
    if (!fileBackup.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }
    QTextStream in(&fileBackup);
    in.setCodec(m_codec); // 设置编码方式
    QTextStream out(&file);
    out.setCodec(m_codec);

    // 在原文件修改，不是重新写入，同时保留原文件中的注释
    QString line;                 // 读取的每一行数据
    static int lineNullCount = 0; // 空行计数
    static QString group;         // 当前组
    QString key;                  // 等号前面的键
    QString value;                // 等号后面的值
    int index;                    // 等号索引

    while (!in.atEnd()) // 按行读取文件内容
    {
        line = in.readLine().trimmed(); // 读取一行并去除两端的空白

        // 空行处理
        if (line.isEmpty())
        {
            lineNullCount++;
            if (lineNullCount > 2) // 最多连续两个空行
                continue;

            out << line << "\n";
            continue;
        }
        lineNullCount = 0;

        // 注释处理
        if (line.startsWith(";"))
        {
            out << line << "\n";
            continue;
        }
        // 跳过特殊选项 [测试项名称] [MES链接] [测试开始] [测试结束]
        if (line.startsWith("[测试项名称]") || line.startsWith("[MES链接]") || line.startsWith("[测试开始]")
            || line.startsWith("[测试结束]"))
        {
            do
            {
                out << line << "\n";
                if ((!in.atEnd()))
                    line = in.readLine().trimmed(); // 读取一行并去除两端的空白
                else
                    goto FILE_END;
            } while ((!(line.startsWith("[")))
                     || ((line.startsWith("[测试项名称]")) || (line.startsWith("[MES链接]"))
                         || (line.startsWith("[测试开始]")) || (line.startsWith("[测试结束]"))));
        }

        // 组处理
        if (line.startsWith("[") && line.endsWith("]")) // 组
        {
            group = line.mid(1, line.length() - 2);
            out << line << "\n";
            continue;
        }

        index = line.indexOf("="); // 等号索引
        if (index < 0 || line.startsWith(";"))
        {
            out << line << "\n";
            continue;
        }

        key = line.left(index).trimmed(); // 键
        if (group.isEmpty())
        {
            value = m_mapKey.value(key, ""); // 值
        }
        else
        {
            value = m_mapGroupKey.value(group + SUBGROUP_SEPARATOR + key, ""); // 值
        }
        out << key << " = " << value << "\n";
    }
FILE_END:
    // 新增组
    foreach (QString group, m_mapGroupNew)
    {
        out << "\n";
        out << "[" << group << "]\n";
        foreach (QString key, m_mapGroupKey.keys())
        {
            if (key.startsWith(group + SUBGROUP_SEPARATOR))
            {
                QString childKey = key.mid(group.length() + 1);
                childKey = childKey.mid(childKey.indexOf(SUBGROUP_SEPARATOR) + 1);
                out << childKey << " = " << m_mapGroupKey.value(key) << "\n";
            }
        }
    }

    file.flush();       // 刷新文件
    file.close();       // 关闭文件
    fileBackup.close(); // 关闭文件
    return true;
}

bool IniSettings::saveFileOrderKey(const QStringList &keyOrder, const QString &groupOrder)
{
    if (m_fileName.isEmpty())
    {
        return false;
    }
    // 创建新文件，来备份原文件，时间
    QString backupPath = m_fileName.mid(0, m_fileName.lastIndexOf("/")) + "/Backup/";
    backupPath = backupPath + m_fileName.right(m_fileName.length() - m_fileName.lastIndexOf("/") - 1);
    backupPath = backupPath + QDateTime::currentDateTime().toString(".yyyyMMddhhmmss") + ".ini";
    if (!backupFile(backupPath))
    {
        return false;
    }

    // 写入新文件
    QFile file(m_fileName);
    QFile fileBackup(backupPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }
    if (!fileBackup.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }
    QTextStream in(&fileBackup);
    in.setCodec(m_codec); // 设置编码方式
    QTextStream out(&file);
    out.setCodec(m_codec);

    // 在原文件修改，不是重新写入，同时保留原文件中的注释
    QString line;  // 读取的每一行数据
    QString key;   // 等号前面的键
    QString value; // 等号后面的值

    // 新增组
    foreach (QString group, m_mapGroup)
    {
        out << "\n";
        out << "[" << group << "]\n";
        QStringList keyList = m_mapGroupKey.keys();
        // 按照顺序写入
        if (groupOrder == group || groupOrder == "")
        {
            foreach (QString key, keyOrder)
            {
                QString keyTemp = group + SUBGROUP_SEPARATOR + key;
                if (keyList.contains(keyTemp))
                {
                    keyList.removeOne(keyTemp);
                    out << key << " = " << m_mapGroupKey.value(keyTemp) << "\n";
                }
            }
        }

        // 剩余的写入
        foreach (QString key, keyList)
        {
            if (key.startsWith(group + SUBGROUP_SEPARATOR))
            {
                QString childKey = key.mid(group.length() + 1);
                childKey = childKey.mid(childKey.indexOf(SUBGROUP_SEPARATOR) + 1);
                out << childKey << " = " << m_mapGroupKey.value(key) << "\n";
            }
        }
    }

    file.flush();       // 刷新文件
    file.close();       // 关闭文件
    fileBackup.close(); // 关闭文件
    return true;
}
