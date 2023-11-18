#include "inisettings.h"
#include <QFile>
#include <QTextStream>

#define SUBGROUP_SEPARATOR "/"

IniSettings::IniSettings(const QString &fileName, QTextCodec *codec, QObject *parent)
    : QObject(parent)
{
    m_fileName = fileName;         // 保存文件名
    m_codec = codec;               // 保存编码方式
    m_isLoad = loadFile(fileName); // 加载文件
}

IniSettings::~IniSettings() { saveFile(); }

bool IniSettings::isLoad() { return m_isLoad; }

bool IniSettings::loadFile() { return loadFile(m_fileName); }

bool IniSettings::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    m_mapKey.clear();
    m_mapGroup.clear();
    m_mapGroupKey.clear();
    m_mapGroupKeyDefault.clear();

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
    m_mapGroupKeyDefault.clear();
}

/**
 * @brief 设置组
 */
void IniSettings::beginGroup(const QString &prefix) { m_group = prefix; }

/**
 * @brief 结束组
 */
void IniSettings::endGroup() { m_group = ""; }

/**
 * @brief 获取组
 */
QString IniSettings::group() const { return m_group; }

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
    QStringList list = m_mapKey.keys();
    foreach (QString key, m_mapGroupKey.keys())
    {
        if (!list.contains(key))
        {
            list.append(key);
        }
    }
    return list;
}

/**
 * @brief 获取所有键
 */
QStringList IniSettings::allKeys(const QString &prefix) const
{
    QStringList list = m_mapKey.keys(prefix);
    foreach (QString key, m_mapGroupKey.keys())
    {
        if (key.startsWith(prefix + SUBGROUP_SEPARATOR))
        {
            QString childKey = key.mid(prefix.length() + 1);
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

/**
 * @brief 保存文件
 */
bool IniSettings::saveFile() { return saveFile(m_fileName); }

/**
 * @brief 保存文件
 */
bool IniSettings::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }
    // 创建新文件
    QFile fileSave(fileName + ".ini");
    if (!fileSave.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }

    // 在原文件修改，不是重新写入，同时保留原文件中的注释
    QString line;                 // 读取的每一行数据
    static int lineNullCount = 0; // 空行计数
    static QString group;         // 当前组
    QString key;                  // 等号前面的键
    QString value;                // 等号后面的值
    int index;                    // 等号索引

    QTextStream in(&file);      // 新建文本流对象
    in.setCodec(m_codec);       // 设置编码方式
    QTextStream out(&fileSave); // 新建文本流对象
    out.setCodec(m_codec);      // 设置编码方式

    while (!in.atEnd()) // 按行读取文件内容
    {
        line = in.readLine().trimmed(); // 读取一行并去除两端的空白

        if (line.isEmpty() || line.startsWith(";")) // 跳过注释、空行
        {
            lineNullCount++;
            if (lineNullCount > 2) // 最多连续两个空行
                continue;

            out << line << "\n";
            continue;
        }
        lineNullCount = 0;

        if (line.startsWith("[") && line.endsWith("]")) // 组
        {
            group = line.mid(1, line.length() - 2);
            out << line << "\n";
            continue;
        }

        index = line.indexOf("="); // 等号索引
        if (index < 0)
        {
            out << line << "\n";
            continue;
        }

        key = line.left(index).trimmed();      // 键
        value = line.mid(index + 1).trimmed(); // 值

        if (group.isEmpty())
        {
            if (m_mapKey.contains(key))
            {
                out << key << "=" << m_mapKey.value(key) << "\n";
            }
            else
            {
                out << line << "\n";
            }
        }
        else
        {
            if (m_mapGroupKey.contains(group + SUBGROUP_SEPARATOR + key))
            {
                out << key << "=" << m_mapGroupKey.value(group + SUBGROUP_SEPARATOR + key) << "\n";
            }
            else
            {
                out << line << "\n";
            }
        }
    }

    fileSave.flush(); // 刷新文件
    fileSave.close(); // 关闭文件
    file.close();     // 关闭文件
    return true;
}
