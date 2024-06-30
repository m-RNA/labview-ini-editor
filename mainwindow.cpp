/*
 * @Author: 陈俊健
 * @Date: 2023-10-28 19:35:01
 * @LastEditors: 陈俊健
 * @LastEditTime: 2024-06-30 23:13:32
 * @FilePath: \LabViewIniEditor2024\mainwindow.cpp
 * @Description:
 *
 * Copyright (c) 2023 by Chenjunjian, All Rights Reserved.
 */
#include "mainwindow.h"
#include "labviewsetting.h"
#include "message.h"
#include "test_item_interface.h"
#include "test_result_interface.h"
#include "ui_mainwindow.h"
#include <QCheckBox>
#include <QDateTime>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>

#if _MSC_VER >= 1600 // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

#define TEST_CMD_HEIGHT    105
#define TEST_RESULT_HEIGHT 52

const QString MainWindowTitle = "LabViewIniEditor-Demo";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Message::setParent(this);

    ui->dwlTestItemAll->setVisible(ui->actSownAllTestItem->isChecked()); //

    ui->lwTestCmd->setGridSize(QSize(0, TEST_CMD_HEIGHT));
    ui->lwTestResult->setGridSize(QSize(0, TEST_RESULT_HEIGHT));

    isNeedConfigFile = ui->actNeedConfigFile->isChecked();

    connect(ui->lwTestCmd, &MyListWidget::itemsReordered, this, &MainWindow::onTestCmdReordered);
    connect(ui->lwTestResult, &MyListWidget::itemsReordered, this, &MainWindow::onTestResultReordered);
    connect(ui->lwTestItemAll, &MyListWidget::itemsReordered, this, &MainWindow::onTestItemAllReordered);
}

MainWindow::~MainWindow() { delete ui; }

QString FindFile(QString path, QString obj, QString target)
{
    // 查找配置文件
    QDir dir(path);
    QStringList filter;
    filter << "*.ini";
    dir.setNameFilters(filter);
    QStringList fileList = dir.entryList();
    QStringList targetFileList;
    qDebug() << "fileList: " << fileList;
    // 遍历 target 列表，移除版本号，与 obj
    for (int i = 0; i < fileList.size(); i++)
    {
        QString str = fileList.at(i);
        str = str.mid(0, str.indexOf(target));
        qDebug() << "str: " << str;
        if (str == obj)
        {
            targetFileList.append(fileList.at(i));
        }
    }
    int newIndex = 0;
    if (targetFileList.size() == 0)
    {
        qDebug() << "未找到" << target;
        return "";
    }
    else if (targetFileList.size() > 1)
    {
        // 查找有无最新版本
        qDebug() << "找到" << targetFileList.size() << "个" << target;
        int maxRev = 0;
        for (int i = 0; i < targetFileList.size(); i++)
        {
            int rev = 0;
            rev = targetFileList.at(i)
                      .mid(targetFileList.at(i).indexOf("_Rev") + 4,
                           targetFileList.at(i).indexOf(".ini") - targetFileList.at(i).indexOf("_Rev") - 4)
                      .toInt();
            if (rev > maxRev)
            {
                maxRev = rev;
                newIndex = i;
            }
        }
    }
    else
    {
        qDebug() << "找到一个" << target;
    }
    // Message::success("找到" + target + "：" + targetFileList.at(newIndex));
    return targetFileList.at(newIndex);
}

// 原文链接：https://blog.csdn.net/zhang_ze1234/article/details/107682660
void MainWindow::on_actOpenIni_triggered()
{
    QString pathName;
    QString pathName_Old = "";
    if (filePathProtocol != "")
        pathName_Old = filePathProtocol.mid(0, filePathProtocol.lastIndexOf("/"));
    else if (filePathConfig != "")
        pathName_Old = filePathConfig.mid(0, filePathConfig.lastIndexOf("/"));
    if (pathName_Old == "")
    {
        //"."表示在当前工作路径下寻找
        pathName_Old = "./";
    }
    // “Ini files(*.ini)”表示指定
    pathName = QFileDialog::getOpenFileName(this, "打开文件", pathName_Old, "Ini files(*.ini)");
    if (pathName == "")
    {
        qDebug() << "未选择文件";
        Message::warning("未选择文件");
        return;
    }
    if (!pathName.endsWith(".ini"))
    {
        qDebug() << "文件格式错误";
        Message::warning("文件格式错误");
        return;
    }
    if (!QFile::exists(pathName))
    {
        qDebug() << "文件不存在";
        Message::warning("文件不存在");
        return;
    }
    if (pathName == filePathProtocol || pathName == filePathConfig)
    {
        qDebug() << "该文件已打开";
        // 弹窗提示：已经打开该文件，是否重新打开，
        QMessageBox::StandardButton result
            = QMessageBox::question(this, "提示", "该文件已打开，是否重新加载？", QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::No)
            return;
    }
    QStringList strQirList = pathName.split("/");
    fileNameProtocol = strQirList.last();
    if (fileNameProtocol.contains("协议文件") == false)
    {
        qDebug() << "文件名不包含“协议文件”关键字";
        QMessageBox::StandardButton result = QMessageBox::question(
            this, "提示", "文件名不包含“协议文件”关键字，是否强制加载？", QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::No)
            return;
    }
    filePathProtocol = pathName;

    // 获取协议文件名，移除尾部
    title = fileNameProtocol.mid(0, fileNameProtocol.lastIndexOf("协议"));
    this->setWindowTitle(title); // 设置标题
    qDebug() << "title: " << title;

    on_actLoadIni_triggered();
}

