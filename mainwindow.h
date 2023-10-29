#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "analysis_ini.h"
#include <QMainWindow>
#include <QTreeWidgetItem>


QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_btnAddTestItem_clicked();

    void on_actOpenIni_triggered();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::MainWindow *ui;

    QVector<TestItem> testItemList = {};
    QVector<QWidget *> testItemInterfaceList = {};
};
#endif // MAINWINDOW_H
