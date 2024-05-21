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

    /**
 * @defgroup RheologyEvaluationSubfuncs Rheology Evaluation Subfunctions
 * @ingroup SaveSubfuncs
 * @brief Helper functions that update UI to reflect on user input and assist in the 'Save' operation for Rheology.
 */

    /**
 * @ingroup RheologyEvaluationSubfuncs
 * @brief Evaluates the softness of a material.
 *
 * @param soft_in The input JSON object containing the softness values.
 * @return The updated JSON object with the calculated mean softness value.
 */
    QJsonObject soft_eval(QJsonObject soft_in);

    /**
 * @ingroup RheologyEvaluationSubfuncs
 * @brief Evaluates the strip of a material.
 *
 * @param strip_in The input JSON object.
 * @return The same JSON object that was input.
 */
    QJsonObject strip_eval(QJsonObject strip_in);

    /**
 * @ingroup RheologyEvaluationSubfuncs
 * @brief Evaluates the penetration of a material.
 *
 * @param pen_in The input JSON object containing the penetration values.
 * @return The updated JSON object with the calculated penetration values.
 */
    QJsonObject pen_eval(QJsonObject pen_in);

    /**
 * @ingroup RheologyEvaluationSubfuncs
 * @brief Evaluates the ductility of a material.
 *
 * @param ductility_in The input JSON object containing the ductility values.
 * @return The updated JSON object with the calculated mean ductility value.
 */
    QJsonObject ductility_eval(QJsonObject ductility_in);

    /**
 * @ingroup RheologyEvaluationSubfuncs
 * @brief Evaluates the flash point of a material.
 *
 * @param flash_in The input JSON object containing the flash point values.
 * @return The updated JSON object with the calculated mean flash point value.
 */
    QJsonObject flash_eval(QJsonObject flash_in);

    /**
 * @ingroup RheologyEvaluationSubfuncs
 * @brief Evaluates the viscosity of a material.
 *
 * @param viscosity_in The input JSON object containing the viscosity values.
 * @return The updated JSON object with the calculated viscosity values.
 */
    QJsonObject viscosity_eval(QJsonObject viscosity_in);

    /**
 * @ingroup RheologyEvaluationSubfuncs
 * @brief Evaluates the specific gravity of a material.
 *
 * @param spc_in The input JSON object containing the specific gravity values.
 * @return The updated JSON object with the calculated specific gravity values.
 */
    QJsonObject spc_eval(QJsonObject spc_in);

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
    void on_gmm_scroll_valueChanged(int value);

    void on_rheology_scroll_valueChanged(int value);

    void on_vol_save_clicked();
    void on_wa_scroll_valueChanged(int value);

    void on_gmm_save_clicked();

    void on_rheology_save_clicked();

    void on_wa_save_clicked();

    void on_vol_saveas_clicked();
    void on_gmm_saveas_clicked();
    void on_rheology_saveas_clicked();
    void on_wa_saveas_clicked();
    void on_marshall_export_clicked();
    void on_tensile_export_clicked();
    void on_vol_export_clicked();
    void on_gmm_export_clicked();
    void on_rheology_export_clicked();
    void on_wa_export_clicked();
private:
    float scroll_sens;

    Ui::MainWindow *ui;

    void wheelEvent(QWheelEvent *event) override;
    void generate_html_tensile();
    void generate_html_marshall();
    void generate_html_vol();
    void generate_html_gmm();
    void generate_html_rheology();
    void generate_html_wa();
    void updateGraph_dbm();
    void generate_bitumen_gmb_graph(std::vector<QJsonObject> marshall_levels);
    void generate_bitumen_stability_graph(std::vector<QJsonObject> marshall_levels);
    void generate_bitumen_flow_graph(std::vector<QJsonObject> marshall_levels);
    void generate_bitumen_va_graph(std::vector<QJsonObject> marshall_levels, QJsonObject vol);
    void generate_bitumen_vma_graph(std::vector<QJsonObject> marshall_levels, QJsonObject vol);
    void generate_bitumen_vfb_graph(std::vector<QJsonObject> marshall_levels, QJsonObject vol);
};
#endif // MAINWINDOW_H