void MainWindow::on_actLoadIni_triggered()
{
    uiClearAll(); // 清空界面
    filePathConfig = "";

    // 获取 dwlTestItemAll 当前宽高
    QSize size = ui->dwlTestItemAll->size();
    isNeedConfigFile = ui->actNeedConfigFile->isChecked();
    if (isNeedConfigFile == true)
    {
        // 查找配置文件
        QString strPath = filePathProtocol.mid(0, filePathProtocol.lastIndexOf("/"));
        strPath = strPath.mid(0, strPath.lastIndexOf("/"));
        strPath += "/配置文件/";
        qDebug() << ": " << strPath;
        fileNameConfig = FindFile(strPath, title, "配置文件");
        if (fileNameConfig.isEmpty() == false)
            filePathConfig = strPath + fileNameConfig;

        // 设置可以关闭 dwlTestItemAll
        ui->dwlTestItemAll->setFeatures(QDockWidget::AllDockWidgetFeatures);
    }
    else
    {
        Message::information("强制显示全部测试项");
        ui->dwlTestItemAll->setVisible(true);
        // 设置不能关闭 dwlTestItemAll
        ui->dwlTestItemAll->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    }
    ui->dwlTestItemAll->resize(size);

    qDebug() << "filePathProtocol: " << filePathProtocol;
    qDebug() << "filePathConfig: " << filePathConfig;

    ui->actLoadIni->setEnabled(true);
    // 删除原来的设置
    if (labviewSetting != nullptr)
    {
        delete labviewSetting;
        labviewSetting = nullptr;
    }
    labviewSetting = new LabViewSetting(filePathProtocol, filePathConfig);
    if (isNeedConfigFile == true)
    {
        if (labviewSetting->isLoadConfig() == false)
        {
            // 弹窗提示：配置文件加载失败
            Message::warning("配置文件加载失败");
        }
        else
            Message::success(fileNameConfig + " 加载成功");
    }
    if (labviewSetting->isLoadProtocol() == false)
    {
        // 弹窗提示：协议文件加载失败
        Message::warning("协议文件加载失败");
        return;
    }
    else
        Message::success(fileNameProtocol + " 加载成功");

    this->testItemListAddr = labviewSetting->getTestItemListAddr(); // 解析 协议文件
    this->configItemList.clear();                                   // 清空
    if (isNeedConfigFile == true)
    {
        this->configItemList = labviewSetting->getConfigItemList(); // 解析 配置文件
    }

    ui->actSave->setEnabled(true);             // 保存按钮可用
    ui->actSSCOM->setEnabled(true);            // 导出SSCOM按钮可用
    ui->actBSP->setEnabled(true);              // 导出BSP按钮可用
    ui->lwTestItemConfigKey->setEnabled(true); // 测试项配置可用
    ui->lwTestItemAll->setEnabled(true);       // 测试项额外可用

    uiUpdateTestItemList(); // 更新测试项列表
}

void MainWindow::on_actSave_triggered()
{
    if (labviewSetting == nullptr)
    {
        qDebug() << "未打开文件";
        Message::error("未打开文件");
        return;
    }

    // 将界面testItem保存到 testItemList
    updateTestItemFromUi(getTestItemCurrent());

    // 将界面testItem保存到 testItemList
    // updateTestItemListFromUi();
    // labviewSetting->testItemListToIni();

    // 将界面configItem保存到 configItemList
    // labviewSetting->setConfigItemList(configItemList);

    // 保存配置文件 & 协议文件
    if (labviewSetting->saveFile() == false)
    {
        qDebug() << "保存失败";
        Message::error("保存失败");
        return;
    }
    Message::success("保存成功");
}

