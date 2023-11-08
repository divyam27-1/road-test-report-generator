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
    struct type_of_material{
        QString Weight_of_sample_of_water[5][4];
//        array[0][0-3] = material_type = 10mm
//        array[1][0-3] = material_type = 20mm
//        array[2][0-3] = material_type = 30mm and so on...
        QString Weight_of_SSD_Sample[5][4];
        QString Weight_of_Oven_dry_sample[5][4];
    };

private slots:
    void on_pushButton_clicked();

    void on_tabWidget_tabCloseRequested(int index);

    void on_pushButton_2_clicked();

    void on_spc_save_clicked();

    void on_spc_data_scroll_valueChanged(int value);

    void on_save_40mm_clicked();

    void on_ind_data_scroll_valueChanged(int value);

    void on_aiv_data_scroll_valueChanged(int value);

    void on_pushButton_3_clicked();

    void specificgravity(QJsonDocument jsonDoc_1,QFile file);

    void on_spc_export_clicked();

private:
    float scroll_sens;

    Ui::MainWindow *ui;

    void wheelEvent(QWheelEvent *event) override;
};
#endif // MAINWINDOW_H
