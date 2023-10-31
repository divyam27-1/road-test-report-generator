#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QEvent>
#include <QWheelEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    struct basic_info {
        QString name_of_work;
        QString name_of_client;
        QString name_of_consultant;
        QString name_of_contractor;
    };

    struct exp_info {
        QString date_of_sampling;
        QString date_of_testing;
        QString sample_no;
        QString sample_at;
        QString source_of_material;
        QString type_of_material;
    };

private slots:
    void on_pushButton_clicked();

    void on_tabWidget_tabCloseRequested(int index);

    void on_pushButton_2_clicked();

    void on_spc_save_clicked();

    void on_spc_data_scroll_valueChanged(int value);


    void on_ind_scroll_bar_3_valueChanged(int value);

private:
    float scroll_sens;

    Ui::MainWindow *ui;

    void wheelEvent(QWheelEvent *event) override;
};
#endif // MAINWINDOW_H