void MainWindow::on_actSSCOM_triggered()
{
    QString filePathName
        = QFileDialog::getSaveFileName(this, "导出SSCOM格式", title + "SSCOM格式", "SSCOM files(*.txt)");
    if (filePathName.isEmpty())
    {
        qDebug() << "未选择文件";
        Message::warning("未选择文件");
        return;
    }

    bool isOk = labviewSetting->exportFileSscom(filePathName);
    if (isOk == false)
    {
        qDebug() << "导出SSCOM格式失败";
        Message::error("导出SSCOM格式失败");
        return;
    }
    Message::success("导出SSCOM格式成功");
}

void MainWindow::on_actBSP_triggered()
{
    QString filePathName = QFileDialog::getSaveFileName(this, "导出BSP格式", title + "BSP格式", "BSP files(*.bsp)");
    if (filePathName.isEmpty())
    {
        qDebug() << "未选择文件";
        Message::warning("未选择文件");
        return;
    }

    bool isOk = labviewSetting->exportFileBsp(filePathName);
    if (isOk == false)
    {
        qDebug() << "导出BSP格式失败";
        Message::error("导出BSP格式失败");
        return;
    }
    Message::success("导出BSP格式成功");
}

void MainWindow::on_actAbout_triggered()
{
    QString data = __DATE__;
    QString time = __TIME__;
    QString year = data.mid(7, 4);
    QString month = data.mid(0, 3);
    QString day = data.mid(4, 2);
    QString author = "ChenJunJian";
    QString version = "0.0.0";
    QString postscript = "Just For Fun! No Other Ideas."; // PS：只是为了好玩方便，没有其他想法
    QStringList monthList = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    for (int i = 0; i < monthList.size(); i++)
    {
        if (month == monthList.at(i))
        {
            month = QString::number(i + 1);
            break;
        }
    }

    QString strData = year + "/" + month + "/" + day;

    QMessageBox::about(this, "About",
                        MainWindowTitle + "\n"
                        "For EMBB\n"
                        "\n"
                        "Author: "+ author+"\n"
                        "Postscript: "+ postscript+"\n"
                        "\n"
                        "Version: " + version + "\n"
                        "Build Data: " + strData +" " + time + "\n"
                        "\n"
                        "CopyRight © "+ year + " by " + author + ", All Rights Reserved.");
}

void MainWindow::on_actDocOnline_triggered()
{
    QDesktopServices::openUrl(QUrl("https://lierda.feishu.cn/docx/WRS0dtcT9oevBLxGsCOcFvY9nLh"));
}

void MainWindow::on_actUpdateVersion_triggered()
{
    QDesktopServices::openUrl(QUrl("https://lierda.feishu.cn/drive/folder/RTYcfYAkallhu2d6QdQcwrpJnHh"));
}

void MainWindow::on_actSownAllTestItem_triggered(bool checked)
{
    if (checked == false)
    {
        if (isNeedConfigFile == false)
        {
            Message::information("强制显示全部测试项");
            ui->actSownAllTestItem->setChecked(true);
            checked = true;
        }
    }
    ui->dwlTestItemAll->setVisible(checked);
}

void MainWindow::on_dwlTestItemAll_visibilityChanged(bool visible) { ui->actSownAllTestItem->setChecked(visible); }

void MainWindow::on_lwTestItemConfig_itemSelectionChanged()
{
    if (labviewSetting == nullptr)
        return;
    QListWidgetItem *item = ui->lwTestItemConfig->currentItem();
    if (item == nullptr)
        return;
    QString name = item->text().trimmed();
    int testItemIndex = labviewSetting->getTestItemIndex(name);
    if (testItemIndex != -1)
        uiUpdateTestItem(name);

    QStringList keyList = labviewSetting->getConfigTestItemKey(name);
    if (keyList.size() == 0)
    {
        qDebug() << "未找到配置组：" << name;
        Message::error("未找到配置组：" + name);
        return;
    }
    // 添加测试项
    ui->lwTestItemConfigKey->clear();

    if (keyList.size() > 0)
    {
        foreach (QString key, keyList) // 往 ListWidget 添加测试项
        {
            QListWidgetItem *item = new QListWidgetItem(ui->lwTestItemConfigKey);
            item->setText(key);
        }
        if (testItemIndex == -1)
            uiUpdateTestItem(keyList.at(0));
    }
}

void MainWindow::on_lwTestItemConfigKey_itemSelectionChanged()
{
    QListWidgetItem *item = ui->lwTestItemConfigKey->currentItem();
    if (item == nullptr)
        return;
    qDebug() << "点击" << item->text().trimmed();
    uiUpdateTestItem(item->text().trimmed());
}

void MainWindow::on_lwTestItemAll_itemSelectionChanged()
{
    QListWidgetItem *item = ui->lwTestItemAll->currentItem();
    if (item == nullptr)
        return;
    ui->lwTestItemConfigKey->clear();
    uiUpdateTestItem(item->text().trimmed());
}

