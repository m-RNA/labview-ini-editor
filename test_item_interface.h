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
    void setUi(int index, const TestCmd &item);

private slots:

    void on_leTx_textChanged(const QString &arg1);

    void on_leRx_textChanged(const QString &arg1);

    void on_leRx_editingFinished();

    void on_leTx_editingFinished();

    void on_cbTestType_currentIndexChanged(int index);

private:
    Ui::TestItemInterface *ui;
};

#endif // TEST_ITEM_INTERFACE_H
