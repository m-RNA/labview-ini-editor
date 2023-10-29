#include "mainwindow.h"
#include "test_item_interface.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    // 往 scrollArea 添加一个 TestItemInterface
    TestItemInterface *item = new TestItemInterface(ui->scrollAreaWidgetContents);
    // item->setObjectName(QString::number(arg1)); // 设置对象名
    item->setIndex(arg1);
    ui->vloTestItem->insertWidget(ui->vloTestItem->count() - 2, item);
    // 滚动到底部
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
}

void MainWindow::on_btnAddTestItem_clicked()
{
    // 往 scrollArea 添加一个 TestItem
    TestItemInterface *item = new TestItemInterface(ui->scrollAreaWidgetContents);
    item->setIndex(1);
    ui->vloTestItem->insertWidget(ui->vloTestItem->count() - 2, item);
    // 滚动到底部
    ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());
}