// void MainWindow::on_lwTestItemConfig_itemClicked(QListWidgetItem *item) { ui->toolBar_2->setEnabled(false); }

// void MainWindow::on_lwTestItemAll_itemClicked(QListWidgetItem *item) { ui->toolBar_2->setEnabled(true); }

// void MainWindow::on_lwTestItemConfigKey_itemClicked(QListWidgetItem *item) { ui->toolBar_2->setEnabled(false); }

void MainWindow::on_leTestItemName_editingFinished()
{
    // 获取当前点击的测试项的名称
    QString str = ui->leTestItemName->text().trimmed();
    if (str == "")
    {
        ui->leTestItemName->setText(leTestItemName_Old);
        return;
    }
    ui->leTestItemName->setText(str);
    if (leTestItemName_Old == "")
        return;
    if (str == leTestItemName_Old)
        return;
    // 获取当前点击的测试项的索引
    int testItemIndex = labviewSetting->getTestItemIndex(leTestItemName_Old);
    if (testItemIndex == -1)
    {
        qDebug() << "未找到测试项：" << leTestItemName_Old;
        Message::error("未找到测试项：" + leTestItemName_Old);
        return;
    }
    // 修改测试项名称
    (*(this->testItemListAddr))[testItemIndex].name = str;
    // this->configItemList[testItemIndex].name = str;
    labviewSetting->renameTestItemProtocol(leTestItemName_Old, str);
    // 更新测试项界面
    // uiUpdateTestItem(str);
    uiUpdateTestItemList();
    leTestItemName_Old = str;
}

void MainWindow::on_spbxRepeatTimes_valueChanged(int arg1)
{
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    testItem->repeat = arg1;
}

void MainWindow::on_spbxDataSize_valueChanged(int arg1)
{
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    testItem->dataByteLen = arg1;
}

void MainWindow::on_spbxDecPlace_valueChanged(int arg1)
{
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    testItem->decimal = arg1;
}

void MainWindow::on_spbxByteOrder_currentTextChanged(const QString &arg1)
{
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    testItem->byteOrder = arg1;
}

void MainWindow::on_cbSign_currentTextChanged(const QString &arg1)
{
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    testItem->sign = arg1;
}

void MainWindow::on_btnAddTestICmd_clicked()
{
    // 获取当前点击的测试项
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    // 获取当前点击的 lwTestCmd 的索引
    int testCmdIndex = ui->lwTestCmd->currentRow();
    if (testCmdIndex == -1)
        testCmdIndex = 0;
    else
        testCmdIndex++;
    // 在当前点击的命令项下方添加一个命令项
    TestCmd cmd;
    insertTestCmd(testItem->cmdList, cmd, testCmdIndex);

    // 更新命令项
    uiUpdateTestCmd(testItem->cmdList);
    ui->lwTestCmd->setCurrentRow(testCmdIndex);
}

void MainWindow::on_btnCopyTestICmd_clicked()
{
    // 获取当前点击的测试项
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    // 获取当前点击的 lwTestCmd 的索引
    int testCmdIndex = ui->lwTestCmd->currentRow();
    if (testCmdIndex == -1)
    {
        qDebug() << "未选择命令项";
        Message::warning("未选择命令项");
        return;
    }

    // 在当前点击的命令项下方添加一个命令项
    TestCmd cmd = ((TestItemInterface *) ui->lwTestCmd->itemWidget(ui->lwTestCmd->item(testCmdIndex)))->getTestCmd();
    insertTestCmd(testItem->cmdList, cmd, testCmdIndex + 1);

    // 更新命令项
    uiUpdateTestCmd(testItem->cmdList);
    ui->lwTestCmd->setCurrentRow(testCmdIndex + 1);
}

void MainWindow::on_btnRemoveTestICmd_clicked()
{
    if (ui->lwTestCmd->count() == 1)
    {
        Message::warning("至少保留一个命令项");
        return;
    }
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    // 获取当前点击的 lwTestCmd 的索引
    int testCmdIndex = ui->lwTestCmd->currentRow();
    if (testCmdIndex == -1)
    {
        qDebug() << "未选择命令项";
        Message::warning("未选择命令项");
        return;
    }

    // 删除当前点击的命令项
    testItem->cmdList.remove(testCmdIndex);

    // 更新命令项
    uiRemoveTestCmd(testCmdIndex);
    if (testCmdIndex >= ui->lwTestCmd->count())
        testCmdIndex--;
    ui->lwTestCmd->setCurrentRow(testCmdIndex);
}

