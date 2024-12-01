#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QTableView>
#include <QLayout>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QSqlDatabase db;

    bool connectToDatabase();


    QSqlTableModel* createTableModel();
    QTableView* createTableView(QSqlTableModel *model);
    QLayout* createInputLayout(QSqlTableModel *model);
    QLayout* createDeleteLayout(QSqlTableModel *model);
    QLayout* createEditLayout(QSqlTableModel *model);
    QLayout* createSearchLayout(QSqlTableModel *model);
    QLayout* createSortLayout(QSqlTableModel *model);
    QLayout* createStatusEditLayout(QSqlTableModel *model);


};

#endif
