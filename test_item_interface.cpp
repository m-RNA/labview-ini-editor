#include "test_item_interface.h"
#include "ui_test_item_interface.h"
#include <QDebug>

TestItemInterface::TestItemInterface(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TestItemInterface)
{
    ui->setupUi(this);
    on_cbAnalysis_currentIndexChanged(ui->cbAnalysis->currentIndex());
    on_cbDisplayResult_currentIndexChanged(ui->cbDisplayResult->currentIndex());
}

TestItemInterface::~TestItemInterface() { delete ui; }

void TestItemInterface::on_cbAnalysis_currentIndexChanged(int index) { ui->leAnalysis->setVisible(index); }

void TestItemInterface::on_cbDisplayResult_currentIndexChanged(int index)
{
    ui->spbxDataSize->setVisible(index);
    ui->spbxDecPlace->setVisible(index);
    ui->spbxByteOrder->setVisible(index);
    ui->cbSign->setVisible(index);
    ui->cbAnalysis->setVisible(index);
    if (index)
    {
        ui->leAnalysis->setVisible(ui->cbAnalysis->currentIndex());
        on_cbDataLimit_currentTextChanged(ui->cbDataLimit->currentText());
    }
    else
    {
        ui->leAnalysis->setVisible(false);
        ui->cbUnit->setVisible(index);
    }
    ui->cbDataLimit->setVisible(index);
}

void TestItemInterface::on_leTx_textChanged(const QString &arg1)
{
    QString tx_Upper = arg1.toUpper();
    // 判断开头为 68 还是 AT
    if (tx_Upper.startsWith("68"))
    {
        QString rx = ui->leRx->text();
        // 判断是否为 16 结尾
        if (rx.endsWith("16"))
            ui->cbTestType->setCurrentIndex(2); // AT<HEX>
        else
            ui->cbTestType->setCurrentIndex(3); // 68
    }
    else if (tx_Upper.startsWith("AT"))
    {
        ui->cbTestType->setCurrentIndex(1); // AT<\r\n>
    }
    // 判断格式是否为 1234RS1234RS...
    else if (tx_Upper.contains("RS"))
    {
        ui->cbTestType->setCurrentIndex(4);
    }
}

void TestItemInterface::on_leRx_textChanged(const QString &arg1)
{
    QString rx_Upper = arg1.toUpper();
    // 判断开头为 68 还是 AT
    if (rx_Upper.startsWith("68"))
    {
        if (rx_Upper.endsWith("16"))
            ui->cbTestType->setCurrentIndex(2); // AT<HEX>
        else
            ui->cbTestType->setCurrentIndex(3); // 68
    }
    else if (rx_Upper.startsWith("AT"))
    {
        ui->cbTestType->setCurrentIndex(1); // AT<\r\n>
    }
}

void TestItemInterface::on_leRx_editingFinished()
{
    // 去除最后的空格、回车、换行
    QString rx = ui->leRx->text();
    rx = rx.trimmed();
    ui->leRx->setText(rx);
}

void TestItemInterface::on_leTx_editingFinished()
{
    // 去除最后的空格、回车、换行
    QString tx = ui->leTx->text();
    tx = tx.trimmed();
    ui->leTx->setText(tx);
}

void TestItemInterface::on_cbTestType_currentIndexChanged(int index)
{
    if (index > 3 || index == 0)
        ui->cbComName->setVisible(false);
    else
        ui->cbComName->setVisible(true);

    if (index == 1)
        ui->cbComName->setCurrentIndex(1); // 产品串口
    else if (index == 3 || index == 2)
        ui->cbComName->setCurrentIndex(0); // 底板串口
}

void TestItemInterface::setIndex(int index) { ui->lbIndex->setText(QString::number(index)); }

void TestItemInterface::on_cbDataLimit_currentTextChanged(const QString &arg1)
{
    if (arg1 == "OK" || arg1.contains("READY"))
        ui->cbUnit->setVisible(false);
    else
        ui->cbUnit->setVisible(true);
}

/**
 * @brief 配置测试项
 * @param index 测试项序号
 * @param item 测试项
 */
void TestItemInterface::setTestItem(int index, const TestCmd &item)
{
    ui->lbIndex->setText(QString::number(index));
    ui->leBrief->setText(item.brief);
    ui->cbComName->setCurrentText(item.comName);
    ui->leTx->setText(item.tx);
    ui->leRx->setText(item.rx);
    ui->cbTestType->setCurrentText(item.cmdType);
    ui->spbxDataSize->setValue(item.dataByteLen);
    ui->spbxDecPlace->setValue(item.decimal);
    ui->spbxByteOrder->setCurrentText(item.byteOrder);
    ui->cbSign->setCurrentText(item.sign);
    ui->cbAnalysis->setCurrentText(item.rxAnalysis);

    if (item.resultShow != "")
    {
        if (item.resultShow.contains("<")) // 截取 < 之前的字符串
            ui->cbDisplayResult->setCurrentIndex(2);
        else
            ui->cbDisplayResult->setCurrentIndex(1);
    }

    // ui->cbDataLimit->setCurrentText(item.dataLimit);
    // ui->cbUnit->setCurrentText(item.dataUnit);
    ui->spbxTimeout->setValue(item.cmdTimeout);
    ui->spbxDelay->setValue(item.cmdDelay);
}