void MainWindow::on_btnAddTestResult_clicked()
{
    // 获取当前点击的测试项
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    // 获取当前点击的 lwTestResult 的索引
    int testResultIndex = ui->lwTestResult->currentRow();
    if (testResultIndex == -1)
        testResultIndex = 0;
    else
        testResultIndex++;
    // 在当前点击的结果项下方添加一个结果项
    TestResult result;
    testItem->resultList.insert(testResultIndex, result);

    // 更新结果项
    uiUpdateTestResult(testItem->resultList);
    ui->lwTestResult->setCurrentRow(testResultIndex);
}

void MainWindow::on_btnCopyTestIResult_clicked()
{
    // 获取当前点击的测试项
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    // 获取当前点击的 lwTestResult 的索引
    int testResultIndex = ui->lwTestResult->currentRow();
    if (testResultIndex == -1)
    {
        qDebug() << "未选择结果项";
        Message::warning("未选择结果项");
        return;
    }

    // 在当前点击的结果项下方添加一个结果项
    TestResult result = ((TestResultInterface *) ui->lwTestResult->itemWidget(ui->lwTestResult->item(testResultIndex)))
                            ->getTestResult();
    testItem->resultList.insert(testResultIndex + 1, result);

    // 更新结果项
    uiUpdateTestResult(testItem->resultList);
    ui->lwTestResult->setCurrentRow(testResultIndex + 1);
}

void MainWindow::on_btnRemoveTestIResult_clicked()
{
    if (ui->lwTestResult->count() == 1)
    {
        Message::warning("至少保留一个结果项");
        return;
    }
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    // 获取当前点击的 lwTestResult 的索引
    int testResultIndex = ui->lwTestResult->currentRow();
    if (testResultIndex == -1)
    {
        qDebug() << "未选择结果项";
        Message::warning("未选择结果项");
        return;
    }

    // 删除当前点击的结果项
    testItem->resultList.remove(testResultIndex);

    // 更新结果项
    uiRemoveResult(testResultIndex);
    if (testResultIndex >= ui->lwTestResult->count())
        testResultIndex--;
    ui->lwTestResult->setCurrentRow(testResultIndex);
}

void MainWindow::on_actTestItemAdd_triggered()
{
    if (labviewSetting == nullptr)
    {
        qDebug() << "未打开文件";
        Message::error("未打开文件");
        return;
    }
    // 获取当前点击的测试项的名称
    QString str = ui->leTestItemName->text().trimmed();
    // 获取当前点击的测试项的索引
    int testItemIndex = labviewSetting->getTestItemIndex(str);

    if (testItemIndex == -1)
        testItemIndex = 0;
    else
        testItemIndex++;

    // 添加测试项
    static int num = 1;
    TestCmd cmd;
    TestResult result;
    TestItem testItem;
    testItem.name = "New Test Item " + QString::number(num++);
    testItem.cmdList.append(cmd);
    testItem.resultList.append(result);
    // testItemListAddr->insert(testItemIndex, testItem);
    labviewSetting->insertTestItemProtocol(testItemIndex, testItem);

    // 更新测试项界面
    int lwIndex = ui->lwTestItemAll->currentRow() + 1;
    uiUpdateTestItemList();
    ui->lwTestItemAll->setCurrentRow(lwIndex);
}

void MainWindow::on_actTestItemCopy_triggered()
{
    if (labviewSetting == nullptr)
    {
        qDebug() << "未打开文件";
        Message::error("未打开文件");
        return;
    }

    // 获取当前点击的测试项的名称
    QString str = ui->leTestItemName->text().trimmed();
    // 获取当前点击的测试项的索引
    int testItemIndex = labviewSetting->getTestItemIndex(str);
    if (testItemIndex == -1)
    {
        qDebug() << "未找到测试项：" << str;
        Message::warning("未找到测试项：" + str);
        return;
    }
    // 获取当前点击的测试项的索引
    TestItem testItemCopy = (*(this->testItemListAddr))[testItemIndex];
    testItemCopy.name += "_Copy";
    // testItemListAddr->insert(testItemIndex + 1, testItemCopy);
    labviewSetting->insertTestItemProtocol(testItemIndex + 1, testItemCopy);

    // 更新测试项界面
    int lwIndex = ui->lwTestItemAll->currentRow() + 1;
    uiUpdateTestItemList();
    ui->lwTestItemAll->setCurrentRow(lwIndex);
}

