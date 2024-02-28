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
    float avg_aggregate_impact_value_10;
    float avg_aggregate_impact_value_20;
//fei
    //individual gradation
    float is_sieve_40[5][9];
    float weight_of_retained_40[5][9];
    float ind_cumulative[5][9];
    float ind_cumulative_percent[5][9];
    float ind_cum_pass[5][9];
    float total_weight[3];


    std::vector<std::string> tracked_files;
    std::string OS;


    void save_check();
    void generate_html_spc();
    void generate_html_grad();
public slots:
    void on_fei_export_clicked();
private slots:

    void on_spc_save_10mm_clicked();

    void on_spc_save_20mm_clicked();

    void on_spc_save_clicked();

    void on_spc_data_scroll_valueChanged(int value);

    void on_spc_save_40mm_clicked();

    void on_ind_data_scroll_valueChanged(int value);

    void on_aiv_data_scroll_valueChanged(int value);

    void on_spc_save_0mm_clicked();

    void on_save_fei_clicked();

    void on_save_ss_clicked();

    void generate_html_fei();

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

    void on_aiv_20_42_textChanged(const QString &arg1);

    void on_aiv_20_43_textChanged(const QString &arg1);

    void on_aiv_20_6_clicked();

    void on_aiv_10_6_clicked();

    void on_fei_save_clicked();

    void on_actionExport_to_PDF_triggered();

    void on_aiv_save_10mm_clicked();

    void generate_html_aiv();

    void on_aiv_save_clicked();

    void on_idg_save_40mm_clicked();

    void on_ind_save_clicked();

    void on_idg_save_20mm_clicked();

    void on_idg_save_10mm_clicked();

    void on_idg_save_d_clicked();

    void generate_html_ind();

    void on_actionSave_Project_triggered();

    void on_cd_save_clicked();

    void on_ind_graph_update_clicked();

    void updateGraph_idg();

    void updateGraph_mdd();

    void on_mdd_save_update_clicked();

    void on_mdd_save_clicked();

    void generate_html_mdd();

    void on_spc_saveas_clicked();

    void on_fei_saveas_clicked();

    void on_aiv_saveas_clicked();

    void on_ind_saveas_clicked();

    void on_mdd_saveas_clicked();

    void on_grad_save_clicked();

    void on_grad_data_scroll_valueChanged(int value);

    void updateGraph_grad();

    void on_actionWMM_triggered();

    void on_actionDBM_triggered();

    void on_spc_export_clicked();

    void on_aiv_export_clicked();
    void on_ind_export_clicked();
    void on_mdd_export_clicked();
    void on_grad_export_clicked();
    void on_grad_saveas_clicked();
    void on_tensile_save_clicked();

    void on_marshall_scroll_valueChanged(int value);

    void on_marshall_saveas_clicked();

    void on_tensile_saveas_clicked();
    void on_vol_scroll_valueChanged(int value);

    void on_marshall_save_clicked();
private:
    float scroll_sens;

    Ui::MainWindow *ui;

    void wheelEvent(QWheelEvent *event) override;
    void generate_html_tensile();
};
#endif // MAINWINDOW_H
