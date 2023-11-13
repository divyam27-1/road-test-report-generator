#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QEvent>
#include <QWheelEvent>

#include <vector>
#include <string>

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
        float Weight_of_sample_of_water[5][4];
//        array[0][0-3] = material_type = 10mm
//        array[1][0-3] = material_type = 20mm
//        array[2][0-3] = material_type = 30mm and so on...
        float Weight_of_SSD_Sample[5][4];
        float Weight_of_Oven_dry_sample[5][4];
    };

    bool get_token(std::string line, int pos, int *output);
    //fei
    float passing[10];
    float retained[10];
    float A[15];
    float B[15];
    float C[15];
    float D[15];
    float flakiness_index;
    float elongation_index;
    float combined_f_e_index;//fei
    //AIV
    float weight_of_cylinder[3][4];
    float weight_of_cylinder_sample[3][4];
    float weight_crushed_material[3][4];
    float weight_of_sample[3][4];
    float aggregate_impact_value[3][4];
    float avg_aggregate_impact_value;

    std::vector<std::string> tracked_files;
    std::string OS;


private slots:
    void on_spc_save_10mm_clicked();

    void on_tabWidget_tabCloseRequested(int index);

    void on_spc_save_20mm_clicked();

    void on_spc_save_clicked();

    void on_spc_data_scroll_valueChanged(int value);

    void on_spc_save_40mm_clicked();

    void on_ind_data_scroll_valueChanged(int value);

    void on_aiv_data_scroll_valueChanged(int value);

    void on_spc_save_0mm_clicked();

    void on_spc_export_clicked();

    void on_save_fei_clicked();

    void on_save_ss_clicked();

    void on_fei_export_clicked();

    void on_aiv_save_20mm_clicked();

    void on_aiv_20_21_textChanged(const QString &arg1);

    void on_aiv_20_11_textChanged(const QString &arg1);

    void on_aiv_20_12_textChanged(const QString &arg1);

    void on_aiv_20_22_textChanged(const QString &arg1);

    void on_aiv_20_13_textChanged(const QString &arg1);

    void on_aiv_20_23_textChanged(const QString &arg1);

    void on_aiv_10_11_textChanged(const QString &arg1);

    void on_aiv_10_12_textChanged(const QString &arg1);

    void on_aiv_10_13_textChanged(const QString &arg1);

    void on_aiv_10_21_textChanged(const QString &arg1);

    void on_aiv_10_22_textChanged(const QString &arg1);

    void on_aiv_10_23_textChanged(const QString &arg1);

    void on_aiv_20_41_textChanged(const QString &arg1);

    void on_aiv_10_41_textChanged(const QString &arg1);

    void on_aiv_10_42_textChanged(const QString &arg1);

    void on_aiv_10_43_textChanged(const QString &arg1);

    void on_aiv_20_6_clicked();

    void on_aiv_10_6_clicked();

    void on_aiv_20_42_textChanged(const QString &arg1);

    void on_aiv_20_43_textChanged(const QString &arg1);

    void on_fei_save_clicked();

    void on_actionExport_to_PDF_triggered();

private:
    float scroll_sens;

    Ui::MainWindow *ui;

    void wheelEvent(QWheelEvent *event) override;
};
#endif // MAINWINDOW_H
