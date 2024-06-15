#if _MSC_VER >= 1600 // MSVC2015>1899,对于MSVC2010以上版本都可以使用
#pragma execution_character_set("utf-8")
#endif

#include "test_item_interface.h"
#include "ui_test_item_interface.h"
#include <QDebug>

#define CB_TX_END_NC  0
#define CB_TX_END_R_N 1
#define CB_TX_END_HEX 2

const QStringList STR_TX_END = {"", "<\\r\\n>", "<HEX>"};

#define TEST_TYPE_INDEX_68 2

const QStringList STR_TEST_TYPE = {
    "AT", "AT1", "68", "串口查询真", "串口查询假", "单按钮弹框", "双按钮弹框",
};

TestItemInterface::TestItemInterface(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TestItemInterface)
{
    ui->setupUi(this);

    ui->cbEncode->setVisible(false);
}

TestItemInterface::~TestItemInterface() { delete ui; }

void TestItemInterface::on_leTx_textChanged(const QString &arg1)
{
    // setTxAndEndIndex(arg1);
}

void TestItemInterface::on_leTx_editingFinished()
{
    // 去除最后的空格、回车、换行
    QString tx = ui->leTx->text();
    tx = tx.trimmed();
    //
    setTxAndEndIndex(tx);
    ui->leTx->setText(tx);
}

void TestItemInterface::on_leRx_editingFinished()
{
    // 去除最后的空格、回车、换行
    QString rx = ui->leRx->text();
    rx = rx.trimmed();
    ui->leRx->setText(rx);
}

void TestItemInterface::setIndex(int index) { ui->lbIndex->setText(QString::number(index)); }

/**
 * @brief 配置测试项
 * @param index 测试项序号
 * @param item 测试项
 */
void TestItemInterface::setUi(int index, const TestCmd &item)
{
    ui->lbIndex->setText(QString::number(index));
    //    ui->leBrief->setText(item.brief);
    ui->cbComName->setCurrentText(item.comName);
    ui->leBrief->setText(item.brief);

    setTxAndEndIndex(item.tx);
    ui->leRx->setText(item.rx);

    ui->cbTestType->setCurrentText(item.cmdType);

    //    ui->spbxDataSize->setValue(item.dataByteLen);
    //    ui->spbxDecPlace->setValue(item.decimal);
    //    ui->spbxByteOrder->setCurrentText(item.byteOrder);
    //    ui->cbEncode->setCurrentText(item.encodeWay);
    //    ui->cbSign->setCurrentText(item.sign);

    ui->spbxDelay->setValue(item.cmdDelay);
    ui->spbxTimeout->setValue(item.cmdTimeout);

    // if (item.resultShow != "")
    // {
    //     if (item.resultShow.contains("<")) // 截取 < 之前的字符串
    //         ui->cbDisplayResult->setCurrentIndex(2);
    //     else
    //         ui->cbDisplayResult->setCurrentIndex(1);
    // }

    // if (item.rxAnalysis != "")
    // {
    //     ui->cbAnalysis->setCurrentText(item.rxAnalysis);
    //     ui->leAnalysis->setText(item.rxAnalysis);
    // }

    // ui->cbDataLimit->setCurrentText(item.dataLimit);
    // ui->cbUnit->setCurrentText(item.dataUnit);
}

void TestItemInterface::setTxAndEndIndex(QString strTx)
{
    if (strTx.isEmpty())
        return;
    if (strTx.toUpper().endsWith(STR_TX_END.at(CB_TX_END_HEX)))
    {
        strTx = strTx.toUpper().remove(STR_TX_END.at(CB_TX_END_HEX));
        ui->cbTxEnd->setCurrentIndex(CB_TX_END_HEX); // <HEX>
    }
    else if (strTx.toLower().endsWith(STR_TX_END.at(CB_TX_END_R_N)))
    {
        strTx = strTx.mid(0, strTx.length() - 6);
        ui->cbTxEnd->setCurrentIndex(CB_TX_END_R_N); // <\r\n>
    }
    else
    {
        ui->cbTxEnd->setCurrentText(CB_TX_END_NC);
    }

    ui->leTx->setText(strTx);
}

TestCmd TestItemInterface::getTestCmd() const
{
    TestCmd cmd;
    cmd.index = ui->lbIndex->text().toInt();
    cmd.comName = ui->cbComName->currentText();
    cmd.brief = ui->leBrief->text();

    cmd.tx = ui->leTx->text();
    if (cmd.tx.isEmpty())
        cmd.tx = "NA";

    if (ui->cbTxEnd->currentIndex() == CB_TX_END_HEX)
        cmd.tx += STR_TX_END.at(CB_TX_END_HEX);
    else if (ui->cbTxEnd->currentIndex() == CB_TX_END_R_N)
        cmd.tx += STR_TX_END.at(CB_TX_END_R_N);

    cmd.rx = ui->leRx->text();
    if (cmd.rx.isEmpty())
        cmd.rx = "NA";

    cmd.cmdType = ui->cbTestType->currentText();

    // cmd.encodeWay = ui->cbEncode->currentText();

    cmd.cmdDelay = ui->spbxDelay->value();
    cmd.cmdTimeout = ui->spbxTimeout->value();

    return cmd;
}

void TestItemInterface::on_cbTestType_currentTextChanged(const QString &arg1)
{
    int index = STR_TEST_TYPE.indexOf(arg1);
    if (index > TEST_TYPE_INDEX_68)
        ui->cbComName->setEnabled(false);
    else
        ui->cbComName->setEnabled(true);
}

void TestItemInterface::on_cbTestType_currentIndexChanged(int index)
{
    if (index > TEST_TYPE_INDEX_68)
        ui->cbComName->setEnabled(false);
    else
        ui->cbComName->setEnabled(true);
}
