#ifndef TEST_ITEM_INTERFACE_H
#define TEST_ITEM_INTERFACE_H

#include "analysis_ini.h"
#include <QWidget>

namespace Ui
{
class TestItemInterface;
}

class TestItemInterface : public QWidget
{
    Q_OBJECT

public:
    explicit TestItemInterface(QWidget *parent = nullptr);
    ~TestItemInterface();
    void setIndex(int index);

    // 配置
    void setTestItem(int index, const TestCmd &item);

private slots:
    void on_cbAnalysis_currentIndexChanged(int index);

    void on_cbDisplayResult_currentIndexChanged(int index);

    void on_leTx_textChanged(const QString &arg1);

    void on_leRx_textChanged(const QString &arg1);

    void on_leRx_editingFinished();

    void on_leTx_editingFinished();

    void on_cbTestType_currentIndexChanged(int index);

    void on_cbDataLimit_currentTextChanged(const QString &arg1);

private:
    Ui::TestItemInterface *ui;
};

#endif // TEST_ITEM_INTERFACE_H