void MainWindow::on_actTestItemDelete_triggered()
{
    // 获取当前点击的测试项的名称
    QString str = ui->leTestItemName->text().trimmed();
    // 获取当前点击的测试项的索引
    int testItemIndex = labviewSetting->getTestItemIndex(str);
    if (testItemIndex == -1)
    {
        qDebug() << "未找到测试项：" << str;
        Message::warning("未找到测试项：" + str);
        return;
    }

    // 删除当前点击的测试项
    testItemListAddr->removeAt(testItemIndex);
    this->labviewSetting->removeTestItemProtocol(str);

    // 更新测试项界面
    int lwIndex = ui->lwTestItemAll->currentRow();
    if (lwIndex > 0)
        lwIndex--;
    uiUpdateTestItemList();
    ui->lwTestItemAll->setCurrentRow(lwIndex);
}

void MainWindow::onTestCmdReordered(void)
{
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    updateTestCmdListFromUi(testItem->cmdList);
}

void MainWindow::onTestResultReordered(void)
{
    TestItem *testItem = getTestItemCurrent();
    if (testItem == nullptr)
        return;
    updateTestResultListFromUi(testItem->resultList);
}

void MainWindow::onTestItemAllReordered(void)
{
    int uiIndex = ui->lwTestItemAll->currentRow();
    if (uiIndex == -1)
        return;
    auto item = ui->lwTestItemAll->currentItem();
    if (item == nullptr)
        return;
    if (uiIndex < ui->lwTestItemAll->count() - 1)
        uiIndex++;
    QString lastName = ui->lwTestItemAll->item(uiIndex)->text().trimmed();

    QString name = item->text().trimmed();

    labviewSetting->moveTestItemProtocol(lastName, name);
    qDebug() << "移动测试项：" << lastName << " -> " << name;
}

TestItem *MainWindow::getTestItemCurrent(void)
{
    // 获取当前点击的测试项的名称
    QString str = ui->leTestItemName->text().trimmed();
    if (str == "")
        return nullptr;
    // 获取当前点击的测试项的索引
    int testItemIndex = labviewSetting->getTestItemIndex(str);
    if (testItemIndex == -1)
    {
        qDebug() << "未找到测试项：" << str;
        Message::warning("未找到测试项：" + str);
        return nullptr;
    }
    return &(*(this->testItemListAddr))[testItemIndex];
}

void MainWindow::uiUpdateTestCmd(const QVector<TestCmd> &cmdList)
{
    if (cmdList.size() == 0)
    {
        ui->lwTestCmd->clear();
        return;
    }
    if (cmdList.size() < ui->lwTestCmd->count())
    {
        int times = ui->lwTestCmd->count();
        for (int i = 0; i < times; i++)
        {
            if (i < cmdList.size())
            {
                ((TestItemInterface *) (ui->lwTestCmd->itemWidget(ui->lwTestCmd->item(i))))->setUi(i, cmdList.at(i));
            }
            else
            {
                // 删除多余的命令项
                uiRemoveTestCmd(ui->lwTestCmd->count() - 1);
            }
        }
    }
    else
    {
        for (int i = 0; i < cmdList.size(); i++)
        {
            if (i < ui->lwTestCmd->count())
            {
                ((TestItemInterface *) (ui->lwTestCmd->itemWidget(ui->lwTestCmd->item(i))))->setUi(i, cmdList.at(i));
            }
            else
            {
                TestItemInterface *uiTestItem = new TestItemInterface(this);
                uiTestItem->setUi(i, cmdList.at(i));
                uiAddTestCmd(uiTestItem);
            }
        }
    }
}

void MainWindow::uiUpdateTestResult(const QVector<TestResult> &resultList)
{
    ui->lwTestResult->clear();
    for (int i = 0; i < resultList.size(); ++i)
    {
        TestResultInterface *item = new TestResultInterface(this);
        connect(item, &TestResultInterface::indexChanged, this, &MainWindow::on_lwTestResultItem_IndexChanged);
        item->setUi(i, resultList.at(i));
        uiAddResult(item);
    }
    on_lwTestResultItem_IndexChanged(resultList.at(0).index);
}

void MainWindow::uiUpdateTestItem(QString testItemName)
{
    int testItemIndex = labviewSetting->getTestItemIndex(testItemName);
    if (testItemIndex == -1)
    {
        qDebug() << "未找到测试项：" << testItemName;
        Message::error("未找到测试项：" + testItemName);
        return;
    }
    // 更新界面前，检查是否有未保存的测试项
    if (leTestItemName_Old != "")
    {
        updateTestItemFromUi(getTestItemCurrent());
    }

    leTestItemName_Old = testItemName;
    qDebug() << "点击: " << testItemName;

    const TestItem *testItem = &testItemListAddr->at(testItemIndex);
    // testItem->print();

    ui->leTestItemName->setText(testItem->name);
    ui->spbxDataSize->setValue(testItem->dataByteLen);
    ui->spbxDecPlace->setValue(testItem->decimal);
    ui->spbxByteOrder->setCurrentText(testItem->byteOrder);
    ui->cbSign->setCurrentText(testItem->sign);
    ui->spbxRepeatTimes->setValue(testItem->repeat);

    uiUpdateTestCmd(testItem->cmdList);       // 更新命令项
    uiUpdateTestResult(testItem->resultList); // 更新结果项
}

