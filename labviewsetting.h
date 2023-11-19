/*
 * @Author: m-RNA m-RNA@qq.com
 * @Date: 2023-11-18 21:46:10
 * @LastEditors: m-RNA m-RNA@qq.com
 * @LastEditTime: 2023-11-19 17:54:43
 * @FilePath: \LabViewIniEditor\labviewsetting.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef LABVIEWSETTING_H
#define LABVIEWSETTING_H

#include "inisettings.h"
#include "test_item_interface.h"
#include <QList>

class LabViewSetting
{
public:
    LabViewSetting(QString fileNameProtocol, QString fileNameConfig);
    ~LabViewSetting();
    void clear();

    QList<TestItem> getTestItemList() const;
    QList<ConfigItem> getConfigItemList() const;

private:
    void analysisTestItem();
    void analysisConfigItem();

    TestItem getTestItem(const QString &testItemName);

    QString fileNameProtocol = "";
    QString fileNameConfig = "";

    IniSettings *iniSettingsProtocol;
    IniSettings *iniSettingsConfig;

    QList<TestItem> testItemList = {};
    QList<ConfigItem> configItemList = {};
};

#endif // LABVIEWSETTING_H
