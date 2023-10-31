#include "test_result_interface.h"
#include "ui_test_result_interface.h"

TestResultInterface::TestResultInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestResultInterface)
{
    ui->setupUi(this);
}

TestResultInterface::~TestResultInterface()
{
    delete ui;
}