void MainWindow::uiUpdateTestItemList()
{
    ui->lwTestItemConfigKey->clear();
    ui->lwTestItemConfig->clear();
    ui->lwTestItemAll->clear();

    QStringList strConfigList;
    QStringList strConfigKeyList;

    if (isNeedConfigFile == true)
    {
        for (int i = 0; i < this->configItemList.size(); i++) // 往 ListWidget 添加测试项
        {
            QString str = this->configItemList.at(i).name;
            strConfigList << str;
            QStringList keyList = labviewSetting->getConfigTestItemKey(str);
            strConfigKeyList << keyList;

            QListWidgetItem *item = new QListWidgetItem(ui->lwTestItemConfig);
            item->setText("   " + str);

            QCheckBox *checkBox = new QCheckBox(ui->lwTestItemConfig);
            checkBox->setChecked(this->configItemList.at(i).enable);
            ui->lwTestItemConfig->setItemWidget(item, checkBox);
        }
    }

    for (int i = 0; i < this->testItemListAddr->size(); i++) // 往 ListWidget 添加测试项
    {
        QString testItemName = this->testItemListAddr->at(i).name;
        QListWidgetItem *item = new QListWidgetItem(ui->lwTestItemAll);
        if (strConfigList.contains(testItemName))
        {
            // 设置为粗体 斜体
            QFont font = item->font();
            font.setBold(true);
            font.setItalic(true);
            font.setUnderline(true);
            item->setFont(font);
        }
        else if (strConfigKeyList.contains(testItemName))
        {
            // 设置为斜体
            QFont font = item->font();
            font.setBold(true);
            font.setUnderline(true);
            item->setFont(font);
        }
        item->setText(testItemName);
    }
}

void MainWindow::uiClearAll()
{
    // this->setWindowTitle(MainWindowTitle);

    ui->lwTestItemConfig->clear();
    ui->lwTestItemConfigKey->clear();
    ui->lwTestItemAll->clear();

    ui->leTestItemName->clear();
    ui->spbxRepeatTimes->setValue(1);
    ui->spbxDataSize->setValue(2);
    ui->spbxDecPlace->setValue(0);
    ui->lwTestCmd->clear();
    ui->lwTestResult->clear();
}

void MainWindow::uiAddTestCmd(TestItemInterface *item) { uiInsertTestCmd(ui->lwTestCmd->count(), item); }

void MainWindow::uiAddResult(TestResultInterface *item) { uiInsertResult(ui->lwTestResult->count(), item); }

void MainWindow::uiInsertTestCmd(int index, TestItemInterface *item)
{
    if (index < 0 || index > ui->lwTestCmd->count())
    {
        qDebug() << "cmdIndex = " << index << " 越界";
        Message::error("cmdIndex = " + QString::number(index) + " 越界");
        return;
    }

    QListWidgetItem *listItem = new QListWidgetItem(ui->lwTestCmd);
    listItem->setSizeHint(QSize(0, TEST_CMD_HEIGHT));
    ui->lwTestCmd->insertItem(index, listItem);   // 插入到列表中
    ui->lwTestCmd->setItemWidget(listItem, item); // 设置为该项的 Widget
    ui->lwTestCmd->setCurrentRow(index);          // 滚动到该项
}

void MainWindow::uiInsertResult(int index, TestResultInterface *item)
{
    if (index < 0 || index > ui->lwTestResult->count())
    {
        qDebug() << "cmdIndex = " << index << " 越界";
        Message::error("cmdIndex = " + QString::number(index) + " 越界");
    }

    QListWidgetItem *listItem = new QListWidgetItem(ui->lwTestResult);
    listItem->setSizeHint(QSize(0, TEST_RESULT_HEIGHT));
    ui->lwTestResult->insertItem(index, listItem);   // 插入到列表中
    ui->lwTestResult->setItemWidget(listItem, item); // 设置为该项的 Widget
    ui->lwTestResult->setCurrentRow(index);          // 滚动到该项
}

void MainWindow::uiRemoveTestCmd(int index)
{
    if (index < 0 || index >= ui->lwTestCmd->count())
    {
        qDebug() << "cmdIndex = " << index << " 越界";
        Message::error("cmdIndex = " + QString::number(index) + " 越界");
        return;
    }
    ui->lwTestCmd->takeItem(index);
}

