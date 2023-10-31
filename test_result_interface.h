#ifndef TEST_RESULT_INTERFACE_H
#define TEST_RESULT_INTERFACE_H

#include <QWidget>

namespace Ui {
class TestResultInterface;
}

class TestResultInterface : public QWidget
{
    Q_OBJECT

public:
    explicit TestResultInterface(QWidget *parent = nullptr);
    ~TestResultInterface();

private:
    Ui::TestResultInterface *ui;
};

#endif // TEST_RESULT_INTERFACE_H