void MainWindow::uiRemoveResult(int index)
{
    if (index < 0 || index >= ui->lwTestResult->count())
    {
        qDebug() << "resultIndex = " << index << " 越界";
        Message::error("resultIndex = " + QString::number(index) + " 越界");
        return;
    }
    ui->lwTestResult->takeItem(index);
}

void MainWindow::insertTestCmd(QVector<TestCmd> &cmdList, const TestCmd &cmd, int cmdIndex)
{
    if (cmdIndex < 0 || cmdIndex > cmdList.size())
    {
        qDebug() << "cmdIndex = " << cmdIndex << " 越界";
        Message::error("cmdIndex = " + QString::number(cmdIndex) + " 越界");
        return;
    }
    cmdList.insert(cmdIndex, cmd);
}

void MainWindow::updateTestCmdListFromUi(QVector<TestCmd> &cmdList)
{
    cmdList.clear();
    for (int i = 0; i < ui->lwTestCmd->count(); i++)
    {
        TestItemInterface *item = (TestItemInterface *) ui->lwTestCmd->itemWidget(ui->lwTestCmd->item(i));
        cmdList.append(item->getTestCmd());
    }
}

void MainWindow::updateTestResultListFromUi(QVector<TestResult> &resultList)
{
    resultList.clear();
    for (int i = 0; i < ui->lwTestResult->count(); i++)
    {
        TestResultInterface *item = (TestResultInterface *) ui->lwTestResult->itemWidget(ui->lwTestResult->item(i));
        resultList.append(item->getTestResult());
    }
}

void MainWindow::updateTestItemFromUi(TestItem *testItem)
{
    if (testItem != nullptr)
    {
        testItem->name = ui->leTestItemName->text().trimmed();
        testItem->byteOrder = ui->spbxByteOrder->currentText();
        testItem->dataByteLen = ui->spbxDataSize->value();
        testItem->decimal = ui->spbxDecPlace->value();
        testItem->repeat = ui->spbxRepeatTimes->value();
        testItem->sign = ui->cbSign->currentText();

        updateTestCmdListFromUi(testItem->cmdList);
        updateTestResultListFromUi(testItem->resultList);
    }
}

void MainWindow::updateTestItemListFromUi()
{
    // testItemList.clear();
    // for (int i = 0; i < ui->lwTestItemAll->count(); i++)
    // {
    //     QListWidgetItem *itemUi = ui->lwTestItemAll->item(i);
    //     TestItem testItem;
    //     testItem.name = itemUi->text().trimmed();
    //     testItem.byteOrder = ui->spbxByteOrder->currentText();
    //     testItem.dataByteLen = ui->spbxDataSize->value();
    //     testItem.decimal = ui->spbxDecPlace->value();
    //     testItem.repeat = ui->spbxRepeatTimes->value();

    //     QVector<TestCmd> cmdList;
    //     for (int j = 0; j < ui->lwTestCmd->count(); j++)
    //     {
    //         TestItemInterface *item = (TestItemInterface *) ui->lwTestCmd->itemWidget(ui->lwTestCmd->item(j));
    //         cmdList.append(item->getTestCmd());
    //     }
    //     testItemList.append(testItem);

    // }
}

void MainWindow::on_btnAddTestItemAll_clicked() { on_actTestItemAdd_triggered(); }

void MainWindow::on_btnCopyTestItemAll_clicked() { on_actTestItemCopy_triggered(); }

void MainWindow::on_btnRemoveTestItemAll_clicked() { on_actTestItemDelete_triggered(); }

void MainWindow::on_lwTestResultItem_IndexChanged(int index)
{
    for (int i = 0; i < ui->lwTestCmd->count(); i++)
    {
        TestItemInterface *item = (TestItemInterface *) ui->lwTestCmd->itemWidget(ui->lwTestCmd->item(i));
        item->setHighlight(false);
    }

    for (int i = 0; i < ui->lwTestResult->count(); i++)
    {
        TestResultInterface *itemResult
            = (TestResultInterface *) ui->lwTestResult->itemWidget(ui->lwTestResult->item(i));
        int showIndex = itemResult->getShowIndex();
        if (showIndex < 0 || showIndex >= ui->lwTestCmd->count())
            continue;
        TestItemInterface *item = (TestItemInterface *) ui->lwTestCmd->itemWidget(ui->lwTestCmd->item(showIndex));
        item->setHighlight(true);
    }

    if (index < 0 || index >= ui->lwTestCmd->count())
    {
        Message::information("显示项：" + QString::number(index) + " > 命令列表数目："
                             + QString::number(ui->lwTestCmd->count()));
    }
    else
    {
        TestItemInterface *item = (TestItemInterface *) ui->lwTestCmd->itemWidget(ui->lwTestCmd->item(index));
        ui->lwTestCmd->setCurrentRow(index);
    }
}
