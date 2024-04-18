#include "mainwindow.h"
#include "qjsondocument.h"
#include "ui_mainwindow.h"
#include "QJsonObject"
#include "QFile"
#include "QJsonDocument"
#include "QTextStream"
#include "qdebug.h"
#include <fstream>
#include <QDir>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <QProcess>
#include <stdlib.h>
#include <qcustomplot.h>
#include <QMessageBox>
#include <QMetaObject>
#include <QFileDialog>
#include <QDateTime>
#include <map>
#include <functional>
#include <algorithm>

/* TODO:
- Generation of HTML for :-
1) finish graphing (6 graphs) DONION RINGS
2) make the rheology elements autoupdate on saving DONION RINGS
3) add link between gmb on vol and gmb on marshall MISSION IMPOSSIBLE
4) make the pdf packages DONION RINGS
5) add the export solo buttons
6) build for distribution
*/

QDir cwd = QDir::current();
QDir swd = cwd;
bool i = cwd.cdUp();

//T Flip Flop, its used in some functions to fix some bugs
bool TFF = true;

std::vector<std::string> tracked_files;
const std::string all_experiments[] = {"spc", "aiv", "fei", "ind", "mdd", "grad", "marshall", "tensile", "vol", "gmm", "rheology", "wa"};
const std::vector<QString> all_exp_qstr {"spc", "aiv", "fei", "ind", "mdd", "grad", "marshall", "tensile", "vol", "gmm", "rheology", "wa"};
const std::map<QString, QString> exp_classify = {{"spc", "wmm"},
                                                 {"aiv", "wmm"},
                                                 {"fei", "wmm"},
                                                 {"ind", "wmm"},
                                                 {"mdd", "wmm"},
                                                 {"grad", "dbm"},
                                                 {"marshall", "dbm"},
                                                 {"tensile", "dbm"},
                                                 {"wa", "dbm"},
                                                 {"vol", "dbm"},
                                                 {"gmm", "dbm"},
                                                 {"rheology", "dbm"}};

std::string OS;
bool saveas_done = false;

void removeDuplicates(std::vector<std::string> &vec, const std::vector<QString> &ref = all_exp_qstr)
{
    // Remove duplicates from vec
    std::sort(vec.begin(), vec.end());
    auto it = std::unique(vec.begin(), vec.end());
    vec.erase(it, vec.end());

    // Create a mapping from elements of ref to their indices
    std::map<QString, int> refIndexMap;
    for (int i = 0; i < ref.size(); ++i) {
        refIndexMap[ref[i]] = i;
    }

    // Custom comparator function to sort vec based on the order of ref
    auto comparator = [&refIndexMap](const std::string &a, const std::string &b) {
        auto itA = refIndexMap.find(QString::fromStdString(a));
        auto itB = refIndexMap.find(QString::fromStdString(b));
        // If either element is not found in ref, keep their relative order
        if (itA == refIndexMap.end() || itB == refIndexMap.end()) {
            return a < b;
        }
        // Otherwise, sort based on their order in ref
        return itA->second < itB->second;
    };

    // Sort vec according to the order of ref
    std::sort(vec.begin(), vec.end(), comparator);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cwd.mkdir("json");
    cwd.mkdir("templates");
    cwd.mkdir("output");
    cwd.mkdir("html");

    ui->spc_frame->move(0, 0);
    ui->aiv_frame->move(0, 0);
    ui->ind_frame->move(0, 0);

    for (int i = 0; i < 4; i++)
    {
        ui->ind_graph_1->addGraph();
        ui->ind_graph_2->addGraph();
        ui->grad_graph_1->addGraph();
        ui->grad_graph_2->addGraph();
    }
    ui->mdd_graph->addGraph();
    ui->mdd_graph->addGraph();
    ui->grad_graph_2->addGraph();
    ui->grad_graph_2->addGraph();

    ui->grad_bld_graph_label->hide();
    ui->jmf_graph_label->hide();

#ifdef _WIN32
    OS = "win";
#elif __linux__
    OS = "linux";
#elif __APPLE__
    OS = "apple";
#endif

    qDebug() << OS;
    qDebug() << cwd;

    save_check();
}

MainWindow::~MainWindow()
{
    delete ui;
}



// Deals with saving to JSON
void MainWindow::on_actionSave_Project_triggered()
{
    ui->spc_save->click();
    ui->fei_save->click();
    ui->aiv_save->click();
    ui->ind_save->click();
    ui->grad_save->click();
}
void MainWindow::on_spc_save_clicked()
{
    ui->spc_save_0mm->click();
    ui->spc_save_10mm->click();
    ui->spc_save_20mm->click();
    ui->spc_save_40mm->click();
}
void MainWindow::on_fei_save_clicked()
{
    ui->save_ss->click();
    ui->save_fei->click();
}
void MainWindow::on_aiv_save_clicked()
{
    ui->aiv_save_10mm->click();
    ui->aiv_save_20mm->click();
}
void MainWindow::on_ind_save_clicked()
{
    ui->idg_save_40mm->click();
    ui->idg_save_20mm->click();
    ui->idg_save_10mm->click();
    ui->idg_save_d->click();
    ui->cd_save->click();
    updateGraph_idg();
}
void MainWindow::on_mdd_save_clicked()
{
    tracked_files.push_back("mdd");
    removeDuplicates(tracked_files);

    float mdd_ds[5][5];
    mdd_ds[0][0] = ui->wsm_1->text().toFloat();
    mdd_ds[0][1] = ui->wsm_2->text().toFloat();
    mdd_ds[0][2] = ui->wsm_3->text().toFloat();
    mdd_ds[0][3] = ui->wsm_4->text().toFloat();
    mdd_ds[0][4] = ui->wsm_5->text().toFloat();

    mdd_ds[1][0] = ui->tray_1->text().toFloat();
    mdd_ds[1][1] = ui->tray_2->text().toFloat();
    mdd_ds[1][2] = ui->tray_3->text().toFloat();
    mdd_ds[1][3] = ui->tray_4->text().toFloat();
    mdd_ds[1][4] = ui->tray_5->text().toFloat();

    mdd_ds[2][0] = ui->wt_tray_1->text().toFloat();
    mdd_ds[2][1] = ui->wt_tray_2->text().toFloat();
    mdd_ds[2][2] = ui->wt_tray_3->text().toFloat();
    mdd_ds[2][3] = ui->wt_tray_4->text().toFloat();
    mdd_ds[2][4] = ui->wt_tray_5->text().toFloat();

    mdd_ds[3][0] = ui->wst_1->text().toFloat();
    mdd_ds[3][1] = ui->wst_2->text().toFloat();
    mdd_ds[3][2] = ui->wst_3->text().toFloat();
    mdd_ds[3][3] = ui->wst_4->text().toFloat();
    mdd_ds[3][4] = ui->wst_5->text().toFloat();

    mdd_ds[4][0] = ui->wds_1->text().toFloat();
    mdd_ds[4][1] = ui->wds_2->text().toFloat();
    mdd_ds[4][2] = ui->wds_3->text().toFloat();
    mdd_ds[4][3] = ui->wds_4->text().toFloat();
    mdd_ds[4][4] = ui->wds_5->text().toFloat();

    float mass = ui->lineEdit->text().toFloat();
    float vol = ui->lineEdit_2->text().toFloat();

    QFile mdd_json(cwd.filePath("json/mdd.json"));

    QJsonObject mdd;
    for (int i = 0; i < 5; i++)
    {
        std::string I = std::to_string(i);
        std::string wsm = "wsm" + I, ws = "ws" + I, wds = "wds" + I, tray = "tray" + I, wt_tray = "wt_tray" + I, wst = "wst" + I, wdst = "wdst" + I, ww = "ww" + I, wdsm = "wdsm" + I, water_contents = "water_content" + I, density = "density" + I;

        float wet_density = (mdd_ds[0][i] - mass) / vol;
        float weight_of_water = mdd_ds[3][i] - mdd_ds[4][i];
        float weight_of_dry_sample = mdd_ds[4][i] - mdd_ds[2][i];
        float water_content = 100 * weight_of_water / weight_of_dry_sample;
        float dry_density = 100 * (wet_density / (100 + water_content));

        mdd[QString::fromStdString(wsm)] = mdd_ds[0][i];
        mdd[QString::fromStdString(ws)] = mdd_ds[0][i] - mass;
        mdd[QString::fromStdString(tray)] = mdd_ds[1][i];
        mdd[QString::fromStdString(wt_tray)] = mdd_ds[2][i];
        mdd[QString::fromStdString(wst)] = mdd_ds[3][i];
        mdd[QString::fromStdString(wdst)] = mdd_ds[4][i];
        mdd[QString::fromStdString(wds)] = wet_density;
        mdd[QString::fromStdString(ww)] = weight_of_water;
        mdd[QString::fromStdString(wdsm)] = weight_of_dry_sample;
        mdd[QString::fromStdString(water_contents)] = water_content;
        mdd[QString::fromStdString(density)] = dry_density;
    }

    mdd["mass"] = mass;
    mdd["vol"] = vol;

    if (mdd_json.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&mdd_json);
        QJsonDocument jsonDoc_1(mdd);

        out << jsonDoc_1.toJson();

        // Close the file
        mdd_json.close();
    }
    else
    {
        qDebug() << "SUYGETSU AIMS THE RESET ABSOLUTELY INCREDIBLE";
    }

    save_check();
}
void MainWindow::on_grad_save_clicked()
{
    tracked_files.push_back("grad");
    removeDuplicates(tracked_files);

    updateGraph_grad();

    QJsonObject grad_json;

    /* Triple nested loop to save JSON for gradation
        i: top level loop for different types of experiments (21mm, 16mm, 4mm)
        j: mid level loop for different is seive values in one experiment
        k: bottom level loop for different samples of a seive value     */
    for (int i = 1; i <= 3; i++)
    {
        QJsonObject grad_json_mm;

        // this block and the if else stmt get the proportion of a specific experiment in the dbm mixture
        QString prop_spinbox_name = QString("grad_prop_%1").arg(i);
        QDoubleSpinBox *spinbox = ui->grad_frame_outer->findChild<QDoubleSpinBox *>(prop_spinbox_name);
        double proportion = spinbox->value() / 100;

        if (spinbox)
        {
            grad_json_mm["proportion"] = proportion * 100;
        }
        else
        {
            qDebug() << "Could not find a child with name" << prop_spinbox_name;
        }

        for (int j = 1; j <= 8; j++)
        {

            // This next part simply takes the average of the 5 samples and saves the sample data along with the average to JSON
            double sum = 0;
            for (int k = 1; k <= 5; k++)
            {

                QString object_name = QString("grad_p%1%2_%3").arg(i).arg(j).arg(k);

                QLineEdit *tedit = ui->dbm_page->findChild<QLineEdit *>(object_name);

                if (tedit)
                {
                    double num = tedit->text().toDouble();
                    grad_json_mm[object_name] = num;
                    sum += num;
                }
                else
                {
                    qDebug() << "Could not find a child with name " << object_name;
                }
            }

            QString avg_key = QString("avg_%1").arg(j);
            grad_json_mm[avg_key] = sum / 5;

            // This part takes the proportion of the average we will take for the final mix
            double avg_prop = proportion * sum / 5;
            grad_json_mm["prop_" + avg_key] = avg_prop;
        }

        QString top_level_key;

        switch (i)
        {
        case 1:
            top_level_key = "25-16mm";
            break;
        case 2:
            top_level_key = "16-4.75mm";
            break;
        case 3:
            top_level_key = "below_4.75mm";
            break;
        default:
            top_level_key = "error in i in on_grad_save_clicked";
            break;
        }

        grad_json[top_level_key] = grad_json_mm;
    }

    /* For blending the values, we will have to do a few more sweeps of the data, this time taking the weighted average
       of our sample averages across diff experiments to blend them. The proportional_passing_averages data structure
       was made to temporarily hold the values as they are averaged*/
    double proportinal_passing_averages[8];
    for (int i = 0; i < 8; i++)
    {
        proportinal_passing_averages[i] = 0;
    }

    for (QString key : grad_json.keys())
    {

        QJsonObject grad_json_mm = grad_json[key].toObject();

        for (int i = 0; i < 8; i++)
        {
            QString prop_avg_key = QString("prop_avg_%1").arg(i+1);
            proportinal_passing_averages[i] += grad_json_mm[prop_avg_key].toDouble();
        }
    }

    // create the blending json object, which will added to the grad json file as its own top level json class
    QJsonObject grad_bld_json;
    for (int i = 0; i < 8; i++)
    {
        grad_bld_json[QString("bld_%1").arg(i + 1)] = proportinal_passing_averages[i];
    }

    grad_json["blending"] = grad_bld_json;

    QJsonObject seive_sizes;
    // Since the sieve sizes are the same we just save the seives from the first one
    for (int i = 1; i <= 8; i++)
    {
        QLineEdit *tedit = ui->grad_frame_outer->findChild<QLineEdit *>(QString("grad_s1_%1").arg(i));
        if (tedit)
        {
            seive_sizes[QString("is_seive_%1").arg(i)] = tedit->text().toDouble();
        }
        else
        {
            qDebug() << "seive tedit error";
        }
    }
    grad_json["seive_sizes"] = seive_sizes;

    // Boilerplate write to file code
    QFile grad_file(cwd.filePath("json/grad.json"));

    if (!grad_file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::information(this, "Tits", "Error opening grad json file");
    }

    QTextStream out(&grad_file);
    QJsonDocument json_doc(grad_json);
    out << json_doc.toJson();
    grad_file.close();

    save_check();

    if (TFF) {
        TFF = false;
        ui->grad_save->click();
    } else {
        TFF = true;
    }
}
void MainWindow::on_tensile_save_clicked()
{
    tracked_files.push_back("tensile");
    removeDuplicates(tracked_files);

    QJsonObject tensile_json;
    QJsonObject min_30, hr_24;

    min_30["tensile_btmn"] = ui->tensile_btmn_1->text().toDouble();
    hr_24["tensile_btmn"] = ui->tensile_btmn_2->text().toDouble();
    tensile_json["ring"] = ui->tensile_ring->text().toDouble();

    QStringList tensile_exp_names = {"tensile_wt_%1_%2", "tensile_ssd_%1_%2", "tensile_gmb_%1_%2", "tensile_read_%1_%2", "tensile_corr_%1_%2", "tensile_flow_%1_%2"};

    for (QString exp : tensile_exp_names)
    {
        for (int i = 1; i <= 2; i++)
        {
            for (int j = 1; j <= 3; j++)
            {

                QString obj_name = exp.arg(i).arg(j);
                QLineEdit *tedit = ui->dbm_page->findChild<QLineEdit *>(obj_name);

                double tedit_text = tedit->text().toDouble();
                if (i == 1)
                {
                    min_30[obj_name] = tedit_text;
                }
                else if (i == 2)
                {
                    hr_24[obj_name] = tedit_text;
                }
            }
        }
    }

    // calculation for volume
    double volum;
    QString volum_key = QString("tensile_wt_%1_%2_vol");
    for (int i = 1; i <= 2; i++)
    {
        for (int j = 1; j <= 3; j++)
        {
            QString wt_obj_name = QString("tensile_wt_%1_%2").arg(i).arg(j);
            QString gmb_obj_name = QString("tensile_gmb_%1_%2").arg(i).arg(j);

            QLineEdit *wt_tedit = ui->dbm_page->findChild<QLineEdit *>(wt_obj_name);
            QLineEdit *gmb_tedit = ui->dbm_page->findChild<QLineEdit *>(gmb_obj_name);
            double wt_value = wt_tedit->text().toDouble();
            double gmb_value = gmb_tedit->text().toDouble();

            volum = wt_value / gmb_value;

            volum_key = QString("tensile_wt_%1_%2_vol").arg(i).arg(j);
            if (i == 1)
            {
                min_30[volum_key] = volum;
            }
            else if (i == 2)
            {
                hr_24[volum_key] = volum;
            }
        }
    }

    // calculation for wt in air
    for (int i = 1; i <= 2; i++)
    {
        for (int j = 1; j <= 3; j++)
        {
            QString wt_obj_name = QString("tensile_wt_%1_%2_vol").arg(i).arg(j);
            QString ssd_obj_name = QString("tensile_ssd_%1_%2").arg(i).arg(j);

            double wt_vol = 0.0;
            if (i == 1)
            {
                wt_vol = min_30[wt_obj_name].toDouble();
            }
            else if (i == 2)
            {
                wt_vol = hr_24[wt_obj_name].toDouble();
            }

            QLineEdit *ssd_tedit = ui->dbm_page->findChild<QLineEdit *>(ssd_obj_name);

            double ssd_value = ssd_tedit->text().toDouble();

            double result = ssd_value - wt_vol;
            qDebug() << result << ssd_value << wt_vol;

            QString result_key = QString("tensile_ssd_wt_diff_%1_%2").arg(i).arg(j);
            if (i == 1)
            {
                min_30[result_key] = result;
            }
            else if (i == 2)
            {
                hr_24[result_key] = result;
            }
        }
    }

    // calculation for load
    for (int i = 1; i <= 2; i++)
    {
        for (int j = 1; j <= 3; j++)
        {
            QString read_obj_name = QString("tensile_read_%1_%2").arg(i).arg(j);

            QLineEdit *read_tedit = ui->dbm_page->findChild<QLineEdit *>(read_obj_name);

            double read_value = read_tedit->text().toDouble();

            double ring = tensile_json["ring"].toDouble();

            double result = ring * read_value;
            QString result_key = QString("tensile_read_%1_%2_load").arg(i).arg(j);

            if (i == 1)
            {
                min_30[result_key] = result;
            }
            else if (i == 2)
            {
                hr_24[result_key] = result;
            }
        }
    }

    // calculation for corrected load
    for (int i = 1; i <= 2; i++)
    {
        for (int j = 1; j <= 3; j++)
        {
            QString wt_obj_name = QString("tensile_read_%1_%2_load").arg(i).arg(j);
            QString corr_obj_name = QString("tensile_corr_%1_%2").arg(i).arg(j);

            double reading = 0.0;
            if (i == 1)
            {
                reading = min_30[wt_obj_name].toDouble();
            }
            else if (i == 2)
            {
                reading = hr_24[wt_obj_name].toDouble();
            }

            QLineEdit *corr_tedit = ui->dbm_page->findChild<QLineEdit *>(corr_obj_name);

            double ssd_value = corr_tedit->text().toDouble();

            double result = ssd_value * reading;

            QString result_key = QString("tensile_corrected_load_%1_%2").arg(i).arg(j);
            if (i == 1)
            {
                min_30[result_key] = result;
            }
            else if (i == 2)
            {
                hr_24[result_key] = result;
            }
        }
    }

    // calculation for averages
    for (int i = 1; i <= 2; i++)
    {
        double avg_gmb = 0, avg_corr_load = 0, avg_flow = 0;
        for (int j = 1; j <= 3; j++)
        {
            QString gmb_name = QString("tensile_gmb_%1_%2").arg(i).arg(j);
            QString corr_load_name = QString("tensile_corrected_load_%1_%2").arg(i).arg(j);
            QString flow_name = QString("tensile_flow_%1_%2").arg(i).arg(j);

            if (i == 1)
            {
                avg_gmb += min_30[gmb_name].toDouble() / 3;
                avg_corr_load += min_30[corr_load_name].toDouble() / 3;
                avg_flow += min_30[flow_name].toDouble() / 3;
            }
            else if (i == 2)
            {
                avg_gmb += hr_24[gmb_name].toDouble() / 3;
                avg_corr_load += hr_24[corr_load_name].toDouble() / 3;
                avg_flow += hr_24[flow_name].toDouble() / 3;
            }
            else
            {
                qDebug() << "index out of bounds while calculating tensile averages";
            }
        }

        if (i == 1)
        {
            min_30["avg_gmb"] = avg_gmb;
            min_30["avg_corrected_load"] = avg_corr_load;
            min_30["avg_flow"] = avg_flow;
        }
        else if (i == 2)
        {
            hr_24["avg_gmb"] = avg_gmb;
            hr_24["avg_corrected_load"] = avg_corr_load;
            hr_24["avg_flow"] = avg_flow;
        }
        else
        {
            qDebug() << "index out of bound while emplacing tensile averages";
        }
    }

    tensile_json["water_sensitivity"] = 100 * hr_24["avg_corrected_load"].toDouble() / min_30["avg_corrected_load"].toDouble();

    tensile_json["30mins"] = min_30;
    tensile_json["24hrs"] = hr_24;
    tensile_json["ring"] = ui->tensile_ring->text().toDouble();

    QFile tensile_json_file(cwd.filePath("json/tensile.json"));

    if (tensile_json_file.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&tensile_json_file);
        QJsonDocument jsonDoc_1(tensile_json);

        out << jsonDoc_1.toJson();

        // Close the file
        tensile_json_file.close();
    }
    else
    {
        qDebug() << "SUYGETSU AIMS THE RESET on tensile_save ABSOLUTELY INCREDIBLE";
    }
    save_check();
}
void MainWindow::on_marshall_save_clicked()
{

    tracked_files.push_back("marshall");
    removeDuplicates(tracked_files);

    QJsonObject marshall_json, level_1, level_2, level_3, level_4, level_5;

    level_1["marshall_1_00"] = ui->marshall_1_00->text().toDouble();
    level_2["marshall_2_00"] = ui->marshall_2_00->text().toDouble();
    level_3["marshall_3_00"] = ui->marshall_3_00->text().toDouble();
    level_4["marshall_4_00"] = ui->marshall_4_00->text().toDouble();
    level_5["marshall_5_00"] = ui->marshall_5_00->text().toDouble();

    QStringList marshall_exp_names = {"marshall_%1_%2%3"};

    for (QString exp : marshall_exp_names)
    {
        for (int i = 1; i <= 5; i++)
        {
            double sum_gmb = 0, sum_flow = 0;

            for (int j = 1; j <= 3; j++)
            {
                for (int k = 1; k <= 6; k++)
                {
                    QString obj_name = exp.arg(i).arg(j).arg(k);
                    QLineEdit *tedit = ui->dbm_page->findChild<QLineEdit *>(obj_name);

                    if (tedit)
                    {
                        double tedit_text = tedit->text().toDouble();

                        if (i == 1)
                        {
                            level_1[obj_name] = tedit_text;
                        }
                        else if (i == 2)
                        {
                            level_2[obj_name] = tedit_text;
                        }
                        else if (i == 3)
                        {
                            level_3[obj_name] = tedit_text;
                        }
                        else if (i == 4)
                        {
                            level_4[obj_name] = tedit_text;
                        }
                        else if (i == 5)
                        {
                            level_5[obj_name] = tedit_text;
                        }

                        if (k == 3) {
                            sum_gmb += tedit_text;
                        } else if (k == 6) {
                            sum_flow += tedit_text;
                        }
                    }
                    else
                    {
                        qDebug() << "input save out of bounds at:" << i << j << k;
                    }
                }
            }

            sum_gmb /= 3; sum_flow /= 3;
            if (i == 1)
            {
                level_1["avg_gmb"] = sum_gmb;
                level_1["avg_flow"] = sum_flow;
            }
            else if (i == 2)
            {
                level_2["avg_gmb"] = sum_gmb;
                level_2["avg_flow"] = sum_flow;
            }
            else if (i == 3)
            {
                level_3["avg_gmb"] = sum_gmb;
                level_3["avg_flow"] = sum_flow;
            }
            else if (i == 4)
            {
                level_4["avg_gmb"] = sum_gmb;
                level_4["avg_flow"] = sum_flow;
            }
            else if (i == 5)
            {
                level_5["avg_gmb"] = sum_gmb;
                level_5["avg_flow"] = sum_flow;
            }
        }
    }

    double sum_stb = 0;
    // level_1
    for (int j = 1; j <= 3; j++)
    {
        int k = 1;

            QString vol_obj_name = QString("marshall_vol_1_%1").arg(j);
            QString num_value = QString("marshall_1_%1%2").arg(j).arg(k);
            QString den_value = QString("marshall_1_%1%2").arg(j).arg(k + 2);

            QLineEdit *num_edit = ui->dbm_page->findChild<QLineEdit *>(num_value);
            QLineEdit *den_edit = ui->dbm_page->findChild<QLineEdit *>(den_value);

            double numerator = num_edit->text().toDouble();
            double denominator = den_edit->text().toDouble();

            if (denominator != 0)
            {
                double result = numerator / denominator;
                level_1[vol_obj_name] = result;
            }
    }

    for (int j = 1; j <= 3; j++)
    {
            int k = 2;
            QString wt_water_key = QString("marshall_wt_water_1_%1").arg(j);
            QString wt_water_num_value = QString("marshall_1_%1%2").arg(j).arg(k);
            QString wt_water_vol_value = QString("marshall_vol_1_%1").arg(j);

            QLineEdit *wt_water_num_edit = ui->dbm_page->findChild<QLineEdit *>(wt_water_num_value);

            if (wt_water_num_edit && level_1.contains(wt_water_vol_value))
            {
                double wt_water_numerator = wt_water_num_edit->text().toDouble();

                double wt_water_volume = level_1[wt_water_vol_value].toDouble();

                double wt_water = wt_water_numerator - wt_water_volume;

                level_1[wt_water_key] = wt_water;
            }
            else
            {
                qDebug() << "Error: LineEdit not found for j=" << j << " and k=" << k;
            }

    }

    for (int j = 1; j <= 3; j++)
    {
        int k = 4;
        QString load_key = QString("marshall_load_1_%1").arg(j);
        QString reading = QString("marshall_1_%1%2").arg(j).arg(k);
        QString ring = ("marshall_ring");
        QLineEdit *reading_num_edit = ui->dbm_page->findChild<QLineEdit *>(reading);
        QLineEdit *ring_edit = ui->dbm_page->findChild<QLineEdit *>(ring);
        double read_it = reading_num_edit->text().toDouble();

        double ring_it = ring_edit->text().toDouble();

        double result = read_it * ring_it;

        level_1[load_key] = result;
    }

    for (int j = 1; j <= 3; j++)
    {
        int k = 4;
            QString correct_load = QString("marshall_corrected_load_1_%1").arg(j);
            QString load = QString("marshall_load_1_%1").arg(j);
            QString volume_crc = QString("marshall_1_%1%2").arg(j).arg(k + 1);

            QLineEdit *vol_crc_edit = ui->dbm_page->findChild<QLineEdit *>(volume_crc);

            if (vol_crc_edit && level_1.contains(load))
            {
                double vol_crc_val = vol_crc_edit->text().toDouble();

                double load_it = level_1[load].toDouble();

                double result = vol_crc_val * load_it;
                sum_stb += result;

                level_1[correct_load] = result;
            }
            else
                {

                qDebug() << "Error: LineEdit not found for j=" << j << " and k=" << k;
            }
    }
    sum_stb /= 3;
    level_1["avg_stability"] = sum_stb;
    sum_stb = 0;

    // level_2
    for (int j = 1; j <= 3; j++)
    {
        int k = 1;

            QString vol_obj_name = QString("marshall_vol_2_%1").arg(j);
            QString num_value = QString("marshall_2_%1%2").arg(j).arg(k);
            QString den_value = QString("marshall_2_%1%2").arg(j).arg(k + 2);

            QLineEdit *num_edit = ui->dbm_page->findChild<QLineEdit *>(num_value);
            QLineEdit *den_edit = ui->dbm_page->findChild<QLineEdit *>(den_value);

            double numerator = num_edit->text().toDouble();
            double denominator = den_edit->text().toDouble();

            if (denominator != 0)
            {
                double result = numerator / denominator;
                level_2[vol_obj_name] = result;
            }
    }

    for (int j = 1; j <= 3; j++)
    {
        int k = 2;
            QString wt_water_key = QString("marshall_wt_water_2_%1").arg(j);
            QString wt_water_num_value = QString("marshall_2_%1%2").arg(j).arg(k);
            QString wt_water_vol_value = QString("marshall_vol_2_%1").arg(j);

            QLineEdit *wt_water_num_edit = ui->dbm_page->findChild<QLineEdit *>(wt_water_num_value);

            if (wt_water_num_edit && level_2.contains(wt_water_vol_value))
            {
                double wt_water_numerator = wt_water_num_edit->text().toDouble();

                double wt_water_volume = level_2[wt_water_vol_value].toDouble();

                double wt_water = wt_water_numerator - wt_water_volume;

                level_2[wt_water_key] = wt_water;
            }
            else
            {
                qDebug() << "Error: LineEdit not found for j=" << j << " and k=" << k;
            }
    }

    for (int j = 1; j <= 3; j++)
    {
        int k = 4;
        QString load_key = QString("marshall_load_2_%1").arg(j);
        QString reading = QString("marshall_2_%1%2").arg(j).arg(k);
        QString ring = ("marshall_ring");
        QLineEdit *reading_num_edit = ui->dbm_page->findChild<QLineEdit *>(reading);
        QLineEdit *ring_edit = ui->dbm_page->findChild<QLineEdit *>(ring);
        double read_it = reading_num_edit->text().toDouble();

        double ring_it = ring_edit->text().toDouble();

        double result = read_it * ring_it;

        level_2[load_key] = result;
    }

    for (int j = 1; j <= 3; j++)
    {
int k = 4;
            QString correct_load = QString("marshall_corrected_load_2_%1").arg(j);
            QString load = QString("marshall_load_2_%1").arg(j);
            QString volume_crc = QString("marshall_2_%1%2").arg(j).arg(k + 1);

            QLineEdit *vol_crc_edit = ui->dbm_page->findChild<QLineEdit *>(volume_crc);

            if (vol_crc_edit && level_2.contains(load))
            {
                double vol_crc_val = vol_crc_edit->text().toDouble();

                double load_it = level_2[load].toDouble();

                double result = vol_crc_val * load_it;
                sum_stb += result;

                level_2[correct_load] = result;
            }
            else
                {

                qDebug() << "Error: LineEdit not found for j=" << j << " and k=" << k;
            }

    }
    sum_stb /= 3;
    level_2["avg_stability"] = sum_stb;
    sum_stb = 0;

    // level_3
    for (int j = 1; j <= 3; j++)
    {
        int k = 1;
            QString vol_obj_name = QString("marshall_vol_3_%1").arg(j);
            QString num_value = QString("marshall_3_%1%2").arg(j).arg(k);
            QString den_value = QString("marshall_3_%1%2").arg(j).arg(k + 2);

            QLineEdit *num_edit = ui->dbm_page->findChild<QLineEdit *>(num_value);
            QLineEdit *den_edit = ui->dbm_page->findChild<QLineEdit *>(den_value);

            double numerator = num_edit->text().toDouble();
            double denominator = den_edit->text().toDouble();

            if (denominator != 0)
            {
                double result = numerator / denominator;
                level_3[vol_obj_name] = result;
            }

    }

    for (int j = 1; j <= 3; j++)
    {
int k = 2;
            QString wt_water_key = QString("marshall_wt_water_3_%1").arg(j);
            QString wt_water_num_value = QString("marshall_3_%1%2").arg(j).arg(k);
            QString wt_water_vol_value = QString("marshall_vol_3_%1").arg(j);

            QLineEdit *wt_water_num_edit = ui->dbm_page->findChild<QLineEdit *>(wt_water_num_value);

            if (wt_water_num_edit && level_3.contains(wt_water_vol_value))
            {
                double wt_water_numerator = wt_water_num_edit->text().toDouble();

                double wt_water_volume = level_3[wt_water_vol_value].toDouble();

                double wt_water = wt_water_numerator - wt_water_volume;

                level_3[wt_water_key] = wt_water;
            }
            else
            {
                qDebug() << "Error: LineEdit not found for j=" << j << " and k=" << k;
            }
    }

    for (int j = 1; j <= 3; j++)
    {
        int k = 4;
        QString load_key = QString("marshall_load_3_%1").arg(j);
        QString reading = QString("marshall_3_%1%2").arg(j).arg(k);
        QString ring = ("marshall_ring");
        QLineEdit *reading_num_edit = ui->dbm_page->findChild<QLineEdit *>(reading);
        QLineEdit *ring_edit = ui->dbm_page->findChild<QLineEdit *>(ring);
        double read_it = reading_num_edit->text().toDouble();

        double ring_it = ring_edit->text().toDouble();

        double result = read_it * ring_it;

        level_3[load_key] = result;
    }

    for (int j = 1; j <= 3; j++)
    {
        int k = 4;
            QString correct_load = QString("marshall_corrected_load_3_%1").arg(j);
            QString load = QString("marshall_load_3_%1").arg(j);
            QString volume_crc = QString("marshall_3_%1%2").arg(j).arg(k + 1);

            QLineEdit *vol_crc_edit = ui->dbm_page->findChild<QLineEdit *>(volume_crc);

            if (vol_crc_edit && level_3.contains(load))
            {
                double vol_crc_val = vol_crc_edit->text().toDouble();

                double load_it = level_3[load].toDouble();

                double result = vol_crc_val * load_it;
                sum_stb += result;

                level_3[correct_load] = result;
            }
            else
            {
                qDebug() << "Error: LineEdit not found for j=" << j << " and k=" << k;
            }
    }
    sum_stb /= 3;
    level_3["avg_stability"] = sum_stb;
    sum_stb = 0;

    // level_4
    for (int j = 1; j <= 3; j++)
    {
        int k = 1;

            QString vol_obj_name = QString("marshall_vol_4_%1").arg(j);
            QString num_value = QString("marshall_4_%1%2").arg(j).arg(k);
            QString den_value = QString("marshall_4_%1%2").arg(j).arg(k + 2);

            QLineEdit *num_edit = ui->dbm_page->findChild<QLineEdit *>(num_value);
            QLineEdit *den_edit = ui->dbm_page->findChild<QLineEdit *>(den_value);

            double numerator = num_edit->text().toDouble();
            double denominator = den_edit->text().toDouble();

            if (denominator != 0)
            {
                double result = numerator / denominator;
                level_4[vol_obj_name] = result;
            }
    }

    for (int j = 1; j <= 3; j++)
    {
        int k = 2;
            QString wt_water_key = QString("marshall_wt_water_4_%1").arg(j);
            QString wt_water_num_value = QString("marshall_4_%1%2").arg(j).arg(k);
            QString wt_water_vol_value = QString("marshall_vol_4_%1").arg(j);

            QLineEdit *wt_water_num_edit = ui->dbm_page->findChild<QLineEdit *>(wt_water_num_value);

            if (wt_water_num_edit && level_4.contains(wt_water_vol_value))
            {
                double wt_water_numerator = wt_water_num_edit->text().toDouble();

                double wt_water_volume = level_4[wt_water_vol_value].toDouble();

                double wt_water = wt_water_numerator - wt_water_volume;

                level_4[wt_water_key] = wt_water;
            }
            else
            {
                qDebug() << "Error: LineEdit not found for j=" << j << " and k=" << k;
            }
    }

    for (int j = 1; j <= 3; j++)
    {
        int k = 4;
        QString load_key = QString("marshall_load_4_%1").arg(j);
        QString reading = QString("marshall_4_%1%2").arg(j).arg(k);
        QString ring = ("marshall_ring");
        QLineEdit *reading_num_edit = ui->dbm_page->findChild<QLineEdit *>(reading);
        QLineEdit *ring_edit = ui->dbm_page->findChild<QLineEdit *>(ring);
        double read_it = reading_num_edit->text().toDouble();

        double ring_it = ring_edit->text().toDouble();

        double result = read_it * ring_it;

        level_4[load_key] = result;
    }

    for (int j = 1; j <= 3; j++)
    {
        int k = 4;
            QString correct_load = QString("marshall_corrected_load_4_%1").arg(j);
            QString load = QString("marshall_load_4_%1").arg(j);
            QString volume_crc = QString("marshall_4_%1%2").arg(j).arg(k + 1);

            QLineEdit *vol_crc_edit = ui->dbm_page->findChild<QLineEdit *>(volume_crc);

            if (vol_crc_edit && level_4.contains(load))
            {
                double vol_crc_val = vol_crc_edit->text().toDouble();

                double load_it = level_4[load].toDouble();

                double result = vol_crc_val * load_it;
                sum_stb += result;

                level_4[correct_load] = result;
            }
            else
            {
                qDebug() << "Error: LineEdit not found for j=" << j << " and k=" << k;
            }
    }
    sum_stb /= 3;
    level_4["avg_stability"] = sum_stb;
    sum_stb = 0;

    // level_5
    for (int j = 1; j <= 3; j++)
    {
        int k = 1;
            QString vol_obj_name = QString("marshall_vol_5_%1").arg(j);
            QString num_value = QString("marshall_5_%1%2").arg(j).arg(k);
            QString den_value = QString("marshall_5_%1%2").arg(j).arg(k + 2);

            QLineEdit *num_edit = ui->dbm_page->findChild<QLineEdit *>(num_value);
            QLineEdit *den_edit = ui->dbm_page->findChild<QLineEdit *>(den_value);

            double numerator = num_edit->text().toDouble();
            double denominator = den_edit->text().toDouble();

            if (denominator != 0)
            {
                double result = numerator / denominator;
                level_5[vol_obj_name] = result;
            }
    }

    for (int j = 1; j <= 3; j++)
    {
        int k = 2;
            QString wt_water_key = QString("marshall_wt_water_5_%1").arg(j);
            QString wt_water_num_value = QString("marshall_5_%1%2").arg(j).arg(k);
            QString wt_water_vol_value = QString("marshall_vol_5_%1").arg(j);

            QLineEdit *wt_water_num_edit = ui->dbm_page->findChild<QLineEdit *>(wt_water_num_value);

            if (wt_water_num_edit && level_5.contains(wt_water_vol_value))
            {
                double wt_water_numerator = wt_water_num_edit->text().toDouble();

                double wt_water_volume = level_5[wt_water_vol_value].toDouble();

                double wt_water = wt_water_numerator - wt_water_volume;

                level_5[wt_water_key] = wt_water;
            }
            else
            {
                qDebug() << "Error: LineEdit not found for j=" << j << " and k=" << k;
            }
    }

    for (int j = 1; j <= 3; j++)
    {
        int k = 4;
        QString load_key = QString("marshall_load_5_%1").arg(j);
        QString reading = QString("marshall_5_%1%2").arg(j).arg(k);
        QString ring = ("marshall_ring");
        QLineEdit *reading_num_edit = ui->dbm_page->findChild<QLineEdit *>(reading);
        QLineEdit *ring_edit = ui->dbm_page->findChild<QLineEdit *>(ring);
        double read_it = reading_num_edit->text().toDouble();

        double ring_it = ring_edit->text().toDouble();

        double result = read_it * ring_it;

        level_5[load_key] = result;
    }

    for (int j = 1; j <= 3; j++)
    {
        int k = 4;
            QString correct_load = QString("marshall_corrected_load_5_%1").arg(j);
            QString load = QString("marshall_load_5_%1").arg(j);
            QString volume_crc = QString("marshall_5_%1%2").arg(j).arg(k + 1);

            QLineEdit *vol_crc_edit = ui->dbm_page->findChild<QLineEdit *>(volume_crc);

            if (vol_crc_edit && level_5.contains(load))
            {
                double vol_crc_val = vol_crc_edit->text().toDouble();

                double load_it = level_5[load].toDouble();

                double result = vol_crc_val * load_it;
                sum_stb += result;

                level_5[correct_load] = result;
            }
            else
            {
                qDebug() << "Error: LineEdit not found for j=" << j << " and k=" << k;
            }
    }
    sum_stb /= 3;
    level_5["avg_stability"] = sum_stb;
    sum_stb = 0;

    QFile marshall_json_file(cwd.filePath("json/marshall.json"));
    marshall_json["level_1"] = level_1;
    marshall_json["level_2"] = level_2;
    marshall_json["level_3"] = level_3;
    marshall_json["level_4"] = level_4;
    marshall_json["level_5"] = level_5;
    std::vector<QJsonObject> levels = {level_1, level_2, level_3, level_4, level_5};

    if (marshall_json_file.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&marshall_json_file);
        QJsonDocument jsonDoc_1(marshall_json);

        out << jsonDoc_1.toJson();

        // Close the file
        marshall_json_file.close();
    }
    else
    {
        qDebug() << "SUYGETSU AIMS THE RESET on tensile_save ABSOLUTELY INCREDIBLE";
    }

    updateGraph_dbm();
    save_check();
}
void MainWindow::on_vol_save_clicked()
{

    tracked_files.push_back("vol");
    removeDuplicates(tracked_files);

    QJsonObject specific_gravity;
    QJsonObject composition;

    QStringList spg_expnames = {"apparent", "bulk", "total", "btmn"};

    for (int i = 1; i <= 3; i++)
    {
        for (QString s : spg_expnames)
        {

            QString constructor = "vol_" + s + "_%1";
            QString obj_name = constructor.arg(i);

            QLineEdit *tedit = ui->vol->findChild<QLineEdit *>(obj_name);

            if (tedit)
            {
                specific_gravity[obj_name] = tedit->text().toDouble();
            }
            else
            {
                qDebug() << "Error on saving 'vol' at" << obj_name << "constructor" << constructor << "tedit not found";
            }
        }
    }

    specific_gravity["vol_eff_1"] = (ui->vol_apparent_1->text().toDouble() + ui->vol_bulk_1->text().toDouble()) / 2;
    specific_gravity["vol_eff_2"] = (ui->vol_apparent_2->text().toDouble() + ui->vol_bulk_2->text().toDouble()) / 2;
    specific_gravity["vol_eff_3"] = (ui->vol_apparent_3->text().toDouble() + ui->vol_bulk_3->text().toDouble()) / 2;

    QStringList comp_expnames = {"p1", "p2", "p3", "pb", "gsb", "gmm", "gmb"};

    for (int i = 0; i <= 5; i++)
    {
        for (QString s : comp_expnames)
        {

            QString constructor = "vol_" + s + "_%1";
            QString arg = (i == 0) ? "obc" : QString::number(i);

            if (s == "pb") {
                qDebug() << "here";
            }

            QString obj_name = constructor.arg(arg);

            QLineEdit *tedit = ui->vol->findChild<QLineEdit *>(obj_name);

            if (tedit)
            {
                composition[obj_name] = tedit->text().toDouble();
            }
            else
            {
                qDebug() << "Error on saving 'vol' at" << obj_name << "constructor" << constructor << "teditnotfound";
            }
        }
    }

    QStringList calc_expnames = {"ps", "vma", "va", "vfb"};
    for (QString s : calc_expnames)
    {
        for (int i = 0; i <= 5; i++)
        {

            QString arg = (i == 0) ? "obc" : QString::number(i);
            QString constructor = "vol_" + s + "_%1";
            QString obj_name = constructor.arg(arg);

            double output;
            // Switch statement handles every experiment differently
            if (s == "ps")
            {
                QString db_constructor = "vol_pb_%1";
                QString db_call = db_constructor.arg(arg);

                double db_val = composition[db_call].toDouble();

                output = 100 - db_val;
                QLabel* label = ui->vol->findChild<QLabel*>(obj_name);
                if (label) {
                    label->setText(QString::number(output));
                }
            }
            else if (s == "vma")
            {
                QString db_constructor_1 = "vol_gmb_%1", db_constructor_2 = "vol_ps_%1", db_constructor_3 = "vol_gsb_%1";
                QString db_call_1 = db_constructor_1.arg(arg), db_call_2 = db_constructor_2.arg(arg), db_call_3 = db_constructor_3.arg(arg);

                double val_gmb = composition[db_call_1].toDouble(), val_ps = composition[db_call_2].toDouble(), val_gsb = composition[db_call_3].toDouble();

                output = 100 - (val_gmb * val_ps / val_gsb);
            }
            else if (s == "va")
            {
                QString db_constructor_1 = "vol_gmm_%1", db_constructor_2 = "vol_gmb_%1";
                QString db_call_1 = db_constructor_1.arg(arg), db_call_2 = db_constructor_2.arg(arg);

                double val_gmm = composition[db_call_1].toDouble(), val_gmb = composition[db_call_2].toDouble();

                output = (val_gmm - val_gmb) * 100 / val_gmm;
            }
            else if (s == "vfb")
            {
                QString db_constructor_1 = "vol_vma_%1", db_constructor_2 = "vol_va_%1";
                QString db_call_1 = db_constructor_1.arg(arg), db_call_2 = db_constructor_2.arg(arg);

                double val_vma = composition[db_call_1].toDouble(), val_va = composition[db_call_2].toDouble();

                output = (val_vma - val_va) * 100 / val_vma;
            }

            composition[obj_name] = output;
        }
    }

    QJsonObject vol_json;
    vol_json["spg"] = specific_gravity;
    vol_json["composition"] = composition;

    QFile vol_json_file(cwd.filePath("json/vol.json"));

    if (vol_json_file.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&vol_json_file);
        QJsonDocument jsonDoc_1(vol_json);

        out << jsonDoc_1.toJson();

        // Close the file
        vol_json_file.close();
    }
    else
    {
        qDebug() << "SUYGETSU AIMS THE RESET on vol_save ABSOLUTELY INCREDIBLE";
    }

    updateGraph_dbm();
    save_check();
}
void MainWindow::on_gmm_save_clicked()
{
    tracked_files.push_back("gmm");
    removeDuplicates(tracked_files);

    QJsonObject gmm_data, gmm_400, gmm_425, gmm_450;

    QString constructor = "gmm%1_%2%3";
    QString json_constructor = "gmm_%1_%2";

    for (int i = 1; i <= 3; i++)
    {
        for (int j = 1; j <= 5; j++)
        {
            for (int k = 1; k <= 2; k++)
            {

                QString obj_name = constructor.arg(i).arg(j).arg(k);
                QString json_key = json_constructor.arg(j).arg(k);

                QLineEdit *tedit = ui->gmm->findChild<QLineEdit *>(obj_name);

                if (tedit)
                {

                    if (i == 1)
                    {
                        gmm_400[json_key] = tedit->text().toDouble();
                    }
                    else if (i == 2)
                    {
                        gmm_425[json_key] = tedit->text().toDouble();
                    }
                    else if (i == 3)
                    {
                        gmm_450[json_key] = tedit->text().toDouble();
                    }
                }
                else
                {
                    qDebug() << "Error on saving 'gmm' at" << obj_name << "constructor:" << constructor << "tedit not found";
                }
            }
        }
    }

    gmm_400["avg"] = (gmm_400["gmm_5_1"].toDouble() + gmm_400["gmm_5_2"].toDouble()) / 2;
    gmm_425["avg"] = (gmm_425["gmm_5_1"].toDouble() + gmm_425["gmm_5_2"].toDouble()) / 2;
    gmm_450["avg"] = (gmm_450["gmm_5_1"].toDouble() + gmm_450["gmm_5_2"].toDouble()) / 2;

    gmm_data["4.00"] = gmm_400;
    gmm_data["4.25"] = gmm_425;
    gmm_data["4.50"] = gmm_450;

    QFile gmm_json_file(cwd.filePath("json/gmm.json"));

    if (gmm_json_file.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&gmm_json_file);
        QJsonDocument jsonDoc_1(gmm_data);

        out << jsonDoc_1.toJson();

        // Close the file
        gmm_json_file.close();
    }
    else
    {
        qDebug() << "SUYGETSU AIMS THE RESET on vol_save ABSOLUTELY INCREDIBLE";
    }

    save_check();
}
void MainWindow::on_rheology_save_clicked()
{
    tracked_files.push_back("rheology");
    removeDuplicates(tracked_files);

    QJsonObject rh_json, strip, soft, pen, ductility, flash, viscosity, spc;
    const QString constructor = "rheology_%1_%2";
    const std::vector<QString> exp_names = {"soft", "strip", "pen", "ductility", "flash", "viscosity", "spc"};
    const std::map<QString, int> exp_valcount = {{"strip", 6}, // Internal iteration from 1 to 2
                                                 {"soft", 7},
                                                 {"pen", 11},
                                                 {"ductility", 9},
                                                 {"flash", 3},
                                                 {"viscosity", 10},
                                                 {"spc", 4}}; // Internal iteration from 1 to 3

    std::map<QString, QJsonObject (MainWindow::*)(QJsonObject)> func_map = {{"strip", strip_eval},
                                                                            {"soft", soft_eval},
                                                                            {"pen", pen_eval},
                                                                            {"ductility", ductility_eval},
                                                                            {"flash", flash_eval},
                                                                            {"viscosity", viscosity_eval},
                                                                            {"spc", spc_eval}};

    std::map<QString, QJsonObject> json_map = {{"strip", strip},
                                               {"soft", soft},
                                               {"pen", pen},
                                               {"ductility", ductility},
                                               {"flash", flash},
                                               {"viscosity", viscosity},
                                               {"spc", spc}};

    for (auto it : exp_names)
    {
        QString exp_name = it;
        int exp_count = exp_valcount.at(it);
        for (int i = 1; i <= exp_count; ++i)
        {

            if (exp_name == "strip")
            {
                QString obj_name_1 = constructor.arg(exp_name).arg(i * 10 + 1);
                QString obj_name_2 = constructor.arg(exp_name).arg(i * 10 + 2);

                QLineEdit *tedit_1 = ui->rheology->findChild<QLineEdit *>(obj_name_1);
                QLineEdit *tedit_2 = ui->rheology->findChild<QLineEdit *>(obj_name_2);

                if (!(tedit_1 && tedit_2))
                {
                    qDebug() << "somethinglikeyou11 is wrong with rheology strip";
                    continue;
                }

                bool ok1, ok2;
                QString text_1 = tedit_1->text(), text_2 = tedit_2->text();
                double num1 = text_1.toDouble(&ok1), num2 = text_2.toDouble(&ok2);

                if (ok1 && ok2) {
                    json_map[exp_name][obj_name_1] = num1;
                    json_map[exp_name][obj_name_2] = num2;
                } else {
                    json_map[exp_name][obj_name_1] = text_1;
                    json_map[exp_name][obj_name_2] = text_2;
                }
            }
            else if (exp_name == "spc")
            {
                QString obj_name_1 = constructor.arg(exp_name).arg("%2_%3").arg(i).arg(1);
                QString obj_name_2 = constructor.arg(exp_name).arg("%2_%3").arg(i).arg(2);
                QString obj_name_3 = constructor.arg(exp_name).arg("%2_%3").arg(i).arg(3);

                QLineEdit *tedit_1 = ui->rheology->findChild<QLineEdit *>(obj_name_1);
                QLineEdit *tedit_2 = ui->rheology->findChild<QLineEdit *>(obj_name_2);
                QLineEdit *tedit_3 = ui->rheology->findChild<QLineEdit *>(obj_name_3);

                if (!(tedit_1 && tedit_2))
                {
                    qDebug() << "somethinglikeyou11 is wrong with rheology strip";
                    continue;
                }

                bool ok1, ok2, ok3;

                QString text_1 = tedit_1->text();
                QString text_2 = tedit_2->text();
                QString text_3 = tedit_3->text();

                double num1 = text_1.toDouble(&ok1);
                double num2 = text_2.toDouble(&ok2);
                double num3 = text_3.toDouble(&ok3);

                if (ok1) {json_map[exp_name][obj_name_1] = num1;}
                else {json_map[exp_name][obj_name_1] = text_1;}

                if (ok2) {json_map[exp_name][obj_name_2] = num2;}
                else {json_map[exp_name][obj_name_2] = text_2;}

                if (ok3) {json_map[exp_name][obj_name_3] = num3;}
                else {json_map[exp_name][obj_name_3] = text_3;}

            }
            else
            {
                QString obj_name = constructor.arg(exp_name).arg(i);
                QString text;

                QObject *obj = ui->rheology->findChild<QObject *>(obj_name);
                if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(obj))
                {
                    text = lineEdit->text();
                }
                else if (QDoubleSpinBox *dspinEdit = qobject_cast<QDoubleSpinBox *>(obj))
                {
                    text = dspinEdit->text();
                }

                else
                {
                    qDebug() << "something broke with rheology save" << exp_name;
                    continue;
                }

                bool ok;
                double text_num = text.toDouble(&ok);

                if (ok) {
                    json_map[exp_name][obj_name] = text_num;
                } else {
                    json_map[exp_name][obj_name] = text;
                }
            }
        }

        json_map[exp_name] = (this->*func_map.at(exp_name))(json_map[exp_name]);
        rh_json[exp_name] = json_map.at(exp_name);
    }

    QFile rheology_json_file(cwd.filePath("json/rheology.json"));

    if (rheology_json_file.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&rheology_json_file);
        QJsonDocument jsonDoc_1(rh_json);

        out << jsonDoc_1.toJson();

        // Close the file
        rheology_json_file.close();
    }
    else
    {
        qDebug() << "SUYGETSU AIMS THE RESET on vol_save ABSOLUTELY INCREDIBLE";
    }

    save_check();
}
void MainWindow::on_wa_save_clicked()
{
    tracked_files.push_back("wa");
    removeDuplicates(tracked_files);

    QJsonObject wa_data, wa_10, wa_20, wa_dust, wa_cleanliness;

    QString constructor = "wa%1_%2%3";
    QString json_constructor = "wa_%1_%2";

    for (int i = 1; i <= 4; i++)
    {
        for (int j = 1; j <= 4; j++)
        {
            for (int k = 1; k <= 2; k++)
            {

                QString obj_name = constructor.arg(i).arg(j).arg(k);
                QString json_key = json_constructor.arg(j).arg(k);

                QLineEdit *tedit = ui->wa->findChild<QLineEdit *>(obj_name);

                if (tedit)
                {

                    if (i == 1)
                    {
                        wa_20[json_key] = tedit->text().toDouble();
                    }
                    else if (i == 2)
                    {
                        wa_10[json_key] = tedit->text().toDouble();
                    }
                    else if (i == 3)
                    {
                        wa_dust[json_key] = tedit->text().toDouble();
                    }
                }
                else
                {
                    qDebug() << "Error on saving 'WA' at" << obj_name << "constructor:" << constructor << "tedit not found";
                }
            }
        }
        if (i == 4)
        {
            for (int j = 1; j < 4; j++)
            {
                for (int k = 1; k < 4; k++)
                {
                    QString obj_name = constructor.arg(i).arg(j).arg(k);
                    QString json_key = json_constructor.arg(j).arg(k);
                    QLineEdit *tedit = ui->wa->findChild<QLineEdit *>(obj_name);
                    if (tedit)
                    {
                        if (i == 4)
                        {
                            wa_cleanliness[json_key] = tedit->text().toDouble();
                        }
                    }
                    else
                    {
                        qDebug() << "Error on saving 'WA_cleanliness' at" << obj_name << "constructor:" << constructor << "tedit not found";
                    }
                }
            }
        }
    }

    QJsonObject waa_10, waa_20, waa_dust, waa_cleanliness;

    for (int k = 1; k <= 2; k++)
    {
        std::string k_str = std::to_string(k);
        QString k_qstr = QString::fromStdString(k_str);

        waa_10["answer_1_" + k_qstr] = wa_10["wa_4_" + k_qstr].toDouble() / (wa_10["wa_3_" + k_qstr].toDouble() + wa_10["wa_2_" + k_qstr].toDouble() - wa_10["wa_1_" + k_qstr].toDouble());
        waa_10["answer_2_" + k_qstr] = wa_10["wa_4_" + k_qstr].toDouble() / (wa_10["wa_4_" + k_qstr].toDouble() + wa_10["wa_2_" + k_qstr].toDouble() - wa_10["wa_1_" + k_qstr].toDouble());
        waa_10["answer_3_" + k_qstr] = ((wa_10["wa_3_" + k_qstr].toDouble() - wa_10["wa_4_" + k_qstr].toDouble()) / wa_10["wa_4_" + k_qstr].toDouble()) * 100;

        waa_20["answer_1_" + k_qstr] = wa_20["wa_4_" + k_qstr].toDouble() / (wa_20["wa_3_" + k_qstr].toDouble() + wa_20["wa_2_" + k_qstr].toDouble() - wa_20["wa_1_" + k_qstr].toDouble());
        waa_20["answer_2_" + k_qstr] = wa_20["wa_4_" + k_qstr].toDouble() / (wa_20["wa_4_" + k_qstr].toDouble() + wa_20["wa_2_" + k_qstr].toDouble() - wa_20["wa_1_" + k_qstr].toDouble());
        waa_20["answer_3_" + k_qstr] = ((wa_20["wa_3_" + k_qstr].toDouble() - wa_20["wa_4_" + k_qstr].toDouble()) / wa_20["wa_4_" + k_qstr].toDouble()) * 100;

        waa_dust["answer_1_" + k_qstr] = wa_dust["wa_1_" + k_qstr].toDouble() / (wa_dust["wa_2_" + k_qstr].toDouble() + wa_dust["wa_3_" + k_qstr].toDouble() - wa_dust["wa_4_" + k_qstr].toDouble());
        waa_dust["answer_2_" + k_qstr] = wa_dust["wa_3_" + k_qstr].toDouble() / (wa_dust["wa_2_" + k_qstr].toDouble() + wa_dust["wa_3_" + k_qstr].toDouble() - wa_dust["wa_4_" + k_qstr].toDouble());
        waa_dust["answer_3_" + k_qstr] = wa_dust["wa_1_" + k_qstr].toDouble() / (wa_dust["wa_1_" + k_qstr].toDouble() + wa_dust["wa_2_" + k_qstr].toDouble() - wa_dust["wa_4_" + k_qstr].toDouble());
        waa_dust["answer_4_" + k_qstr] = ((wa_dust["wa_3_" + k_qstr].toDouble() - wa_dust["wa_1_" + k_qstr].toDouble()) / wa_dust["wa_1_" + k_qstr].toDouble()) * 100;
    }

    for (int k = 1; k <= 3; k++)
    {
        std::string k_str = std::to_string(k);
        QString k_qstr = QString::fromStdString(k_str);
        waa_cleanliness["aggregate%" + k_qstr] = (wa_cleanliness["wa_3_" + k_qstr].toDouble() / wa_cleanliness["wa_1_" + k_qstr].toDouble()) * 100;
    }

    for (int i = 1; i < 4; i++)
    {
        std::string i_str = std::to_string(i);
        QString i_qstr = QString::fromStdString(i_str);

        wa_10["wa10_average_" + i_qstr] = (waa_10["answer_" + i_qstr + "_1"].toDouble() + waa_10["answer_" + i_qstr + "_2"].toDouble()) / 2;
    }
    for (int i = 1; i < 4; i++)
    {
        std::string i_str = std::to_string(i);
        QString i_qstr = QString::fromStdString(i_str);

        wa_20["wa20_average_" + i_qstr] = (waa_20["answer_" + i_qstr + "_1"].toDouble() + waa_20["answer_" + i_qstr + "_2"].toDouble()) / 2;
    }

    wa_data["10"] = wa_10;
    wa_data["20"] = wa_20;
    wa_data["dust"] = wa_dust;
    wa_data["cleanliness"] = wa_cleanliness;
    wa_data["answer_10"] = waa_10;
    wa_data["answer_20"] = waa_20;
    wa_data["answer_dust"] = waa_dust;
    wa_data["answer_cleanliness"] = waa_cleanliness;

    QFile wa_json(cwd.filePath("json/wa.json"));

    if (wa_json.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&wa_json);
        QJsonDocument jsonDoc_1(wa_data);

        out << jsonDoc_1.toJson();

        // Close the file
        wa_json.close();
    }
    else
    {
        qDebug() << "SUYGETSU AIMS THE RESET on vol_save ABSOLUTELY INCREDIBLE";
    }

    save_check();
}



// Deals with save as requests
void MainWindow::on_spc_saveas_clicked()
{
    swd = QDir(QFileDialog::getExistingDirectory(this, tr("Open Directory"), cwd.currentPath(), QFileDialog::ShowDirsOnly));
    if (!saveas_done)
    {
        QMessageBox::information(this, "Outputs Folder", tr("When you click EXPORT in the 'File' tab the output will be stored on that particular directory"));
        saveas_done = true;
    }
    ui->spc_save->click();
}
void MainWindow::on_fei_saveas_clicked()
{
    swd = QDir(QFileDialog::getExistingDirectory(this, tr("Open Directory"), cwd.currentPath(), QFileDialog::ShowDirsOnly));
    if (!saveas_done)
    {
        QMessageBox::information(this, "Outputs Folder", tr("When you click EXPORT in the 'File' tab the output will be stored on that particular directory"));
        saveas_done = true;
    }
    ui->fei_save->click();
}
void MainWindow::on_aiv_saveas_clicked()
{
    swd = QDir(QFileDialog::getExistingDirectory(this, tr("Open Directory"), cwd.currentPath(), QFileDialog::ShowDirsOnly));
    if (!saveas_done)
    {
        QMessageBox::information(this, "Outputs Folder", tr("When you click EXPORT in the 'File' tab the output will be stored on that particular directory"));
        saveas_done = true;
    }
    ui->aiv_save->click();
}
void MainWindow::on_ind_saveas_clicked()
{
    swd = QDir(QFileDialog::getExistingDirectory(this, tr("Open Directory"), cwd.currentPath(), QFileDialog::ShowDirsOnly));
    if (!saveas_done)
    {
        QMessageBox::information(this, "Outputs Folder", tr("When you click EXPORT in the 'File' tab the output will be stored on that particular directory"));
        saveas_done = true;
    }
    ui->ind_save->click();
}
void MainWindow::on_mdd_saveas_clicked()
{
    swd = QDir(QFileDialog::getExistingDirectory(this, tr("Open Directory"), cwd.currentPath(), QFileDialog::ShowDirsOnly));
    if (!saveas_done)
    {
        QMessageBox::information(this, "Outputs Folder", tr("When you click EXPORT in the 'File' tab the output will be stored on that particular directory"));
        saveas_done = true;
    }
    ui->mdd_save->click();
}
void MainWindow::on_grad_saveas_clicked()
{
    swd = QDir(QFileDialog::getExistingDirectory(this, tr("Open Directory"), cwd.currentPath(), QFileDialog::ShowDirsOnly));
    if (!saveas_done)
    {
        QMessageBox::information(this, "Outputs Folder", tr("When you click EXPORT in the 'File' tab the output will be stored on that particular directory"));
        saveas_done = true;
    }
    ui->mdd_save->click();
}
void MainWindow::on_marshall_saveas_clicked()
{
    swd = QDir(QFileDialog::getExistingDirectory(this, tr("Open Directory"), cwd.currentPath(), QFileDialog::ShowDirsOnly));
    if (!saveas_done)
    {
        QMessageBox::information(this, "Outputs Folder", tr("When you click EXPORT in the 'File' tab the output will be stored on that particular directory"));
        saveas_done = true;
    }
    ui->marshall_save->click();
}
void MainWindow::on_tensile_saveas_clicked()
{
    swd = QDir(QFileDialog::getExistingDirectory(this, tr("Open Directory"), cwd.currentPath(), QFileDialog::ShowDirsOnly));
    if (!saveas_done)
    {
        QMessageBox::information(this, "Outputs Folder", tr("When you click EXPORT in the 'File' tab the output will be stored on that particular directory"));
        saveas_done = true;
    }
    ui->tensile_save->click();
}
void MainWindow::on_vol_saveas_clicked() {
    swd = QDir(QFileDialog::getExistingDirectory(this, tr("Open Directory"), cwd.currentPath(), QFileDialog::ShowDirsOnly));
    if (!saveas_done)
    {
        QMessageBox::information(this, "Outputs Folder", tr("When you click EXPORT in the 'File' tab the output will be stored on that particular directory"));
        saveas_done = true;
    }
    ui->vol_save->click();
}
void MainWindow::on_gmm_saveas_clicked() {
    swd = QDir(QFileDialog::getExistingDirectory(this, tr("Open Directory"), cwd.currentPath(), QFileDialog::ShowDirsOnly));
    if (!saveas_done)
    {
        QMessageBox::information(this, "Outputs Folder", tr("When you click EXPORT in the 'File' tab the output will be stored on that particular directory"));
        saveas_done = true;
    }
    ui->gmm_save->click();
}
void MainWindow::on_rheology_saveas_clicked() {
    swd = QDir(QFileDialog::getExistingDirectory(this, tr("Open Directory"), cwd.currentPath(), QFileDialog::ShowDirsOnly));
    if (!saveas_done)
    {
        QMessageBox::information(this, "Outputs Folder", tr("When you click EXPORT in the 'File' tab the output will be stored on that particular directory"));
        saveas_done = true;
    }
    ui->rheology_save->click();
}
void MainWindow::on_wa_saveas_clicked() {
    swd = QDir(QFileDialog::getExistingDirectory(this, tr("Open Directory"), cwd.currentPath(), QFileDialog::ShowDirsOnly));
    if (!saveas_done)
    {
        QMessageBox::information(this, "Outputs Folder", tr("When you click EXPORT in the 'File' tab the output will be stored on that particular directory"));
        saveas_done = true;
    }
    ui->wa_save->click();
}



/**
 * @brief Checks the save status of all experiments and generates the corresponding HTML files.
 *
 * This function iterates over all experiments and checks if they are present in the tracked_files.
 * If an experiment is tracked, it sets the corresponding indicator to the saved icon.
 * If not, it sets the indicator to the unsaved icon.
 * After checking the save status, it generates the corresponding HTML file for each tracked file.
 *
 * This function must be called at the end of the 'save' function of any experiment, to first verify the save state of the experiment and to
 * automatically generate the HTML files regarding the experiment
 *
 * @note This function does not take any parameters or return any values.
 */
void MainWindow::save_check()
{
    // Iterate over all experiments
    for (auto exp : all_experiments)
    {
        // Find the experiment in the tracked files
        if (std::find(tracked_files.begin(), tracked_files.end(), exp) != tracked_files.end())
        {
            // Get the save indicator for the experiment
            QLabel *indicator = ui->stackedWidget->findChild<QLabel *>(QString::fromStdString(exp) + "_saved");
            if (indicator)
            {
                // Set the indicator to the saved icon
                indicator->setPixmap(QPixmap(":/saved_icons/icons/tab_saved_icon.png"));
                if (indicator->height() < 200)
                {
                    // Use a smaller icon for small indicators
                    indicator->setPixmap(QPixmap(":/saved_icons/icons/tab_saved_icon_small.png"));
                }
            }
        }
        else
        {
            // Get the save indicator for the experiment
            QLabel *indicator = ui->stackedWidget->findChild<QLabel *>(QString::fromStdString(exp) + "_saved");
            if (indicator)
            {
                // Set the indicator to the unsaved icon
                indicator->setPixmap(QPixmap(":/saved_icons/icons/tab_unsaved_icon.png"));
                if (indicator->height() < 200)
                {
                    // Use a smaller icon for small indicators
                    indicator->setPixmap(QPixmap(":/saved_icons/icons/tab_unsaved_icon_small.png"));
                }
            }
        }
    }

    // Generate the corresponding HTML file for each tracked file
    for (auto file : tracked_files)
    {
        if (file == "spc")
        {
            generate_html_spc();
        }
        else if (file == "fei")
        {
            generate_html_fei();
        }
        else if (file == "aiv")
        {
            generate_html_aiv();
        }
        else if (file == "ind")
        {
            generate_html_ind();
        }
        else if (file == "mdd")
        {
            generate_html_mdd();
        }
        else if (file == "grad")
        {
            generate_html_grad();
        }
        else if (file == "tensile")
        {
            generate_html_tensile();
        }
        else if (file == "vol")
        {
            generate_html_vol();
        }
        else if (file == "gmm")
        {
            generate_html_gmm();
        }
        else if (file == "rheology")
        {
            generate_html_rheology();
        }
        else if (file == "marshall")
        {
            generate_html_marshall();
        }
        else if (file == "wa")
        {
            generate_html_wa();
        }
    }
}



// Deals with saving to JSON (subfunctions)
void MainWindow::on_mdd_save_update_clicked()
{
    ui->mdd_save->click();
    MainWindow::updateGraph_mdd();
}
QJsonObject Specific_Gravity;
QFile spe_gravity(cwd.filePath("json/spc.json"));
void MainWindow::on_spc_save_40mm_clicked()
{
    tracked_files.push_back("spc");
    removeDuplicates(tracked_files);

    type_of_material _40mm;

    // achha lagega:)
    _40mm.Weight_of_sample_of_water[4][0] = 0;
    _40mm.Weight_of_SSD_Sample[4][0] = 0;
    _40mm.Weight_of_Oven_dry_sample[4][0] = 0;

    _40mm.Weight_of_sample_of_water[4][1] = ui->spc_40_w1_t1->text().toFloat();
    _40mm.Weight_of_sample_of_water[4][2] = ui->spc_40_w1_t2->text().toFloat();
    _40mm.Weight_of_sample_of_water[4][3] = ui->spc_40_w1_t3->text().toFloat();
    _40mm.Weight_of_SSD_Sample[4][1] = ui->spc_40_w2_t1->text().toFloat();
    _40mm.Weight_of_SSD_Sample[4][2] = ui->spc_40_w2_t2->text().toFloat();
    _40mm.Weight_of_SSD_Sample[4][3] = ui->spc_40_w2_t3->text().toFloat();
    _40mm.Weight_of_Oven_dry_sample[4][1] = ui->spc_40_w3_t1->text().toFloat();
    _40mm.Weight_of_Oven_dry_sample[4][2] = ui->spc_40_w3_t2->text().toFloat();
    _40mm.Weight_of_Oven_dry_sample[4][3] = ui->spc_40_w3_t3->text().toFloat();
    float Bulk_specific_gravity_40[4];
    float apparent_specific_gravity_40[4];
    float water_absorption_40[4];
    float average_bulk_specific_gravity_40;
    float average_apparent_specific_gravity_40;
    float average_water_absorption_40;

    Bulk_specific_gravity_40[1] = _40mm.Weight_of_Oven_dry_sample[4][1] / (_40mm.Weight_of_SSD_Sample[4][1] - _40mm.Weight_of_sample_of_water[4][1]);
    Bulk_specific_gravity_40[2] = _40mm.Weight_of_Oven_dry_sample[4][2] / (_40mm.Weight_of_SSD_Sample[4][2] - _40mm.Weight_of_sample_of_water[4][2]);
    Bulk_specific_gravity_40[3] = _40mm.Weight_of_Oven_dry_sample[4][3] / (_40mm.Weight_of_SSD_Sample[4][3] - _40mm.Weight_of_sample_of_water[4][3]);
    apparent_specific_gravity_40[1] = _40mm.Weight_of_Oven_dry_sample[4][1] / (_40mm.Weight_of_Oven_dry_sample[4][1] - _40mm.Weight_of_sample_of_water[4][1]);
    apparent_specific_gravity_40[2] = _40mm.Weight_of_Oven_dry_sample[4][2] / (_40mm.Weight_of_Oven_dry_sample[4][2] - _40mm.Weight_of_sample_of_water[4][2]);
    apparent_specific_gravity_40[3] = _40mm.Weight_of_Oven_dry_sample[4][3] / (_40mm.Weight_of_Oven_dry_sample[4][3] - _40mm.Weight_of_sample_of_water[4][3]);
    water_absorption_40[1] = 100 * (_40mm.Weight_of_SSD_Sample[4][1] - _40mm.Weight_of_Oven_dry_sample[4][1]) / _40mm.Weight_of_Oven_dry_sample[4][1];
    water_absorption_40[2] = 100 * (_40mm.Weight_of_SSD_Sample[4][2] - _40mm.Weight_of_Oven_dry_sample[4][2]) / _40mm.Weight_of_Oven_dry_sample[4][2];
    water_absorption_40[3] = 100 * (_40mm.Weight_of_SSD_Sample[4][3] - _40mm.Weight_of_Oven_dry_sample[4][3]) / _40mm.Weight_of_Oven_dry_sample[4][3];
    average_bulk_specific_gravity_40 = (Bulk_specific_gravity_40[1] + Bulk_specific_gravity_40[2] + Bulk_specific_gravity_40[3]) / 3;
    average_apparent_specific_gravity_40 = (apparent_specific_gravity_40[1] + apparent_specific_gravity_40[2] + apparent_specific_gravity_40[3]) / 3;
    average_water_absorption_40 = (water_absorption_40[1] + water_absorption_40[2] + water_absorption_40[3]) / 3;

    QJsonObject _40_mm;
    _40_mm["Bulk_specific_gravity_1"] = Bulk_specific_gravity_40[1];
    _40_mm["Bulk_specific_gravity_2"] = Bulk_specific_gravity_40[2];
    _40_mm["Bulk_specific_gravity_3"] = Bulk_specific_gravity_40[3];
    _40_mm["apparent_specific_gravity_1"] = apparent_specific_gravity_40[1];
    _40_mm["apparent_specific_gravity_2"] = apparent_specific_gravity_40[2];
    _40_mm["apparent_specific_gravity_3"] = apparent_specific_gravity_40[3];
    _40_mm["water_absorption_1"] = water_absorption_40[1];
    _40_mm["water_absorption_2"] = water_absorption_40[2];
    _40_mm["water_absorption_3"] = water_absorption_40[3];
    _40_mm["average_bulk_specific_gravity"] = average_bulk_specific_gravity_40;
    _40_mm["average_apparent_specific_gravity"] = average_apparent_specific_gravity_40;
    _40_mm["average_water_absorption"] = average_water_absorption_40;

    // achha lg rha hai??:) i am noobda
    _40_mm.insert("Weight_of_sample_of_water_1", _40mm.Weight_of_sample_of_water[4][1]);
    _40_mm["Weight_of_sample_of_water_2"] = _40mm.Weight_of_sample_of_water[4][2];
    _40_mm["Weight_of_sample_of_water_3"] = _40mm.Weight_of_sample_of_water[4][3];
    _40_mm["Weight_of_SSD_Sample_1"] = _40mm.Weight_of_SSD_Sample[4][1];
    _40_mm["Weight_of_SSD_Sample_2"] = _40mm.Weight_of_SSD_Sample[4][2];
    _40_mm["Weight_of_SSD_Sample_3"] = _40mm.Weight_of_SSD_Sample[4][3];
    _40_mm["Weight_of_Oven_dry_sample_1"] = _40mm.Weight_of_Oven_dry_sample[4][1];
    _40_mm["Weight_of_Oven_dry_sample_2"] = _40mm.Weight_of_Oven_dry_sample[4][2];
    _40_mm["Weight_of_Oven_dry_sample_3"] = _40mm.Weight_of_Oven_dry_sample[4][3];

    Specific_Gravity["40mm"] = _40_mm;
    if (spe_gravity.open(QFile::WriteOnly | QFile::Text))
    {
        // json data ko file me likhta hua mai
        QTextStream out(&spe_gravity);
        QJsonDocument jsonDoc_1(Specific_Gravity);

        out << jsonDoc_1.toJson();

        // Close the file
        spe_gravity.close();
    }

    save_check();
}
void MainWindow::on_spc_save_20mm_clicked()
{
    tracked_files.push_back("spc");
    removeDuplicates(tracked_files);

    type_of_material _20mm;

    _20mm.Weight_of_sample_of_water[2][0] = 0;
    _20mm.Weight_of_SSD_Sample[2][0] = 0;
    _20mm.Weight_of_Oven_dry_sample[2][0] = 0;

    _20mm.Weight_of_sample_of_water[2][1] = ui->spc_20_w1_t1->text().toFloat();
    _20mm.Weight_of_sample_of_water[2][2] = ui->spc_20_w1_t2->text().toFloat();
    _20mm.Weight_of_sample_of_water[2][3] = ui->spc_20_w1_t3->text().toFloat();
    _20mm.Weight_of_SSD_Sample[2][1] = ui->spc_20_w2_t1->text().toFloat();
    _20mm.Weight_of_SSD_Sample[2][2] = ui->spc_20_w2_t2->text().toFloat();
    _20mm.Weight_of_SSD_Sample[2][3] = ui->spc_20_w2_t3->text().toFloat();
    _20mm.Weight_of_Oven_dry_sample[2][1] = ui->spc_20_w3_t1->text().toFloat();
    _20mm.Weight_of_Oven_dry_sample[2][2] = ui->spc_20_w3_t2->text().toFloat();
    _20mm.Weight_of_Oven_dry_sample[2][3] = ui->spc_20_w3_t3->text().toFloat();

    float Bulk_specific_gravity_20[4];
    float apparent_specific_gravity_20[4];
    float water_absorption_20[4];
    float average_bulk_specific_gravity_20;
    float average_apparent_specific_gravity_20;
    float average_water_absorption_20;

    Bulk_specific_gravity_20[1] = _20mm.Weight_of_Oven_dry_sample[2][1] / (_20mm.Weight_of_SSD_Sample[2][1] - _20mm.Weight_of_sample_of_water[2][1]);
    Bulk_specific_gravity_20[2] = _20mm.Weight_of_Oven_dry_sample[2][2] / (_20mm.Weight_of_SSD_Sample[2][2] - _20mm.Weight_of_sample_of_water[2][2]);
    Bulk_specific_gravity_20[3] = _20mm.Weight_of_Oven_dry_sample[2][3] / (_20mm.Weight_of_SSD_Sample[2][3] - _20mm.Weight_of_sample_of_water[2][3]);
    apparent_specific_gravity_20[1] = _20mm.Weight_of_Oven_dry_sample[2][1] / (_20mm.Weight_of_Oven_dry_sample[2][1] - _20mm.Weight_of_sample_of_water[2][1]);
    apparent_specific_gravity_20[2] = _20mm.Weight_of_Oven_dry_sample[2][2] / (_20mm.Weight_of_Oven_dry_sample[2][2] - _20mm.Weight_of_sample_of_water[2][2]);
    apparent_specific_gravity_20[3] = _20mm.Weight_of_Oven_dry_sample[2][3] / (_20mm.Weight_of_Oven_dry_sample[2][3] - _20mm.Weight_of_sample_of_water[2][3]);
    water_absorption_20[1] = 100 * (_20mm.Weight_of_SSD_Sample[2][1] - _20mm.Weight_of_Oven_dry_sample[2][1]) / _20mm.Weight_of_Oven_dry_sample[2][1];
    water_absorption_20[2] = 100 * (_20mm.Weight_of_SSD_Sample[2][2] - _20mm.Weight_of_Oven_dry_sample[2][2]) / _20mm.Weight_of_Oven_dry_sample[2][2];
    water_absorption_20[3] = 100 * (_20mm.Weight_of_SSD_Sample[2][3] - _20mm.Weight_of_Oven_dry_sample[2][3]) / _20mm.Weight_of_Oven_dry_sample[2][3];
    average_bulk_specific_gravity_20 = (Bulk_specific_gravity_20[1] + Bulk_specific_gravity_20[2] + Bulk_specific_gravity_20[3]) / 3;
    average_apparent_specific_gravity_20 = (apparent_specific_gravity_20[1] + apparent_specific_gravity_20[2] + apparent_specific_gravity_20[3]) / 3;
    average_water_absorption_20 = (water_absorption_20[1] + water_absorption_20[2] + water_absorption_20[3]) / 3;

    QJsonObject _20_mm;
    _20_mm["Bulk_specific_gravity_1"] = Bulk_specific_gravity_20[1];
    _20_mm["Bulk_specific_gravity_2"] = Bulk_specific_gravity_20[2];
    _20_mm["Bulk_specific_gravity_3"] = Bulk_specific_gravity_20[3];
    _20_mm["apparent_specific_gravity_1"] = apparent_specific_gravity_20[1];
    _20_mm["apparent_specific_gravity_2"] = apparent_specific_gravity_20[2];
    _20_mm["apparent_specific_gravity_3"] = apparent_specific_gravity_20[3];
    _20_mm["water_absorption_1"] = water_absorption_20[1];
    _20_mm["water_absorption_2"] = water_absorption_20[2];
    _20_mm["water_absorption_3"] = water_absorption_20[3];
    _20_mm["average_bulk_specific_gravity"] = average_bulk_specific_gravity_20;
    _20_mm["average_apparent_specific_gravity"] = average_apparent_specific_gravity_20;
    _20_mm["average_water_absorption"] = average_water_absorption_20;

    _20_mm["Weight_of_sample_of_water_1"] = _20mm.Weight_of_sample_of_water[2][1];
    _20_mm["Weight_of_sample_of_water_2"] = _20mm.Weight_of_sample_of_water[2][2];
    _20_mm["Weight_of_sample_of_water_3"] = _20mm.Weight_of_sample_of_water[2][3];
    _20_mm["Weight_of_SSD_Sample_1"] = _20mm.Weight_of_SSD_Sample[2][1];
    _20_mm["Weight_of_SSD_Sample_2"] = _20mm.Weight_of_SSD_Sample[2][2];
    _20_mm["Weight_of_SSD_Sample_3"] = _20mm.Weight_of_SSD_Sample[2][3];
    _20_mm["Weight_of_Oven_dry_sample_1"] = _20mm.Weight_of_Oven_dry_sample[2][1];
    _20_mm["Weight_of_Oven_dry_sample_2"] = _20mm.Weight_of_Oven_dry_sample[2][2];
    _20_mm["Weight_of_Oven_dry_sample_3"] = _20mm.Weight_of_Oven_dry_sample[2][3];

    Specific_Gravity["20mm"] = _20_mm;

    if (spe_gravity.open(QFile::WriteOnly | QFile::Text))
    {
        // json data ko file me likhta hua mai
        QTextStream out(&spe_gravity);
        QJsonDocument jsonDoc_1(Specific_Gravity);
        out << jsonDoc_1.toJson();

        // Close the file
        spe_gravity.close();
    }

    save_check();
}
void MainWindow::on_spc_save_10mm_clicked()
{
    tracked_files.push_back("spc");
    removeDuplicates(tracked_files);

    type_of_material _10mm;

    _10mm.Weight_of_sample_of_water[1][0] = 0;
    _10mm.Weight_of_SSD_Sample[1][0] = 0;
    _10mm.Weight_of_Oven_dry_sample[1][0] = 0;

    _10mm.Weight_of_sample_of_water[1][1] = ui->spc_10_w1_t1->text().toFloat();
    _10mm.Weight_of_sample_of_water[1][2] = ui->spc_10_w1_t2->text().toFloat();
    _10mm.Weight_of_sample_of_water[1][3] = ui->spc_10_w1_t3->text().toFloat();
    _10mm.Weight_of_SSD_Sample[1][1] = ui->spc_10_w2_t1->text().toFloat();
    _10mm.Weight_of_SSD_Sample[1][2] = ui->spc_10_w2_t2->text().toFloat();
    _10mm.Weight_of_SSD_Sample[1][3] = ui->spc_10_w2_t3->text().toFloat();
    _10mm.Weight_of_Oven_dry_sample[1][1] = ui->spc_10_w3_t1->text().toFloat();
    _10mm.Weight_of_Oven_dry_sample[1][2] = ui->spc_10_w3_t2->text().toFloat();
    _10mm.Weight_of_Oven_dry_sample[1][3] = ui->spc_10_w3_t3->text().toFloat();
    float Bulk_specific_gravity_10[4];
    float apparent_specific_gravity_10[4];
    float water_absorption_10[4];
    float average_bulk_specific_gravity_10;
    float average_apparent_specific_gravity_10;
    float average_water_absorption_10;

    Bulk_specific_gravity_10[1] = _10mm.Weight_of_Oven_dry_sample[1][1] / (_10mm.Weight_of_SSD_Sample[1][1] - _10mm.Weight_of_sample_of_water[1][1]);
    Bulk_specific_gravity_10[2] = _10mm.Weight_of_Oven_dry_sample[1][2] / (_10mm.Weight_of_SSD_Sample[1][2] - _10mm.Weight_of_sample_of_water[1][2]);
    Bulk_specific_gravity_10[3] = _10mm.Weight_of_Oven_dry_sample[1][3] / (_10mm.Weight_of_SSD_Sample[1][3] - _10mm.Weight_of_sample_of_water[1][3]);
    apparent_specific_gravity_10[1] = _10mm.Weight_of_Oven_dry_sample[1][1] / (_10mm.Weight_of_Oven_dry_sample[1][1] - _10mm.Weight_of_sample_of_water[1][1]);
    apparent_specific_gravity_10[2] = _10mm.Weight_of_Oven_dry_sample[1][2] / (_10mm.Weight_of_Oven_dry_sample[1][2] - _10mm.Weight_of_sample_of_water[1][2]);
    apparent_specific_gravity_10[3] = _10mm.Weight_of_Oven_dry_sample[1][3] / (_10mm.Weight_of_Oven_dry_sample[1][3] - _10mm.Weight_of_sample_of_water[1][3]);
    water_absorption_10[1] = 100 * (_10mm.Weight_of_SSD_Sample[1][1] - _10mm.Weight_of_Oven_dry_sample[1][1]) / _10mm.Weight_of_Oven_dry_sample[1][1];
    water_absorption_10[2] = 100 * (_10mm.Weight_of_SSD_Sample[1][2] - _10mm.Weight_of_Oven_dry_sample[1][2]) / _10mm.Weight_of_Oven_dry_sample[1][2];
    water_absorption_10[3] = 100 * (_10mm.Weight_of_SSD_Sample[1][3] - _10mm.Weight_of_Oven_dry_sample[1][3]) / _10mm.Weight_of_Oven_dry_sample[1][3];
    average_bulk_specific_gravity_10 = (Bulk_specific_gravity_10[1] + Bulk_specific_gravity_10[2] + Bulk_specific_gravity_10[3]) / 3;
    average_apparent_specific_gravity_10 = (apparent_specific_gravity_10[1] + apparent_specific_gravity_10[2] + apparent_specific_gravity_10[3]) / 3;
    average_water_absorption_10 = (water_absorption_10[1] + water_absorption_10[2] + water_absorption_10[3]) / 3;

    QJsonObject _10_mm;
    _10_mm["Bulk_specific_gravity_1"] = Bulk_specific_gravity_10[1];
    _10_mm["Bulk_specific_gravity_2"] = Bulk_specific_gravity_10[2];
    _10_mm["Bulk_specific_gravity_3"] = Bulk_specific_gravity_10[3];
    _10_mm["apparent_specific_gravity_1"] = apparent_specific_gravity_10[1];
    _10_mm["apparent_specific_gravity_2"] = apparent_specific_gravity_10[2];
    _10_mm["apparent_specific_gravity_3"] = apparent_specific_gravity_10[3];
    _10_mm["water_absorption_1"] = water_absorption_10[1];
    _10_mm["water_absorption_2"] = water_absorption_10[2];
    _10_mm["water_absorption_3"] = water_absorption_10[3];
    _10_mm["average_bulk_specific_gravity"] = average_bulk_specific_gravity_10;
    _10_mm["average_apparent_specific_gravity"] = average_apparent_specific_gravity_10;
    _10_mm["average_water_absorption"] = average_water_absorption_10;

    _10_mm["Weight_of_sample_of_water_1"] = _10mm.Weight_of_sample_of_water[1][1];
    _10_mm["Weight_of_sample_of_water_2"] = _10mm.Weight_of_sample_of_water[1][2];
    _10_mm["Weight_of_sample_of_water_3"] = _10mm.Weight_of_sample_of_water[1][3];
    _10_mm["Weight_of_SSD_Sample_1"] = _10mm.Weight_of_SSD_Sample[1][1];
    _10_mm["Weight_of_SSD_Sample_2"] = _10mm.Weight_of_SSD_Sample[1][2];
    _10_mm["Weight_of_SSD_Sample_3"] = _10mm.Weight_of_SSD_Sample[1][3];
    _10_mm["Weight_of_Oven_dry_sample_1"] = _10mm.Weight_of_Oven_dry_sample[1][1];
    _10_mm["Weight_of_Oven_dry_sample_2"] = _10mm.Weight_of_Oven_dry_sample[1][2];
    _10_mm["Weight_of_Oven_dry_sample_3"] = _10mm.Weight_of_Oven_dry_sample[1][3];
    Specific_Gravity["10mm"] = _10_mm;

    if (spe_gravity.open(QFile::WriteOnly | QFile::Text))
    {
        // json data ko file me likhta hua mai
        QTextStream out(&spe_gravity);
        QJsonDocument jsonDoc_1(Specific_Gravity);
        out << jsonDoc_1.toJson();

        // Close the file
        spe_gravity.close();
    }

    save_check();
}
void MainWindow::on_spc_save_0mm_clicked()
{
    tracked_files.push_back("spc");
    removeDuplicates(tracked_files);

    type_of_material stone_dust;

    stone_dust.Weight_of_sample_of_water[0][0] = 0;
    stone_dust.Weight_of_SSD_Sample[0][0] = 0;
    stone_dust.Weight_of_Oven_dry_sample[0][0] = 0;

    stone_dust.Weight_of_sample_of_water[0][1] = ui->spc_0_w1_t1->text().toFloat();
    stone_dust.Weight_of_sample_of_water[0][2] = ui->spc_0_w1_t2->text().toFloat();
    stone_dust.Weight_of_sample_of_water[0][3] = ui->spc_0_w1_t3->text().toFloat();
    stone_dust.Weight_of_SSD_Sample[0][1] = ui->spc_0_w2_t1->text().toFloat();
    stone_dust.Weight_of_SSD_Sample[0][2] = ui->spc_0_w2_t2->text().toFloat();
    stone_dust.Weight_of_SSD_Sample[0][3] = ui->spc_0_w2_t3->text().toFloat();
    stone_dust.Weight_of_Oven_dry_sample[0][1] = ui->spc_0_w3_t1->text().toFloat();
    stone_dust.Weight_of_Oven_dry_sample[0][2] = ui->spc_0_w3_t2->text().toFloat();
    stone_dust.Weight_of_Oven_dry_sample[0][3] = ui->spc_0_w3_t3->text().toFloat();

    float Bulk_specific_gravity_s_d[4];
    float apparent_specific_gravity_s_d[4];
    float water_absorption_s_d[4];
    float average_bulk_specific_gravity_s_d;
    float average_apparent_specific_gravity_s_d;
    float average_water_absorption_s_d;

    Bulk_specific_gravity_s_d[1] = stone_dust.Weight_of_Oven_dry_sample[0][1] / (stone_dust.Weight_of_SSD_Sample[0][1] - stone_dust.Weight_of_sample_of_water[0][1]);
    Bulk_specific_gravity_s_d[2] = stone_dust.Weight_of_Oven_dry_sample[0][2] / (stone_dust.Weight_of_SSD_Sample[0][2] - stone_dust.Weight_of_sample_of_water[0][2]);
    Bulk_specific_gravity_s_d[3] = stone_dust.Weight_of_Oven_dry_sample[0][3] / (stone_dust.Weight_of_SSD_Sample[0][3] - stone_dust.Weight_of_sample_of_water[0][3]);
    apparent_specific_gravity_s_d[1] = stone_dust.Weight_of_Oven_dry_sample[0][1] / (stone_dust.Weight_of_Oven_dry_sample[0][1] - stone_dust.Weight_of_sample_of_water[0][1]);
    apparent_specific_gravity_s_d[2] = stone_dust.Weight_of_Oven_dry_sample[0][2] / (stone_dust.Weight_of_Oven_dry_sample[0][2] - stone_dust.Weight_of_sample_of_water[0][2]);
    apparent_specific_gravity_s_d[3] = stone_dust.Weight_of_Oven_dry_sample[0][3] / (stone_dust.Weight_of_Oven_dry_sample[0][3] - stone_dust.Weight_of_sample_of_water[0][3]);
    water_absorption_s_d[1] = 100 * (stone_dust.Weight_of_SSD_Sample[0][1] - stone_dust.Weight_of_Oven_dry_sample[0][1]) / stone_dust.Weight_of_Oven_dry_sample[0][1];
    water_absorption_s_d[2] = 100 * (stone_dust.Weight_of_SSD_Sample[0][2] - stone_dust.Weight_of_Oven_dry_sample[0][2]) / stone_dust.Weight_of_Oven_dry_sample[0][2];
    water_absorption_s_d[3] = 100 * (stone_dust.Weight_of_SSD_Sample[0][3] - stone_dust.Weight_of_Oven_dry_sample[0][3]) / stone_dust.Weight_of_Oven_dry_sample[0][3];
    average_bulk_specific_gravity_s_d = (Bulk_specific_gravity_s_d[1] + Bulk_specific_gravity_s_d[2] + Bulk_specific_gravity_s_d[3]) / 3;
    average_apparent_specific_gravity_s_d = (apparent_specific_gravity_s_d[1] + apparent_specific_gravity_s_d[2] + apparent_specific_gravity_s_d[3]) / 3;
    average_water_absorption_s_d = (water_absorption_s_d[1] + water_absorption_s_d[2] + water_absorption_s_d[3]) / 3;

    QJsonObject _stone_dust;
    _stone_dust["Bulk_specific_gravity_1"] = Bulk_specific_gravity_s_d[1];
    _stone_dust["Bulk_specific_gravity_2"] = Bulk_specific_gravity_s_d[2];
    _stone_dust["Bulk_specific_gravity_3"] = Bulk_specific_gravity_s_d[3];
    _stone_dust["apparent_specific_gravity_1"] = apparent_specific_gravity_s_d[1];
    _stone_dust["apparent_specific_gravity_2"] = apparent_specific_gravity_s_d[2];
    _stone_dust["apparent_specific_gravity_3"] = apparent_specific_gravity_s_d[3];
    _stone_dust["water_absorption_1"] = water_absorption_s_d[1];
    _stone_dust["water_absorption_2"] = water_absorption_s_d[2];
    _stone_dust["water_absorption_3"] = water_absorption_s_d[3];
    _stone_dust["average_bulk_specific_gravity"] = average_bulk_specific_gravity_s_d;
    _stone_dust["average_apparent_specific_gravity"] = average_apparent_specific_gravity_s_d;
    _stone_dust["average_water_absorption"] = average_water_absorption_s_d;

    _stone_dust["Weight_of_sample_of_water_1"] = stone_dust.Weight_of_sample_of_water[0][1];
    _stone_dust["Weight_of_sample_of_water_2"] = stone_dust.Weight_of_sample_of_water[0][2];
    _stone_dust["Weight_of_sample_of_water_3"] = stone_dust.Weight_of_sample_of_water[0][3];
    _stone_dust["Weight_of_SSD_Sample_1"] = stone_dust.Weight_of_SSD_Sample[0][1];
    _stone_dust["Weight_of_SSD_Sample_2"] = stone_dust.Weight_of_SSD_Sample[0][2];
    _stone_dust["Weight_of_SSD_Sample_3"] = stone_dust.Weight_of_SSD_Sample[0][3];
    _stone_dust["Weight_of_Oven_dry_sample_1"] = stone_dust.Weight_of_Oven_dry_sample[0][1];
    _stone_dust["Weight_of_Oven_dry_sample_2"] = stone_dust.Weight_of_Oven_dry_sample[0][2];
    _stone_dust["Weight_of_Oven_dry_sample_3"] = stone_dust.Weight_of_Oven_dry_sample[0][3];
    Specific_Gravity["stone_dust"] = _stone_dust;
    if (spe_gravity.open(QFile::WriteOnly | QFile::Text))
    {
        // json data ko file me likhta hua mai
        QTextStream out(&spe_gravity);
        QJsonDocument jsonDoc_1(Specific_Gravity);
        out << jsonDoc_1.toJson();

        // Close the file
        spe_gravity.close();
    }

    save_check();
}
QJsonObject Flakiness_Elongation_Indices;
QFile Flakiness_Elongation(cwd.filePath("json/fei.json"));
void MainWindow::on_save_ss_clicked()
{
    tracked_files.push_back("fei");
    removeDuplicates(tracked_files);

    passing[1] = ui->fei_ss_p1->text().toFloat();
    passing[2] = ui->fei_ss_p2->text().toFloat();
    passing[3] = ui->fei_ss_p3->text().toFloat();
    passing[4] = ui->fei_ss_p4->text().toFloat();
    passing[5] = ui->fei_ss_p5->text().toFloat();
    passing[6] = ui->fei_ss_p6->text().toFloat();
    passing[7] = ui->fei_ss_p7->text().toFloat();
    passing[8] = ui->fei_ss_p8->text().toFloat();
    passing[9] = ui->fei_ss_p9->text().toFloat();

    Flakiness_Elongation_Indices["p_1"] = passing[1];
    Flakiness_Elongation_Indices["p_2"] = passing[2];
    Flakiness_Elongation_Indices["p_3"] = passing[3];
    Flakiness_Elongation_Indices["p_4"] = passing[4];
    Flakiness_Elongation_Indices["p_5"] = passing[5];
    Flakiness_Elongation_Indices["p_6"] = passing[6];
    Flakiness_Elongation_Indices["p_7"] = passing[7];
    Flakiness_Elongation_Indices["p_8"] = passing[8];
    Flakiness_Elongation_Indices["p_9"] = passing[9];

    retained[1] = ui->fei_ss_r1->text().toFloat();
    retained[2] = ui->fei_ss_r2->text().toFloat();
    retained[3] = ui->fei_ss_r3->text().toFloat();
    retained[4] = ui->fei_ss_r4->text().toFloat();
    retained[5] = ui->fei_ss_r5->text().toFloat();
    retained[6] = ui->fei_ss_r6->text().toFloat();
    retained[7] = ui->fei_ss_r7->text().toFloat();
    retained[8] = ui->fei_ss_r8->text().toFloat();
    retained[9] = ui->fei_ss_r9->text().toFloat();

    Flakiness_Elongation_Indices["r_1"] = retained[1];
    Flakiness_Elongation_Indices["r_2"] = retained[2];
    Flakiness_Elongation_Indices["r_3"] = retained[3];
    Flakiness_Elongation_Indices["r_4"] = retained[4];
    Flakiness_Elongation_Indices["r_5"] = retained[5];
    Flakiness_Elongation_Indices["r_6"] = retained[6];
    Flakiness_Elongation_Indices["r_7"] = retained[7];
    Flakiness_Elongation_Indices["r_8"] = retained[8];
    Flakiness_Elongation_Indices["r_9"] = retained[9];
    if (Flakiness_Elongation.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&Flakiness_Elongation);
        QJsonDocument jsonDoc_2(Flakiness_Elongation_Indices);
        out << jsonDoc_2.toJson();

        Flakiness_Elongation.close();
    }

    save_check();
}
void MainWindow::on_save_fei_clicked()
{
    tracked_files.push_back("fei");
    removeDuplicates(tracked_files);

    A[0] = 0;
    A[1] = ui->fei_1_1->text().toFloat();
    A[2] = ui->fei_1_2->text().toFloat();
    A[3] = ui->fei_1_3->text().toFloat();
    A[4] = ui->fei_1_4->text().toFloat();
    A[5] = ui->fei_1_5->text().toFloat();
    A[6] = ui->fei_1_6->text().toFloat();
    A[7] = ui->fei_1_7->text().toFloat();
    A[8] = ui->fei_1_8->text().toFloat();
    A[9] = ui->fei_1_9->text().toFloat();
    A[10] = A[1] + A[2] + A[3] + A[4] + A[5] + A[6] + A[7] + A[8] + A[9];

    Flakiness_Elongation_Indices["A_1"] = A[1];
    Flakiness_Elongation_Indices["A_2"] = A[2];
    Flakiness_Elongation_Indices["A_3"] = A[3];
    Flakiness_Elongation_Indices["A_4"] = A[4];
    Flakiness_Elongation_Indices["A_5"] = A[5];
    Flakiness_Elongation_Indices["A_6"] = A[6];
    Flakiness_Elongation_Indices["A_7"] = A[7];
    Flakiness_Elongation_Indices["A_8"] = A[8];
    Flakiness_Elongation_Indices["A_9"] = A[9];
    Flakiness_Elongation_Indices["TOTAL_WEIGHT_A"] = A[10];

    B[0] = 0;
    B[1] = ui->fei_2_1->text().toFloat();
    B[2] = ui->fei_2_2->text().toFloat();
    B[3] = ui->fei_2_3->text().toFloat();
    B[4] = ui->fei_2_4->text().toFloat();
    B[5] = ui->fei_2_5->text().toFloat();
    B[6] = ui->fei_2_6->text().toFloat();
    B[7] = ui->fei_2_7->text().toFloat();
    B[8] = ui->fei_2_8->text().toFloat();
    B[9] = ui->fei_2_9->text().toFloat();
    B[10] = B[1] + B[2] + B[3] + B[4] + B[5] + B[6] + B[7] + B[8] + B[9];

    Flakiness_Elongation_Indices["B_1"] = B[1];
    Flakiness_Elongation_Indices["B_2"] = B[2];
    Flakiness_Elongation_Indices["B_3"] = B[3];
    Flakiness_Elongation_Indices["B_4"] = B[4];
    Flakiness_Elongation_Indices["B_5"] = B[5];
    Flakiness_Elongation_Indices["B_6"] = B[6];
    Flakiness_Elongation_Indices["B_7"] = B[7];
    Flakiness_Elongation_Indices["B_8"] = B[8];
    Flakiness_Elongation_Indices["B_9"] = B[9];
    Flakiness_Elongation_Indices["TOTAL_WEIGHT_B"] = B[10];

    C[0] = 0;
    C[1] = ui->fei_3_1->text().toFloat();
    C[2] = ui->fei_3_2->text().toFloat();
    C[3] = ui->fei_3_3->text().toFloat();
    C[4] = ui->fei_3_4->text().toFloat();
    C[5] = ui->fei_3_5->text().toFloat();
    C[6] = ui->fei_3_6->text().toFloat();
    C[7] = ui->fei_3_7->text().toFloat();
    C[8] = ui->fei_3_8->text().toFloat();
    C[9] = ui->fei_3_9->text().toFloat();
    C[10] = C[1] + C[2] + C[3] + C[4] + C[5] + C[6] + C[7] + C[8] + C[9];

    Flakiness_Elongation_Indices["C_1"] = C[1];
    Flakiness_Elongation_Indices["C_2"] = C[2];
    Flakiness_Elongation_Indices["C_3"] = C[3];
    Flakiness_Elongation_Indices["C_4"] = C[4];
    Flakiness_Elongation_Indices["C_5"] = C[5];
    Flakiness_Elongation_Indices["C_6"] = C[6];
    Flakiness_Elongation_Indices["C_7"] = C[7];
    Flakiness_Elongation_Indices["C_8"] = C[8];
    Flakiness_Elongation_Indices["C_9"] = C[9];
    Flakiness_Elongation_Indices["TOTAL_WEIGHT_C"] = C[10];

    D[0] = 0;
    D[1] = ui->fei_4_1->text().toFloat();
    D[2] = ui->fei_4_2->text().toFloat();
    D[3] = ui->fei_4_3->text().toFloat();
    D[4] = ui->fei_4_4->text().toFloat();
    D[5] = ui->fei_4_5->text().toFloat();
    D[6] = ui->fei_4_6->text().toFloat();
    D[7] = ui->fei_4_7->text().toFloat();
    D[8] = ui->fei_4_8->text().toFloat();
    D[9] = ui->fei_4_9->text().toFloat();
    D[10] = D[1] + D[2] + D[3] + D[4] + D[5] + D[6] + D[7] + D[8] + D[9]; // total weight
    D[11] = 100 * C[10] / A[10];                                          // flekiness index
    D[12] = 100 * D[10] / D[10];                                          // elongation index
    D[13] = D[11] + D[12];                                                // combined index
    Flakiness_Elongation_Indices["D_1"] = D[1];
    Flakiness_Elongation_Indices["D_2"] = D[2];
    Flakiness_Elongation_Indices["D_3"] = D[3];
    Flakiness_Elongation_Indices["D_4"] = D[4];
    Flakiness_Elongation_Indices["D_5"] = D[5];
    Flakiness_Elongation_Indices["D_6"] = D[6];
    Flakiness_Elongation_Indices["D_7"] = D[7];
    Flakiness_Elongation_Indices["D_8"] = D[8];
    Flakiness_Elongation_Indices["D_9"] = D[9];
    Flakiness_Elongation_Indices["TOTAL_WEIGHT_D"] = D[10];
    Flakiness_Elongation_Indices["flakiness index"] = D[11];
    Flakiness_Elongation_Indices["elongation index"] = D[12];
    Flakiness_Elongation_Indices["combined index"] = D[13];

    if (Flakiness_Elongation.open(QFile::WriteOnly | QFile::Text))
    {

        QTextStream out(&Flakiness_Elongation);
        QJsonDocument jsonDoc_2(Flakiness_Elongation_Indices);
        out << jsonDoc_2.toJson();

        Flakiness_Elongation.close();
    }

    save_check();
}
QJsonObject aiv_json;
QFile aiv_(cwd.filePath("json/aiv.json"));
void MainWindow::on_aiv_save_20mm_clicked()
{
    QJsonObject aiv_json_20mm;

    tracked_files.push_back("aiv");
    removeDuplicates(tracked_files);

    weight_of_cylinder[2][1] = ui->aiv_20_11->text().toFloat();
    weight_of_cylinder[2][2] = ui->aiv_20_12->text().toFloat();
    weight_of_cylinder[2][3] = ui->aiv_20_13->text().toFloat();
    weight_of_cylinder_sample[2][1] = ui->aiv_20_21->text().toFloat();
    weight_of_cylinder_sample[2][2] = ui->aiv_20_22->text().toFloat();
    weight_of_cylinder_sample[2][3] = ui->aiv_20_23->text().toFloat();
    weight_of_sample[2][1] = weight_of_cylinder_sample[2][1] - weight_of_cylinder[2][1];
    weight_of_sample[2][2] = weight_of_cylinder_sample[2][2] - weight_of_cylinder[2][2];
    weight_of_sample[2][3] = weight_of_cylinder_sample[2][3] - weight_of_cylinder[2][3];
    weight_crushed_material[2][1] = ui->aiv_20_41->text().toFloat();
    weight_crushed_material[2][2] = ui->aiv_20_42->text().toFloat();
    weight_crushed_material[2][3] = ui->aiv_20_43->text().toFloat();
    aggregate_impact_value[2][1] = 100 * weight_crushed_material[2][1] / (weight_of_sample[2][1]);
    aggregate_impact_value[2][2] = 100 * weight_crushed_material[2][2] / (weight_of_sample[2][2]);
    aggregate_impact_value[2][3] = 100 * weight_crushed_material[2][3] / (weight_of_sample[2][3]);
    avg_aggregate_impact_value_10 = (aggregate_impact_value[2][1] + aggregate_impact_value[2][1] + aggregate_impact_value[2][1]) / 3;
    aiv_json_20mm["weight_of_cylinder_1"] = weight_of_cylinder[2][1];
    aiv_json_20mm["weight_of_cylinder_2"] = weight_of_cylinder[2][2];
    aiv_json_20mm["weight_of_cylinder_3"] = weight_of_cylinder[2][3];
    aiv_json_20mm["weight_of_cylinder_sample_1"] = weight_of_cylinder_sample[2][1];
    aiv_json_20mm["weight_of_cylinder_sample_2"] = weight_of_cylinder_sample[2][2];
    aiv_json_20mm["weight_of_cylinder_sample_3"] = weight_of_cylinder_sample[2][3];
    aiv_json_20mm["weight_sample_1"] = weight_of_cylinder_sample[2][1];
    aiv_json_20mm["weight_sample_2"] = weight_of_cylinder_sample[2][2];
    aiv_json_20mm["weight_sample_3"] = weight_of_cylinder_sample[2][3];
    aiv_json_20mm["weight_crushed_material_1"] = weight_crushed_material[2][1];
    aiv_json_20mm["weight_crushed_material_2"] = weight_crushed_material[2][2];
    aiv_json_20mm["weight_crushed_material_3"] = weight_crushed_material[2][3];
    aiv_json_20mm["aggeregate_impact_value_1"] = aggregate_impact_value[2][1];
    aiv_json_20mm["aggeregate_impact_value_2"] = aggregate_impact_value[2][2];
    aiv_json_20mm["aggeregate_impact_value_3"] = aggregate_impact_value[2][3];
    aiv_json_20mm["avg_aggregate_impact_value_20"] = avg_aggregate_impact_value_10;

    aiv_json["20mm"] = aiv_json_20mm;
    if (aiv_.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&aiv_);
        QJsonDocument jsonDoc_3(aiv_json);
        out << jsonDoc_3.toJson();
        aiv_.close();
    }

    save_check();
}
void MainWindow::on_aiv_save_10mm_clicked()
{
    QJsonObject aiv_json_10mm;

    tracked_files.push_back("aiv");
    removeDuplicates(tracked_files);

    weight_of_cylinder[1][1] = ui->aiv_10_11->text().toFloat();
    weight_of_cylinder[1][2] = ui->aiv_10_12->text().toFloat();
    weight_of_cylinder[1][3] = ui->aiv_10_13->text().toFloat();
    weight_of_cylinder_sample[1][1] = ui->aiv_10_21->text().toFloat();
    weight_of_cylinder_sample[1][2] = ui->aiv_10_22->text().toFloat();
    weight_of_cylinder_sample[1][3] = ui->aiv_10_23->text().toFloat();
    weight_of_sample[1][1] = weight_of_cylinder_sample[1][1] - weight_of_cylinder[1][1];
    weight_of_sample[1][2] = weight_of_cylinder_sample[1][2] - weight_of_cylinder[1][2];
    weight_of_sample[1][3] = weight_of_cylinder_sample[1][3] - weight_of_cylinder[1][3];
    weight_crushed_material[1][1] = ui->aiv_10_41->text().toFloat();
    weight_crushed_material[1][2] = ui->aiv_10_42->text().toFloat();
    weight_crushed_material[1][3] = ui->aiv_10_43->text().toFloat();
    aggregate_impact_value[1][1] = 100 * weight_crushed_material[1][1] / (weight_of_sample[1][1]);
    aggregate_impact_value[1][2] = 100 * weight_crushed_material[1][2] / (weight_of_sample[1][2]);
    aggregate_impact_value[1][3] = 100 * weight_crushed_material[1][3] / (weight_of_sample[1][3]);
    avg_aggregate_impact_value_10 = (aggregate_impact_value[1][1] + aggregate_impact_value[1][1] + aggregate_impact_value[1][1]) / 3;
    aiv_json_10mm["weight_of_cylinder_1"] = weight_of_cylinder[1][1];
    aiv_json_10mm["weight_of_cylinder_2"] = weight_of_cylinder[1][2];
    aiv_json_10mm["weight_of_cylinder_3"] = weight_of_cylinder[1][3];
    aiv_json_10mm["weight_of_cylinder_sample_1"] = weight_of_cylinder_sample[1][1];
    aiv_json_10mm["weight_of_cylinder_sample_2"] = weight_of_cylinder_sample[1][2];
    aiv_json_10mm["weight_of_cylinder_sample_3"] = weight_of_cylinder_sample[1][3];
    aiv_json_10mm["weight_sample_1"] = weight_of_cylinder_sample[1][1];
    aiv_json_10mm["weight_sample_2"] = weight_of_cylinder_sample[1][2];
    aiv_json_10mm["weight_sample_3"] = weight_of_cylinder_sample[1][3];
    aiv_json_10mm["weight_crushed_material_1"] = weight_crushed_material[1][1];
    aiv_json_10mm["weight_crushed_material_2"] = weight_crushed_material[1][2];
    aiv_json_10mm["weight_crushed_material_3"] = weight_crushed_material[1][3];
    aiv_json_10mm["aggeregate_impact_value_1"] = aggregate_impact_value[1][1];
    aiv_json_10mm["aggeregate_impact_value_2"] = aggregate_impact_value[1][2];
    aiv_json_10mm["aggeregate_impact_value_3"] = aggregate_impact_value[1][3];
    aiv_json_10mm["avg_aggregate_impact_value_10"] = avg_aggregate_impact_value_10;
    aiv_json["10mm"] = aiv_json_10mm;
    if (aiv_.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&aiv_);
        QJsonDocument jsonDoc_3(aiv_json);
        out << jsonDoc_3.toJson();
        aiv_.close();
    }

    save_check();
}
QJsonObject idg_json;
QFile idg(cwd.filePath("json/idg.json"));
void MainWindow::on_idg_save_40mm_clicked()
{
    tracked_files.push_back("ind");
    removeDuplicates(tracked_files);

    float cumsum = 0;

    is_sieve_40[1][1] = ui->idg_40_s11->text().toFloat();
    is_sieve_40[1][2] = ui->idg_40_s12->text().toFloat();
    is_sieve_40[1][3] = ui->idg_40_s13->text().toFloat();
    is_sieve_40[1][4] = ui->idg_40_s14->text().toFloat();
    is_sieve_40[1][5] = ui->idg_40_s15->text().toFloat();
    is_sieve_40[1][6] = ui->idg_40_s16->text().toFloat();
    is_sieve_40[1][7] = ui->idg_40_s17->text().toFloat();
    is_sieve_40[1][8] = ui->idg_40_s18->text().toFloat();

    weight_of_retained_40[1][1] = ui->idg_40_w11->text().toFloat();
    weight_of_retained_40[1][2] = ui->idg_40_w12->text().toFloat();
    weight_of_retained_40[1][3] = ui->idg_40_w13->text().toFloat();
    weight_of_retained_40[1][4] = ui->idg_40_w14->text().toFloat();
    weight_of_retained_40[1][5] = ui->idg_40_w15->text().toFloat();
    weight_of_retained_40[1][6] = ui->idg_40_w16->text().toFloat();
    weight_of_retained_40[1][7] = ui->idg_40_w17->text().toFloat();
    weight_of_retained_40[1][8] = ui->idg_40_w18->text().toFloat();

    is_sieve_40[2][1] = ui->idg_40_s21->text().toFloat();
    is_sieve_40[2][2] = ui->idg_40_s22->text().toFloat();
    is_sieve_40[2][3] = ui->idg_40_s23->text().toFloat();
    is_sieve_40[2][4] = ui->idg_40_s24->text().toFloat();
    is_sieve_40[2][5] = ui->idg_40_s25->text().toFloat();
    is_sieve_40[2][6] = ui->idg_40_s26->text().toFloat();
    is_sieve_40[2][7] = ui->idg_40_s27->text().toFloat();
    is_sieve_40[2][8] = ui->idg_40_s28->text().toFloat();

    weight_of_retained_40[2][1] = ui->idg_40_w21->text().toFloat();
    weight_of_retained_40[2][2] = ui->idg_40_w22->text().toFloat();
    weight_of_retained_40[2][3] = ui->idg_40_w23->text().toFloat();
    weight_of_retained_40[2][4] = ui->idg_40_w24->text().toFloat();
    weight_of_retained_40[2][5] = ui->idg_40_w25->text().toFloat();
    weight_of_retained_40[2][6] = ui->idg_40_w26->text().toFloat();
    weight_of_retained_40[2][7] = ui->idg_40_w27->text().toFloat();
    weight_of_retained_40[2][8] = ui->idg_40_w28->text().toFloat();

    is_sieve_40[3][1] = ui->idg_40_s31->text().toFloat();
    is_sieve_40[3][2] = ui->idg_40_s32->text().toFloat();
    is_sieve_40[3][3] = ui->idg_40_s33->text().toFloat();
    is_sieve_40[3][4] = ui->idg_40_s34->text().toFloat();
    is_sieve_40[3][5] = ui->idg_40_s35->text().toFloat();
    is_sieve_40[3][6] = ui->idg_40_s36->text().toFloat();
    is_sieve_40[3][7] = ui->idg_40_s36->text().toFloat();
    is_sieve_40[3][8] = ui->idg_40_s38->text().toFloat();

    weight_of_retained_40[3][1] = ui->idg_40_w31->text().toFloat();
    weight_of_retained_40[3][2] = ui->idg_40_w32->text().toFloat();
    weight_of_retained_40[3][3] = ui->idg_40_w33->text().toFloat();
    weight_of_retained_40[3][4] = ui->idg_40_w34->text().toFloat();
    weight_of_retained_40[3][5] = ui->idg_40_w35->text().toFloat();
    weight_of_retained_40[3][6] = ui->idg_40_w36->text().toFloat();
    weight_of_retained_40[3][7] = ui->idg_40_w37->text().toFloat();
    weight_of_retained_40[3][8] = ui->idg_40_w38->text().toFloat();

    total_weight[0] = ui->ind_40_w1->text().toFloat();
    total_weight[1] = ui->ind_40_w2->text().toFloat();
    total_weight[2] = ui->ind_40_w3->text().toFloat();

    for (int i = 1; i <= 3; i++)
    {
        cumsum = 0;
        for (int j = 1; j <= 8; j++)
        {
            cumsum += weight_of_retained_40[i][j];
            ind_cumulative[i][j] = cumsum;
            ind_cumulative_percent[i][j] = 100 * cumsum / total_weight[i - 1];
            ind_cum_pass[i][j] = 100 - (100 * cumsum / total_weight[i - 1]);
            // qDebug() << cumsum << " " << ind_cumulative_percent[i][j] << " " << ind_cum_pass[i][j];
        }
    }

    QJsonObject individual_gradation_40mm;

    individual_gradation_40mm["total_weight_1"] = total_weight[0];
    individual_gradation_40mm["total_weight_2"] = total_weight[1];
    individual_gradation_40mm["total_weight_3"] = total_weight[2];

    individual_gradation_40mm["is_sieve_s11"] = is_sieve_40[1][1];
    individual_gradation_40mm["is_sieve_s12"] = is_sieve_40[1][2];
    individual_gradation_40mm["is_sieve_s13"] = is_sieve_40[1][3];
    individual_gradation_40mm["is_sieve_s14"] = is_sieve_40[1][4];
    individual_gradation_40mm["is_sieve_s15"] = is_sieve_40[1][5];
    individual_gradation_40mm["is_sieve_s16"] = is_sieve_40[1][6];
    individual_gradation_40mm["is_sieve_s17"] = is_sieve_40[1][7];
    individual_gradation_40mm["is_sieve_s18"] = is_sieve_40[1][8];

    individual_gradation_40mm["weight_of_retained_w11"] = weight_of_retained_40[1][1];
    individual_gradation_40mm["weight_of_retained_w12"] = weight_of_retained_40[1][2];
    individual_gradation_40mm["weight_of_retained_w13"] = weight_of_retained_40[1][3];
    individual_gradation_40mm["weight_of_retained_w14"] = weight_of_retained_40[1][4];
    individual_gradation_40mm["weight_of_retained_w15"] = weight_of_retained_40[1][5];
    individual_gradation_40mm["weight_of_retained_w16"] = weight_of_retained_40[1][6];
    individual_gradation_40mm["weight_of_retained_w17"] = weight_of_retained_40[1][7];
    individual_gradation_40mm["weight_of_retained_w18"] = weight_of_retained_40[1][8];

    individual_gradation_40mm["is_sieve_s21"] = is_sieve_40[2][1];
    individual_gradation_40mm["is_sieve_s22"] = is_sieve_40[2][2];
    individual_gradation_40mm["is_sieve_s23"] = is_sieve_40[2][3];
    individual_gradation_40mm["is_sieve_s24"] = is_sieve_40[2][4];
    individual_gradation_40mm["is_sieve_s25"] = is_sieve_40[2][5];
    individual_gradation_40mm["is_sieve_s26"] = is_sieve_40[2][6];
    individual_gradation_40mm["is_sieve_s27"] = is_sieve_40[2][7];
    individual_gradation_40mm["is_sieve_s28"] = is_sieve_40[2][8];

    individual_gradation_40mm["weight_of_retained_w21"] = weight_of_retained_40[2][1];
    individual_gradation_40mm["weight_of_retained_w22"] = weight_of_retained_40[2][2];
    individual_gradation_40mm["weight_of_retained_w23"] = weight_of_retained_40[2][3];
    individual_gradation_40mm["weight_of_retained_w24"] = weight_of_retained_40[2][4];
    individual_gradation_40mm["weight_of_retained_w25"] = weight_of_retained_40[2][5];
    individual_gradation_40mm["weight_of_retained_w26"] = weight_of_retained_40[2][6];
    individual_gradation_40mm["weight_of_retained_w27"] = weight_of_retained_40[2][7];
    individual_gradation_40mm["weight_of_retained_w28"] = weight_of_retained_40[2][8];

    individual_gradation_40mm["is_sieve_s31"] = is_sieve_40[3][1];
    individual_gradation_40mm["is_sieve_s32"] = is_sieve_40[3][2];
    individual_gradation_40mm["is_sieve_s33"] = is_sieve_40[3][3];
    individual_gradation_40mm["is_sieve_s34"] = is_sieve_40[3][4];
    individual_gradation_40mm["is_sieve_s35"] = is_sieve_40[3][5];
    individual_gradation_40mm["is_sieve_s36"] = is_sieve_40[3][6];
    individual_gradation_40mm["is_sieve_s37"] = is_sieve_40[3][7];
    individual_gradation_40mm["is_sieve_s38"] = is_sieve_40[3][8];

    individual_gradation_40mm["weight_of_retained_w31"] = weight_of_retained_40[3][1];
    individual_gradation_40mm["weight_of_retained_w32"] = weight_of_retained_40[3][2];
    individual_gradation_40mm["weight_of_retained_w33"] = weight_of_retained_40[3][3];
    individual_gradation_40mm["weight_of_retained_w34"] = weight_of_retained_40[3][4];
    individual_gradation_40mm["weight_of_retained_w35"] = weight_of_retained_40[3][5];
    individual_gradation_40mm["weight_of_retained_w36"] = weight_of_retained_40[3][6];
    individual_gradation_40mm["weight_of_retained_w37"] = weight_of_retained_40[3][7];
    individual_gradation_40mm["weight_of_retained_w38"] = weight_of_retained_40[3][8];
    individual_gradation_40mm["proportion"] = ui->ind_40_7->text().toFloat();

    std::string base_CUM = "CUM_";
    std::string base_cum = "cum_";
    std::string base_pass = "pass_";
    std::string base_total = "total_weight_";

    for (int i = 1; i <= 3; i++)
    {
        for (int j = 1; j <= 8; j++)
        {
            std::string istr = std::to_string(i);
            std::string jstr = std::to_string(j);

            QString target_cum = QString::fromStdString(base_cum + istr + jstr);
            QString target_CUM = QString::fromStdString(base_CUM + istr + jstr);
            QString target_pass = QString::fromStdString(base_pass + istr + jstr);
            QString target_total = QString::fromStdString(base_total + istr);

            individual_gradation_40mm[target_cum] = ind_cumulative[i][j];
            individual_gradation_40mm[target_CUM] = ind_cumulative_percent[i][j];
            individual_gradation_40mm[target_pass] = ind_cum_pass[i][j];
        }
    }

    idg_json["40mm"] = individual_gradation_40mm;

    if (idg.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&idg);
        QJsonDocument jsonDoc_4(idg_json);
        out << jsonDoc_4.toJson();
        idg.close();
    }

    save_check();
}
void MainWindow::on_idg_save_20mm_clicked()
{
    tracked_files.push_back("ind");
    removeDuplicates(tracked_files);

    float cumsum = 0;

    is_sieve_40[1][1] = ui->idg_20_s11->text().toFloat();
    is_sieve_40[1][2] = ui->idg_20_s12->text().toFloat();
    is_sieve_40[1][3] = ui->idg_20_s13->text().toFloat();
    is_sieve_40[1][4] = ui->idg_20_s14->text().toFloat();
    is_sieve_40[1][5] = ui->idg_20_s15->text().toFloat();
    is_sieve_40[1][6] = ui->idg_20_s16->text().toFloat();
    is_sieve_40[1][7] = ui->idg_20_s17->text().toFloat();
    is_sieve_40[1][8] = ui->idg_20_s18->text().toFloat();

    weight_of_retained_40[1][1] = ui->idg_20_w11->text().toFloat();
    weight_of_retained_40[1][2] = ui->idg_20_w12->text().toFloat();
    weight_of_retained_40[1][3] = ui->idg_20_w13->text().toFloat();
    weight_of_retained_40[1][4] = ui->idg_20_w14->text().toFloat();
    weight_of_retained_40[1][5] = ui->idg_20_w15->text().toFloat();
    weight_of_retained_40[1][6] = ui->idg_20_w16->text().toFloat();
    weight_of_retained_40[1][7] = ui->idg_20_w17->text().toFloat();
    weight_of_retained_40[1][8] = ui->idg_20_w18->text().toFloat();

    is_sieve_40[2][1] = ui->idg_20_s21->text().toFloat();
    is_sieve_40[2][2] = ui->idg_20_s22->text().toFloat();
    is_sieve_40[2][3] = ui->idg_20_s23->text().toFloat();
    is_sieve_40[2][4] = ui->idg_20_s24->text().toFloat();
    is_sieve_40[2][5] = ui->idg_20_s25->text().toFloat();
    is_sieve_40[2][6] = ui->idg_20_s26->text().toFloat();
    is_sieve_40[2][7] = ui->idg_20_s27->text().toFloat();
    is_sieve_40[2][8] = ui->idg_20_s28->text().toFloat();

    weight_of_retained_40[2][1] = ui->idg_20_w21->text().toFloat();
    weight_of_retained_40[2][2] = ui->idg_20_w22->text().toFloat();
    weight_of_retained_40[2][3] = ui->idg_20_w23->text().toFloat();
    weight_of_retained_40[2][4] = ui->idg_20_w24->text().toFloat();
    weight_of_retained_40[2][5] = ui->idg_20_w25->text().toFloat();
    weight_of_retained_40[2][6] = ui->idg_20_w26->text().toFloat();
    weight_of_retained_40[2][7] = ui->idg_20_w27->text().toFloat();
    weight_of_retained_40[2][8] = ui->idg_20_w28->text().toFloat();

    is_sieve_40[3][1] = ui->idg_20_s31->text().toFloat();
    is_sieve_40[3][2] = ui->idg_20_s32->text().toFloat();
    is_sieve_40[3][3] = ui->idg_20_s33->text().toFloat();
    is_sieve_40[3][4] = ui->idg_20_s34->text().toFloat();
    is_sieve_40[3][5] = ui->idg_20_s35->text().toFloat();
    is_sieve_40[3][6] = ui->idg_20_s36->text().toFloat();
    is_sieve_40[3][7] = ui->idg_20_s36->text().toFloat();
    is_sieve_40[3][8] = ui->idg_20_s38->text().toFloat();

    weight_of_retained_40[3][1] = ui->idg_20_w31->text().toFloat();
    weight_of_retained_40[3][2] = ui->idg_20_w32->text().toFloat();
    weight_of_retained_40[3][3] = ui->idg_20_w33->text().toFloat();
    weight_of_retained_40[3][4] = ui->idg_20_w34->text().toFloat();
    weight_of_retained_40[3][5] = ui->idg_20_w35->text().toFloat();
    weight_of_retained_40[3][6] = ui->idg_20_w36->text().toFloat();
    weight_of_retained_40[3][7] = ui->idg_20_w37->text().toFloat();
    weight_of_retained_40[3][8] = ui->idg_20_w38->text().toFloat();

    total_weight[0] = ui->ind_20_w1->text().toFloat();
    total_weight[1] = ui->ind_20_w2->text().toFloat();
    total_weight[2] = ui->ind_20_w3->text().toFloat();

    for (int i = 1; i <= 3; i++)
    {
        cumsum = 0;
        for (int j = 1; j <= 8; j++)
        {
            cumsum += weight_of_retained_40[i][j];
            ind_cumulative[i][j] = cumsum;
            ind_cumulative_percent[i][j] = 100 * cumsum / total_weight[i - 1];
            ind_cum_pass[i][j] = 100 - ind_cumulative_percent[i][j];
        }
    }

    QJsonObject individual_gradation_20mm;

    individual_gradation_20mm["total_weight_1"] = total_weight[0];
    individual_gradation_20mm["total_weight_2"] = total_weight[1];
    individual_gradation_20mm["total_weight_3"] = total_weight[2];

    individual_gradation_20mm["is_sieve_s11"] = is_sieve_40[1][1];
    individual_gradation_20mm["is_sieve_s12"] = is_sieve_40[1][2];
    individual_gradation_20mm["is_sieve_s13"] = is_sieve_40[1][3];
    individual_gradation_20mm["is_sieve_s14"] = is_sieve_40[1][4];
    individual_gradation_20mm["is_sieve_s15"] = is_sieve_40[1][5];
    individual_gradation_20mm["is_sieve_s16"] = is_sieve_40[1][6];
    individual_gradation_20mm["is_sieve_s17"] = is_sieve_40[1][7];
    individual_gradation_20mm["is_sieve_s18"] = is_sieve_40[1][8];

    individual_gradation_20mm["weight_of_retained_w11"] = weight_of_retained_40[1][1];
    individual_gradation_20mm["weight_of_retained_w12"] = weight_of_retained_40[1][2];
    individual_gradation_20mm["weight_of_retained_w13"] = weight_of_retained_40[1][3];
    individual_gradation_20mm["weight_of_retained_w14"] = weight_of_retained_40[1][4];
    individual_gradation_20mm["weight_of_retained_w15"] = weight_of_retained_40[1][5];
    individual_gradation_20mm["weight_of_retained_w16"] = weight_of_retained_40[1][6];
    individual_gradation_20mm["weight_of_retained_w17"] = weight_of_retained_40[1][7];
    individual_gradation_20mm["weight_of_retained_w18"] = weight_of_retained_40[1][8];

    individual_gradation_20mm["is_sieve_s21"] = is_sieve_40[2][1];
    individual_gradation_20mm["is_sieve_s22"] = is_sieve_40[2][2];
    individual_gradation_20mm["is_sieve_s23"] = is_sieve_40[2][3];
    individual_gradation_20mm["is_sieve_s24"] = is_sieve_40[2][4];
    individual_gradation_20mm["is_sieve_s25"] = is_sieve_40[2][5];
    individual_gradation_20mm["is_sieve_s26"] = is_sieve_40[2][6];
    individual_gradation_20mm["is_sieve_s27"] = is_sieve_40[2][7];
    individual_gradation_20mm["is_sieve_s28"] = is_sieve_40[2][8];

    individual_gradation_20mm["weight_of_retained_w21"] = weight_of_retained_40[2][1];
    individual_gradation_20mm["weight_of_retained_w22"] = weight_of_retained_40[2][2];
    individual_gradation_20mm["weight_of_retained_w23"] = weight_of_retained_40[2][3];
    individual_gradation_20mm["weight_of_retained_w24"] = weight_of_retained_40[2][4];
    individual_gradation_20mm["weight_of_retained_w25"] = weight_of_retained_40[2][5];
    individual_gradation_20mm["weight_of_retained_w26"] = weight_of_retained_40[2][6];
    individual_gradation_20mm["weight_of_retained_w27"] = weight_of_retained_40[2][7];
    individual_gradation_20mm["weight_of_retained_w28"] = weight_of_retained_40[2][8];

    individual_gradation_20mm["is_sieve_s31"] = is_sieve_40[3][1];
    individual_gradation_20mm["is_sieve_s32"] = is_sieve_40[3][2];
    individual_gradation_20mm["is_sieve_s33"] = is_sieve_40[3][3];
    individual_gradation_20mm["is_sieve_s34"] = is_sieve_40[3][4];
    individual_gradation_20mm["is_sieve_s35"] = is_sieve_40[3][5];
    individual_gradation_20mm["is_sieve_s36"] = is_sieve_40[3][6];
    individual_gradation_20mm["is_sieve_s37"] = is_sieve_40[3][7];
    individual_gradation_20mm["is_sieve_s38"] = is_sieve_40[3][8];

    individual_gradation_20mm["weight_of_retained_w31"] = weight_of_retained_40[3][1];
    individual_gradation_20mm["weight_of_retained_w32"] = weight_of_retained_40[3][2];
    individual_gradation_20mm["weight_of_retained_w33"] = weight_of_retained_40[3][3];
    individual_gradation_20mm["weight_of_retained_w34"] = weight_of_retained_40[3][4];
    individual_gradation_20mm["weight_of_retained_w35"] = weight_of_retained_40[3][5];
    individual_gradation_20mm["weight_of_retained_w36"] = weight_of_retained_40[3][6];
    individual_gradation_20mm["weight_of_retained_w37"] = weight_of_retained_40[3][7];
    individual_gradation_20mm["weight_of_retained_w38"] = weight_of_retained_40[3][8];
    individual_gradation_20mm["proportion"] = ui->ind_20_7->text().toFloat();

    std::string base_cum = "cum_";
    std::string base_CUM = "CUM_";
    std::string base_pass = "pass_";
    std::string base_total = "total_weight_";

    for (int i = 1; i <= 3; i++)
    {
        for (int j = 1; j <= 8; j++)
        {
            std::string istr = std::to_string(i);
            std::string jstr = std::to_string(j);

            QString target_cum = QString::fromStdString(base_cum + istr + jstr);
            QString target_CUM = QString::fromStdString(base_CUM + istr + jstr);
            QString target_pass = QString::fromStdString(base_pass + istr + jstr);
            QString target_total = QString::fromStdString(base_total + istr);
            individual_gradation_20mm[target_cum] = ind_cumulative[i][j];
            individual_gradation_20mm[target_CUM] = ind_cumulative_percent[i][j];
            individual_gradation_20mm[target_pass] = ind_cum_pass[i][j];
        }
    }

    idg_json["20mm"] = individual_gradation_20mm;

    if (idg.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&idg);
        QJsonDocument jsonDoc_4(idg_json);
        out << jsonDoc_4.toJson();
        idg.close();
    }

    save_check();
}
void MainWindow::on_idg_save_10mm_clicked()
{
    tracked_files.push_back("ind");
    removeDuplicates(tracked_files);

    float cumsum = 0;

    is_sieve_40[1][1] = ui->idg_10_s11->text().toFloat();
    is_sieve_40[1][2] = ui->idg_10_s12->text().toFloat();
    is_sieve_40[1][3] = ui->idg_10_s13->text().toFloat();
    is_sieve_40[1][4] = ui->idg_10_s14->text().toFloat();
    is_sieve_40[1][5] = ui->idg_10_s15->text().toFloat();
    is_sieve_40[1][6] = ui->idg_10_s16->text().toFloat();
    is_sieve_40[1][7] = ui->idg_10_s17->text().toFloat();
    is_sieve_40[1][8] = ui->idg_10_s18->text().toFloat();

    weight_of_retained_40[1][1] = ui->idg_10_w11->text().toFloat();
    weight_of_retained_40[1][2] = ui->idg_10_w12->text().toFloat();
    weight_of_retained_40[1][3] = ui->idg_10_w13->text().toFloat();
    weight_of_retained_40[1][4] = ui->idg_10_w14->text().toFloat();
    weight_of_retained_40[1][5] = ui->idg_10_w15->text().toFloat();
    weight_of_retained_40[1][6] = ui->idg_10_w16->text().toFloat();
    weight_of_retained_40[1][7] = ui->idg_10_w17->text().toFloat();
    weight_of_retained_40[1][8] = ui->idg_10_w18->text().toFloat();

    is_sieve_40[2][1] = ui->idg_10_s21->text().toFloat();
    is_sieve_40[2][2] = ui->idg_10_s22->text().toFloat();
    is_sieve_40[2][3] = ui->idg_10_s23->text().toFloat();
    is_sieve_40[2][4] = ui->idg_10_s24->text().toFloat();
    is_sieve_40[2][5] = ui->idg_10_s25->text().toFloat();
    is_sieve_40[2][6] = ui->idg_10_s26->text().toFloat();
    is_sieve_40[2][7] = ui->idg_10_s27->text().toFloat();
    is_sieve_40[2][8] = ui->idg_10_s28->text().toFloat();

    weight_of_retained_40[2][1] = ui->idg_10_w21->text().toFloat();
    weight_of_retained_40[2][2] = ui->idg_10_w22->text().toFloat();
    weight_of_retained_40[2][3] = ui->idg_10_w23->text().toFloat();
    weight_of_retained_40[2][4] = ui->idg_10_w24->text().toFloat();
    weight_of_retained_40[2][5] = ui->idg_10_w25->text().toFloat();
    weight_of_retained_40[2][6] = ui->idg_10_w26->text().toFloat();
    weight_of_retained_40[2][7] = ui->idg_10_w27->text().toFloat();
    weight_of_retained_40[2][8] = ui->idg_10_w28->text().toFloat();

    is_sieve_40[3][1] = ui->idg_10_s31->text().toFloat();
    is_sieve_40[3][2] = ui->idg_10_s32->text().toFloat();
    is_sieve_40[3][3] = ui->idg_10_s33->text().toFloat();
    is_sieve_40[3][4] = ui->idg_10_s34->text().toFloat();
    is_sieve_40[3][5] = ui->idg_10_s35->text().toFloat();
    is_sieve_40[3][6] = ui->idg_10_s36->text().toFloat();
    is_sieve_40[3][7] = ui->idg_10_s36->text().toFloat();
    is_sieve_40[3][8] = ui->idg_10_s38->text().toFloat();

    weight_of_retained_40[3][1] = ui->idg_10_w31->text().toFloat();
    weight_of_retained_40[3][2] = ui->idg_10_w32->text().toFloat();
    weight_of_retained_40[3][3] = ui->idg_10_w33->text().toFloat();
    weight_of_retained_40[3][4] = ui->idg_10_w34->text().toFloat();
    weight_of_retained_40[3][5] = ui->idg_10_w35->text().toFloat();
    weight_of_retained_40[3][6] = ui->idg_10_w36->text().toFloat();
    weight_of_retained_40[3][7] = ui->idg_10_w37->text().toFloat();
    weight_of_retained_40[3][8] = ui->idg_10_w38->text().toFloat();

    total_weight[0] = ui->ind_10_w1->text().toFloat();
    total_weight[1] = ui->ind_10_w2->text().toFloat();
    total_weight[2] = ui->ind_10_w3->text().toFloat();

    for (int i = 1; i <= 3; i++)
    {
        cumsum = 0;
        for (int j = 1; j <= 8; j++)
        {
            cumsum += weight_of_retained_40[i][j];
            ind_cumulative[i][j] = cumsum;
            ind_cumulative_percent[i][j] = 100 * cumsum / total_weight[i - 1];
            ind_cum_pass[i][j] = 100 - ind_cumulative_percent[i][j];
        }
    }

    QJsonObject individual_gradation_10mm;

    individual_gradation_10mm["total_weight_1"] = total_weight[0];
    individual_gradation_10mm["total_weight_2"] = total_weight[1];
    individual_gradation_10mm["total_weight_3"] = total_weight[2];

    individual_gradation_10mm["is_sieve_s11"] = is_sieve_40[1][1];
    individual_gradation_10mm["is_sieve_s12"] = is_sieve_40[1][2];
    individual_gradation_10mm["is_sieve_s13"] = is_sieve_40[1][3];
    individual_gradation_10mm["is_sieve_s14"] = is_sieve_40[1][4];
    individual_gradation_10mm["is_sieve_s15"] = is_sieve_40[1][5];
    individual_gradation_10mm["is_sieve_s16"] = is_sieve_40[1][6];
    individual_gradation_10mm["is_sieve_s17"] = is_sieve_40[1][7];
    individual_gradation_10mm["is_sieve_s18"] = is_sieve_40[1][8];

    individual_gradation_10mm["weight_of_retained_w11"] = weight_of_retained_40[1][1];
    individual_gradation_10mm["weight_of_retained_w12"] = weight_of_retained_40[1][2];
    individual_gradation_10mm["weight_of_retained_w13"] = weight_of_retained_40[1][3];
    individual_gradation_10mm["weight_of_retained_w14"] = weight_of_retained_40[1][4];
    individual_gradation_10mm["weight_of_retained_w15"] = weight_of_retained_40[1][5];
    individual_gradation_10mm["weight_of_retained_w16"] = weight_of_retained_40[1][6];
    individual_gradation_10mm["weight_of_retained_w17"] = weight_of_retained_40[1][7];
    individual_gradation_10mm["weight_of_retained_w18"] = weight_of_retained_40[1][8];

    individual_gradation_10mm["is_sieve_s21"] = is_sieve_40[2][1];
    individual_gradation_10mm["is_sieve_s22"] = is_sieve_40[2][2];
    individual_gradation_10mm["is_sieve_s23"] = is_sieve_40[2][3];
    individual_gradation_10mm["is_sieve_s24"] = is_sieve_40[2][4];
    individual_gradation_10mm["is_sieve_s25"] = is_sieve_40[2][5];
    individual_gradation_10mm["is_sieve_s26"] = is_sieve_40[2][6];
    individual_gradation_10mm["is_sieve_s27"] = is_sieve_40[2][7];
    individual_gradation_10mm["is_sieve_s28"] = is_sieve_40[2][8];

    individual_gradation_10mm["weight_of_retained_w21"] = weight_of_retained_40[2][1];
    individual_gradation_10mm["weight_of_retained_w22"] = weight_of_retained_40[2][2];
    individual_gradation_10mm["weight_of_retained_w23"] = weight_of_retained_40[2][3];
    individual_gradation_10mm["weight_of_retained_w24"] = weight_of_retained_40[2][4];
    individual_gradation_10mm["weight_of_retained_w25"] = weight_of_retained_40[2][5];
    individual_gradation_10mm["weight_of_retained_w26"] = weight_of_retained_40[2][6];
    individual_gradation_10mm["weight_of_retained_w27"] = weight_of_retained_40[2][7];
    individual_gradation_10mm["weight_of_retained_w28"] = weight_of_retained_40[2][8];

    individual_gradation_10mm["is_sieve_s31"] = is_sieve_40[3][1];
    individual_gradation_10mm["is_sieve_s32"] = is_sieve_40[3][2];
    individual_gradation_10mm["is_sieve_s33"] = is_sieve_40[3][3];
    individual_gradation_10mm["is_sieve_s34"] = is_sieve_40[3][4];
    individual_gradation_10mm["is_sieve_s35"] = is_sieve_40[3][5];
    individual_gradation_10mm["is_sieve_s36"] = is_sieve_40[3][6];
    individual_gradation_10mm["is_sieve_s37"] = is_sieve_40[3][7];
    individual_gradation_10mm["is_sieve_s38"] = is_sieve_40[3][8];

    individual_gradation_10mm["weight_of_retained_w31"] = weight_of_retained_40[3][1];
    individual_gradation_10mm["weight_of_retained_w32"] = weight_of_retained_40[3][2];
    individual_gradation_10mm["weight_of_retained_w33"] = weight_of_retained_40[3][3];
    individual_gradation_10mm["weight_of_retained_w34"] = weight_of_retained_40[3][4];
    individual_gradation_10mm["weight_of_retained_w35"] = weight_of_retained_40[3][5];
    individual_gradation_10mm["weight_of_retained_w36"] = weight_of_retained_40[3][6];
    individual_gradation_10mm["weight_of_retained_w37"] = weight_of_retained_40[3][7];
    individual_gradation_10mm["weight_of_retained_w38"] = weight_of_retained_40[3][8];

    individual_gradation_10mm["proportion"] = ui->ind_10_7->text().toFloat();

    std::string base_cum = "cum_";
    std::string base_CUM = "CUM_";
    std::string base_pass = "pass_";
    std::string base_total = "total_weight_";

    for (int i = 1; i <= 3; i++)
    {
        for (int j = 1; j <= 8; j++)
        {
            std::string istr = std::to_string(i);
            std::string jstr = std::to_string(j);

            QString target_cum = QString::fromStdString(base_cum + istr + jstr);
            QString target_CUM = QString::fromStdString(base_CUM + istr + jstr);
            QString target_pass = QString::fromStdString(base_pass + istr + jstr);
            QString target_total = QString::fromStdString(base_total + istr);

            individual_gradation_10mm[target_cum] = ind_cumulative[i][j];
            individual_gradation_10mm[target_CUM] = ind_cumulative_percent[i][j];
            individual_gradation_10mm[target_pass] = ind_cum_pass[i][j];
        }
    }

    idg_json["10mm"] = individual_gradation_10mm;

    if (idg.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&idg);
        QJsonDocument jsonDoc_4(idg_json);
        out << jsonDoc_4.toJson();
        idg.close();
    }

    save_check();
}
void MainWindow::on_idg_save_d_clicked()
{

    tracked_files.push_back("ind");
    removeDuplicates(tracked_files);

    float cumsum = 0;

    is_sieve_40[1][1] = ui->idg_d_s11->text().toFloat();
    is_sieve_40[1][2] = ui->idg_d_s12->text().toFloat();
    is_sieve_40[1][3] = ui->idg_d_s13->text().toFloat();
    is_sieve_40[1][4] = ui->idg_d_s14->text().toFloat();
    is_sieve_40[1][5] = ui->idg_d_s15->text().toFloat();
    is_sieve_40[1][6] = ui->idg_d_s16->text().toFloat();
    is_sieve_40[1][7] = ui->idg_d_s17->text().toFloat();
    is_sieve_40[1][8] = ui->idg_d_s18->text().toFloat();

    weight_of_retained_40[1][1] = ui->idg_d_w11->text().toFloat();
    weight_of_retained_40[1][2] = ui->idg_d_w12->text().toFloat();
    weight_of_retained_40[1][3] = ui->idg_d_w13->text().toFloat();
    weight_of_retained_40[1][4] = ui->idg_d_w14->text().toFloat();
    weight_of_retained_40[1][5] = ui->idg_d_w15->text().toFloat();
    weight_of_retained_40[1][6] = ui->idg_d_w16->text().toFloat();
    weight_of_retained_40[1][7] = ui->idg_d_w17->text().toFloat();
    weight_of_retained_40[1][8] = ui->idg_d_w18->text().toFloat();

    is_sieve_40[2][1] = ui->idg_d_s21->text().toFloat();
    is_sieve_40[2][2] = ui->idg_d_s22->text().toFloat();
    is_sieve_40[2][3] = ui->idg_d_s23->text().toFloat();
    is_sieve_40[2][4] = ui->idg_d_s24->text().toFloat();
    is_sieve_40[2][5] = ui->idg_d_s25->text().toFloat();
    is_sieve_40[2][6] = ui->idg_d_s26->text().toFloat();
    is_sieve_40[2][7] = ui->idg_d_s27->text().toFloat();
    is_sieve_40[2][8] = ui->idg_d_s28->text().toFloat();

    weight_of_retained_40[2][1] = ui->idg_d_w21->text().toFloat();
    weight_of_retained_40[2][2] = ui->idg_d_w22->text().toFloat();
    weight_of_retained_40[2][3] = ui->idg_d_w23->text().toFloat();
    weight_of_retained_40[2][4] = ui->idg_d_w24->text().toFloat();
    weight_of_retained_40[2][5] = ui->idg_d_w25->text().toFloat();
    weight_of_retained_40[2][6] = ui->idg_d_w26->text().toFloat();
    weight_of_retained_40[2][7] = ui->idg_d_w27->text().toFloat();
    weight_of_retained_40[2][8] = ui->idg_d_w28->text().toFloat();

    is_sieve_40[3][1] = ui->idg_d_s31->text().toFloat();
    is_sieve_40[3][2] = ui->idg_d_s32->text().toFloat();
    is_sieve_40[3][3] = ui->idg_d_s33->text().toFloat();
    is_sieve_40[3][4] = ui->idg_d_s34->text().toFloat();
    is_sieve_40[3][5] = ui->idg_d_s35->text().toFloat();
    is_sieve_40[3][6] = ui->idg_d_s36->text().toFloat();
    is_sieve_40[3][7] = ui->idg_d_s36->text().toFloat();
    is_sieve_40[3][8] = ui->idg_d_s38->text().toFloat();

    weight_of_retained_40[3][1] = ui->idg_d_w31->text().toFloat();
    weight_of_retained_40[3][2] = ui->idg_d_w32->text().toFloat();
    weight_of_retained_40[3][3] = ui->idg_d_w33->text().toFloat();
    weight_of_retained_40[3][4] = ui->idg_d_w34->text().toFloat();
    weight_of_retained_40[3][5] = ui->idg_d_w35->text().toFloat();
    weight_of_retained_40[3][6] = ui->idg_d_w36->text().toFloat();
    weight_of_retained_40[3][7] = ui->idg_d_w37->text().toFloat();
    weight_of_retained_40[3][8] = ui->idg_d_w38->text().toFloat();

    total_weight[0] = ui->ind_0_w1->text().toFloat();
    total_weight[1] = ui->ind_0_w2->text().toFloat();
    total_weight[2] = ui->ind_0_w3->text().toFloat();

    for (int i = 1; i <= 3; i++)
    {
        cumsum = 0;
        for (int j = 1; j <= 8; j++)
        {
            cumsum += weight_of_retained_40[i][j];
            ind_cumulative[i][j] = cumsum;
            ind_cumulative_percent[i][j] = 100 * cumsum / total_weight[i - 1];
            ind_cum_pass[i][j] = 100 - ind_cumulative_percent[i][j];
        }
    }

    QJsonObject individual_gradation_d;

    individual_gradation_d["total_weight_1"] = total_weight[0];
    individual_gradation_d["total_weight_2"] = total_weight[1];
    individual_gradation_d["total_weight_3"] = total_weight[2];

    individual_gradation_d["is_sieve_s11"] = is_sieve_40[1][1];
    individual_gradation_d["is_sieve_s12"] = is_sieve_40[1][2];
    individual_gradation_d["is_sieve_s13"] = is_sieve_40[1][3];
    individual_gradation_d["is_sieve_s14"] = is_sieve_40[1][4];
    individual_gradation_d["is_sieve_s15"] = is_sieve_40[1][5];
    individual_gradation_d["is_sieve_s16"] = is_sieve_40[1][6];
    individual_gradation_d["is_sieve_s17"] = is_sieve_40[1][7];
    individual_gradation_d["is_sieve_s18"] = is_sieve_40[1][8];

    individual_gradation_d["weight_of_retained_w11"] = weight_of_retained_40[1][1];
    individual_gradation_d["weight_of_retained_w12"] = weight_of_retained_40[1][2];
    individual_gradation_d["weight_of_retained_w13"] = weight_of_retained_40[1][3];
    individual_gradation_d["weight_of_retained_w14"] = weight_of_retained_40[1][4];
    individual_gradation_d["weight_of_retained_w15"] = weight_of_retained_40[1][5];
    individual_gradation_d["weight_of_retained_w16"] = weight_of_retained_40[1][6];
    individual_gradation_d["weight_of_retained_w17"] = weight_of_retained_40[1][7];
    individual_gradation_d["weight_of_retained_w18"] = weight_of_retained_40[1][8];

    individual_gradation_d["is_sieve_s21"] = is_sieve_40[2][1];
    individual_gradation_d["is_sieve_s22"] = is_sieve_40[2][2];
    individual_gradation_d["is_sieve_s23"] = is_sieve_40[2][3];
    individual_gradation_d["is_sieve_s24"] = is_sieve_40[2][4];
    individual_gradation_d["is_sieve_s25"] = is_sieve_40[2][5];
    individual_gradation_d["is_sieve_s26"] = is_sieve_40[2][6];
    individual_gradation_d["is_sieve_s27"] = is_sieve_40[2][7];
    individual_gradation_d["is_sieve_s28"] = is_sieve_40[2][8];

    individual_gradation_d["weight_of_retained_w21"] = weight_of_retained_40[2][1];
    individual_gradation_d["weight_of_retained_w22"] = weight_of_retained_40[2][2];
    individual_gradation_d["weight_of_retained_w23"] = weight_of_retained_40[2][3];
    individual_gradation_d["weight_of_retained_w24"] = weight_of_retained_40[2][4];
    individual_gradation_d["weight_of_retained_w25"] = weight_of_retained_40[2][5];
    individual_gradation_d["weight_of_retained_w26"] = weight_of_retained_40[2][6];
    individual_gradation_d["weight_of_retained_w27"] = weight_of_retained_40[2][7];
    individual_gradation_d["weight_of_retained_w28"] = weight_of_retained_40[2][8];

    individual_gradation_d["is_sieve_s31"] = is_sieve_40[3][1];
    individual_gradation_d["is_sieve_s32"] = is_sieve_40[3][2];
    individual_gradation_d["is_sieve_s33"] = is_sieve_40[3][3];
    individual_gradation_d["is_sieve_s34"] = is_sieve_40[3][4];
    individual_gradation_d["is_sieve_s35"] = is_sieve_40[3][5];
    individual_gradation_d["is_sieve_s36"] = is_sieve_40[3][6];
    individual_gradation_d["is_sieve_s37"] = is_sieve_40[3][7];
    individual_gradation_d["is_sieve_s38"] = is_sieve_40[3][8];

    individual_gradation_d["weight_of_retained_w31"] = weight_of_retained_40[3][1];
    individual_gradation_d["weight_of_retained_w32"] = weight_of_retained_40[3][2];
    individual_gradation_d["weight_of_retained_w33"] = weight_of_retained_40[3][3];
    individual_gradation_d["weight_of_retained_w34"] = weight_of_retained_40[3][4];
    individual_gradation_d["weight_of_retained_w35"] = weight_of_retained_40[3][5];
    individual_gradation_d["weight_of_retained_w36"] = weight_of_retained_40[3][6];
    individual_gradation_d["weight_of_retained_w37"] = weight_of_retained_40[3][7];
    individual_gradation_d["weight_of_retained_w38"] = weight_of_retained_40[3][8];

    individual_gradation_d["proportion"] = ui->ind_0_7->text().toFloat();

    std::string base_cum = "cum_";
    std::string base_CUM = "CUM_";
    std::string base_pass = "pass_";
    std::string base_total = "total_weight_";

    for (int i = 1; i <= 3; i++)
    {
        for (int j = 1; j <= 8; j++)
        {
            std::string istr = std::to_string(i);
            std::string jstr = std::to_string(j);

            QString target_cum = QString::fromStdString(base_cum + istr + jstr);
            QString target_CUM = QString::fromStdString(base_CUM + istr + jstr);
            QString target_pass = QString::fromStdString(base_pass + istr + jstr);
            QString target_total = QString::fromStdString(base_total + istr);

            individual_gradation_d[target_cum] = ind_cumulative[i][j];
            individual_gradation_d[target_CUM] = ind_cumulative_percent[i][j];
            individual_gradation_d[target_pass] = ind_cum_pass[i][j];
        }
    }

    idg_json["d"] = individual_gradation_d;

    if (idg.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&idg);
        QJsonDocument jsonDoc_4(idg_json);
        out << jsonDoc_4.toJson();
        idg.close();
    }

    save_check();
}
void MainWindow::on_cd_save_clicked()
{
    tracked_files.push_back("ind");
    removeDuplicates(tracked_files);

    float cumsum = 0;
    is_sieve_40[1][1] = ui->cg_d_is_1->text().toFloat();
    is_sieve_40[1][2] = ui->cg_d_is_2->text().toFloat();
    is_sieve_40[1][3] = ui->cg_d_is_3->text().toFloat();
    is_sieve_40[1][4] = ui->cg_d_is_4->text().toFloat();
    is_sieve_40[1][5] = ui->cg_d_is_5->text().toFloat();
    is_sieve_40[1][6] = ui->cg_d_is_6->text().toFloat();
    is_sieve_40[1][7] = ui->cg_d_is_7->text().toFloat();
    is_sieve_40[1][8] = ui->cg_d_is_8->text().toFloat();

    weight_of_retained_40[2][1] = ui->cg_d_wr_1->text().toFloat();
    weight_of_retained_40[2][2] = ui->cg_d_wr_2->text().toFloat();
    weight_of_retained_40[2][3] = ui->cg_d_wr_3->text().toFloat();
    weight_of_retained_40[2][4] = ui->cg_d_wr_4->text().toFloat();
    weight_of_retained_40[2][5] = ui->cg_d_wr_5->text().toFloat();
    weight_of_retained_40[2][6] = ui->cg_d_wr_6->text().toFloat();
    weight_of_retained_40[2][7] = ui->cg_d_wr_7->text().toFloat();
    weight_of_retained_40[2][8] = ui->cg_d_wr_8->text().toFloat();

    total_weight[0] = ui->cd_d_w_1->text().toFloat();

    cumsum = 0;
    for (int j = 1; j <= 8; j++)
    {
        cumsum += weight_of_retained_40[2][j];
        ind_cumulative[0][j] = cumsum;
        ind_cumulative_percent[0][j] = 100 * cumsum / total_weight[0];
        ind_cum_pass[0][j] = 100 - ind_cumulative_percent[0][j];
    }

    QJsonObject combined_gradation_r;

    combined_gradation_r["total_weight_1"] = total_weight[0];

    combined_gradation_r["is_sieve_s1"] = is_sieve_40[1][1];
    combined_gradation_r["is_sieve_s2"] = is_sieve_40[1][2];
    combined_gradation_r["is_sieve_s3"] = is_sieve_40[1][3];
    combined_gradation_r["is_sieve_s4"] = is_sieve_40[1][4];
    combined_gradation_r["is_sieve_s5"] = is_sieve_40[1][5];
    combined_gradation_r["is_sieve_s6"] = is_sieve_40[1][6];
    combined_gradation_r["is_sieve_s7"] = is_sieve_40[1][7];
    combined_gradation_r["is_sieve_s8"] = is_sieve_40[1][8];

    combined_gradation_r["weight_of_retained_w1"] = weight_of_retained_40[2][1];
    combined_gradation_r["weight_of_retained_w2"] = weight_of_retained_40[2][2];
    combined_gradation_r["weight_of_retained_w3"] = weight_of_retained_40[2][3];
    combined_gradation_r["weight_of_retained_w4"] = weight_of_retained_40[2][4];
    combined_gradation_r["weight_of_retained_w5"] = weight_of_retained_40[2][5];
    combined_gradation_r["weight_of_retained_w6"] = weight_of_retained_40[2][6];
    combined_gradation_r["weight_of_retained_w7"] = weight_of_retained_40[2][7];
    combined_gradation_r["weight_of_retained_w8"] = weight_of_retained_40[2][8];

    std::string base_cum = "cum_";
    std::string base_CUM = "CUM_";
    std::string base_pass = "pass_";
    std::string base_total = "total_weight_";

    for (int j = 1; j <= 8; j++)
    {

        std::string jstr = std::to_string(j);

        QString target_cum = QString::fromStdString(base_cum + jstr);
        QString target_CUM = QString::fromStdString(base_CUM + jstr);
        QString target_pass = QString::fromStdString(base_pass + jstr);
        QString target_total = QString::fromStdString(base_total);

        combined_gradation_r[target_cum] = ind_cumulative[0][j];
        combined_gradation_r[target_CUM] = ind_cumulative_percent[0][j];
        combined_gradation_r[target_pass] = ind_cum_pass[0][j];
    }

    idg_json["cg"] = combined_gradation_r;

    if (idg.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&idg);
        QJsonDocument jsonDoc_4(idg_json);
        out << jsonDoc_4.toJson();
        idg.close();
    }

    save_check();
}
QJsonObject MainWindow::soft_eval(QJsonObject soft_in)
{
    qDebug() << "beggining soft eval";
    qDebug() << soft_in;
    QString constructor = "rheology_soft_%1";
    qDebug() << constructor;
    QString obj_name;

    double sum = 0;
    for (int i = 4; i < 8; i++)
    {
        obj_name = constructor.arg(i);

        sum += soft_in[obj_name].toDouble();
        qDebug() << obj_name << sum;
    }

    soft_in["rheology_soft_8"] = sum / 4;

    ui->rheology_soft_mean->setText(QString::number(sum/4, 'g', 4));

    return soft_in;
}
QJsonObject MainWindow::strip_eval(QJsonObject strip_in)
{
    return strip_in;
}
QJsonObject MainWindow::pen_eval(QJsonObject pen_in)
{
    QString constructor = "rheology_pen_%1";
    QString init_name, fin_name, pen_key;

    double sum = 0;
    for (int i = 6; i <= 10; i += 2)
    {
        init_name = constructor.arg(i);
        fin_name = constructor.arg(i + 1);
        double penetration = pen_in[fin_name].toDouble() - pen_in[init_name].toDouble();

        int pen_index = 9 + (i / 2);
        pen_key = constructor.arg(pen_index);
        pen_in[pen_key] = penetration;
        sum += penetration;
    }

    pen_in["rheology_pen_15"] = sum / 3;

    ui->rh_pen_val1->setText(QString::number(pen_in["rheology_pen_12"].toDouble(), 'g', 4));
    ui->rh_pen_val2->setText(QString::number(pen_in["rheology_pen_13"].toDouble(), 'g', 4));
    ui->rh_pen_val3->setText(QString::number(pen_in["rheology_pen_14"].toDouble(), 'g', 4));
    ui->rheology_pen_mean->setText(QString::number(pen_in["rheology_pen_15"].toDouble(), 'g', 4));

    return pen_in;
}
QJsonObject MainWindow::ductility_eval(QJsonObject ductility_in)
{
    QString constructor = "rheology_ductility_%1";
    QString obj_name;

    double sum = 0;
    for (int i = 7; i < 10; i++)
    {
        obj_name = constructor.arg(i);

        sum += ductility_in[obj_name].toDouble();
    }

    ductility_in["rheology_ductility_10"] = sum / 3;
    ui->rheology_ductility_mean->setText(QString::number(ductility_in["rheology_ductility_10"].toDouble(), 'g', 4));

    return ductility_in;
}
QJsonObject MainWindow::flash_eval(QJsonObject flash_in)
{
    flash_in["rheology_flash_4"] = (flash_in["rheology_flash_1"].toDouble() + flash_in["rheology_flash_2"].toDouble() + flash_in["rheology_flash_3"].toDouble()) / 3;

    ui->rheology_flash_mean->setText(QString::number(flash_in["rheology_flash_4"].toDouble(), 'g', 4));
    return flash_in;
}
QJsonObject MainWindow::viscosity_eval(QJsonObject viscosity_in)
{
    viscosity_in["rheology_viscosity_11"] = (viscosity_in["rheology_viscosity_7"].toDouble() * viscosity_in["rheology_viscosity_9"].toDouble());
    viscosity_in["rheology_viscosity_12"] = (viscosity_in["rheology_viscosity_8"].toDouble() * viscosity_in["rheology_viscosity_10"].toDouble());

    return viscosity_in;
}
QJsonObject MainWindow::spc_eval(QJsonObject spc_in)
{
    QString constructor = "rheology_spc_%1_%2";
    QString obj_name;

    double a, b, c, d, spc, sum;
    for (int i = 1; i <= 3; i++)
    {
        a = spc_in[constructor.arg(1).arg(i)].toDouble();
        b = spc_in[constructor.arg(2).arg(i)].toDouble();
        c = spc_in[constructor.arg(3).arg(i)].toDouble();
        d = spc_in[constructor.arg(4).arg(i)].toDouble();

        spc = (c - a) / (b + c - a - d);
        sum += spc;

        obj_name = QString::fromStdString("rheology_spc_%1").arg(i);

        spc_in[obj_name] = spc;
        QLabel* label = ui->rheology->findChild<QLabel*>(obj_name);
        label->setText(QString::number(spc, 'g', 4));
    }

    spc_in["rheology_spc_mean"] = sum / 3;
    ui->rheology_spc_mean->setText(QString::number(sum/3, 'g', 4));

    return spc_in;
}



// Deals with Graphing
std::vector<double> quadfit(std::vector<double> &x, std::vector<double> &y)
{

    double x1, x2, x3, y1, y2, y3; // The coordinates of the three points
    double a, b, c;                // The coefficients of the quadratic equation
    std::vector<double> result;    // The vector to store the coefficients

    int max_index = std::max_element(y.begin(), y.end()) - y.begin();
    int min_index = std::min_element(y.begin(), y.end()) - y.begin();
    int med_index = -1;
    for (int i = 0; i < y.size(); i++)
    {
        if (i != max_index && i != min_index)
        {
            med_index = i;
            break;
        }
    }

    x1 = x[max_index];
    y1 = y[max_index];
    x2 = x[min_index];
    y2 = y[min_index];
    x3 = x[med_index];
    y3 = y[med_index];

    // a*x1^2 + b*x1 + c = y1
    // a*x2^2 + b*x2 + c = y2
    // a*x3^2 + b*x3 + c = y3
    a = ((y3 - y2) * (x1 - x2) - (y1 - y2) * (x3 - x2)) / ((x3 * x3 - x2 * x2) * (x1 - x2) - (x1 * x1 - x2 * x2) * (x3 - x2));
    b = ((y1 - y2) - a * (x1 * x1 - x2 * x2)) / (x1 - x2);
    c = y1 - a * x1 * x1 - b * x1;

    result.push_back(a);
    result.push_back(b);
    result.push_back(c);

    return result;
}
std::vector<double> quadval(std::vector<double> &x, std::vector<double> &coeff)
{

    double a, b, c;
    int m = x.size();
    std::vector<double> y;

    a = coeff[0];
    b = coeff[1];
    c = coeff[2];

    for (int i = 0; i < m; i++)
    {
        y.push_back(a * x[i] * x[i] + b * x[i] + c); // Store the y value
    }

    return y;
}
double lagrangeInterpolation(const std::vector<std::pair<double, double>>& points, double x) {
    double result = 0; // Initialize result

    // Compute the result using Lagrange Interpolation formula
    for (int i = 0; i < points.size(); i++) {
        double term = points[i].second;
        for (int j = 0; j < points.size(); j++) {
            if (j != i) {
                term = term * (x - points[j].first) / double(points[i].first - points[j].first);
            }
        }

        // Add current term to result
        result += term;
    }

    return result;
}
std::vector<std::pair<double, double>> curvefit(const std::vector<std::pair<double, double>> &points, double start, double end, double accuracy = 100) {
    double step = (end - start)/accuracy;

    std::vector<std::pair<double, double>> curve;
    for (double d = start; d < end; d += step) {
        curve.push_back({d, lagrangeInterpolation(points, d)});
    }

    return curve;
}

void MainWindow::on_ind_graph_update_clicked()
{
    MainWindow::updateGraph_idg();
}
void MainWindow::updateGraph_idg()
{
    // Graph 1 is the combined gradation graph
    QString json_path = cwd.filePath("json/idg.json");
    QFile ind(json_path);
    QString file_savepath;

    if (!ind.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "json file not opened";
        return;
    }
    else
    {
        qDebug() << "json file opened";
    }
    QByteArray ind_json_vals_bytearray = ind.readAll();
    QJsonDocument ind_json_doc = QJsonDocument::fromJson(ind_json_vals_bytearray);
    QJsonObject ind_json_lookups = ind_json_doc.object();

    QJsonObject cmb_json = ind_json_lookups["cg"].toObject(); // QJsonObject 10mm = cmb_json["10mm"].toObject();
    double mid_[] = {100.00, 97.50, 70.00, 50.00, 32.50, 22.50, 15.00, 2.50};
    double low_[] = {100.0, 95.0, 60.0, 40.0, 25.0, 15.0, 8.0, 0.0};
    double high_[] = {100.0, 100.0, 80.0, 60.0, 40.0, 30.0, 22.0, 5.0};

    QVector<double> is_sieve, passing, high, mid, low;
    for (int i = 1; i <= 8; i++)
    {
        std::string sieve_iterator = "is_sieve_s", pass_iterator = "pass_";
        char I = i + 48;
        sieve_iterator += I;
        pass_iterator += I;
        QString qsieve_iterator = QString::fromStdString(sieve_iterator), qpass_iterator = QString::fromStdString(pass_iterator);
        is_sieve << cmb_json[qsieve_iterator].toDouble();
        passing << cmb_json[qpass_iterator].toDouble();
        high << high_[i - 1];
        low << low_[i - 1];
        mid << mid_[i - 1];
    }

    ui->ind_graph_1->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    ui->ind_graph_1->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    ui->ind_graph_1->legend->setFont(legendFont);
    ui->ind_graph_1->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    ui->ind_graph_1->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignRight);

    ui->ind_graph_1->graph(0)->setData(is_sieve, passing);
    ui->ind_graph_1->graph(0)->setName("Passing of %");
    ui->ind_graph_1->graph(1)->setData(is_sieve, low);
    ui->ind_graph_1->graph(1)->setName("Lower Limit");
    ui->ind_graph_1->graph(2)->setData(is_sieve, mid);
    ui->ind_graph_1->graph(2)->setName("Middle Limit");
    ui->ind_graph_1->graph(3)->setData(is_sieve, high);
    ui->ind_graph_1->graph(3)->setName("Upper Limit");

    QPen redPen(Qt::red, 2);

    ui->ind_graph_1->graph(3)->setChannelFillGraph(ui->ind_graph_1->graph(1));
    ui->ind_graph_1->graph(3)->setBrush(QBrush(QColor(0, 255, 0, 20))); // light green 20% transparent
    ui->ind_graph_1->graph(0)->setPen(redPen);
    ui->ind_graph_1->graph(1)->setPen(QPen(QColor(102, 153, 130)));
    ui->ind_graph_1->graph(2)->setPen(QPen(QColor(170, 101, 0, 200)));
    ui->ind_graph_1->graph(3)->setPen(QPen(QColor(140, 102, 169)));

    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
    ui->ind_graph_1->xAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->ind_graph_1->xAxis->setTicker(logTicker);

    ui->ind_graph_1->xAxis->setLabel("IS SIEVE IN MM");
    ui->ind_graph_1->xAxis->setRange(0.05, 100);
    ui->ind_graph_1->yAxis->setLabel("PASSING OF %");
    ui->ind_graph_1->yAxis->setRange(0, 100);
    ui->ind_graph_1->replot();

    file_savepath = cwd.filePath("html/combined_graph.png");
    QFile graph_file_cmb(file_savepath);
    if (!graph_file_cmb.open(QIODevice::WriteOnly))
    {
        qDebug() << graph_file_cmb.errorString();
    }
    else
    {
        ui->ind_graph_1->savePng(file_savepath);
    }

    // Now we update the Blending Graph
    // First step is getting the values from the individual gradation json file and blending the values together in proportion
    QJsonObject idg40 = ind_json_lookups["40mm"].toObject(), idg20 = ind_json_lookups["20mm"].toObject(), idg10 = ind_json_lookups["10mm"].toObject(), idg0 = ind_json_lookups["d"].toObject();
    qDebug() << idg40;
    QVector<double> blend_val;
    for (int i = 1; i <= 8; i++)
    {
        double components[4];

        // char I = i+48;
        std::string it1 = "pass_1" + std::to_string(i), it2 = "pass_2" + std::to_string(i), it3 = "pass_3" + std::to_string(i);
        qDebug() << it1 << it2 << it3;
        QString qit1 = QString::fromStdString(it1), qit2 = QString::fromStdString(it2), qit3 = QString::fromStdString(it3);
        components[0] = (idg40[qit1].toDouble() + idg40[qit2].toDouble() + idg40[qit3].toDouble()) * 0.33;
        components[1] = (idg20[qit1].toDouble() + idg20[qit2].toDouble() + idg20[qit3].toDouble()) * 0.33;
        components[2] = (idg10[qit1].toDouble() + idg10[qit2].toDouble() + idg10[qit3].toDouble()) * 0.33;
        components[3] = (idg0[qit1].toDouble() + idg0[qit2].toDouble() + idg0[qit3].toDouble()) * 0.33;

        qDebug() << components[0] << components[1] << components[2] << components[3];

        components[0] *= idg40["proportion"].toDouble() / 100;
        components[1] *= idg20["proportion"].toDouble() / 100;
        components[2] *= idg10["proportion"].toDouble() / 100;
        components[3] *= idg0["proportion"].toDouble() / 100;

        double sum = 0;
        for (int j = 0; j < 4; j++)
        {
            sum += components[j];
        }

        blend_val << sum;
    }

    ui->ind_graph_2->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    ui->ind_graph_2->legend->setVisible(true);
    ui->ind_graph_2->legend->setFont(legendFont);
    ui->ind_graph_2->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    ui->ind_graph_2->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignRight);

    ui->ind_graph_2->graph(0)->setData(is_sieve, blend_val);
    ui->ind_graph_2->graph(0)->setName("Passing of %");
    ui->ind_graph_2->graph(1)->setData(is_sieve, low);
    ui->ind_graph_2->graph(1)->setName("Lower Limit");
    ui->ind_graph_2->graph(2)->setData(is_sieve, mid);
    ui->ind_graph_2->graph(2)->setName("Middle Limit");
    ui->ind_graph_2->graph(3)->setData(is_sieve, high);
    ui->ind_graph_2->graph(3)->setName("Upper Limit");

    ui->ind_graph_2->graph(3)->setChannelFillGraph(ui->ind_graph_2->graph(1));
    ui->ind_graph_2->graph(3)->setBrush(QBrush(QColor(0, 255, 0, 20))); // light green 20% transparent
    ui->ind_graph_2->graph(0)->setPen(redPen);
    ui->ind_graph_2->graph(1)->setPen(QPen(QColor(102, 153, 130)));
    ui->ind_graph_2->graph(2)->setPen(QPen(QColor(170, 101, 0)));
    ui->ind_graph_2->graph(3)->setPen(QPen(QColor(140, 102, 169)));

    QSharedPointer<QCPAxisTickerLog> logTicker2(new QCPAxisTickerLog);
    ui->ind_graph_2->xAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->ind_graph_2->xAxis->setTicker(logTicker2);

    ui->ind_graph_2->xAxis->setLabel("IS SIEVE IN MM");
    ui->ind_graph_2->xAxis->setRange(0.05, 100);
    ui->ind_graph_2->yAxis->setLabel("PASSING OF %");
    ui->ind_graph_2->yAxis->setRange(0, 100);
    ui->ind_graph_2->replot();

    file_savepath = cwd.filePath("html/blending_graph.png");
    QFile graph_file_bld(file_savepath);
    if (!graph_file_bld.open(QIODevice::WriteOnly))
    {
        qDebug() << graph_file_bld.errorString();
    }
    else
    {
        ui->ind_graph_2->savePng(file_savepath);
    }
}
void MainWindow::updateGraph_mdd()
{

    QString json_path = cwd.filePath("json/mdd.json");
    QFile mdd_file(json_path);
    QString img_savepath = cwd.filePath("html/mdd_graph.png");

    if (!mdd_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "mdd json not opened";
        return;
    }

    QByteArray mdd_bytes = mdd_file.readAll();
    QJsonDocument mdd_json_doc = QJsonDocument::fromJson(mdd_bytes);
    QJsonObject mdd = mdd_json_doc.object();

    std::vector<double> moisture, density;

    for (int i = 0; i < 5; i++)
    {
        std::string I = std::to_string(i);
        moisture.push_back(mdd[QString::fromStdString("water_content" + I)].toDouble());
        density.push_back(mdd[QString::fromStdString("density" + I)].toDouble());
    }

    mdd_file.close();

    auto coeffs = quadfit(moisture, density);
    std::vector<double> x_fit;
    int x_max_index = std::max_element(moisture.begin(), moisture.end()) - moisture.begin();
    int x_min_index = std::min_element(moisture.begin(), moisture.end()) - moisture.begin();
    int y_max_index = std::max_element(density.begin(), density.end()) - density.begin();
    int y_min_index = std::min_element(density.begin(), density.end()) - density.begin();
    double max_x = moisture[x_max_index], min_x = moisture[x_min_index];
    double tick = (0.01 * (max_x - min_x));
    for (int i = 0; i < 100; i++)
    {
        double to_push = min_x + i * tick;
        x_fit.push_back(to_push);
    }

    auto y_fit = quadval(x_fit, coeffs);

    int y_fit_max_index = std::max_element(y_fit.begin(), y_fit.end()) - y_fit.begin();
    mdd["MDD"] = y_fit[y_fit_max_index];
    mdd["OMC"] = x_fit[y_fit_max_index];

    QFile mdd_file_write(json_path);

    if (mdd_file_write.open(QFile::WriteOnly | QFile::Text))
    {
        QTextStream out(&mdd_file_write);
        QJsonDocument jsonDoc_1(mdd);

        out << jsonDoc_1.toJson();

        // Close the file
        mdd_file_write.close();
    }
    else
    {
        qDebug() << "SUYGETSU AIMS THE RESET ABSOLUTELY INCREDIBLE";
    }

    QVector<double> x, y, graph_x_fit, graph_y_fit;
    for (int i = 0; i < moisture.size(); i++)
    {
        x << moisture[i];
        y << density[i];
    }
    for (int i = 0; i < x_fit.size(); i++)
    {
        graph_x_fit << x_fit[i];
        graph_y_fit << y_fit[i];
    }

    ui->mdd_graph->graph(0)->setData(x, y);
    ui->mdd_graph->graph(1)->setData(graph_x_fit, graph_y_fit);

    ui->mdd_graph->xAxis->setRange(min_x - 0.5, max_x + 0.5);
    ui->mdd_graph->xAxis->setLabel("Moisture Content %");
    ui->mdd_graph->yAxis->setRange(density[y_min_index] - 0.1 * density[y_min_index], density[y_max_index] + 0.1 * density[y_max_index]);
    ui->mdd_graph->yAxis->setLabel("Dry Density gm/cc");

    ui->mdd_graph->graph(0)->setScatterStyle(QCPScatterStyle::ssCross);
    ui->mdd_graph->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->mdd_graph->graph(0)->setPen(QPen(Qt::red));
    ui->mdd_graph->graph(1)->setPen(QPen(Qt::black));

    ui->mdd_graph->replot();

    QFile mdd_graph(img_savepath);
    if (!mdd_graph.open(QIODevice::WriteOnly))
    {
        qDebug() << mdd_graph.errorString();
    }
    else
    {
        ui->mdd_graph->savePng(img_savepath);
    }
}
void MainWindow::updateGraph_grad()
{

    QString json_path = cwd.filePath("json/grad.json");
    QFile grad_file(json_path);
    QString grad_bld_savepath = cwd.filePath("html/grad_graph.png");
    QString grad_jmf_savepath = cwd.filePath("html/jmf_graph.png");

    if (!grad_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "grad json not opened";
        return;
    }

    QByteArray grad_bytes = grad_file.readAll();
    QJsonDocument grad_json_doc = QJsonDocument::fromJson(grad_bytes);
    QJsonObject grad = grad_json_doc.object();
    QJsonObject grad_bld = grad["blending"].toObject();
    QJsonObject grad_seive = grad["seive_sizes"].toObject();

    QVector<double> max, min, mid, dbm_pass, seives, jmf_max, jmf_min;

    max = {100, 100, 95, 80, 54, 42, 21, 8};
    min = {100, 90, 71, 56, 38, 28, 7, 2};
    mid = {100, 95, 83, 68, 46, 35, 14, 5};
    jmf_max = {100, 100, 95, 79, 48, 36, 18, 6};
    jmf_min = {100, 90, 82, 65, 36, 26, 10, 2};

    for (auto key : grad_seive.keys())
    {
        seives << grad_seive[key].toDouble();
    }
    for (auto key : grad_bld.keys())
    {
        dbm_pass << grad_bld[key].toDouble();
    }

    ui->grad_graph_1->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    ui->grad_graph_1->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    ui->grad_graph_1->legend->setFont(legendFont);
    ui->grad_graph_1->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    ui->grad_graph_1->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignRight);

    ui->grad_graph_1->graph(0)->setData(seives, dbm_pass);
    ui->grad_graph_1->graph(0)->setName("Passing of %");
    ui->grad_graph_1->graph(1)->setData(seives, min);
    ui->grad_graph_1->graph(1)->setName("Lower Limit");
    ui->grad_graph_1->graph(2)->setData(seives, mid);
    ui->grad_graph_1->graph(2)->setName("Middle Limit");
    ui->grad_graph_1->graph(3)->setData(seives, max);
    ui->grad_graph_1->graph(3)->setName("Upper Limit");

    QPen redPen(Qt::red, 2);

    ui->grad_graph_1->graph(0)->setPen(redPen);
    ui->grad_graph_1->graph(1)->setPen(QPen(QColor(102, 153, 130)));
    ui->grad_graph_1->graph(2)->setPen(QPen(QColor(170, 101, 0, 200)));
    ui->grad_graph_1->graph(3)->setPen(QPen(QColor(140, 102, 169)));

    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
    ui->grad_graph_1->xAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->grad_graph_1->xAxis->setTicker(logTicker);
    ui->grad_graph_1->xAxis->grid()->setVisible(true);
    ui->grad_graph_1->yAxis->grid()->setVisible(true);
    ui->grad_graph_1->xAxis->grid()->setSubGridVisible(true);
    ui->grad_graph_1->yAxis->grid()->setSubGridVisible(true);
    ui->grad_graph_1->xAxis->grid()->setSubGridPen(QPen(Qt::gray, 1, Qt::DotLine));
    ui->grad_graph_1->yAxis->grid()->setSubGridPen(QPen(Qt::gray, 1, Qt::DotLine));
    ui->grad_graph_1->axisRect()->setBackground(QBrush(QColor(255,255,204,200)));

    ui->grad_graph_1->xAxis->setLabel("IS SIEVE IN MM");
    ui->grad_graph_1->xAxis->setRange(0.05, 100);
    ui->grad_graph_1->yAxis->setLabel("PASSING OF %");
    ui->grad_graph_1->yAxis->setRange(0, 100);
    ui->grad_graph_1->plotLayout()->addElement(0, 0, new QCPTextElement(ui->grad_graph_1, "DBM Gradation"));

    ui->grad_graph_1->replot();

    ui->grad_bld_graph_label->show();

    ui->grad_graph_2->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    ui->grad_graph_2->legend->setVisible(true);
    ui->grad_graph_2->legend->setFont(legendFont);
    ui->grad_graph_2->legend->setBrush(QBrush(QColor(255, 255, 255, 230)));
    ui->grad_graph_2->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom | Qt::AlignRight);

    ui->grad_graph_2->graph(0)->setData(seives, dbm_pass);
    ui->grad_graph_2->graph(0)->setName("Passing of %");
    ui->grad_graph_2->graph(1)->setData(seives, min);
    ui->grad_graph_2->graph(1)->setName("Lower Limit");
    ui->grad_graph_2->graph(3)->setData(seives, max);
    ui->grad_graph_2->graph(3)->setName("Upper Limit");
    ui->grad_graph_2->graph(4)->setData(seives, jmf_max);
    ui->grad_graph_2->graph(4)->setName("JMF Upper Limit");
    ui->grad_graph_2->graph(5)->setData(seives, jmf_min);
    ui->grad_graph_2->graph(5)->setName("JMF Lower Limit");

    QPen jmf_max_pen;
    jmf_max_pen.setColor(QColor(188, 66, 245));
    jmf_max_pen.setStyle(Qt::DashLine);

    QPen jmf_min_pen;
    jmf_min_pen.setColor(QColor(240, 125, 40));
    jmf_min_pen.setStyle(Qt::DashDotLine);

    ui->grad_graph_2->graph(0)->setPen(redPen);
    ui->grad_graph_2->graph(1)->setPen(QPen(QColor(0, 0, 0)));
    ui->grad_graph_2->graph(3)->setPen(QPen(QColor(0, 0, 0)));
    ui->grad_graph_2->graph(4)->setPen(QPen(QColor(188, 66, 245)));
    ui->grad_graph_2->graph(5)->setPen(QPen(QColor(240, 125, 40)));
    ui->grad_graph_2->xAxis->grid()->setVisible(true);
    ui->grad_graph_2->yAxis->grid()->setVisible(true);
    ui->grad_graph_2->xAxis->grid()->setPen(QPen(Qt::gray, 1, Qt::SolidLine));
    ui->grad_graph_2->yAxis->grid()->setPen(QPen(Qt::gray, 1, Qt::SolidLine));
    ui->grad_graph_2->xAxis->grid()->setSubGridVisible(true);
    ui->grad_graph_2->yAxis->grid()->setSubGridVisible(true);
    ui->grad_graph_2->xAxis->grid()->setSubGridPen(QPen(Qt::gray, 1, Qt::DotLine));
    ui->grad_graph_2->yAxis->grid()->setSubGridPen(QPen(Qt::gray, 1, Qt::DotLine));

    ui->grad_graph_2->xAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->grad_graph_2->xAxis->setTicker(logTicker);

    ui->grad_graph_2->xAxis->setLabel("IS SIEVE IN MM");
    ui->grad_graph_2->xAxis->setRange(0.05, 100);
    ui->grad_graph_2->yAxis->setLabel("PASSING OF %");
    ui->grad_graph_2->yAxis->setRange(0, 100);
    ui->grad_graph_2->plotLayout()->addElement(0, 0, new QCPTextElement(ui->grad_graph_2, "DBM Job Mix Formula"));

    ui->grad_graph_2->replot();

    ui->jmf_graph_label->show();

    QFile grad_graph(grad_bld_savepath);
    if (!grad_graph.open(QIODevice::WriteOnly))
    {
        qDebug() << grad_graph.errorString();
    }
    else
    {
        ui->grad_graph_1->savePng(grad_bld_savepath);
        grad_graph.close();
    }

    QFile grad_jmf_graph(grad_jmf_savepath);
    if (!grad_jmf_graph.open(QIODevice::WriteOnly))
    {
        qDebug() << grad_jmf_graph.errorString();
    }
    else
    {
        ui->grad_graph_2->savePng(grad_jmf_savepath);
        grad_jmf_graph.close();
    }
}
void MainWindow::updateGraph_dbm() {
    //For these 6 graphs we do not make any ui element we just make the plots in instances of qcp and then save it as png

    QString marshall_json_path = cwd.filePath("json/marshall.json");
    QString vol_json_path = cwd.filePath("json/vol.json");

    QFile marshall_json_file(marshall_json_path);
    std::vector<QJsonObject> marshall_levels;
    if (marshall_json_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray marshall_bytes = marshall_json_file.readAll();
        QJsonDocument marshall_json_doc = QJsonDocument::fromJson(marshall_bytes);
        QJsonObject marshall_json = marshall_json_doc.object();
        for (auto key: marshall_json.keys()) {
            marshall_levels.push_back(marshall_json[key].toObject());
        }
    }
    marshall_json_file.close();

    QFile vol_json_file(vol_json_path);
    QJsonObject vol_comp;
    if (vol_json_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray vol_bytes = vol_json_file.readAll();
        QJsonDocument vol_json_doc = QJsonDocument::fromJson(vol_bytes);
        QJsonObject vol_json = vol_json_doc.object();
        vol_comp = vol_json["composition"].toObject();
    }
    vol_json_file.close();

    generate_bitumen_gmb_graph(marshall_levels);
    generate_bitumen_stability_graph(marshall_levels);
    generate_bitumen_flow_graph(marshall_levels);
    generate_bitumen_va_graph(marshall_levels, vol_comp);
    generate_bitumen_vma_graph(marshall_levels, vol_comp);
    generate_bitumen_vfb_graph(marshall_levels, vol_comp);

    std::string graph_page_path = cwd.filePath("html/properties_curves.html").toStdString();
    std::ofstream graph_page(graph_page_path, std::ios::out);

    if (graph_page.is_open())
    {
        qDebug() << "output html file opened";

        QFile template_file(":/templates/templates/test_properties_curves.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {

            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush = "";

                    switch (token) {
                    case 1:
                        topush = ui->marshal_nameofclient->toHtml().toStdString();
                        break;
                    case 2:
                        topush = ui->marshal_nameofcontractor->toHtml().toStdString();
                        break;
                    }

                    graph_page << topush;
                }
                else
                {
                    graph_page << line[i];
                }
            }
        }

        graph_page.close();
        qDebug() << "file written to";

        template_file.close();
    }
}
void MainWindow::generate_bitumen_stability_graph(std::vector<QJsonObject> marshall_levels) {

    QCustomPlot *stability_graph = new QCustomPlot();

    QString stability_savepath = cwd.filePath("html/stability_graph.png");

    stability_graph->addGraph();
    stability_graph->addGraph();

    // Prepare Data
    std::vector<std::pair<double, double>> stability_points;
    std::vector<double> bitumen_content;
    for (int i = 0; i < marshall_levels.size(); i++) {
        double bitumen = marshall_levels[i][QString::fromStdString("marshall_%1_00").arg(i+1)].toDouble();
        double stability = marshall_levels[i]["avg_stability"].toDouble();

        stability_points.push_back({bitumen, stability});
        bitumen_content.push_back(bitumen);
    }
    double xmax = *std::max_element(bitumen_content.begin(), bitumen_content.end());
    double xmin = *std::min_element(bitumen_content.begin(), bitumen_content.end());
    double range = xmax-xmin;

    // Add data to the first graph and set it to scatter style with squares
    QVector<double> x, y;
    for (int i = 0; i < stability_points.size(); ++i) {
        x << stability_points[i].first;
        y << stability_points[i].second;
    }
    stability_graph->graph(0)->setData(x, y);
    stability_graph->graph(0)->setLineStyle(QCPGraph::lsNone);
    stability_graph->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, Qt::black, Qt::white, 7));

    // Apply curvefit function to stability_points and add the result to the second graph
    auto fitted_points = curvefit(stability_points, xmin-(range/20), xmax+(range/20));
    QVector<double> x_fit, y_fit;
    for (int i = 0; i < fitted_points.size(); ++i) {
        x_fit << fitted_points[i].first;
        y_fit << fitted_points[i].second;
    }
    stability_graph->graph(1)->setData(x_fit, y_fit);
    stability_graph->graph(1)->setPen(QPen(Qt::black));
    xmin = *std::min_element(x_fit.constBegin(), x_fit.constEnd());
    xmax = *std::max_element(x_fit.constBegin(), x_fit.constEnd());
    double ymin = *std::min_element(y_fit.constBegin(), y_fit.constEnd());
    double ymax = *std::max_element(y_fit.constBegin(), y_fit.constEnd());

    stability_graph->setBackground(Qt::white);
    stability_graph->plotLayout()->addElement(0, 0, new QCPTextElement(stability_graph, "Bitumen vs Stability"));

    stability_graph->xAxis->setRange(xmin-(range/4), xmax+(range/4));
    stability_graph->xAxis->setLabel("Bitumen Content (%)");

    stability_graph->yAxis->setRange(ymin-(range/3), ymax+(range/3));
    stability_graph->yAxis->setLabel("Stability (Kg)");

    stability_graph->replot();

    stability_graph->savePng(stability_savepath);

    delete stability_graph;
}
void MainWindow::generate_bitumen_gmb_graph(std::vector<QJsonObject> marshall_levels) {

    QCustomPlot *gmb_graph = new QCustomPlot();

    QString gmb_savepath = cwd.filePath("html/gmb_graph.png");

    gmb_graph->addGraph();
    gmb_graph->addGraph();

    //Prepare Data
    std::vector<std::pair<double, double>> gmb_points;
    std::vector<double> bitumen_content;
    for (int i = 0; i < marshall_levels.size(); i++) {
        double bitumen = marshall_levels[i][QString::fromStdString("marshall_%1_00").arg(i+1)].toDouble();
        double gmb = marshall_levels[i]["avg_gmb"].toDouble();

        gmb_points.push_back({bitumen, gmb});
        bitumen_content.push_back(bitumen);
    }
    double xmax = *std::max_element(bitumen_content.begin(), bitumen_content.end());
    double xmin = *std::min_element(bitumen_content.begin(), bitumen_content.end());
    double range = xmax - xmin;

    // Add data to the first graph and set it to scatter style with squares
    QVector<double> x, y;
    for (int i = 0; i < gmb_points.size(); ++i) {
        x << gmb_points[i].first;
        y << gmb_points[i].second;
    }
    gmb_graph->graph(0)->setData(x, y);
    gmb_graph->graph(0)->setLineStyle(QCPGraph::lsNone);
    gmb_graph->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, Qt::black, Qt::white, 7));

    // Apply curvefit function to gmb_points and add the result to the second graph
    auto fitted_points = curvefit(gmb_points, xmin-(range/20), xmax+(range/20));
    QVector<double> x_fit, y_fit;
    for (int i = 0; i < fitted_points.size(); ++i) {
        x_fit << fitted_points[i].first;
        y_fit << fitted_points[i].second;
    }
    gmb_graph->graph(1)->setData(x_fit, y_fit);
    gmb_graph->graph(1)->setPen(QPen(Qt::black));
    xmin = *std::min_element(x_fit.constBegin(), x_fit.constEnd());
    xmax = *std::max_element(x_fit.constBegin(), x_fit.constEnd());
    double ymin = *std::min_element(y_fit.constBegin(), y_fit.constEnd());
    double ymax = *std::max_element(y_fit.constBegin(), y_fit.constEnd());

    gmb_graph->setBackground(Qt::white);
    gmb_graph->plotLayout()->addElement(0, 0, new QCPTextElement(gmb_graph, "Bitumen vs Bulk Density"));

    gmb_graph->xAxis->setRange(xmin-(range/4), xmax+(range/4));
    gmb_graph->xAxis->setLabel("Bitumen Content (%)");

    gmb_graph->yAxis->setRange(ymin-(range/3), ymax+(range/3));
    gmb_graph->yAxis->setLabel("Bulk Density (gcc)");

    gmb_graph->replot();

    gmb_graph->savePng(gmb_savepath);

    delete gmb_graph;
}
void MainWindow::generate_bitumen_flow_graph(std::vector<QJsonObject> marshall_levels) {

    QCustomPlot *flow_graph = new QCustomPlot();

    QString flow_savepath = cwd.filePath("html/flow_graph.png");

    flow_graph->addGraph();
    flow_graph->addGraph();

    //Prepare Data
    std::vector<std::pair<double, double>> flow_points;
    std::vector<double> bitumen_content;
    for (int i = 0; i < marshall_levels.size(); i++) {
        double bitumen = marshall_levels[i][QString::fromStdString("marshall_%1_00").arg(i+1)].toDouble();
        double flow = marshall_levels[i]["avg_flow"].toDouble();

        flow_points.push_back({bitumen, flow});
        bitumen_content.push_back(bitumen);
    }
    double xmax = *std::max_element(bitumen_content.begin(), bitumen_content.end());
    double xmin = *std::min_element(bitumen_content.begin(), bitumen_content.end());
    double range = xmax - xmin;

    // Add data to the first graph and set it to scatter style with squares
    QVector<double> x, y;
    for (int i = 0; i < flow_points.size(); ++i) {
        x << flow_points[i].first;
        y << flow_points[i].second;
    }
    flow_graph->graph(0)->setData(x, y);
    flow_graph->graph(0)->setLineStyle(QCPGraph::lsNone);
    flow_graph->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, Qt::black, Qt::white, 7));

    // Apply curvefit function to gmb_points and add the result to the second graph
    auto fitted_points = curvefit(flow_points, xmin-(range/20), xmax+(range/20));
    QVector<double> x_fit, y_fit;
    for (int i = 0; i < fitted_points.size(); ++i) {
        x_fit << fitted_points[i].first;
        y_fit << fitted_points[i].second;
    }
    flow_graph->graph(1)->setData(x_fit, y_fit);
    flow_graph->graph(1)->setPen(QPen(Qt::black));
    xmin = *std::min_element(x_fit.constBegin(), x_fit.constEnd());
    xmax = *std::max_element(x_fit.constBegin(), x_fit.constEnd());
    double ymin = *std::min_element(y_fit.constBegin(), y_fit.constEnd());
    double ymax = *std::max_element(y_fit.constBegin(), y_fit.constEnd());

    flow_graph->setBackground(Qt::white);
    flow_graph->plotLayout()->addElement(0, 0, new QCPTextElement(flow_graph, "Bitumen vs Bulk Density"));

    flow_graph->xAxis->setRange(xmin-(range/4), xmax+(range/4));
    flow_graph->xAxis->setLabel("Bitumen Content (%)");

    flow_graph->yAxis->setRange(ymin-(range/3), ymax+(range/3));
    flow_graph->yAxis->setLabel("Flow (mm)");

    flow_graph->replot();

    flow_graph->savePng(flow_savepath);

    delete flow_graph;
}
void MainWindow::generate_bitumen_va_graph(std::vector<QJsonObject> marshall_levels, QJsonObject vol) {

    QCustomPlot *va_graph = new QCustomPlot();

    QString va_swd = cwd.filePath("html/va_graph.png");

    va_graph->addGraph();
    va_graph->addGraph();

    //Prepare Data
    std::vector<std::pair<double, double>> va_points;
    std::vector<double> bitumen_content;
    for (int i = 0; i < marshall_levels.size(); i++) {
        double va = vol[QString::fromStdString("vol_va_%1").arg(i+1)].toDouble();
        double bitumen = marshall_levels[i][QString::fromStdString("marshall_%1_00").arg(i+1)].toDouble();

        va_points.push_back({bitumen, va});
        bitumen_content.push_back(bitumen);
    }
    double xmax = *std::max_element(bitumen_content.begin(), bitumen_content.end());
    double xmin = *std::min_element(bitumen_content.begin(), bitumen_content.end());
    double range = xmax - xmin;

    // Add data to the first graph and set it to scatter style with squares
    QVector<double> x, y;
    for (int i = 0; i < va_points.size(); ++i) {
        x << va_points[i].first;
        y << va_points[i].second;
    }
    va_graph->graph(0)->setData(x, y);
    va_graph->graph(0)->setLineStyle(QCPGraph::lsNone);
    va_graph->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, Qt::black, Qt::white, 7));

    // Apply curvefit function to gmb_points and add the result to the second graph
    auto fitted_points = curvefit(va_points, xmin-(range/20), xmax+(range/20));
    QVector<double> x_fit, y_fit;
    for (int i = 0; i < fitted_points.size(); ++i) {
        x_fit << fitted_points[i].first;
        y_fit << fitted_points[i].second;
    }
    va_graph->graph(1)->setData(x_fit, y_fit);
    va_graph->graph(1)->setPen(QPen(Qt::black));
    xmin = *std::min_element(x_fit.constBegin(), x_fit.constEnd());
    xmax = *std::max_element(x_fit.constBegin(), x_fit.constEnd());
    double ymin = *std::min_element(y_fit.constBegin(), y_fit.constEnd());
    double ymax = *std::max_element(y_fit.constBegin(), y_fit.constEnd());

    va_graph->setBackground(Qt::white);
    va_graph->plotLayout()->addElement(0, 0, new QCPTextElement(va_graph, "Bitumen vs Air Voids"));

    va_graph->xAxis->setRange(xmin-(range/4), xmax+(range/4));
    va_graph->xAxis->setLabel("Bitumen Content (%)");

    va_graph->yAxis->setRange(ymin-(range/3), ymax+(range/3));
    va_graph->yAxis->setLabel("Air Voids (%)");

    va_graph->replot();

    va_graph->savePng(va_swd);

    delete va_graph;
}
void MainWindow::generate_bitumen_vma_graph(std::vector<QJsonObject> marshall_levels, QJsonObject vol) {

    QCustomPlot *vma_graph = new QCustomPlot();

    QString vma_swd = cwd.filePath("html/vma_graph.png");

    vma_graph->addGraph();
    vma_graph->addGraph();

    //Prepare Data
    std::vector<std::pair<double, double>> vma_points;
    std::vector<double> bitumen_content;
    for (int i = 0; i < marshall_levels.size(); i++) {
        double vma = vol[QString::fromStdString("vol_vma_%1").arg(i+1)].toDouble();
        double bitumen = marshall_levels[i][QString::fromStdString("marshall_%1_00").arg(i+1)].toDouble();

        vma_points.push_back({bitumen, vma});
        bitumen_content.push_back(bitumen);
    }
    double xmax = *std::max_element(bitumen_content.begin(), bitumen_content.end());
    double xmin = *std::min_element(bitumen_content.begin(), bitumen_content.end());
    double range = xmax - xmin;

    // Add data to the first graph and set it to scatter style with squares
    QVector<double> x, y;
    for (int i = 0; i < vma_points.size(); ++i) {
        x << vma_points[i].first;
        y << vma_points[i].second;
    }
    vma_graph->graph(0)->setData(x, y);
    vma_graph->graph(0)->setLineStyle(QCPGraph::lsNone);
    vma_graph->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, Qt::black, Qt::white, 7));

    // Apply curvefit function to gmb_points and add the result to the second graph
    auto fitted_points = curvefit(vma_points, xmin-(range/20), xmax+(range/20));
    QVector<double> x_fit, y_fit;
    for (int i = 0; i < fitted_points.size(); ++i) {
        x_fit << fitted_points[i].first;
        y_fit << fitted_points[i].second;
    }
    vma_graph->graph(1)->setData(x_fit, y_fit);
    vma_graph->graph(1)->setPen(QPen(Qt::black));
    xmin = *std::min_element(x_fit.constBegin(), x_fit.constEnd());
    xmax = *std::max_element(x_fit.constBegin(), x_fit.constEnd());
    double ymin = *std::min_element(y_fit.constBegin(), y_fit.constEnd());
    double ymax = *std::max_element(y_fit.constBegin(), y_fit.constEnd());

    vma_graph->setBackground(Qt::white);
    vma_graph->plotLayout()->addElement(0, 0, new QCPTextElement(vma_graph, "Bitumen vs VMA"));

    vma_graph->xAxis->setRange(xmin-(range/4), xmax+(range/4));
    vma_graph->xAxis->setLabel("Bitumen Content (%)");

    vma_graph->yAxis->setRange(ymin-(range/3), ymax+(range/3));
    vma_graph->yAxis->setLabel("VMA");

    vma_graph->replot();

    vma_graph->savePng(vma_swd);

    delete vma_graph;
}
void MainWindow::generate_bitumen_vfb_graph(std::vector<QJsonObject> marshall_levels, QJsonObject vol) {

    QCustomPlot *vfb_graph = new QCustomPlot();

    QString vfb_swd = cwd.filePath("html/vfb_graph.png");

    vfb_graph->addGraph();
    vfb_graph->addGraph();

    //Prepare Data
    std::vector<std::pair<double, double>> vfb_points;
    std::vector<double> bitumen_content;
    for (int i = 0; i < marshall_levels.size(); i++) {
        double vfb = vol[QString::fromStdString("vol_vfb_%1").arg(i+1)].toDouble();
        double bitumen = marshall_levels[i][QString::fromStdString("marshall_%1_00").arg(i+1)].toDouble();

        vfb_points.push_back({bitumen, vfb});
        bitumen_content.push_back(bitumen);
    }
    double xmax = *std::max_element(bitumen_content.begin(), bitumen_content.end());
    double xmin = *std::min_element(bitumen_content.begin(), bitumen_content.end());
    double range = xmax - xmin;

    // Add data to the first graph and set it to scatter style with squares
    QVector<double> x, y;
    for (int i = 0; i < vfb_points.size(); ++i) {
        x << vfb_points[i].first;
        y << vfb_points[i].second;
    }
    vfb_graph->graph(0)->setData(x, y);
    vfb_graph->graph(0)->setLineStyle(QCPGraph::lsNone);
    vfb_graph->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, Qt::black, Qt::white, 7));

    // Apply curvefit function to gmb_points and add the result to the second graph
    auto fitted_points = curvefit(vfb_points, xmin-(range/20), xmax+(range/20));
    QVector<double> x_fit, y_fit;
    for (int i = 0; i < fitted_points.size(); ++i) {
        x_fit << fitted_points[i].first;
        y_fit << fitted_points[i].second;
    }
    vfb_graph->graph(1)->setData(x_fit, y_fit);
    vfb_graph->graph(1)->setPen(QPen(Qt::black));
    xmin = *std::min_element(x_fit.constBegin(), x_fit.constEnd());
    xmax = *std::max_element(x_fit.constBegin(), x_fit.constEnd());
    double ymin = *std::min_element(y_fit.constBegin(), y_fit.constEnd());
    double ymax = *std::max_element(y_fit.constBegin(), y_fit.constEnd());

    vfb_graph->setBackground(Qt::white);
    vfb_graph->plotLayout()->addElement(0, 0, new QCPTextElement(vfb_graph, "Bitumen vs VFB"));

    vfb_graph->xAxis->setRange(xmin-(range/4), xmax+(range/4));
    vfb_graph->xAxis->setLabel("Bitumen Content (%)");

    vfb_graph->yAxis->setRange(ymin-(range/3), ymax+(range/3));
    vfb_graph->yAxis->setLabel("VFB (%)");

    vfb_graph->replot();

    vfb_graph->savePng(vfb_swd);

    delete vfb_graph;
}




// Deals with exports to PDF
void MainWindow::on_actionExport_to_PDF_triggered()
{
    qDebug() << "EXPORT triggered";
    QString program_wmm;
    QStringList args_wmm;

    QString program_dbm;
    QStringList args_dbm;

    std::string command_wmm, command_dbm;

    if (OS == "win")
    {
        program_wmm = QString("..\\executable\\wkhtmltopdf.exe");
        program_dbm = QString("..\\executable\\wkhtmltopdf.exe");
    }
    else if (OS == "linux")
    {
        program_wmm = cwd.filePath("executable/wkhtmltopdf");
        program_dbm = cwd.filePath("executable/wkhtmltopdf");
    }
    else if (OS == "apple")
    {
        // TO WRITE
        program_wmm = QString("wkhtmltopdf");
        program_dbm = QString("wkhtmltopdf");
    }
    command_wmm = command_wmm + program_wmm.toStdString();
    command_dbm = command_dbm + program_dbm.toStdString();

    for (auto i = tracked_files.begin(); i != tracked_files.end(); ++i)
    {
        qDebug() << *i;

        if (*i == "spc")
        {
            args_wmm << cwd.filePath("html/spc_10mm.html");
            args_wmm << cwd.filePath("html/spc_20mm.html");
            args_wmm << cwd.filePath("html/spc_40mm.html");
            args_wmm << cwd.filePath("html/spc_stone_dust.html");
        }
        else if (*i == "fei")
        {
            args_wmm << cwd.filePath("html/fei.html");
        }
        else if (*i == "aiv")
        {
            args_wmm << cwd.filePath("html/aiv_10mm.html");
            args_wmm << cwd.filePath("html/aiv_20mm.html");
        }
        else if (*i == "ind")
        {
            args_wmm << cwd.filePath("html/ind_40mm.html");
            args_wmm << cwd.filePath("html/ind_20mm.html");
            args_wmm << cwd.filePath("html/ind_10mm.html");
            args_wmm << cwd.filePath("html/ind_d.html");
            args_wmm << cwd.filePath("html/bld.html");
            args_wmm << cwd.filePath("html/cmb.html");
            args_wmm << cwd.filePath("html/pass.html");
        }
        else if (*i == "mdd")
        {
            args_wmm << cwd.filePath("html/mdd.html");
        }
        else if (*i == "grad")
        {
            args_dbm << cwd.filePath("html/grad.html");
            args_dbm << cwd.filePath("html/grad_bld.html");
            args_dbm << cwd.filePath("html/grad_jmf.html");
        }
        else if (*i == "marshall") {
            args_dbm << cwd.filePath("html/marshall.html");
            if (!args_dbm.contains(cwd.filePath("html/properties_curves.html"))) {
                args_dbm << cwd.filePath("html/properties_curves.html");
            }
        } else if (*i == "tensile") {
            args_dbm << cwd.filePath("html/tensile.html");
        } else if (*i == "vol") {
            args_dbm << cwd.filePath("html/determination_of_eff_spg.html");
            args_dbm << cwd.filePath("html/vol_analysis.html");
            args_dbm << cwd.filePath("html/vol_obc_analysis.html");
            if (!args_dbm.contains(cwd.filePath("html/properties_curves.html"))) {
                args_dbm << cwd.filePath("html/properties_curves.html");
            }
        } else if (*i == "wa") {
            args_dbm << cwd.filePath("html/wa_20.html");
            args_dbm << cwd.filePath("html/wa_10.html");
            args_dbm << cwd.filePath("html/wa_dust.html");
        } else if (*i == "gmm") {
            args_dbm << cwd.filePath("html/gmm_4.00.html");
            args_dbm << cwd.filePath("html/gmm_4.25.html");
            args_dbm << cwd.filePath("html/gmm_4.50.html");
        } else if (*i == "rheology") {
            std::vector<QString> rh_exps = {"strip", "softening", "penetration", "ductility", "rh_vis", "flash", "rh_spc"};
            for (QString exp: rh_exps) {
                args_dbm << cwd.filePath(QString::fromStdString("html/%1.html").arg(exp));
            }
        }
    }

    QString wmm_fname = "WMM_REPORT_" + QDateTime::currentDateTime().toString(Qt::ISODateWithMs) + ".pdf";
    wmm_fname.replace(QRegularExpression("[^\\w\\.]"), "_");
    args_wmm << swd.filePath(wmm_fname);

    QString dbm_fname = "DBM_REPORT_" + QDateTime::currentDateTime().toString(Qt::ISODateWithMs) + ".pdf";
    dbm_fname.replace(QRegularExpression("[^\\w\\.]"), "_");
    args_dbm << swd.filePath(dbm_fname);

    QList<QString>::iterator i;
    for (i = args_wmm.begin(); i != args_wmm.end(); ++i)
        command_wmm = command_wmm + " " + i->toStdString();
    qDebug() << command_wmm;

    for (auto i: args_dbm) {
        command_dbm += " " + i.toStdString();
    } qDebug() << command_dbm;

    if (OS != "apple")
    {
        QProcess *converter_wmm = new QProcess();
        converter_wmm->start(program_wmm, args_wmm);

        QProcess *converter_dbm = new QProcess();
        converter_dbm->start(program_dbm, args_dbm);
    }
    else
    {
        QMessageBox::information(this, "Copy-Paste this command in your terminal to get your PDF!", QString(command_wmm.c_str()), QMessageBox::Ok);
        std::string output_txt_path = cwd.filePath("output/command.txt").toStdString();
        std::ofstream output_txt_file(output_txt_path, std::ios::out);
        if (output_txt_file.is_open())
        {
            output_txt_file << command_wmm;
        }
        output_txt_file.close();
    }
}
void MainWindow::on_spc_export_clicked()
{
    std::vector<std::string> tracked_files_cpy = tracked_files;
    tracked_files.clear();
    tracked_files.push_back("spc");
    ui->actionExport_to_PDF->trigger();
    tracked_files = tracked_files_cpy;

    qDebug() << tracked_files;
}
void MainWindow::on_fei_export_clicked()
{
    std::vector<std::string> tracked_files_cpy = tracked_files;
    tracked_files.clear();
    tracked_files.push_back("fei");
    ui->actionExport_to_PDF->trigger();
    tracked_files = tracked_files_cpy;

    qDebug() << tracked_files;
}
void MainWindow::on_aiv_export_clicked()
{
    std::vector<std::string> tracked_files_cpy = tracked_files;
    tracked_files.clear();
    tracked_files.push_back("aiv");
    ui->actionExport_to_PDF->trigger();
    tracked_files = tracked_files_cpy;

    qDebug() << tracked_files;
}
void MainWindow::on_ind_export_clicked()
{
    std::vector<std::string> tracked_files_cpy = tracked_files;
    tracked_files.clear();
    tracked_files.push_back("ind");
    ui->actionExport_to_PDF->trigger();
    tracked_files = tracked_files_cpy;

    qDebug() << tracked_files;
}
void MainWindow::on_mdd_export_clicked()
{
    std::vector<std::string> tracked_files_cpy = tracked_files;
    tracked_files.clear();
    tracked_files.push_back("mdd");
    ui->actionExport_to_PDF->trigger();
    tracked_files = tracked_files_cpy;

    qDebug() << tracked_files;
}
void MainWindow::on_grad_export_clicked()
{
    std::vector<std::string> tracked_files_cpy = tracked_files;
    tracked_files.clear();
    tracked_files.push_back("grad");
    ui->actionExport_to_PDF->trigger();
    tracked_files = tracked_files_cpy;

    qDebug() << tracked_files;
}
void MainWindow::on_marshall_export_clicked()
{
    std::vector<std::string> tracked_files_cpy = tracked_files;
    tracked_files.clear();
    tracked_files.push_back("marshall");
    ui->actionExport_to_PDF->trigger();
    tracked_files = tracked_files_cpy;

    qDebug() << tracked_files;
}
void MainWindow::on_tensile_export_clicked()
{
    std::vector<std::string> tracked_files_cpy = tracked_files;
    tracked_files.clear();
    tracked_files.push_back("tensile");
    ui->actionExport_to_PDF->trigger();
    tracked_files = tracked_files_cpy;

    qDebug() << tracked_files;
}
void MainWindow::on_vol_export_clicked()
{
    std::vector<std::string> tracked_files_cpy = tracked_files;
    tracked_files.clear();
    tracked_files.push_back("vol");
    ui->actionExport_to_PDF->trigger();
    tracked_files = tracked_files_cpy;

    qDebug() << tracked_files;
}
void MainWindow::on_gmm_export_clicked()
{
    std::vector<std::string> tracked_files_cpy = tracked_files;
    tracked_files.clear();
    tracked_files.push_back("gmm");
    ui->actionExport_to_PDF->trigger();
    tracked_files = tracked_files_cpy;

    qDebug() << tracked_files;
}
void MainWindow::on_rheology_export_clicked()
{
    std::vector<std::string> tracked_files_cpy = tracked_files;
    tracked_files.clear();
    tracked_files.push_back("rheology");
    ui->actionExport_to_PDF->trigger();
    tracked_files = tracked_files_cpy;

    qDebug() << tracked_files;
}
void MainWindow::on_wa_export_clicked()
{
    std::vector<std::string> tracked_files_cpy = tracked_files;
    tracked_files.clear();
    tracked_files.push_back("wa");
    ui->actionExport_to_PDF->trigger();
    tracked_files = tracked_files_cpy;

    qDebug() << tracked_files;
}



// Deals with HTML generation
void MainWindow::generate_html_spc()
{
    // ui->spc_save->click();
    qDebug() << "beginning spc save...";
    QString json_path = cwd.filePath("json/spc.json");

    QFile json_file(json_path);
    if (!json_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "json file not opened";
        return;
    }
    else
    {
        qDebug() << "json file opened";
    }
    QByteArray json_vals_bytearray = json_file.readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(json_vals_bytearray);
    QJsonObject json_lookups = json_doc.object();
    QStringList json_keys = json_lookups.keys();
    int json_keys_len = (int)json_keys.size();

    for (int t = 0; t < json_keys_len; t++)
    {
        if (json_keys[t] == "40mm" || json_keys[t] == "20mm" || json_keys[t] == "10mm" || json_keys[t] == "stone_dust")
        {
            std::string output_html_path = cwd.filePath("html/spc_").toStdString();
            output_html_path = output_html_path + json_keys[t].toStdString() + ".html";
            std::ofstream output_html_file(output_html_path, std::ios::out);

            if (output_html_file.is_open())
            {
                qDebug() << "output html file opened";

                QString template_path = cwd.filePath("templates/spc.html");
                QFile template_file(":/templates/templates/spc.html");
                if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    qDebug() << "html not opened";
                    return;
                }
                else
                {
                    qDebug() << "html file opened";
                }
                QTextStream infile(&template_file);

                while (!infile.atEnd())
                {

                    std::string line_str = infile.readLine().toStdString();
                    const char *line = line_str.c_str();
                    int tilda = 0;
                    int token;
                    for (int i = 0; i < (int)strlen(line); i++)
                    {
                        if (line[i] == '~' && tilda == 0)
                        {
                            tilda = 1;

                            // Gets the token from HTML file
                            for (int j = i + 1; j < (int)strlen(line); j++)
                            {
                                if (line[j] == '~' && j - i == 2)
                                {
                                    token = (int)line[i + 1] - 48;
                                    i = j;
                                    break;
                                }
                                else if (line[j] == '~' && j - i == 3)
                                {
                                    token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                                    i = j;
                                    break;
                                }
                            }

                            QJsonObject json_lookups_data = json_lookups[json_keys[t]].toObject();

                            std::string topush;
                            double topushf;
                            switch (token)
                            {
                            case 1:
                                topush = ui->spc_bsc_1->toPlainText().toStdString();
                                break;
                            case 2:
                                topush = ui->spc_bsc_2->toPlainText().toStdString();
                                break;
                            case 3:
                                topush = ui->spc_bsc_3->toPlainText().toStdString();
                                break;
                            case 4:
                                topush = ui->spc_bsc_4->toPlainText().toStdString();
                                break;
                            case 5:
                                topush = ui->spc_exp_1->text().toStdString();
                                break;
                            case 6:
                                topush = ui->spc_exp_2->text().toStdString();
                                break;
                            case 7:
                                topush = ui->spc_exp_3->text().toStdString();
                                break;
                            case 8:
                                topush = ui->spc_exp_4->text().toStdString();
                                break;
                            case 9:
                                if (json_keys[t] != "stone_dust")
                                {
                                    topush = json_keys[t].toStdString();
                                }
                                else
                                {
                                    topush = "Stone Dust";
                                }
                                break;
                            case 10:
                                topush = ui->spc_exp_6->text().toStdString();
                                break;
                            case 11:
                                topushf = json_lookups_data["Weight_of_sample_of_water_1"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 12:
                                topushf = json_lookups_data["Weight_of_sample_of_water_2"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 13:
                                topushf = json_lookups_data["Weight_of_sample_of_water_3"].toDouble();
                                output_html_file << topushf;

                                break;
                            case 14:
                                topushf = json_lookups_data["Weight_of_SSD_Sample_1"].toDouble();
                                output_html_file << topushf;

                                break;
                            case 15:
                                topushf = json_lookups_data["Weight_of_SSD_Sample_2"].toDouble();
                                output_html_file << topushf;

                                break;
                            case 16:
                                topushf = json_lookups_data["Weight_of_SSD_Sample_3"].toDouble();
                                output_html_file << topushf;

                                break;
                            case 17:
                                topushf = json_lookups_data["Weight_of_Oven_dry_sample_1"].toDouble();
                                output_html_file << topushf;

                                break;
                            case 18:
                                topushf = json_lookups_data["Weight_of_Oven_dry_sample_2"].toDouble();
                                output_html_file << topushf;

                                break;
                            case 19:
                                topushf = json_lookups_data["Weight_of_Oven_dry_sample_3"].toDouble();
                                output_html_file << topushf;

                                break;
                            case 20:
                                topushf = json_lookups_data["Bulk_specific_gravity_1"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 21:
                                topushf = json_lookups_data["Bulk_specific_gravity_2"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 22:
                                topushf = json_lookups_data["Bulk_specific_gravity_3"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 23:
                                topushf = json_lookups_data["apparent_specific_gravity_1"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 24:
                                topushf = json_lookups_data["apparent_specific_gravity_2"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 25:
                                topushf = json_lookups_data["apparent_specific_gravity_3"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 26:
                                topushf = json_lookups_data["water_absorption_1"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 27:
                                topushf = json_lookups_data["water_absorption_2"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 28:
                                topushf = json_lookups_data["water_absorption_3"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 29:
                                topushf = json_lookups_data["average_bulk_specific_gravity"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 30:
                                topushf = json_lookups_data["average_apparent_specific_gravity"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 31:
                                topushf = json_lookups_data["average_water_absorption"].toDouble();
                                output_html_file << topushf;
                                break;
                            }

                            output_html_file << topush;
                            topush = "";
                        }
                        else
                        {
                            output_html_file << line[i];
                        }
                    }
                }

                output_html_file.close();
                qDebug() << "file written to";

                template_file.close();
            }
            else
            {
                qDebug() << "output html not opened";
            }
        }
    }
}
void MainWindow::generate_html_fei()
{
    // ui->fei_save->click();
    qDebug() << "beginning fei save...";
    QString json_path = cwd.filePath("json/fei.json");

    QFile json_file(json_path);
    if (!json_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "json file not opened";
        return;
    }
    else
    {
        qDebug() << "json file opened";
    }
    QByteArray json_vals_bytearray = json_file.readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(json_vals_bytearray);
    QJsonObject json_lookups = json_doc.object();

    std::string output_html_path = cwd.filePath("html/fei").toStdString();
    output_html_path = output_html_path + ".html";
    std::ofstream output_html_file(output_html_path, std::ios::out);

    if (output_html_file.is_open())
    {
        qDebug() << "output html file opened";

        QString template_path = cwd.filePath("templates/fei.html");
        QFile template_file(":/templates/templates/fei.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {

            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush;
                    double topushf;
                    switch (token)
                    {
                    case 1:
                        topush = ui->fei_bsc_1->toPlainText().toStdString();
                        break;
                    case 2:
                        topush = ui->fei_bsc_2->toPlainText().toStdString();
                        break;
                    case 3:
                        topush = ui->fei_bsc_3->toPlainText().toStdString();
                        break;
                    case 4:
                        topush = ui->fei_bsc_4->toPlainText().toStdString();
                        break;
                    case 5:
                        topush = ui->fei_exp_1->text().toStdString();
                        break;
                    case 6:
                        topush = ui->fei_exp_2->text().toStdString();
                        break;
                    case 7:
                        topush = ui->fei_exp_3->text().toStdString();
                        break;
                    case 8:
                        topush = ui->fei_exp_4->text().toStdString();
                        break;
                    case 9:
                        topush = ui->fei_exp_5->text().toStdString();
                        break;
                    case 10:
                        topush = ui->fei_exp_6->text().toStdString();
                        break;
                    case 11:
                        topushf = json_lookups["p_1"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 12:
                        topushf = json_lookups["r_1"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 13:
                        topushf = json_lookups["A_1"].toDouble();
                        output_html_file << topushf;

                        break;
                    case 14:
                        topushf = json_lookups["B_1"].toDouble();
                        output_html_file << topushf;

                        break;
                    case 15:
                        topushf = json_lookups["C_1"].toDouble();
                        output_html_file << topushf;

                        break;
                    case 16:
                        topushf = json_lookups["D_1"].toDouble();
                        output_html_file << topushf;

                        break;
                    case 17:
                        topushf = json_lookups["p_2"].toDouble();
                        output_html_file << topushf;

                        break;
                    case 18:
                        topushf = json_lookups["r_2"].toDouble();
                        output_html_file << topushf;

                        break;
                    case 19:
                        topushf = json_lookups["A_2"].toDouble();
                        output_html_file << topushf;

                        break;
                    case 20:
                        topushf = json_lookups["B_2"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 21:
                        topushf = json_lookups["C_2"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 22:
                        topushf = json_lookups["D_2"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 23:
                        topushf = json_lookups["p_3"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 24:
                        topushf = json_lookups["r_3"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 25:
                        topushf = json_lookups["A_3"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 26:
                        topushf = json_lookups["B_3"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 27:
                        topushf = json_lookups["C_3"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 28:
                        topushf = json_lookups["D_3"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 29:
                        topushf = json_lookups["p_4"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 30:
                        topushf = json_lookups["r_4"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 31:
                        topushf = json_lookups["A_4"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 32:
                        topushf = json_lookups["B_4"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 33:
                        topushf = json_lookups["C_4"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 34:
                        topushf = json_lookups["D_4"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 35:
                        topushf = json_lookups["p_5"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 36:
                        topushf = json_lookups["r_5"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 37:
                        topushf = json_lookups["A_5"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 38:
                        topushf = json_lookups["B_5"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 39:
                        topushf = json_lookups["C_5"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 40:
                        topushf = json_lookups["D_5"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 41:
                        topushf = json_lookups["p_6"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 42:
                        topushf = json_lookups["r_6"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 43:
                        topushf = json_lookups["A_6"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 44:
                        topushf = json_lookups["B_6"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 45:
                        topushf = json_lookups["C_6"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 46:
                        topushf = json_lookups["D_6"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 47:
                        topushf = json_lookups["p_7"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 48:
                        topushf = json_lookups["r_7"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 49:
                        topushf = json_lookups["A_7"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 50:
                        topushf = json_lookups["B_7"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 51:
                        topushf = json_lookups["C_7"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 52:
                        topushf = json_lookups["D_7"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 53:
                        topushf = json_lookups["p_8"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 54:
                        topushf = json_lookups["r_8"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 55:
                        topushf = json_lookups["A_8"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 56:
                        topushf = json_lookups["B_8"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 57:
                        topushf = json_lookups["C_8"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 58:
                        topushf = json_lookups["D_8"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 59:
                        topushf = json_lookups["p_9"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 60:
                        topushf = json_lookups["r_9"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 61:
                        topushf = json_lookups["A_9"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 62:
                        topushf = json_lookups["B_9"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 63:
                        topushf = json_lookups["C_9"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 64:
                        topushf = json_lookups["D_9"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 65:
                        topushf = json_lookups["TOTAL_WEIGHT_A"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 66:
                        topushf = json_lookups["TOTAL_WEIGHT_B"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 67:
                        topushf = json_lookups["TOTAL_WEIGHT_C"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 68:
                        topushf = json_lookups["TOTAL_WEIGHT_D"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 69:
                        topushf = json_lookups["flakiness index"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 70:
                        topushf = json_lookups["elongation index"].toDouble();
                        output_html_file << topushf;
                        break;
                    case 71:
                        topushf = json_lookups["combined index"].toDouble();
                        output_html_file << topushf;
                        break;
                    default:
                        qDebug() << "something failed....";
                    }

                    output_html_file << topush;
                    topush = "";
                }
                else
                {
                    output_html_file << line[i];
                }
            }
        }

        output_html_file.close();
        qDebug() << "file written to";

        template_file.close();
    }
}
void MainWindow::generate_html_aiv()
{
    // ui->aiv_save->click();
    qDebug() << "beginning aiv save...";
    QString json_path = cwd.filePath("json/aiv.json");

    QFile json_file(json_path);
    if (!json_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "json file not opened";
        return;
    }
    else
    {
        qDebug() << "json file opened";
    }
    QByteArray json_vals_bytearray = json_file.readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(json_vals_bytearray);
    QJsonObject json_lookups = json_doc.object();
    QStringList json_keys = json_lookups.keys();
    int json_keys_len = (int)json_keys.size();

    for (int t = 0; t < json_keys_len; t++)
    {
        if (json_keys[t] == "20mm" || json_keys[t] == "10mm")
        {
            std::string output_html_path = cwd.filePath("html/aiv_").toStdString();
            output_html_path = output_html_path + json_keys[t].toStdString() + ".html";
            std::ofstream output_html_file(output_html_path, std::ios::out);

            if (output_html_file.is_open())
            {
                qDebug() << "output html file opened";

                QString template_path = cwd.filePath("templates/aiv.html");
                QFile template_file(":/templates/templates/aiv.html");
                if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    qDebug() << "html not opened";
                    return;
                }
                else
                {
                    qDebug() << "html file opened";
                }
                QTextStream infile(&template_file);

                while (!infile.atEnd())
                {

                    std::string line_str = infile.readLine().toStdString();
                    const char *line = line_str.c_str();
                    int tilda = 0;
                    int token;
                    for (int i = 0; i < (int)strlen(line); i++)
                    {
                        if (line[i] == '~' && tilda == 0)
                        {
                            tilda = 1;

                            // Gets the token from HTML file
                            for (int j = i + 1; j < (int)strlen(line); j++)
                            {
                                if (line[j] == '~' && j - i == 2)
                                {
                                    token = (int)line[i + 1] - 48;
                                    i = j;
                                    break;
                                }
                                else if (line[j] == '~' && j - i == 3)
                                {
                                    token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                                    i = j;
                                    break;
                                }
                            }

                            QJsonObject json_lookups_data = json_lookups[json_keys[t]].toObject();

                            std::string topush;
                            double topushf;
                            switch (token)
                            {
                            case 1:
                                topush = ui->aiv_bsc_1->toPlainText().toStdString();
                                break;
                            case 2:
                                topush = ui->aiv_bsc_2->toPlainText().toStdString();
                                break;
                            case 3:
                                topush = ui->aiv_bsc_3->toPlainText().toStdString();
                                break;
                            case 4:
                                topush = ui->aiv_bsc_4->toPlainText().toStdString();
                                break;
                            case 5:
                                topush = ui->aiv_exp_1->text().toStdString();
                                break;
                            case 6:
                                topush = ui->aiv_exp_2->text().toStdString();
                                break;
                            case 7:
                                topush = ui->aiv_exp_3->text().toStdString();
                                break;
                            case 8:
                                topush = ui->aiv_exp_4->text().toStdString();
                                break;
                            case 9:
                                topush = json_keys[t].toStdString();
                                break;
                            case 10:
                                topush = ui->aiv_exp_6->text().toStdString();
                                break;
                            case 11:
                                topushf = json_lookups_data["weight_of_cylinder_1"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 12:
                                topushf = json_lookups_data["weight_of_cylinder_2"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 13:
                                topushf = json_lookups_data["weight_of_cylinder_3"].toDouble();
                                output_html_file << topushf;

                                break;
                            case 14:
                                topushf = json_lookups_data["weight_of_cylinder_sample_1"].toDouble();
                                output_html_file << topushf;

                                break;
                            case 15:
                                topushf = json_lookups_data["weight_of_cylinder_sample_2"].toDouble();
                                output_html_file << topushf;

                                break;
                            case 16:
                                topushf = json_lookups_data["weight_of_cylinder_sample_3"].toDouble();
                                output_html_file << topushf;

                                break;
                            case 17:
                                topushf = json_lookups_data["weight_sample_1"].toDouble();
                                output_html_file << topushf;

                                break;
                            case 18:
                                topushf = json_lookups_data["weight_sample_2"].toDouble();
                                output_html_file << topushf;

                                break;
                            case 19:
                                topushf = json_lookups_data["weight_sample_3"].toDouble();
                                output_html_file << topushf;

                                break;
                            case 20:
                                topushf = json_lookups_data["weight_crushed_material_1"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 21:
                                topushf = json_lookups_data["weight_crushed_material_2"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 22:
                                topushf = json_lookups_data["weight_crushed_material_3"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 23:
                                topushf = json_lookups_data["aggeregate_impact_value_1"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 24:
                                topushf = json_lookups_data["aggeregate_impact_value_2"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 25:
                                topushf = json_lookups_data["aggeregate_impact_value_3"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 26:
                                topushf = json_lookups_data["avg_aggregate_impact_value"].toDouble();
                                output_html_file << topushf;
                                break;
                            }

                            output_html_file << topush;
                            topush = "";
                        }
                        else
                        {
                            output_html_file << line[i];
                        }
                    }
                }

                output_html_file.close();
                qDebug() << "file written to";

                template_file.close();
            }
            else
            {
                qDebug() << "output html not opened";
            }
        }
    }
}
void MainWindow::generate_html_ind()
{
    // ui->ind_save->click();
    qDebug() << "beginning ind save... at " << cwd.filePath("");
    QString json_path = cwd.filePath("json/idg.json");

    QFile json_file(json_path);
    if (!json_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "json file not opened";
        return;
    }
    else
    {
        qDebug() << "json file opened";
    }
    QByteArray json_vals_bytearray = json_file.readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(json_vals_bytearray);
    QJsonObject json_lookups = json_doc.object();
    QStringList json_keys = json_lookups.keys();
    int json_keys_len = (int)json_keys.size();

    for (int t = 0; t < json_keys_len; t++)
    {
        if (json_keys[t] == "40mm" || json_keys[t] == "20mm" || json_keys[t] == "10mm" || json_keys[t] == "d")
        {
            std::string output_html_path = cwd.filePath("html/ind_").toStdString();
            output_html_path = output_html_path + json_keys[t].toStdString() + ".html";
            std::ofstream output_html_file(output_html_path, std::ios::out);

            if (output_html_file.is_open())
            {
                qDebug() << "output html file opened";

                QString template_path = cwd.filePath("templates/ind.html");
                QFile template_file(":/templates/templates/ind.html");
                if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    qDebug() << "html not opened";
                    return;
                }
                else
                {
                    qDebug() << "html file opened";
                }
                QTextStream infile(&template_file);

                while (!infile.atEnd())
                {

                    std::string line_str = infile.readLine().toStdString();
                    const char *line = line_str.c_str();
                    int tilda = 0;
                    int token;
                    for (int i = 0; i < (int)strlen(line); i++)
                    {
                        if (line[i] == '~' && tilda == 0)
                        {
                            tilda = 1;

                            // Gets the token from HTML file
                            for (int j = i + 1; j < (int)strlen(line); j++)
                            {
                                if (line[j] == '~' && j - i == 2)
                                {
                                    token = (int)line[i + 1] - 48;
                                    i = j;
                                    break;
                                }
                                else if (line[j] == '~' && j - i == 3)
                                {
                                    token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                                    i = j;
                                    break;
                                }
                                else if (line[j] == '~' && j - i == 4)
                                {
                                    token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                                    i = j;
                                    break;
                                }
                            }

                            QJsonObject json_lookups_data = json_lookups[json_keys[t]].toObject();

                            std::string topush;
                            double topushf;
                            switch (token)
                            {
                            case 1:
                                topush = ui->ind_bsc_1->toPlainText().toStdString();
                                break;
                            case 2:
                                topush = ui->ind_bsc_2->toPlainText().toStdString();
                                break;
                            case 3:
                                topush = ui->ind_bsc_3->toPlainText().toStdString();
                                break;
                            case 4:
                                topush = ui->ind_bsc_4->toPlainText().toStdString();
                                break;
                            case 5:
                                topush = ui->ind_exp_1->text().toStdString();
                                break;
                            case 6:
                                topush = ui->ind_exp_2->text().toStdString();
                                break;
                            case 7:
                                topush = ui->ind_exp_3->text().toStdString();
                                break;
                            case 8:
                                topush = ui->ind_exp_4->text().toStdString();
                                break;
                            case 9:
                                topush = json_keys[t].toStdString();
                                qDebug() << "name error" << json_keys[t];
                                break;
                            case 10:
                                topush = ui->ind_exp_6->text().toStdString();
                                break;
                            case 11:
                                topushf = json_lookups_data["is_sieve_s11"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 12:
                                topushf = json_lookups_data["weight_of_retained_w11"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 13:
                                topushf = json_lookups_data["cum_11"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 14:
                                topushf = json_lookups_data["CUM_11"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 15:
                                topushf = json_lookups_data["pass_11"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 16:
                                topushf = json_lookups_data["is_sieve_s12"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 17:
                                topushf = json_lookups_data["weight_of_retained_w12"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 18:
                                topushf = json_lookups_data["cum_12"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 19:
                                topushf = json_lookups_data["CUM_12"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 20:
                                topushf = json_lookups_data["pass_12"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 21:
                                topushf = json_lookups_data["is_sieve_s13"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 22:
                                topushf = json_lookups_data["weight_of_retained_w13"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 23:
                                topushf = json_lookups_data["cum_13"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 24:
                                topushf = json_lookups_data["CUM_13"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 25:
                                topushf = json_lookups_data["pass_13"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 26:
                                topushf = json_lookups_data["is_sieve_s14"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 27:
                                topushf = json_lookups_data["weight_of_retained_w14"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 28:
                                topushf = json_lookups_data["cum_14"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 29:
                                topushf = json_lookups_data["CUM_14"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 30:
                                topushf = json_lookups_data["pass_14"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 31:
                                topushf = json_lookups_data["is_sieve_s15"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 32:
                                topushf = json_lookups_data["weight_of_retained_w15"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 33:
                                topushf = json_lookups_data["cum_15"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 34:
                                topushf = json_lookups_data["CUM_15"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 35:
                                topushf = json_lookups_data["pass_15"].toDouble();
                                output_html_file << topushf;
                                break;

                            case 36:
                                topushf = json_lookups_data["is_sieve_s16"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 37:
                                topushf = json_lookups_data["weight_of_retained_w16"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 38:
                                topushf = json_lookups_data["cum_16"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 39:
                                topushf = json_lookups_data["CUM_16"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 40:
                                topushf = json_lookups_data["pass_16"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 41:
                                topushf = json_lookups_data["is_sieve_s17"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 42:
                                topushf = json_lookups_data["weight_of_retained_w17"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 43:
                                topushf = json_lookups_data["cum_17"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 44:
                                topushf = json_lookups_data["CUM_17"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 45:
                                topushf = json_lookups_data["pass_17"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 46:
                                topushf = json_lookups_data["is_sieve_s18"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 47:
                                topushf = json_lookups_data["weight_of_retained_w18"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 48:
                                topushf = json_lookups_data["cum_18"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 49:
                                topushf = json_lookups_data["CUM_18"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 50:
                                topushf = json_lookups_data["pass_18"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 51:
                                topushf = json_lookups_data["is_sieve_s21"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 52:
                                topushf = json_lookups_data["weight_of_retained_w21"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 53:
                                topushf = json_lookups_data["cum_21"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 54:
                                topushf = json_lookups_data["CUM_21"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 55:
                                topushf = json_lookups_data["pass_21"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 56:
                                topushf = json_lookups_data["is_sieve_s22"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 57:
                                topushf = json_lookups_data["weight_of_retained_w22"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 58:
                                topushf = json_lookups_data["cum_22"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 59:
                                topushf = json_lookups_data["CUM_22"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 60:
                                topushf = json_lookups_data["pass_22"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 61:
                                topushf = json_lookups_data["is_sieve_s23"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 62:
                                topushf = json_lookups_data["weight_of_retained_w23"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 63:
                                topushf = json_lookups_data["cum_23"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 64:
                                topushf = json_lookups_data["CUM_23"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 65:
                                topushf = json_lookups_data["pass_23"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 66:
                                topushf = json_lookups_data["is_sieve_s24"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 67:
                                topushf = json_lookups_data["weight_of_retained_w24"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 68:
                                topushf = json_lookups_data["cum_24"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 69:
                                topushf = json_lookups_data["CUM_24"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 70:
                                topushf = json_lookups_data["pass_24"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 71:
                                topushf = json_lookups_data["is_sieve_s25"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 72:
                                topushf = json_lookups_data["weight_of_retained_w25"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 73:
                                topushf = json_lookups_data["cum_25"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 74:
                                topushf = json_lookups_data["CUM_25"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 75:
                                topushf = json_lookups_data["pass_25"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 76:
                                topushf = json_lookups_data["is_sieve_s26"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 77:
                                topushf = json_lookups_data["pass_26"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 78:
                                topushf = json_lookups_data["cum_27"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 79:
                                topushf = json_lookups_data["is_sieve_s27"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 80:
                                topushf = json_lookups_data["weight_of_retained_w27"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 81:
                                topushf = json_lookups_data["cum_27"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 82:
                                topushf = json_lookups_data["pass_27"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 83:
                                topushf = json_lookups_data["cum_28"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 84:
                                topushf = json_lookups_data["is_sieve_s28"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 85:
                                topushf = json_lookups_data["weight_of_retained_w28"].toDouble();
                                output_html_file << topushf;
                                break;

                            case 91:
                                topushf = json_lookups_data["is_sieve_s31"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 92:
                                topushf = json_lookups_data["weight_of_retained_w31"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 93:
                                topushf = json_lookups_data["cum_31"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 94:
                                topushf = json_lookups_data["CUM_31"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 95:
                                topushf = json_lookups_data["pass_31"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 96:
                                topushf = json_lookups_data["is_sieve_s32"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 97:
                                topushf = json_lookups_data["weight_of_retained_w32"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 98:
                                topushf = json_lookups_data["cum_32"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 99:
                                topushf = json_lookups_data["CUM_32"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 100:
                                topushf = json_lookups_data["pass_32"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 101:
                                topushf = json_lookups_data["is_sieve_s33"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 102:
                                topushf = json_lookups_data["weight_of_retained_w33"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 103:
                                topushf = json_lookups_data["cum_33"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 104:
                                topushf = json_lookups_data["CUM_33"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 105:
                                topushf = json_lookups_data["pass_33"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 106:
                                topushf = json_lookups_data["is_sieve_s34"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 107:
                                topushf = json_lookups_data["weight_of_retained_w34"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 108:
                                topushf = json_lookups_data["cum_34"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 109:
                                topushf = json_lookups_data["CUM_34"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 110:
                                topushf = json_lookups_data["pass_34"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 111:
                                topushf = json_lookups_data["is_sieve_s35"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 112:
                                topushf = json_lookups_data["weight_of_retained_w35"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 113:
                                topushf = json_lookups_data["cum_35"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 114:
                                topushf = json_lookups_data["CUM_35"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 115:
                                topushf = json_lookups_data["pass_35"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 116:
                                topushf = json_lookups_data["is_sieve_s36"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 117:
                                topushf = json_lookups_data["weight_of_retained_w36"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 118:
                                topushf = json_lookups_data["cum_36"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 119:
                                topushf = json_lookups_data["CUM_36"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 120:
                                topushf = json_lookups_data["pass_36"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 121:
                                topushf = json_lookups_data["is_sieve_s37"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 122:
                                topushf = json_lookups_data["weight_of_retained_w37"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 123:
                                topushf = json_lookups_data["cum_37"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 124:
                                topushf = json_lookups_data["CUM_37"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 125:
                                topushf = json_lookups_data["pass_37"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 126:
                                topushf = json_lookups_data["is_sieve_s38"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 127:
                                topushf = json_lookups_data["weight_of_retained_w38"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 128:
                                topushf = json_lookups_data["cum_38"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 129:
                                topushf = json_lookups_data["CUM_38"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 130:
                                topushf = json_lookups_data["pass_38"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 131:
                                topushf = json_lookups_data["total_weight_1"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 132:
                                topushf = json_lookups_data["total_weight_2"].toDouble();
                                output_html_file << topushf;
                                break;
                            case 133:
                                topushf = json_lookups_data["total_weight_3"].toDouble();
                                output_html_file << topushf;
                                break;

                                output_html_file << topush;
                                topush = "";
                            }
                        }
                        else
                        {
                            output_html_file << line[i];
                        }
                    }
                }

                output_html_file.close();
                qDebug() << "file written to";

                template_file.close();
            }
            else
            {
                qDebug() << "output html not opened";
            }
        }
    }

    std::string bld_output_html_path = cwd.filePath("html/bld").toStdString();
    bld_output_html_path = bld_output_html_path + ".html";
    std::ofstream bld_output_html_file(bld_output_html_path, std::ios::out);
    if (bld_output_html_file.is_open())
    {
        QString bld_template_path = cwd.filePath("templates/bld.html");
        QFile bld_template_file(":/templates/templates/bld.html");
        if (!bld_template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "bld template html not opened";
            return;
        }
        else
        {
            qDebug() << "bld html template opened";
        }
        QTextStream bld_infile(&bld_template_file);

        while (!bld_infile.atEnd())
        {

            std::string bld_line_str = bld_infile.readLine().toStdString();
            const char *line = bld_line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }
                    QJsonObject json_lookups_data_40 = json_lookups["40mm"].toObject();
                    QJsonObject json_lookups_data_20 = json_lookups["20mm"].toObject();
                    QJsonObject json_lookups_data_10 = json_lookups["10mm"].toObject();
                    QJsonObject json_lookups_data_d = json_lookups["d"].toObject();
                    double bld_passing[5][9];
                    bld_passing[4][1] = 0.33 * (json_lookups_data_40["pass_11"].toDouble() + json_lookups_data_40["pass_21"].toDouble() + json_lookups_data_40["pass_31"].toDouble());
                    bld_passing[4][2] = 0.33 * (json_lookups_data_40["pass_12"].toDouble() + json_lookups_data_40["pass_22"].toDouble() + json_lookups_data_40["pass_32"].toDouble());
                    bld_passing[4][3] = 0.33 * (json_lookups_data_40["pass_13"].toDouble() + json_lookups_data_40["pass_23"].toDouble() + json_lookups_data_40["pass_33"].toDouble());
                    bld_passing[4][4] = 0.33 * (json_lookups_data_40["pass_14"].toDouble() + json_lookups_data_40["pass_24"].toDouble() + json_lookups_data_40["pass_34"].toDouble());
                    bld_passing[4][5] = 0.33 * (json_lookups_data_40["pass_15"].toDouble() + json_lookups_data_40["pass_25"].toDouble() + json_lookups_data_40["pass_35"].toDouble());
                    bld_passing[4][6] = 0.33 * (json_lookups_data_40["pass_16"].toDouble() + json_lookups_data_40["pass_26"].toDouble() + json_lookups_data_40["pass_36"].toDouble());
                    bld_passing[4][7] = 0.33 * (json_lookups_data_40["pass_17"].toDouble() + json_lookups_data_40["pass_27"].toDouble() + json_lookups_data_40["pass_37"].toDouble());
                    bld_passing[4][8] = 0.33 * (json_lookups_data_40["pass_18"].toDouble() + json_lookups_data_40["pass_28"].toDouble() + json_lookups_data_40["pass_38"].toDouble());
                    bld_passing[2][1] = 0.33 * (json_lookups_data_20["pass_11"].toDouble() + json_lookups_data_20["pass_21"].toDouble() + json_lookups_data_20["pass_31"].toDouble());
                    bld_passing[2][2] = 0.33 * (json_lookups_data_20["pass_12"].toDouble() + json_lookups_data_20["pass_22"].toDouble() + json_lookups_data_20["pass_32"].toDouble());
                    bld_passing[2][3] = 0.33 * (json_lookups_data_20["pass_13"].toDouble() + json_lookups_data_20["pass_23"].toDouble() + json_lookups_data_20["pass_33"].toDouble());
                    bld_passing[2][4] = 0.33 * (json_lookups_data_20["pass_14"].toDouble() + json_lookups_data_20["pass_24"].toDouble() + json_lookups_data_20["pass_34"].toDouble());
                    bld_passing[2][5] = 0.33 * (json_lookups_data_20["pass_15"].toDouble() + json_lookups_data_20["pass_25"].toDouble() + json_lookups_data_20["pass_35"].toDouble());
                    bld_passing[2][6] = 0.33 * (json_lookups_data_20["pass_16"].toDouble() + json_lookups_data_20["pass_26"].toDouble() + json_lookups_data_20["pass_36"].toDouble());
                    bld_passing[2][7] = 0.33 * (json_lookups_data_20["pass_17"].toDouble() + json_lookups_data_20["pass_27"].toDouble() + json_lookups_data_20["pass_37"].toDouble());
                    bld_passing[2][8] = 0.33 * (json_lookups_data_20["pass_18"].toDouble() + json_lookups_data_20["pass_28"].toDouble() + json_lookups_data_20["pass_38"].toDouble());
                    bld_passing[1][1] = 0.33 * (json_lookups_data_10["pass_11"].toDouble() + json_lookups_data_10["pass_21"].toDouble() + json_lookups_data_10["pass_31"].toDouble());
                    bld_passing[1][2] = 0.33 * (json_lookups_data_10["pass_12"].toDouble() + json_lookups_data_10["pass_22"].toDouble() + json_lookups_data_10["pass_32"].toDouble());
                    bld_passing[1][3] = 0.33 * (json_lookups_data_10["pass_13"].toDouble() + json_lookups_data_10["pass_23"].toDouble() + json_lookups_data_10["pass_33"].toDouble());
                    bld_passing[1][4] = 0.33 * (json_lookups_data_10["pass_14"].toDouble() + json_lookups_data_10["pass_24"].toDouble() + json_lookups_data_10["pass_34"].toDouble());
                    bld_passing[1][5] = 0.33 * (json_lookups_data_10["pass_15"].toDouble() + json_lookups_data_10["pass_25"].toDouble() + json_lookups_data_10["pass_35"].toDouble());
                    bld_passing[1][6] = 0.33 * (json_lookups_data_10["pass_16"].toDouble() + json_lookups_data_10["pass_26"].toDouble() + json_lookups_data_10["pass_36"].toDouble());
                    bld_passing[1][7] = 0.33 * (json_lookups_data_10["pass_17"].toDouble() + json_lookups_data_10["pass_27"].toDouble() + json_lookups_data_10["pass_37"].toDouble());
                    bld_passing[1][8] = 0.33 * (json_lookups_data_10["pass_18"].toDouble() + json_lookups_data_10["pass_28"].toDouble() + json_lookups_data_10["pass_38"].toDouble());
                    bld_passing[0][1] = 0.33 * (json_lookups_data_d["pass_11"].toDouble() + json_lookups_data_d["pass_21"].toDouble() + json_lookups_data_d["pass_31"].toDouble());
                    bld_passing[0][2] = 0.33 * (json_lookups_data_d["pass_12"].toDouble() + json_lookups_data_d["pass_22"].toDouble() + json_lookups_data_d["pass_32"].toDouble());
                    bld_passing[0][3] = 0.33 * (json_lookups_data_d["pass_13"].toDouble() + json_lookups_data_d["pass_23"].toDouble() + json_lookups_data_d["pass_33"].toDouble());
                    bld_passing[0][4] = 0.33 * (json_lookups_data_d["pass_14"].toDouble() + json_lookups_data_d["pass_24"].toDouble() + json_lookups_data_d["pass_34"].toDouble());
                    bld_passing[0][5] = 0.33 * (json_lookups_data_d["pass_15"].toDouble() + json_lookups_data_d["pass_25"].toDouble() + json_lookups_data_d["pass_35"].toDouble());
                    bld_passing[0][6] = 0.33 * (json_lookups_data_d["pass_16"].toDouble() + json_lookups_data_d["pass_26"].toDouble() + json_lookups_data_d["pass_36"].toDouble());
                    bld_passing[0][7] = 0.33 * (json_lookups_data_d["pass_17"].toDouble() + json_lookups_data_d["pass_27"].toDouble() + json_lookups_data_d["pass_37"].toDouble());
                    bld_passing[0][8] = 0.33 * (json_lookups_data_d["pass_18"].toDouble() + json_lookups_data_d["pass_28"].toDouble() + json_lookups_data_d["pass_38"].toDouble());
                    double bld_taken[5][9];
                    bld_taken[4][1] = (bld_passing[4][1] * json_lookups_data_40["proportion"].toDouble()) * 0.01;
                    bld_taken[2][1] = (bld_passing[2][1] * json_lookups_data_20["proportion"].toDouble()) * 0.01;
                    bld_taken[1][1] = (bld_passing[1][1] * json_lookups_data_10["proportion"].toDouble()) * 0.01;
                    bld_taken[0][1] = (bld_passing[0][1] * json_lookups_data_d["proportion"].toDouble()) * 0.01;
                    for (int i = 1; i < 9; i++)
                    {
                        bld_taken[4][i + 1] = (bld_passing[4][i + 1] * bld_taken[4][i]) * 0.01;
                    }
                    for (int i = 1; i < 9; i++)
                    {
                        bld_taken[2][i + 1] = (bld_passing[2][i + 1] * bld_taken[2][i]) * 0.01;
                    }
                    for (int i = 1; i < 9; i++)
                    {
                        bld_taken[1][i + 1] = (bld_passing[1][i + 1] * bld_taken[1][i]) * 0.01;
                    }
                    for (int i = 1; i < 9; i++)
                    {
                        bld_taken[0][i + 1] = (bld_passing[0][i + 1] * bld_taken[0][i]) * 0.01;
                    }
                    double combined_passing[9];
                    for (int i = 1; i < 9; i++)
                    {
                        combined_passing[i] = bld_taken[4][i] + bld_taken[2][i] + bld_taken[1][i] + bld_taken[0][i];
                    }

                    QVector<double> passing(9), lower_limit(9), upper_limit(9), ss(9);
                    double lower_limit_[8] = {100, 95, 60, 40, 25, 15, 8, 0};
                    double upper_limit_[8] = {100, 100, 80, 60, 40, 30, 22, 15};
                    ss[0] = json_lookups_data_40["is_sieve_s11"].toDouble();
                    ss[1] = json_lookups_data_40["is_sieve_s12"].toDouble();
                    ss[2] = json_lookups_data_40["is_sieve_s13"].toDouble();
                    ss[3] = json_lookups_data_40["is_sieve_s14"].toDouble();
                    ss[4] = json_lookups_data_40["is_sieve_s15"].toDouble();
                    ss[5] = json_lookups_data_40["is_sieve_s16"].toDouble();
                    ss[6] = json_lookups_data_40["is_sieve_s17"].toDouble();
                    ss[7] = json_lookups_data_40["is_sieve_s18"].toDouble();
                    for (int i = 0; i < 8; ++i)
                    {
                        lower_limit[i] = lower_limit_[i];
                        upper_limit[i] = upper_limit_[i];
                        passing[i] = combined_passing[i + 1];
                    }

                    std::string topush;

                    double topushf;
                    switch (token)
                    {
                    case 1:
                        topush = ui->ind_bsc_1->toPlainText().toStdString();
                        break;
                    case 2:
                        topush = ui->ind_bsc_2->toPlainText().toStdString();
                        break;
                    case 3:
                        topush = ui->ind_bsc_3->toPlainText().toStdString();
                        break;
                    case 4:
                        topush = ui->ind_bsc_4->toPlainText().toStdString();
                        break;

                    // Deals with is_sieve
                    case 5:
                        topushf = json_lookups_data_40["is_sieve_s11"].toDouble();
                        bld_output_html_file << topushf;
                        break;
                    case 18:
                        topushf = json_lookups_data_40["is_sieve_s12"].toDouble();
                        bld_output_html_file << topushf;
                        break;

                    case 31:
                        topushf = json_lookups_data_40["is_sieve_s13"].toDouble();
                        bld_output_html_file << topushf;
                        break;

                    case 43:
                        topushf = json_lookups_data_40["is_sieve_s14"].toDouble();
                        bld_output_html_file << topushf;
                        break;

                    case 56:
                        topushf = json_lookups_data_40["is_sieve_s15"].toDouble();
                        bld_output_html_file << topushf;
                        break;

                    case 68:
                        topushf = json_lookups_data_40["is_sieve_s16"].toDouble();
                        bld_output_html_file << topushf;
                        break;

                    case 81:
                        topushf = json_lookups_data_40["is_sieve_s17"].toDouble();
                        bld_output_html_file << topushf;
                        break;

                    case 93:
                        topushf = json_lookups_data_40["is_sieve_s18"].toDouble();
                        bld_output_html_file << topushf;
                        break;

                    // Deals with passing of 40mm
                    case 6:
                        topushf = 0.33 * (json_lookups_data_40["pass_11"].toDouble() + json_lookups_data_40["pass_21"].toDouble() + json_lookups_data_40["pass_31"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 19:
                        topushf = 0.33 * (json_lookups_data_40["pass_12"].toDouble() + json_lookups_data_40["pass_22"].toDouble() + json_lookups_data_40["pass_32"].toDouble());

                        bld_output_html_file << topushf;
                        break;

                    case 32:
                        topushf = 0.33 * (json_lookups_data_40["pass_13"].toDouble() + json_lookups_data_40["pass_23"].toDouble() + json_lookups_data_40["pass_33"].toDouble());

                        bld_output_html_file << topushf;
                        break;

                    case 44:
                        topushf = 0.33 * (json_lookups_data_40["pass_14"].toDouble() + json_lookups_data_40["pass_24"].toDouble() + json_lookups_data_40["pass_34"].toDouble());

                        bld_output_html_file << topushf;
                        break;

                    case 57:
                        topushf = 0.33 * (json_lookups_data_40["pass_15"].toDouble() + json_lookups_data_40["pass_25"].toDouble() + json_lookups_data_40["pass_35"].toDouble());

                        bld_output_html_file << topushf;
                        break;

                    case 69:
                        topushf = 0.33 * (json_lookups_data_40["pass_16"].toDouble() + json_lookups_data_40["pass_26"].toDouble() + json_lookups_data_40["pass_36"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 82:
                        topushf = 0.33 * (json_lookups_data_40["pass_17"].toDouble() + json_lookups_data_40["pass_27"].toDouble() + json_lookups_data_40["pass_37"].toDouble());

                        bld_output_html_file << topushf;
                        break;

                    case 94:
                        topushf = 0.33 * (json_lookups_data_40["pass_18"].toDouble() + json_lookups_data_40["pass_28"].toDouble() + json_lookups_data_40["pass_38"].toDouble());

                        bld_output_html_file << topushf;
                        break;

                    // Deals with 20mm passing
                    case 7:
                        topushf = 0.33 * (json_lookups_data_20["pass_11"].toDouble() + json_lookups_data_20["pass_21"].toDouble() + json_lookups_data_20["pass_31"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 20:
                        topushf = 0.33 * (json_lookups_data_20["pass_12"].toDouble() + json_lookups_data_20["pass_22"].toDouble() + json_lookups_data_20["pass_32"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 33:
                        topushf = 0.33 * (json_lookups_data_20["pass_13"].toDouble() + json_lookups_data_20["pass_23"].toDouble() + json_lookups_data_20["pass_33"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 45:
                        topushf = 0.33 * (json_lookups_data_20["pass_14"].toDouble() + json_lookups_data_20["pass_24"].toDouble() + json_lookups_data_20["pass_34"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 58:
                        topushf = 0.33 * (json_lookups_data_20["pass_15"].toDouble() + json_lookups_data_20["pass_25"].toDouble() + json_lookups_data_20["pass_35"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 70:
                        topushf = 0.33 * (json_lookups_data_20["pass_16"].toDouble() + json_lookups_data_20["pass_26"].toDouble() + json_lookups_data_20["pass_36"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 83:
                        topushf = 0.33 * (json_lookups_data_20["pass_17"].toDouble() + json_lookups_data_20["pass_27"].toDouble() + json_lookups_data_20["pass_37"].toDouble());
                        bld_output_html_file << topushf;
                        break;
                    case 95:
                        topushf = 0.33 * (json_lookups_data_20["pass_18"].toDouble() + json_lookups_data_20["pass_28"].toDouble() + json_lookups_data_20["pass_38"].toDouble());
                        bld_output_html_file << topushf;
                        break;
                    case 8:
                        topushf = 0.33 * (json_lookups_data_10["pass_11"].toDouble() + json_lookups_data_10["pass_21"].toDouble() + json_lookups_data_10["pass_31"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 21:
                        topushf = 0.33 * (json_lookups_data_10["pass_12"].toDouble() + json_lookups_data_10["pass_22"].toDouble() + json_lookups_data_10["pass_32"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 34:
                        topushf = 0.33 * (json_lookups_data_10["pass_13"].toDouble() + json_lookups_data_10["pass_23"].toDouble() + json_lookups_data_10["pass_33"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 46:
                        topushf = 0.33 * (json_lookups_data_10["pass_14"].toDouble() + json_lookups_data_10["pass_24"].toDouble() + json_lookups_data_10["pass_34"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 59:
                        topushf = 0.33 * (json_lookups_data_10["pass_15"].toDouble() + json_lookups_data_10["pass_25"].toDouble() + json_lookups_data_10["pass_35"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 71:
                        topushf = 0.33 * (json_lookups_data_10["pass_16"].toDouble() + json_lookups_data_10["pass_26"].toDouble() + json_lookups_data_10["pass_36"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 84:
                        topushf = 0.33 * (json_lookups_data_10["pass_17"].toDouble() + json_lookups_data_10["pass_27"].toDouble() + json_lookups_data_10["pass_37"].toDouble());
                        bld_output_html_file << topushf;
                        break;
                    case 96:
                        topushf = 0.33 * (json_lookups_data_10["pass_18"].toDouble() + json_lookups_data_10["pass_28"].toDouble() + json_lookups_data_10["pass_38"].toDouble());
                        bld_output_html_file << topushf;
                        break;
                    case 9:
                        topushf = 0.33 * (json_lookups_data_d["pass_11"].toDouble() + json_lookups_data_d["pass_21"].toDouble() + json_lookups_data_d["pass_31"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 22:
                        topushf = 0.33 * (json_lookups_data_d["pass_12"].toDouble() + json_lookups_data_d["pass_22"].toDouble() + json_lookups_data_d["pass_32"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 35:
                        topushf = 0.33 * (json_lookups_data_d["pass_13"].toDouble() + json_lookups_data_d["pass_23"].toDouble() + json_lookups_data_d["pass_33"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 47:
                        topushf = 0.33 * (json_lookups_data_d["pass_14"].toDouble() + json_lookups_data_d["pass_24"].toDouble() + json_lookups_data_d["pass_34"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 60:
                        topushf = 0.33 * (json_lookups_data_d["pass_15"].toDouble() + json_lookups_data_d["pass_25"].toDouble() + json_lookups_data_d["pass_35"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 72:
                        topushf = 0.33 * (json_lookups_data_d["pass_16"].toDouble() + json_lookups_data_d["pass_26"].toDouble() + json_lookups_data_d["pass_36"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 85:
                        topushf = 0.33 * (json_lookups_data_d["pass_17"].toDouble() + json_lookups_data_d["pass_27"].toDouble() + json_lookups_data_d["pass_37"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    case 97:
                        topushf = 0.33 * (json_lookups_data_d["pass_18"].toDouble() + json_lookups_data_d["pass_28"].toDouble() + json_lookups_data_d["pass_38"].toDouble());
                        bld_output_html_file << topushf;
                        break;

                    // Deals with proportion of sizes
                    case 106:
                        topushf = json_lookups_data_40["proportion"].toDouble();
                        bld_output_html_file << topushf;
                        break;
                    case 107:
                        topushf = json_lookups_data_20["proportion"].toDouble();
                        bld_output_html_file << topushf;
                        break;
                    case 108:
                        topushf = json_lookups_data_10["proportion"].toDouble();
                        bld_output_html_file << topushf;
                        break;
                    case 109:
                        topushf = json_lookups_data_d["proportion"].toDouble();
                        bld_output_html_file << topushf;
                        break;
                    case 10:
                        topushf = bld_taken[4][1];
                        bld_output_html_file << topushf;
                        break;
                    case 23:
                        topushf = bld_taken[4][2];
                        bld_output_html_file << topushf;
                        break;

                    case 36:
                        topushf = bld_taken[4][3];
                        bld_output_html_file << topushf;
                        break;

                    case 48:
                        topushf = bld_taken[4][4];
                        bld_output_html_file << topushf;
                        break;

                    case 61:
                        topushf = bld_taken[4][5];
                        bld_output_html_file << topushf;
                        break;

                    case 73:

                        topushf = bld_taken[4][6];
                        bld_output_html_file << topushf;
                        break;

                    case 86:
                        topushf = bld_taken[4][7];
                        bld_output_html_file << topushf;
                        break;

                    case 98:
                        topushf = bld_taken[4][8];
                        bld_output_html_file << topushf;
                        break;
                    case 11:
                        topushf = bld_taken[2][1];
                        bld_output_html_file << topushf;
                        break;

                    case 24:
                        topushf = bld_taken[2][2];
                        bld_output_html_file << topushf;
                        break;

                    case 37:
                        topushf = bld_taken[2][3];
                        bld_output_html_file << topushf;
                        break;

                    case 49:
                        topushf = bld_taken[2][4];
                        bld_output_html_file << topushf;
                        break;

                    case 62:
                        topushf = bld_taken[2][5];
                        bld_output_html_file << topushf;
                        break;

                    case 74:
                        topushf = bld_taken[2][6];
                        bld_output_html_file << topushf;
                        break;

                    case 87:
                        topushf = bld_taken[2][7];
                        bld_output_html_file << topushf;
                        break;

                    case 99:
                        topushf = bld_taken[2][8];
                        bld_output_html_file << topushf;
                        break;
                    case 12:
                        topushf = bld_taken[1][1];
                        bld_output_html_file << topushf;
                        break;

                    case 25:
                        topushf = bld_taken[1][2];
                        bld_output_html_file << topushf;
                        break;

                    case 38:
                        topushf = bld_taken[1][3];
                        bld_output_html_file << topushf;
                        break;

                    case 50:
                        topushf = bld_taken[1][4];
                        bld_output_html_file << topushf;
                        break;

                    case 63:
                        topushf = bld_taken[1][5];
                        bld_output_html_file << topushf;
                        break;

                    case 75:
                        topushf = bld_taken[1][6];
                        bld_output_html_file << topushf;
                        break;

                    case 88:
                        topushf = bld_taken[1][7];
                        bld_output_html_file << topushf;
                        break;

                    case 100:
                        topushf = bld_taken[1][8];
                        bld_output_html_file << topushf;
                        break;
                    case 13:
                        topushf = bld_taken[0][1];
                        bld_output_html_file << topushf;
                        break;

                    case 26:
                        topushf = bld_taken[0][2];
                        bld_output_html_file << topushf;
                        break;

                    case 39:
                        topushf = bld_taken[0][3];
                        bld_output_html_file << topushf;
                        break;

                    case 51:
                        topushf = bld_taken[0][4];
                        bld_output_html_file << topushf;
                        break;

                    case 64:
                        topushf = bld_taken[0][5];
                        bld_output_html_file << topushf;
                        break;

                    case 76:
                        topushf = bld_taken[0][6];
                        bld_output_html_file << topushf;
                        break;

                    case 89:
                        topushf = bld_taken[0][7];
                        bld_output_html_file << topushf;
                        break;

                    case 101:
                        topushf = bld_taken[0][8];
                        bld_output_html_file << topushf;
                        break;
                    case 14:
                        topushf = combined_passing[1];
                        bld_output_html_file << topushf;
                        break;

                    case 27:
                        topushf = combined_passing[2];
                        bld_output_html_file << topushf;
                        break;

                    case 40:
                        topushf = combined_passing[3];
                        bld_output_html_file << topushf;
                        break;

                    case 52:
                        topushf = combined_passing[4];
                        bld_output_html_file << topushf;
                        break;

                    case 65:
                        topushf = combined_passing[5];
                        bld_output_html_file << topushf;
                        break;

                    case 77:
                        topushf = combined_passing[6];
                        bld_output_html_file << topushf;
                        break;

                    case 90:
                        topushf = combined_passing[7];
                        bld_output_html_file << topushf;
                        break;

                    case 102:
                        topushf = combined_passing[8];
                        bld_output_html_file << topushf;
                        break;

                    default:
                        qDebug() << "smthlikeyou11";
                    }
                    bld_output_html_file << topush;
                    topush = "";
                }
                else
                {

                    bld_output_html_file << line[i];
                }
            }
        }
        bld_output_html_file.close();
        qDebug() << "file written to";

        bld_template_file.close();
    }
    else
    {
        qDebug() << "output html not opened";
    }
    std::string cmb_output_html_path = cwd.filePath("html/cmb").toStdString();
    cmb_output_html_path = cmb_output_html_path + ".html";
    std::ofstream cmb_output_html_file(cmb_output_html_path, std::ios::out);
    if (cmb_output_html_file.is_open())
    {
        QString cmb_template_path = cwd.filePath("templates/cmb.html");
        QFile cmb_template_file(":/templates/templates/cmb.html");
        if (!cmb_template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "cmb html template not opened";
            return;
        }
        else
        {
            qDebug() << "cmb html template opened";
        }
        QTextStream cmb_infile(&cmb_template_file);
        while (!cmb_infile.atEnd())
        {

            std::string cmb_line_str = cmb_infile.readLine().toStdString();
            const char *line = cmb_line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }
                    QJsonObject json_lookups_data_cg = json_lookups["cg"].toObject();
                    QJsonObject json_lookups_data_40 = json_lookups["40mm"].toObject();
                    QJsonObject json_lookups_data_20 = json_lookups["20mm"].toObject();
                    QJsonObject json_lookups_data_10 = json_lookups["10mm"].toObject();
                    QJsonObject json_lookups_data_d = json_lookups["d"].toObject();
                    std::string topush;

                    double topushf;
                    switch (token)
                    {
                    case 1:
                        topush = ui->ind_bsc_1->toPlainText().toStdString();
                        break;
                    case 2:
                        topush = ui->ind_bsc_2->toPlainText().toStdString();
                        break;
                    case 3:
                        topush = ui->ind_bsc_3->toPlainText().toStdString();
                        break;
                    case 4:
                        topush = ui->ind_bsc_4->toPlainText().toStdString();
                        break;
                    case 5:
                        topushf = json_lookups_data_cg["is_sieve_s1"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 6:
                        topushf = json_lookups_data_cg["weight_of_retained_w1"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 7:
                        topushf = json_lookups_data_cg["cum_1"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 8:
                        topushf = json_lookups_data_cg["CUM_1"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 9:
                        topushf = json_lookups_data_cg["Pass_1"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 10:
                        topushf = json_lookups_data_cg["is_sieve_s2"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 11:
                        topushf = json_lookups_data_cg["weight_of_retained_w2"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 12:
                        topushf = json_lookups_data_cg["cum_2"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 13:
                        topushf = json_lookups_data_cg["CUM_2"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 14:
                        topushf = json_lookups_data_cg["Pass_2"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 15:
                        topushf = json_lookups_data_cg["is_sieve_s3"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 16:
                        topushf = json_lookups_data_cg["weight_of_retained_w3"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 17:
                        topushf = json_lookups_data_cg["cum_3"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 18:
                        topushf = json_lookups_data_cg["CUM_3"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 19:
                        topushf = json_lookups_data_cg["Pass_3"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 20:
                        topushf = json_lookups_data_cg["is_sieve_s4"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 21:
                        topushf = json_lookups_data_cg["weight_of_retained_w4"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 22:
                        topushf = json_lookups_data_cg["cum_4"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 23:
                        topushf = json_lookups_data_cg["CUM_4"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 24:
                        topushf = json_lookups_data_cg["Pass_4"].toDouble();
                        cmb_output_html_file << topushf;
                        break;

                    case 25:
                        topushf = json_lookups_data_cg["is_sieve_s5"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 26:
                        topushf = json_lookups_data_cg["weight_of_retained_w5"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 27:
                        topushf = json_lookups_data_cg["cum_5"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 28:
                        topushf = json_lookups_data_cg["CUM_5"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 29:
                        topushf = json_lookups_data_cg["Pass_5"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 30:
                        topushf = json_lookups_data_cg["is_sieve_s6"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 31:
                        topushf = json_lookups_data_cg["weight_of_retained_w6"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 32:
                        topushf = json_lookups_data_cg["cum_6"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 33:
                        topushf = json_lookups_data_cg["CUM_6"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 34:
                        topushf = json_lookups_data_cg["Pass_6"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 35:
                        topushf = json_lookups_data_cg["is_sieve_s7"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 36:
                        topushf = json_lookups_data_cg["weight_of_retained_w7"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 37:
                        topushf = json_lookups_data_cg["cum_7"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 38:
                        topushf = json_lookups_data_cg["CUM_7"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 39:
                        topushf = json_lookups_data_cg["Pass_7"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 40:
                        topushf = json_lookups_data_cg["is_sieve_s8"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 41:
                        topushf = json_lookups_data_cg["weight_of_retained_w8"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 42:
                        topushf = json_lookups_data_cg["cum_8"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 43:
                        topushf = json_lookups_data_cg["CUM_8"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 44:
                        topushf = json_lookups_data_cg["Pass_8"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 45:
                        topushf = json_lookups_data_40["proportion"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 46:
                        topushf = json_lookups_data_20["proportion"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 47:
                        topushf = json_lookups_data_10["proportion"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 48:
                        topushf = json_lookups_data_d["proportion"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 49:
                        topushf = json_lookups_data_cg["total_weight_1"].toDouble();
                        cmb_output_html_file << topushf;
                        break;
                    case 50:
                        topush = ui->ind_exp_2->text().toStdString();
                        break;
                    default:
                        qDebug() << "smthlikeyou11 " << token;
                    }
                    cmb_output_html_file << topush;
                    topush = "";
                }
                else
                {
                    cmb_output_html_file << line[i];
                }
            }
        }
        cmb_output_html_file.close();
        qDebug() << "file written to";

        cmb_template_file.close();
    }
    else
    {
        qDebug() << "output html not opened";
    }
    std::string pass_output_html_path = cwd.filePath("html/pass").toStdString();
    pass_output_html_path = pass_output_html_path + ".html";
    std::ofstream pass_output_html_file(pass_output_html_path, std::ios::out);
    if (pass_output_html_file.is_open())
    {
        QString pass_template_path = cwd.filePath("templates/pass.html");
        QFile pass_template_file(":/templates/templates/pass.html");
        if (!pass_template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "pass html template not opened";
            return;
        }
        else
        {
            qDebug() << "pass html template opened";
        }
        QTextStream pass_infile(&pass_template_file);
        while (!pass_infile.atEnd())
        {

            std::string pass_line_str = pass_infile.readLine().toStdString();
            const char *line = pass_line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }
                    QJsonObject json_lookups_data_40 = json_lookups["40mm"].toObject();
                    QJsonObject json_lookups_data_20 = json_lookups["20mm"].toObject();
                    QJsonObject json_lookups_data_10 = json_lookups["10mm"].toObject();
                    QJsonObject json_lookups_data_d = json_lookups["d"].toObject();
                    std::string topush;

                    double topushf;
                    switch (token)
                    {
                    case 1:
                        topush = ui->ind_bsc_1->toPlainText().toStdString();
                        break;
                    case 2:
                        topush = ui->ind_bsc_2->toPlainText().toStdString();
                        break;
                    case 3:
                        topush = ui->ind_bsc_3->toPlainText().toStdString();
                        break;
                    case 4:
                        topush = ui->ind_bsc_4->toPlainText().toStdString();
                        break;
                    case 11:
                        topushf = json_lookups_data_40["is_sieve_s1"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 12:
                        topushf = json_lookups_data_40["pass_11"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 13:
                        topushf = json_lookups_data_40["pass_21"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 14:
                        topushf = json_lookups_data_40["pass_31"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 15:
                        topushf = (json_lookups_data_40["pass_11"].toDouble() + json_lookups_data_40["pass_21"].toDouble() + json_lookups_data_40["pass_31"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 16:
                        topushf = json_lookups_data_40["is_sieve_s2"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 17:
                        topushf = json_lookups_data_40["pass_12"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 18:
                        topushf = json_lookups_data_40["pass_22"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 19:
                        topushf = json_lookups_data_40["pass_32"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 20:
                        topushf = (json_lookups_data_40["pass_12"].toDouble() + json_lookups_data_40["pass_22"].toDouble() + json_lookups_data_40["pass_32"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 21:
                        topushf = json_lookups_data_40["is_sieve_s3"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 22:
                        topushf = json_lookups_data_40["pass_13"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 23:
                        topushf = json_lookups_data_40["pass_23"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 24:
                        topushf = json_lookups_data_40["pass_33"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 25:
                        topushf = (json_lookups_data_40["pass_13"].toDouble() + json_lookups_data_40["pass_23"].toDouble() + json_lookups_data_40["pass_33"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 26:
                        topushf = json_lookups_data_40["is_sieve_s4"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 27:
                        topushf = json_lookups_data_40["pass_14"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 28:
                        topushf = json_lookups_data_40["pass_24"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 29:
                        topushf = json_lookups_data_40["pass_34"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 30:
                        topushf = (json_lookups_data_40["pass_14"].toDouble() + json_lookups_data_40["pass_24"].toDouble() + json_lookups_data_40["pass_34"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;

                    case 31:
                        topushf = json_lookups_data_40["is_sieve_s5"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 32:
                        topushf = json_lookups_data_40["pass_15"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 33:
                        topushf = json_lookups_data_40["pass_25"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 34:
                        topushf = json_lookups_data_40["pass_35"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 35:
                        topushf = (json_lookups_data_40["pass_15"].toDouble() + json_lookups_data_40["pass_25"].toDouble() + json_lookups_data_40["pass_35"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 36:
                        topushf = json_lookups_data_40["is_sieve_s6"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 37:
                        topushf = json_lookups_data_40["pass_16"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 38:
                        topushf = json_lookups_data_40["pass_26"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 39:
                        topushf = json_lookups_data_40["pass_36"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 40:
                        topushf = (json_lookups_data_40["pass_16"].toDouble() + json_lookups_data_40["pass_26"].toDouble() + json_lookups_data_40["pass_36"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 41:
                        topushf = json_lookups_data_40["is_sieve_s7"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 42:
                        topushf = json_lookups_data_40["pass_17"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 43:
                        topushf = json_lookups_data_40["pass_27"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 44:
                        topushf = json_lookups_data_40["pass_37"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 45:
                        topushf = (json_lookups_data_40["pass_17"].toDouble() + json_lookups_data_40["pass_27"].toDouble() + json_lookups_data_40["pass_37"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 46:
                        topushf = json_lookups_data_40["is_sieve_s8"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 47:
                        topushf = json_lookups_data_40["pass_18"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 48:
                        topushf = json_lookups_data_40["pass_28"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 49:
                        topushf = json_lookups_data_40["pass_38"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 50:
                        topushf = (json_lookups_data_40["pass_18"].toDouble() + json_lookups_data_40["pass_28"].toDouble() + json_lookups_data_40["pass_38"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 51:
                        topushf = json_lookups_data_20["is_sieve_s1"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 52:
                        topushf = json_lookups_data_20["pass_11"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 53:
                        topushf = json_lookups_data_20["pass_21"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 54:
                        topushf = json_lookups_data_20["pass_31"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 55:
                        topushf = (json_lookups_data_20["pass_11"].toDouble() + json_lookups_data_20["pass_21"].toDouble() + json_lookups_data_20["pass_31"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 56:
                        topushf = json_lookups_data_20["is_sieve_s2"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 57:
                        topushf = json_lookups_data_20["pass_12"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 58:
                        topushf = json_lookups_data_20["pass_22"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 59:
                        topushf = json_lookups_data_20["pass_32"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 60:
                        topushf = (json_lookups_data_20["pass_12"].toDouble() + json_lookups_data_20["pass_22"].toDouble() + json_lookups_data_20["pass_32"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 61:
                        topushf = json_lookups_data_20["is_sieve_s3"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 62:
                        topushf = json_lookups_data_20["pass_13"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 63:
                        topushf = json_lookups_data_20["pass_23"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 64:
                        topushf = json_lookups_data_20["pass_33"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 65:
                        topushf = (json_lookups_data_20["pass_13"].toDouble() + json_lookups_data_20["pass_23"].toDouble() + json_lookups_data_20["pass_33"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 66:
                        topushf = json_lookups_data_20["is_sieve_s4"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 67:
                        topushf = json_lookups_data_20["pass_14"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 68:
                        topushf = json_lookups_data_20["pass_24"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 69:
                        topushf = json_lookups_data_20["pass_34"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 70:
                        topushf = (json_lookups_data_20["pass_14"].toDouble() + json_lookups_data_20["pass_24"].toDouble() + json_lookups_data_20["pass_34"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 71:
                        topushf = json_lookups_data_20["is_sieve_s5"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 72:
                        topushf = json_lookups_data_20["pass_15"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 73:
                        topushf = json_lookups_data_20["pass_25"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 74:
                        topushf = json_lookups_data_20["pass_35"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 75:
                        topushf = (json_lookups_data_20["pass_15"].toDouble() + json_lookups_data_20["pass_25"].toDouble() + json_lookups_data_20["pass_35"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 76:
                        topushf = json_lookups_data_20["is_sieve_s6"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 77:
                        topushf = json_lookups_data_20["pass_16"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 78:
                        topushf = json_lookups_data_20["pass_26"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 79:
                        topushf = json_lookups_data_20["pass_36"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 80:
                        topushf = (json_lookups_data_20["pass_16"].toDouble() + json_lookups_data_20["pass_26"].toDouble() + json_lookups_data_20["pass_36"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 81:
                        topushf = json_lookups_data_20["is_sieve_s7"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 82:
                        topushf = json_lookups_data_20["pass_17"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 83:
                        topushf = json_lookups_data_20["pass_27"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 84:
                        topushf = json_lookups_data_20["pass_37"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 85:
                        topushf = (json_lookups_data_20["pass_17"].toDouble() + json_lookups_data_20["pass_27"].toDouble() + json_lookups_data_20["pass_37"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 86:
                        topushf = json_lookups_data_20["is_sieve_s8"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 87:
                        topushf = json_lookups_data_20["pass_18"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 88:
                        topushf = json_lookups_data_20["pass_28"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 89:
                        topushf = json_lookups_data_20["pass_38"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 90:
                        topushf = (json_lookups_data_20["pass_18"].toDouble() + json_lookups_data_20["pass_28"].toDouble() + json_lookups_data_20["pass_38"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 91:
                        topushf = json_lookups_data_10["is_sieve_s1"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 92:
                        topushf = json_lookups_data_10["pass_11"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 93:
                        topushf = json_lookups_data_10["pass_21"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 94:
                        topushf = json_lookups_data_10["pass_31"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 95:
                        topushf = (json_lookups_data_10["pass_11"].toDouble() + json_lookups_data_10["pass_21"].toDouble() + json_lookups_data_10["pass_31"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 96:
                        topushf = json_lookups_data_10["is_sieve_s2"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 97:
                        topushf = json_lookups_data_10["pass_12"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 98:
                        topushf = json_lookups_data_10["pass_22"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 99:
                        topushf = json_lookups_data_10["pass_32"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 100:
                        topushf = (json_lookups_data_10["pass_12"].toDouble() + json_lookups_data_10["pass_22"].toDouble() + json_lookups_data_10["pass_32"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 101:
                        topushf = json_lookups_data_10["is_sieve_s3"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 102:
                        topushf = json_lookups_data_10["pass_13"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 103:
                        topushf = json_lookups_data_10["pass_23"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 104:
                        topushf = json_lookups_data_10["pass_33"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 105:
                        topushf = (json_lookups_data_10["pass_13"].toDouble() + json_lookups_data_10["pass_23"].toDouble() + json_lookups_data_10["pass_33"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 106:
                        topushf = json_lookups_data_10["is_sieve_s4"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 107:
                        topushf = json_lookups_data_10["pass_14"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 108:
                        topushf = json_lookups_data_10["Pass_24"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 109:
                        topushf = json_lookups_data_10["Pass_34"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 110:
                        topushf = (json_lookups_data_10["Pass_14"].toDouble() + json_lookups_data_10["Pass_24"].toDouble() + json_lookups_data_10["Pass_34"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 111:
                        topushf = json_lookups_data_10["is_sieve_s5"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 112:
                        topushf = json_lookups_data_10["Pass_15"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 113:
                        topushf = json_lookups_data_10["Pass_25"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 114:
                        topushf = json_lookups_data_10["Pass_35"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 115:
                        topushf = (json_lookups_data_10["Pass_15"].toDouble() + json_lookups_data_10["Pass_25"].toDouble() + json_lookups_data_10["Pass_35"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 116:
                        topushf = json_lookups_data_10["is_sieve_s6"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 117:
                        topushf = json_lookups_data_10["Pass_16"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 118:
                        topushf = json_lookups_data_10["Pass_26"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 119:
                        topushf = json_lookups_data_10["Pass_36"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 120:
                        topushf = (json_lookups_data_10["Pass_16"].toDouble() + json_lookups_data_10["Pass_26"].toDouble() + json_lookups_data_10["Pass_36"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 121:
                        topushf = json_lookups_data_10["is_sieve_s7"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 122:
                        topushf = json_lookups_data_10["Pass_17"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 123:
                        topushf = json_lookups_data_10["Pass_27"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 124:
                        topushf = json_lookups_data_10["pass_37"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 125:
                        topushf = (json_lookups_data_10["pass_17"].toDouble() + json_lookups_data_10["pass_27"].toDouble() + json_lookups_data_10["pass_37"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 126:
                        topushf = json_lookups_data_10["is_sieve_s8"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 127:
                        topushf = json_lookups_data_10["pass_18"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 128:
                        topushf = json_lookups_data_10["pass_28"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 129:
                        topushf = json_lookups_data_10["pass_38"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 130:
                        topushf = (json_lookups_data_10["pass_18"].toDouble() + json_lookups_data_10["pass_28"].toDouble() + json_lookups_data_10["pass_38"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 131:
                        topushf = json_lookups_data_d["is_sieve_s1"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 132:
                        topushf = json_lookups_data_d["pass_11"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 133:
                        topushf = json_lookups_data_d["pass_21"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 134:
                        topushf = json_lookups_data_d["pass_31"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 135:
                        topushf = (json_lookups_data_d["pass_11"].toDouble() + json_lookups_data_d["pass_21"].toDouble() + json_lookups_data_d["pass_31"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 136:
                        topushf = json_lookups_data_d["is_sieve_s2"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 137:
                        topushf = json_lookups_data_d["pass_12"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 138:
                        topushf = json_lookups_data_d["pass_22"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 139:
                        topushf = json_lookups_data_d["pass_32"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 140:
                        topushf = (json_lookups_data_d["pass_12"].toDouble() + json_lookups_data_d["pass_22"].toDouble() + json_lookups_data_d["pass_32"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 141:
                        topushf = json_lookups_data_d["is_sieve_s3"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 142:
                        topushf = json_lookups_data_d["pass_13"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 143:
                        topushf = json_lookups_data_d["pass_23"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 144:
                        topushf = json_lookups_data_d["pass_33"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 145:
                        topushf = (json_lookups_data_d["pass_13"].toDouble() + json_lookups_data_d["pass_23"].toDouble() + json_lookups_data_d["pass_33"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 146:
                        topushf = json_lookups_data_d["is_sieve_s4"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 147:
                        topushf = json_lookups_data_10["pass_14"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 148:
                        topushf = json_lookups_data_10["pass_24"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 149:
                        topushf = json_lookups_data_10["pass_34"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 150:
                        topushf = (json_lookups_data_10["pass_14"].toDouble() + json_lookups_data_10["pass_24"].toDouble() + json_lookups_data_10["pass_34"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 151:
                        topushf = json_lookups_data_10["is_sieve_s5"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 152:
                        topushf = json_lookups_data_10["pass_15"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 153:
                        topushf = json_lookups_data_10["pass_25"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 154:
                        topushf = json_lookups_data_10["pass_35"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 155:
                        topushf = (json_lookups_data_10["pass_15"].toDouble() + json_lookups_data_10["pass_25"].toDouble() + json_lookups_data_10["pass_35"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 156:
                        topushf = json_lookups_data_10["is_sieve_s6"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 157:
                        topushf = json_lookups_data_10["pass_16"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 158:
                        topushf = json_lookups_data_10["pass_26"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 159:
                        topushf = json_lookups_data_10["pass_36"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 160:
                        topushf = (json_lookups_data_10["pass_16"].toDouble() + json_lookups_data_10["pass_26"].toDouble() + json_lookups_data_10["pass_36"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 161:
                        topushf = json_lookups_data_10["is_sieve_s7"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 162:
                        topushf = json_lookups_data_10["pass_17"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 163:
                        topushf = json_lookups_data_10["pass_27"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 164:
                        topushf = json_lookups_data_10["pass_37"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 165:
                        topushf = (json_lookups_data_10["pass_17"].toDouble() + json_lookups_data_10["pass_27"].toDouble() + json_lookups_data_10["pass_37"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    case 166:
                        topushf = json_lookups_data_10["is_sieve_s8"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 167:
                        topushf = json_lookups_data_10["pass_18"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 168:
                        topushf = json_lookups_data_10["pass_28"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 169:
                        topushf = json_lookups_data_10["pass_38"].toDouble();
                        pass_output_html_file << topushf;
                        break;
                    case 170:
                        topushf = (json_lookups_data_10["pass_18"].toDouble() + json_lookups_data_10["pass_28"].toDouble() + json_lookups_data_10["pass_38"].toDouble()) / 3;
                        pass_output_html_file << topushf;
                        break;
                    default:
                        qDebug() << "smthlikeyou11 " << token;
                    }
                    pass_output_html_file << topush;
                    topush = "";
                }
                else
                {
                    pass_output_html_file << line[i];
                }
            }
        }
        pass_output_html_file.close();
        qDebug() << "file written to";

        pass_template_file.close();
    }
    else
    {
        qDebug() << "output html not opened";
    }
}
void MainWindow::generate_html_mdd()
{
    // ui->mdd_save->click();
    qDebug() << "beginning mdd save...";
    QString json_path = cwd.filePath("json/mdd.json");

    QFile json_file(json_path);
    if (!json_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "json file not opened";
        return;
    }
    else
    {
        qDebug() << "json file opened";
    }
    QByteArray json_vals_bytearray = json_file.readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(json_vals_bytearray);
    QJsonObject json_obj = json_doc.object();

    std::string output_html_path = cwd.filePath("html/mdd.html").toStdString();
    std::ofstream output_html_file(output_html_path, std::ios::out);

    if (output_html_file.is_open())
    {
        qDebug() << "output html file opened";

        QString template_path = cwd.filePath("templates/mdd.html");
        QFile template_file(":/templates/templates/mdd.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {

            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush;
                    double topushf;
                    int j = (token - 14) % 6;

                    if (token == 11)
                    {
                        QString it = QString::fromStdString("mass");
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    }
                    else if (token == 12)
                    {
                        topushf = json_obj["vol"].toDouble();
                        output_html_file << topushf;
                    }
                    else if (token == 13)
                    {
                        topushf = json_obj["MDD"].toDouble();
                    }
                    else if (token == 80)
                    {
                        topushf = json_obj["OMC"].toDouble();
                    }
                    else if ((token >= 14) && (token < 20))
                    {
                        QString it = QString::fromStdString("wsm" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    }
                    else if ((token >= 20) && (token < 26))
                    {
                        QString it = QString::fromStdString("ws" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    }
                    else if ((token >= 26) && (token < 32))
                    {
                        QString it = QString::fromStdString("wds" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    }
                    else if ((token >= 32) && (token < 38))
                    {
                        QString it = QString::fromStdString("tray" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    }
                    else if ((token >= 38) && (token < 44))
                    {
                        QString it = QString::fromStdString("wt_tray" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    }
                    else if ((token >= 44) && (token < 50))
                    {
                        QString it = QString::fromStdString("wst" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    }
                    else if ((token >= 50) && (token < 56))
                    {
                        QString it = QString::fromStdString("wdst" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    }
                    else if ((token >= 56) && (token < 62))
                    {
                        QString it = QString::fromStdString("ww" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    }
                    else if ((token >= 62) && (token < 68))
                    {
                        QString it = QString::fromStdString("wdsm" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    }
                    else if ((token >= 68) && (token < 74))
                    {
                        QString it = QString::fromStdString("water_content" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    }
                    else if ((token >= 74) && (token < 80))
                    {
                        QString it = QString::fromStdString("density" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    }

                    switch (token)
                    {
                    case 1:
                        topush = ui->mdd_bsc_1->toPlainText().toStdString();
                        break;
                    case 2:
                        topush = ui->mdd_bsc_2->toPlainText().toStdString();
                        break;
                    case 3:
                        topush = ui->mdd_bsc_3->toPlainText().toStdString();
                        break;
                    case 4:
                        topush = ui->mdd_bsc_4->toPlainText().toStdString();
                        break;
                    case 5:
                        topush = ui->mdd_exp_1->text().toStdString();
                        break;
                    case 6:
                        topush = ui->mdd_exp_2->text().toStdString();
                        break;
                    case 7:
                        topush = ui->mdd_exp_3->text().toStdString();
                        break;
                    case 8:
                        topush = ui->mdd_exp_4->text().toStdString();
                        break;
                    case 81:
                        topush = ui->mdd_exp_5->text().toStdString();
                        break;
                    case 9:
                        topush = ui->mdd_exp_6->text().toStdString();
                        break;
                    case 82:
                        topush = ui->mdd_exp_7->text().toStdString();
                        break;
                    case 10:
                        topush = ui->mdd_exp_8->text().toStdString();
                        break;
                    }

                    output_html_file << topush;
                    topush = "";
                }
                else
                {
                    output_html_file << line[i];
                }
            }
        }
        json_file.close();
        output_html_file.close();
        qDebug() << "file written to";

        template_file.close();
    }
    else
    {
        qDebug() << "mdd output html file not opened";
    }
}
void MainWindow::generate_html_grad()
{
    // ui->mdd_save->click();
    qDebug() << "beginning grad save...";
    QString json_path = cwd.filePath("json/grad.json");

    QFile json_file(json_path);
    if (!json_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "json file not opened";
        return;
    }
    else
    {
        qDebug() << "json file opened";
    }
    QByteArray json_vals_bytearray = json_file.readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(json_vals_bytearray);
    QJsonObject json_obj = json_doc.object();

    QJsonObject json1 = json_obj["25-16mm"].toObject();
    QJsonObject json2 = json_obj["16-4.75mm"].toObject();
    QJsonObject json3 = json_obj["below_4.75mm"].toObject();
    QJsonObject seives = json_obj["seive_sizes"].toObject();
    QJsonObject bld = json_obj["blending"].toObject();

    std::string output_html_path = cwd.filePath("html/grad.html").toStdString();
    std::ofstream output_html_file(output_html_path, std::ios::out);

    if (output_html_file.is_open())
    {
        qDebug() << "output html file opened";

        QString template_path = cwd.filePath("templates/mdd.html");
        QFile template_file(":/templates/templates/Gradation.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {

            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush;
                    double topushf;

                    if ((token >= 4) && (token <= 59))
                    {
                        token -= 4;
                        QString arg;

                        switch (token % 7)
                        {
                        case 0:
                            arg = QString("is_seive_%1").arg(1 + token / 7);
                            topushf = seives[arg].toDouble();
                            output_html_file << topushf;
                            break;
                        case 6:
                            arg = QString("avg_%1").arg(1 + token / 7);
                            topushf = json1[arg].toDouble();
                            output_html_file << topushf;
                            break;
                        default:
                            arg = QString("grad_p1%1_%2").arg(1 + token / 7).arg(token % 7);
                            topushf = json1[arg].toDouble();
                            output_html_file << topushf;
                            break;
                        }
                    }
                    else if ((token >= 60) && (token <= 115))
                    {
                        token -= 60;
                        QString arg;

                        switch (token % 7)
                        {
                        case 0:
                            arg = QString("is_seive_%1").arg(1 + token / 7);
                            topushf = seives[arg].toDouble();
                            output_html_file << topushf;
                            break;
                        case 6:
                            arg = QString("avg_%1").arg(1 + token / 7);
                            topushf = json2[arg].toDouble();
                            output_html_file << topushf;
                            break;
                        default:
                            arg = QString("grad_p2%1_%2").arg(1 + token / 7).arg(token % 7);
                            topushf = json2[arg].toDouble();
                            output_html_file << topushf;
                            break;
                        }
                    }
                    else if ((token >= 116) && (token <= 171))
                    {
                        token -= 116;
                        QString arg;

                        switch (token % 7)
                        {
                        case 0:
                            arg = QString("is_seive_%1").arg(1 + token / 7);
                            topushf = seives[arg].toDouble();
                            output_html_file << topushf;
                            break;
                        case 6:
                            arg = QString("avg_%1").arg(1 + token / 7);
                            topushf = json3[arg].toDouble();
                            output_html_file << topushf;
                            break;
                        default:
                            arg = QString("grad_p3%1_%2").arg(1 + token / 7).arg(token % 7);
                            topushf = json3[arg].toDouble();
                            output_html_file << topushf;
                            break;
                        }
                    }
                    else
                    {
                        switch (token)
                        {
                        case 1:
                            topush = ui->grad_exp_1->text().toStdString();
                            break;
                        case 2:
                            topush = ui->grad_exp_2->text().toStdString();
                            break;
                        case 3:
                            topush = ui->grad_exp_5->text().toStdString();
                            break;
                        case 176:
                            topush = ui->grad_exp_3->text().toStdString();
                            break;
                        case 177:
                            topush = ui->grad_exp_7->text().toStdString();
                            break;
                        case 178:
                            topush = ui->grad_exp_5->text().toStdString();
                            break;
                        case 179:
                            topush = ui->grad_exp_4->text().toStdString();
                            break;
                        case 180:
                            topush = ui->grad_exp_6->text().toStdString();
                            break;
                        }
                    }

                    output_html_file << topush;
                    topush = "";
                }
                else
                {
                    output_html_file << line[i];
                }
            }
        }
        json_file.close();
        output_html_file.close();
        qDebug() << "file written to";

        template_file.close();
    }
    else
    {
        qDebug() << "mdd output html file not opened";
    }

    output_html_path = cwd.filePath("html/grad_bld.html").toStdString();
    std::ofstream output_grad_bld_file(output_html_path, std::ios::out);

    if (output_grad_bld_file.is_open())
    {
        qDebug() << "output html file opened";

        std::vector<double> max, min, mid;
        max = {100, 100, 95, 80, 54, 42, 21, 8};
        min = {100, 90, 71, 56, 38, 28, 7, 2};
        mid = {100, 95, 83, 68, 46, 35, 14, 5};

        QString template_path = cwd.filePath("templates/mdd.html");
        QFile template_file(":/templates/templates/grad_bld.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {

            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush;
                    double topushf;

                    switch (token)
                    {
                    case 1:
                        output_grad_bld_file << json1["proportion"].toDouble();
                        break;
                    case 2:
                        output_grad_bld_file << json2["proportion"].toDouble();
                        break;
                    case 3:
                        output_grad_bld_file << json3["proportion"].toDouble();
                        break;
                    case 69:
                        topush = ui->grad_bsc_1->toPlainText().toStdString();
                        break;
                    case 70:
                        topush = ui->grad_bsc_2->toPlainText().toStdString();
                        break;
                    case 71:
                        topush = ui->grad_bsc_3->toPlainText().toStdString();
                        break;
                    case 72:
                        topush = ui->grad_bsc_4->toPlainText().toStdString();
                        break;
                    }
                    if ((token >= 4) && (token <= 11))
                    {
                        token -= 4;
                        QString arg = QString("avg_%1").arg(1 + token % 10);
                        output_grad_bld_file << json1[arg].toDouble();
                    }
                    else if ((token >= 20) && (token <= 27))
                    {
                        token -= 20;
                        output_grad_bld_file << json2[QString("avg_%1").arg(1 + token % 10)].toDouble();
                    }
                    else if ((token >= 36) && (token <= 43))
                    {
                        token -= 36;
                        output_grad_bld_file << json3[QString("avg_%1").arg(1 + token % 10)].toDouble();
                    }
                    else if ((token >= 12) && (token <= 19))
                    {
                        token -= 12;
                        output_grad_bld_file << json1[QString("prop_avg_%1").arg(1 + token % 10)].toDouble();
                    }
                    else if ((token >= 28) && (token <= 35))
                    {
                        token -= 28;
                        output_grad_bld_file << json2[QString("prop_avg_%1").arg(1 + token % 10)].toDouble();
                    }
                    else if ((token >= 44) && (token <= 51))
                    {
                        token -= 44;
                        output_grad_bld_file << json3[QString("prop_avg_%1").arg(1 + token % 10)].toDouble();
                    }
                    else if ((token >= 53) && (token <= 60))
                    {
                        token -= 53;
                        output_grad_bld_file << bld[QString("bld_%1").arg(1 + (token % 10))].toDouble();
                    }
                    else if (token == 52)
                    {
                        topush = "100";
                    } else if ((token >= 61)&&(token <= 68)) {
                        token -= 61;
                        output_grad_bld_file << mid[token];
                    } else if ((token >= 69)&&(token <= 76)) {
                        output_grad_bld_file << min[token - 69];
                    } else if ((token >= 77)&&(token <= 84)) {
                        output_grad_bld_file << max[token - 77];
                    }

                    output_grad_bld_file << topush;
                    topush = "";
                }
                else
                {
                    output_grad_bld_file << line[i];
                }
            }
        }
        json_file.close();
        output_grad_bld_file.close();
        qDebug() << "file written to";

        template_file.close();
    }
    else
    {
        qDebug() << "grad bld output html file not opened";
    }

    output_html_path = cwd.filePath("html/grad_jmf.html").toStdString();
    std::ofstream output_grad_jmf_file(output_html_path, std::ios::out);

    if (output_grad_jmf_file.is_open())
    {
        qDebug() << "output html file opened";

        QFile template_file(":/templates/templates/dbm_job_mix_formula.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {

            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush;

                    if (token == 1)
                    {
                        topush = ui->grad_bsc_1->toPlainText().toStdString();
                    }
                    else if (token == 2)
                    {
                        topush = ui->grad_bsc_2->toHtml().toStdString();
                    }
                    else if (token == 3)
                    {
                        topush = "IS SEIVE";
                    }
                    else if ((token >= 4) && (token <= 11))
                    {
                        token -= 4;
                        output_grad_jmf_file << seives[QString("is_seive_%1").arg(1 + token % 10)].toDouble();
                    }
                    else if ((token >= 13) && (token <= 20))
                    {
                        token -= 13;
                        output_grad_jmf_file << bld[QString("bld_%1").arg(1 + (token % 10))].toDouble();
                    }

                    output_grad_jmf_file << topush;
                    topush = "";
                }
                else
                {
                    output_grad_jmf_file << line[i];
                }
            }
        }
        json_file.close();
        output_grad_jmf_file.close();
        qDebug() << "file written to";

        template_file.close();
    }
    else
    {
        qDebug() << "grad bld output html file not opened";
    }
}
void MainWindow::generate_html_tensile()
{
    qDebug() << "beginning tensile save...";
    QString json_path = cwd.filePath("json/tensile.json");

    QFile json_file(json_path);
    if (!json_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "json file not opened";
        return;
    }
    else
    {
        qDebug() << "json file opened";
    }
    QByteArray json_vals_bytearray = json_file.readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(json_vals_bytearray);
    QJsonObject tensile_json = json_doc.object();
    QJsonObject min_30 = tensile_json["30mins"].toObject();
    QJsonObject hr_24 = tensile_json["24hrs"].toObject();

    std::vector<std::string> json_values_parser = {"tensile_wt_%1_%2", "tensile_ssd_wt_diff_%1_%2", "tensile_ssd_%1_%2", "tensile_wt_%1_%2_vol", "tensile_gmb_%1_%2", "tensile_read_%1_%2", "tensile_read_%1_%2_load", "tensile_corr_%1_%2", "tensile_corrected_load_%1_%2", "tensile_flow_%1_%2"};

    std::string output_html_path = cwd.filePath("html/tensile.html").toStdString();
    std::ofstream output_html_file(output_html_path, std::ios::out);

    if (output_html_file.is_open())
    {
        qDebug() << "output html file opened";

        QFile template_file(":/templates/templates/tensile_strength_ratio.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {

            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush;
                    double topushf;

                    switch (token)
                    {
                    case 1:
                        topushf = tensile_json["ring"].toDouble();
                        break;
                    case 2:
                        topushf = min_30["tensile_btmn"].toDouble();
                        break;
                    case 33:
                        topushf = min_30["avg_gmb"].toDouble();
                        break;
                    case 34:
                        topushf = min_30["avg_corrected_load"].toDouble();
                        break;
                    case 35:
                        topushf = min_30["avg_flow"].toDouble();
                        break;
                    case 36:
                        topushf = hr_24["tensile_btmn"].toDouble();
                        break;
                    case 67:
                        topushf = hr_24["avg_gmb"].toDouble();
                        break;
                    case 68:
                        topushf = hr_24["avg_corrected_load"].toDouble();
                        break;
                    case 69:
                        topushf = hr_24["avg_flow"].toDouble();
                        break;
                    case 70:
                        topushf = tensile_json["water_sensitivity"].toDouble();
                        break;
                    default:
                        if ((token >= 3) && (token <= 32))
                        {
                            token -= 3;
                            int i = token % 10;

                            QString lookup_key = QString::fromStdString(json_values_parser[i]);
                            lookup_key = lookup_key.arg(1).arg(1 + token / 10);

                            qDebug() << "lookup key:" << lookup_key;

                            topushf = min_30[lookup_key].toDouble();
                            break;
                        }
                        else if ((token >= 37) && (token <= 66))
                        {
                            token -= 37;
                            int i = token % 10;

                            QString lookup_key = QString::fromStdString(json_values_parser[i]);
                            lookup_key = lookup_key.arg(2).arg(1 + token / 10);

                            qDebug() << "lookup key:" << lookup_key;

                            topushf = hr_24[lookup_key].toDouble();
                            break;
                        }
                        else
                        {
                            qDebug() << "token" << token << "out of bounds, this really should not be happening";
                            break;
                        }
                    }

                    output_html_file << topush;
                    output_html_file << topushf;
                    topush = "";
                }
                else
                {
                    output_html_file << line[i];
                }
            }
        }
        json_file.close();
        output_html_file.close();
        qDebug() << "file written to";

        template_file.close();
    }
    else
    {
        qDebug() << "tensile output html file not opened";
    }
}
void MainWindow::generate_html_marshall()
{
    qDebug() << "beginning marshall save...";
    QString json_path = cwd.filePath("json/marshall.json");

    QFile json_file(json_path);
    if (!json_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "json file not opened";
        return;
    }
    else
    {
        qDebug() << "json file opened";
    }
    QByteArray json_vals_bytearray = json_file.readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(json_vals_bytearray);
    QJsonObject marshall_json = json_doc.object();

    QJsonObject lv1 = marshall_json["level_1"].toObject(), lv2 = marshall_json["level_2"].toObject(), lv3 = marshall_json["level_3"].toObject(), lv4 = marshall_json["level_4"].toObject(), lv5 = marshall_json["level_5"].toObject();

    std::string output_html_path = cwd.filePath("html/marshall.html").toStdString();
    std::ofstream output_html_file(output_html_path, std::ios::out);

    if (output_html_file.is_open())
    {
        qDebug() << "output html file opened";

        QFile template_file(":/templates/templates/Marshall_test_data.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {

            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush = "";
                    double topushf = -999999;
                    double sum = 0;


                    switch (token) {
                     case 1:
                        topush = ui->marshal_nameofclient->toPlainText().toStdString();
                        break;
                     case 2:
                        topush = ui->marshal_nameofcontractor->toPlainText().toStdString();
                        break;
                     case 3:
                        topushf = ui->marshall_ring->text().toDouble();
                        break;
                     case 35:
                        for (int j = 1; j <= 3; j++) {
                            sum += lv1[QString::fromStdString("marshall_1_%1").arg(j)+QString::number(3)].toDouble();
                        }
                        topushf = sum/3;
                        break;
                     case 36:
                        for (int j = 1; j <= 3; j++) {
                            sum += lv1[QString::fromStdString("marshall_corrected_load_1_%1").arg(j)].toDouble();
                        }
                        topushf = sum/3;
                        break;
                     case 37:
                        for (int j = 1; j <= 3; j++) {
                            sum += lv1[QString::fromStdString("marshall_1_%1").arg(6 + j*10)].toDouble();
                        }
                        topushf = sum/3;
                        break;
                     case 68:
                        for (int j = 1; j <= 3; j++) {
                            sum += lv2[QString::fromStdString("marshall_2_%1").arg(j)+QString::number(3)].toDouble();
                        }
                        topushf = sum/3;
                        break;
                     case 69:
                        for (int j = 1; j <= 3; j++) {
                            sum += lv2[QString::fromStdString("marshall_corrected_load_2_%1").arg(j)].toDouble();
                        }
                        topushf = sum/3;
                        break;
                     case 70:
                        for (int j = 1; j <= 3; j++) {
                            sum += lv2[QString::fromStdString("marshall_2_%1").arg(6 + j*10)].toDouble();
                        }
                        topushf = sum/3;
                        break;
                     case 101:
                        for (int j = 1; j <= 3; j++) {
                            sum += lv3[QString::fromStdString("marshall_3_%1").arg(j)+QString::number(3)].toDouble();
                        }
                        topushf = sum/3;
                        break;
                     case 102:
                        for (int j = 1; j <= 3; j++) {
                            sum += lv3[QString::fromStdString("marshall_corrected_load_3_%1").arg(j)].toDouble();
                        }
                        topushf = sum/3;
                        break;
                     case 103:
                        for (int j = 1; j <= 3; j++) {
                            sum += lv3[QString::fromStdString("marshall_3_%1").arg(6 + j*10)].toDouble();
                        }
                        topushf = sum/3;
                        break;
                     case 138:
                        for (int j = 1; j <= 3; j++) {
                            sum += lv4[QString::fromStdString("marshall_4_%1").arg(j)+QString::number(3)].toDouble();
                        }
                        topushf = sum/3;
                        break;
                     case 139:
                        for (int j = 1; j <= 3; j++) {
                            sum += lv4[QString::fromStdString("marshall_corrected_load_4_%1").arg(j)].toDouble();
                        }
                        topushf = sum/3;
                        break;
                     case 140:
                        for (int j = 1; j <= 3; j++) {
                            sum += lv4[QString::fromStdString("marshall_4_%1").arg(6 + j*10)].toDouble();
                        }
                        topushf = sum/3;
                        break;
                     case 185:
                        for (int j = 1; j <= 3; j++) {
                            sum += lv5[QString::fromStdString("marshall_5_%1").arg(j)+QString::number(3)].toDouble();
                        }
                        topushf = sum/3;
                        break;
                     case 186:
                        for (int j = 1; j <= 3; j++) {
                            sum += lv5[QString::fromStdString("marshall_corrected_load_5_%1").arg(j)].toDouble();
                        }
                        topushf = sum/3;
                        break;
                     case 187:
                        for (int j = 1; j <= 3; j++) {
                            sum += lv5[QString::fromStdString("marshall_5_%1").arg(6 + j*10)].toDouble();
                        }
                        topushf = sum/3;
                        break;
                     case 500:
                        topushf = lv1["marshall_1_00"].toDouble();
                        break;
                     case 501:
                        topushf = lv2["marshall_2_00"].toDouble();
                        break;
                     case 502:
                        topushf = lv3["marshall_3_00"].toDouble();
                        break;
                     case 503:
                        topushf = lv4["marshall_4_00"].toDouble();
                        break;
                     case 504:
                        topushf = lv5["marshall_5_00"].toDouble();
                        break;
                    }

                     if ((token >= 5)&&(token <= 34)) {
                        token -= 5;
                        int j = token%10;
                        int k = 1 + (int)token/10;

                        if (j == 0) {
                            topushf = lv1[QString::fromStdString("marshall_1_%1%2").arg(k).arg(1)].toDouble();
                        } else if (j == 1) {
                            topushf = lv1[QString::fromStdString("marshall_wt_water_1_%1").arg(k)].toDouble();
                        } else if (j == 2) {
                            topushf = lv1[QString::fromStdString("marshall_1_%1%2").arg(k).arg(2)].toDouble();
                        } else if (j == 3) {
                            topushf = lv1[QString::fromStdString("marshall_vol_1_%1").arg(k)].toDouble();
                        } else if (j == 4) {
                            topushf = lv1[QString::fromStdString("marshall_1_%1%2").arg(k).arg(3)].toDouble();
                        } else if (j == 5) {
                            topushf = lv1[QString::fromStdString("marshall_1_%1%2").arg(k).arg(4)].toDouble();
                        } else if (j == 6) {
                            topushf = lv1[QString::fromStdString("marshall_load_1_%1").arg(k)].toDouble();
                        } else if (j == 7) {
                            topushf = lv1[QString::fromStdString("marshall_1_%1%2").arg(k).arg(5)].toDouble();
                        } else if (j == 8) {
                            topushf = lv1[QString::fromStdString("marshall_corrected_load_1_%1").arg(k)].toDouble();
                        } else if (j == 9) {
                            topushf = lv1[QString::fromStdString("marshall_1_%1%2").arg(k).arg(6)].toDouble();
                        }
                     } else if ((token >= 38)&&(token <= 67)) {
                        token -= 38;
                        int j = token%10;
                        int k = 1 + (int)token/10;

                        if (j == 0) {
                            topushf = lv2[QString::fromStdString("marshall_2_%1%2").arg(k).arg(1)].toDouble();
                        } else if (j == 1) {
                            topushf = lv2[QString::fromStdString("marshall_wt_water_2_%1").arg(k)].toDouble();
                        } else if (j == 2) {
                            topushf = lv2[QString::fromStdString("marshall_2_%1%2").arg(k).arg(2)].toDouble();
                        } else if (j == 3) {
                            topushf = lv2[QString::fromStdString("marshall_vol_2_%1").arg(k)].toDouble();
                        } else if (j == 4) {
                            topushf = lv2[QString::fromStdString("marshall_2_%1%2").arg(k).arg(3)].toDouble();
                        } else if (j == 5) {
                            topushf = lv2[QString::fromStdString("marshall_2_%1%2").arg(k).arg(4)].toDouble();
                        } else if (j == 6) {
                            topushf = lv2[QString::fromStdString("marshall_load_2_%1").arg(k)].toDouble();
                        } else if (j == 7) {
                            topushf = lv2[QString::fromStdString("marshall_2_%1%2").arg(k).arg(5)].toDouble();
                        } else if (j == 8) {
                            topushf = lv2[QString::fromStdString("marshall_corrected_load_2_%1").arg(k)].toDouble();
                        } else if (j == 9) {
                            topushf = lv2[QString::fromStdString("marshall_2_%1%2").arg(k).arg(6)].toDouble();
                        }
                     } else if ((token >= 71)&&(token <= 100)) {
                        token -= 71;
                        int j = token%10;
                        int k = 1 + (int)token/10;

                        if (j == 0) {
                            topushf = lv3[QString::fromStdString("marshall_3_%1%2").arg(k).arg(1)].toDouble();
                        } else if (j == 1) {
                            topushf = lv3[QString::fromStdString("marshall_wt_water_3_%1").arg(k)].toDouble();
                        } else if (j == 2) {
                            topushf = lv3[QString::fromStdString("marshall_3_%1%2").arg(k).arg(2)].toDouble();
                        } else if (j == 3) {
                            topushf = lv3[QString::fromStdString("marshall_vol_3_%1").arg(k)].toDouble();
                        } else if (j == 4) {
                            topushf = lv3[QString::fromStdString("marshall_3_%1%2").arg(k).arg(3)].toDouble();
                        } else if (j == 5) {
                            topushf = lv3[QString::fromStdString("marshall_3_%1%2").arg(k).arg(4)].toDouble();
                        } else if (j == 6) {
                            topushf = lv3[QString::fromStdString("marshall_load_3_%1").arg(k)].toDouble();
                        } else if (j == 7) {
                            topushf = lv3[QString::fromStdString("marshall_3_%1%2").arg(k).arg(5)].toDouble();
                        } else if (j == 8) {
                            topushf = lv3[QString::fromStdString("marshall_corrected_load_3_%1").arg(k)].toDouble();
                        } else if (j == 9) {
                            topushf = lv3[QString::fromStdString("marshall_3_%1%2").arg(k).arg(6)].toDouble();
                        }
                     } else if ((token >= 104)&&(token <= 133)) {
                        token -= 104;
                        int j = token%10;
                        int k = 1 + (int)token/10;

                        if (j == 0) {
                            topushf = lv4[QString::fromStdString("marshall_4_%1%2").arg(k).arg(1)].toDouble();
                        } else if (j == 1) {
                            topushf = lv4[QString::fromStdString("marshall_wt_water_4_%1").arg(k)].toDouble();
                        } else if (j == 2) {
                            topushf = lv4[QString::fromStdString("marshall_4_%1%2").arg(k).arg(2)].toDouble();
                        } else if (j == 3) {
                            topushf = lv4[QString::fromStdString("marshall_vol_4_%1").arg(k)].toDouble();
                        } else if (j == 4) {
                            topushf = lv4[QString::fromStdString("marshall_4_%1%2").arg(k).arg(3)].toDouble();
                        } else if (j == 5) {
                            topushf = lv4[QString::fromStdString("marshall_4_%1%2").arg(k).arg(4)].toDouble();
                        } else if (j == 6) {
                            topushf = lv4[QString::fromStdString("marshall_load_4_%1").arg(k)].toDouble();
                        } else if (j == 7) {
                            topushf = lv4[QString::fromStdString("marshall_4_%1%2").arg(k).arg(5)].toDouble();
                        } else if (j == 8) {
                            topushf = lv4[QString::fromStdString("marshall_corrected_load_4_%1").arg(k)].toDouble();
                        } else if (j == 9) {
                            topushf = lv4[QString::fromStdString("marshall_4_%1%2").arg(k).arg(6)].toDouble();
                        }
                     } else if ((token >= 142)&&(token <= 170)) {
                        token -= 142;
                        int j = token%10;
                        int k = 1 + (int)token/10;

                        if (j == 0) {
                            topushf = lv5[QString::fromStdString("marshall_5_%1%2").arg(k).arg(1)].toDouble();
                        } else if (j == 1) {
                            topushf = lv5[QString::fromStdString("marshall_wt_water_5_%1").arg(k)].toDouble();
                        } else if (j == 2) {
                            topushf = lv5[QString::fromStdString("marshall_5_%1%2").arg(k).arg(2)].toDouble();
                        } else if (j == 3) {
                            topushf = lv5[QString::fromStdString("marshall_vol_5_%1").arg(k)].toDouble();
                        } else if (j == 4) {
                            topushf = lv5[QString::fromStdString("marshall_5_%1%2").arg(k).arg(3)].toDouble();
                        } else if (j == 5) {
                            topushf = lv5[QString::fromStdString("marshall_5_%1%2").arg(k).arg(4)].toDouble();
                        } else if (j == 6) {
                            topushf = lv5[QString::fromStdString("marshall_load_5_%1").arg(k)].toDouble();
                        } else if (j == 7) {
                            topushf = lv5[QString::fromStdString("marshall_5_%1%2").arg(k).arg(5)].toDouble();
                        } else if (j == 8) {
                            topushf = lv5[QString::fromStdString("marshall_corrected_load_5_%1").arg(k)].toDouble();
                        } else if (j == 9) {
                            topushf = lv5[QString::fromStdString("marshall_5_%1%2").arg(k).arg(6)].toDouble();
                        }
                     }

                    output_html_file << topush;
                    if (topushf != -999999) {output_html_file << topushf;}
                }
                else
                {
                    output_html_file << line[i];
                }
            }
        }
        json_file.close();
        output_html_file.close();
        qDebug() << "file written to";

        template_file.close();
    }
    else
    {
        qDebug() << "tensile output html file not opened";
    }
}
void MainWindow::generate_html_vol() {
    qDebug() << "beginning vol save...";
    QString json_path = cwd.filePath("json/vol.json");

    QFile json_file(json_path);
    if (!json_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "json file not opened";
        return;
    }
    else
    {
        qDebug() << "json file opened";
    }
    QByteArray json_vals_bytearray = json_file.readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(json_vals_bytearray);
    QJsonObject json_obj = json_doc.object();
    QJsonObject spg_json = json_obj["spg"].toObject(), vol_json = json_obj["composition"].toObject();

    std::string determination_of_eff_spg_path = cwd.filePath("html/determination_of_eff_spg.html").toStdString();
    std::ofstream determination_of_eff_spg_file(determination_of_eff_spg_path, std::ios::out);

    if (determination_of_eff_spg_file.is_open())
    {
        qDebug() << "output html file opened";

        QFile template_file(":/templates/templates/detemination_of_spc_gravity.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {

            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush = "";
                    double topushf;

                    switch (token) {
                    case 1:
                        topush = ui->vol_bsc_1->toPlainText().toStdString();
                        break;
                    case 2:
                        topush = ui->vol_bsc_2->toPlainText().toStdString();
                        break;
                    case 3:
                        topush = "25-16 mm";
                        break;
                    case 7:
                        topush = "16-4.75 mm";
                        break;
                    case 11:
                        topush = "4.75 down";
                        break;
                    default:
                        if ((token - 4)%4 == 0) {
                            topushf = spg_json[QString::fromStdString("vol_bulk_%1").arg(1 + (int)(token-4)/4)].toDouble();
                        } else if ((token - 4)%4 == 1) {
                            topushf = spg_json[QString::fromStdString("vol_apparent_%1").arg(1 + (int)(token-4)/4)].toDouble();
                        } else if ((token - 4)%4 == 2) {
                            topushf = spg_json[QString::fromStdString("vol_eff_%1").arg(1 + (int)(token-4)/4)].toDouble();
                        }
                        determination_of_eff_spg_file << topushf;
                    }


                    determination_of_eff_spg_file << topush;
                }
                else
                {
                    determination_of_eff_spg_file << line[i];
                }
            }
        }
        json_file.close();
        determination_of_eff_spg_file.close();
        qDebug() << "file written to";

        template_file.close();
    }

    std::string vol_output_path = cwd.filePath("html/vol_analysis.html").toStdString();
    std::ofstream vol_output_file(vol_output_path, std::ios::out);

    std::vector<QString>row_vol = {"vol_p1_%1", "vol_p2_%1", "vol_p3_%1", "vol_ps_%1", "vol_pb_%1", "vol_gsb_%1", "vol_gmm_%1", "vol_gmb_%1", "vol_vma_%1", "vol_va_%1", "vol_vfb_%1"};
    std::vector<QString>spg_vol = {"vol_%1_1", "vol_%1_2", "vol_%1_3"};

    if (vol_output_file.is_open())
    {
        qDebug() << "output html file opened";

        QFile template_file(":/templates/templates/worksheet_volumetric_analysis.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {

            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush = "";
                    double topushf;

                    if (token <= 24) {
                        token--;

                        int token_hpos = token%8;
                        int token_vpos = (int)token/8;

                        if (token_hpos < 3) {
                            std::map<int, QString> hpos_spg_map = {{0, "apparent"}, {1, "bulk"}, {2, "eff"}};

                            QString k = spg_vol[token_vpos].arg(hpos_spg_map[token_hpos]);
                            topushf = spg_json[k].toDouble();
                        } else {
                            QString k = row_vol[token_vpos].arg(token_hpos-2);
                            topushf = vol_json[k].toDouble();
                        }
                    } else if (token <= 40) {

                        if (token == 25) {
                            topushf = spg_json["vol_total_1"].toDouble();
                        } else if (token == 26) {
                            topushf = spg_json["vol_total_2"].toDouble();
                        } else if (token == 33) {
                            topushf = spg_json["vol_btmn_1"].toDouble();
                        } else if (token == 34) {
                            topushf = spg_json["vol_btmn_2"].toDouble();
                        } else {
                            token--;

                            int token_hpos = token%8;
                            int token_vpos = (int)token/8;

                            QString k = row_vol[token_vpos].arg(token_hpos-2);
                            topushf = vol_json[k].toDouble();
                        }
                    } else if (token <= 76) {
                        token -= 41;

                        int token_hpos = token%6;
                        int token_vpos = 5 + (int)token/6;

                        if (token_hpos == 0) {continue;}

                        QString k = row_vol[token_vpos].arg(token_hpos);
                        topushf = vol_json[k].toDouble();
                    } else {
                        qDebug() << "error at generating normal volumetric worksheet html file: token outside bound 76";
                    }

                    vol_output_file << topush;
                    vol_output_file << topushf;
                }
                else
                {
                    vol_output_file << line[i];
                }
            }
        }
        json_file.close();
        vol_output_file.close();
        qDebug() << "file written to";

        template_file.close();
    }

    std::string obc_output_path = cwd.filePath("html/vol_obc_analysis.html").toStdString();
    std::ofstream obc_output_file(obc_output_path, std::ios::out);

    std::map<int, QString> obc_vals_map = {
        {16, QString("vol_p1_%1").arg("obc")},
        {17, QString("vol_p2_%1").arg("obc")},
        {18, QString("vol_p3_%1").arg("obc")},
        {19, QString("vol_ps_%1").arg("obc")},
        {20, QString("vol_pb_%1").arg("obc")},
        {21, QString("vol_gsb_%1").arg("obc")},
        {22, QString("vol_gmm_%1").arg("obc")},
        {23, QString("vol_gmb_%1").arg("obc")},
        {24, QString("vol_vma_%1").arg("obc")},
        {25, QString("vol_va_%1").arg("obc")},
        {26, QString("vol_vfb_%1").arg("obc")}
    };
    std::map<int, QString> obc_spg_map = {{1, "vol_apparent_%1"},
                                                {2, "vol_bulk_%1"},
                                                {3, "vol_eff_%1"}};

    if (obc_output_file.is_open())
    {
        qDebug() << "output html file opened";

        QFile template_file(":/templates/templates/worksheet_volumetric_analysis_obc.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {

            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush = "";

                    if (token == 1) {
                        topush = ui->vol_bsc_1->toPlainText().toStdString();
                    } else if (token == 2) {
                        topush = ui->vol_bsc_2->toPlainText().toStdString();
                    } else if ((token >= 3)&&(token <= 11)) {
                        int token_row = 1 + token%3;
                        int token_col = (int)token/3;

                        QString db_query = obc_spg_map[token_col].arg(token_row);
                        double db_val = spg_json[db_query].toDouble();

                        obc_output_file << db_val;
                    } else if (token >= 16) {
                        QString db_query = obc_vals_map[token];
                        double db_val = vol_json[db_query].toDouble();

                        obc_output_file << db_val;
                    } else if (token == 12) {
                        double db_val = spg_json["vol_total_1"].toDouble();

                        obc_output_file << db_val;
                    } else if (token == 13) {
                        double db_val = spg_json["vol_total_2"].toDouble();

                        obc_output_file << db_val;
                    } else if (token == 14) {
                        double db_val = spg_json["vol_btmn_1"].toDouble();

                        obc_output_file << db_val;
                    } else if (token == 15) {
                        double db_val = spg_json["vol_btmn_2"].toDouble();

                        obc_output_file << db_val;
                    }

                    obc_output_file << topush;
                }
                else
                {
                    obc_output_file << line[i];
                }
            }
        }
        json_file.close();
        obc_output_file.close();
        qDebug() << "file written to";

        template_file.close();
    }
}
void MainWindow::generate_html_gmm() {
    qDebug() << "beginning gmm save...";

    QString json_path = cwd.filePath("json/gmm.json");
    QFile json_file(json_path);
    if (!json_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "gmm json file not opened";
        return;
    }
    else
    {
        qDebug() << "gmm json file opened";
    }
    QByteArray json_vals_bytearray = json_file.readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(json_vals_bytearray);
    QJsonObject gmm_json = json_doc.object();


    QString vol_json_path = cwd.filePath("json/vol.json");
    QFile vol_json_file(vol_json_path);
    if (!vol_json_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "vol json file not opened";
        return;
    }
    else
    {
        qDebug() << "vol json file opened";
    }
    QByteArray vol_vals_bytearray = vol_json_file.readAll();
    QJsonDocument vol_json_doc = QJsonDocument::fromJson(vol_vals_bytearray);
    QJsonObject vol_json = vol_json_doc.object();
    QJsonObject vol_comp_json = vol_json["composition"].toObject();

    QString rheology_json_path = cwd.filePath("json/rheology.json");
    QFile rheology_json_file(rheology_json_path);
    if (!rheology_json_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "rheology json file not opened";
        return;
    }
    else
    {
        qDebug() << "rheology json file opened";
    }
    QByteArray rheology_vals_bytearray = rheology_json_file.readAll();
    QJsonDocument rheology_json_doc = QJsonDocument::fromJson(rheology_vals_bytearray);
    QJsonObject rheology_json = rheology_json_doc.object();
    QJsonObject rhspc_json = rheology_json["spc"].toObject();

    std::vector<QString> gmm_json_titles = {"4.00", "4.25", "4.50"};

    for (QString title: gmm_json_titles) {

        std::string gmm_html_path = cwd.filePath(QString::fromStdString("html/gmm_%1.html").arg(title)).toStdString();
        std::ofstream gmm_html_file(gmm_html_path, std::ios::out);

        if (gmm_html_file.is_open())
        {
            qDebug() << "gmm html file" << title <<  "opened";

            QJsonObject gmm_obj = gmm_json[title].toObject();

            QFile template_file(":/templates/templates/GMM_pg18.html");
            if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                qDebug() << "html not opened";
                return;
            }
            else
            {
                qDebug() << "html file opened";
            }
            QTextStream infile(&template_file);

            while (!infile.atEnd())
            {

                std::string line_str = infile.readLine().toStdString();
                const char *line = line_str.c_str();
                int tilda = 0;
                int token;
                for (int i = 0; i < (int)strlen(line); i++)
                {
                    if (line[i] == '~' && tilda == 0)
                    {
                        tilda = 1;

                        // Gets the token from HTML file
                        for (int j = i + 1; j < (int)strlen(line); j++)
                        {
                            if (line[j] == '~' && j - i == 2)
                            {
                                token = (int)line[i + 1] - 48;
                                i = j;
                                break;
                            }
                            else if (line[j] == '~' && j - i == 3)
                            {
                                token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                                i = j;
                                break;
                            }
                            else if (line[j] == '~' && j - i == 4)
                            {
                                token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                                i = j;
                                break;
                            }
                        }

                        std::string topush;
                        double topushf = -12362701;

                        switch (token) {
                        case 1:
                            topush = ui->gmm_work->toHtml().toStdString();
                            break;
                        case 2:
                            topush = ui->gmm_name->toHtml().toStdString();
                            break;
                        case 3:
                            topush = ui->gmm_consultant->toHtml().toStdString();
                            break;
                        case 4:
                            topush = ui->gmm_contractor->toHtml().toStdString();
                            break;
                        case 5:
                            topush = ui->gmm_exp_1->text().toStdString();
                            break;
                        case 6:
                            topush = ui->gmm_exp_3->text().toStdString();
                            break;
                        case 7:
                            topush = ui->gmm_exp_2->text().toStdString();
                            break;
                        case 8:
                            topushf = rhspc_json["rheology_spc_mean"].toDouble();
                            break;
                        case 9:
                            topushf = vol_comp_json["vol_gsb_obc"].toDouble();
                            break;
                        case 10:
                            topush = title.toStdString();
                            break;
                        case 21:
                            topushf = gmm_obj["avg"].toDouble();
                            break;
                        default:
                            int j = (int)((token-9)/2);
                            int k = token%2? 2:1;
                            topushf = gmm_obj[QString::fromStdString("gmm_%1_%2").arg(j).arg(k)].toDouble();
                            break;
                        }

                        if (topushf != -12362701) {gmm_html_file << topushf;}
                        gmm_html_file << topush;
                    }
                    else
                    {
                        gmm_html_file << line[i];
                    }
                }
            }
            json_file.close();
            gmm_html_file.close();
            qDebug() << "file written to";

            template_file.close();
        }
        else
        {
            qDebug() << "gmm output html file not opened";
        }

    }

}
void MainWindow::generate_html_rheology() {

    qDebug() << "beginning rheology save...";
    QString json_path = cwd.filePath("json/rheology.json");

    QFile json_file(json_path);
    if (!json_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "json file not opened";
        return;
    }
    else
    {
        qDebug() << "json file opened";
    }
    QByteArray json_vals_bytearray = json_file.readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(json_vals_bytearray);
    QJsonObject rheology_json = json_doc.object();

    std::string ductility_html_path = cwd.filePath("html/ductility.html").toStdString();
    std::ofstream ductility_html_file(ductility_html_path, std::ios::out);

    if (ductility_html_file.is_open())
    {
        qDebug() << "ductility html file opened";

        QJsonObject duc_json = rheology_json["ductility"].toObject();

        QFile template_file(":/templates/templates/rheology_duktility.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {

            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush;
                    double topushf;

                    //bool ok;
                    switch (token) {
                    case 100:
                        topush = ui->rheology_name->toPlainText().toStdString();
                        break;
                    case 101:
                        topush = ui->rheology_contractor->toPlainText().toStdString();
                        break;
                    case 1:
                        topush = ui->rh_loc->text().toStdString();
                        break;
                    case 2:
                        topush = ui->rh_sample->text().toStdString();
                        break;
                    case 3:
                        topush = ui->rh_src->text().toStdString();
                        break;
                    case 4:
                        topush = ui->rh_test->text().toStdString();
                        break;
                    case 5:
                        topush = ui->rh_grade->text().toStdString();
                        break;
                    default:
                        topushf = duc_json[QString::fromStdString("rheology_ductility_%1").arg(token-5)].toDouble();
                        ductility_html_file << topushf;
                    }

                    ductility_html_file << topush;
                }
                else
                {
                    ductility_html_file << line[i];
                }
            }
        }
        json_file.close();
        ductility_html_file.close();
        qDebug() << "file written to";

        template_file.close();
    }
    else
    {
        qDebug() << "tensile output html file not opened";
    }

    std::string flash_html_path = cwd.filePath("html/flash.html").toStdString();
    std::ofstream flash_html_file(flash_html_path, std::ios::out);

    if (flash_html_file.is_open())
    {
        qDebug() << "flash html file opened";

        QJsonObject flash_json = rheology_json["flash"].toObject();

        QFile template_file(":/templates/templates/rheology_flash.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {

            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush;
                    double topushf;

                    //bool ok;
                    switch (token) {
                    case 1:
                        topush = ui->rheology_name->toPlainText().toStdString();
                        break;
                    case 2:
                        topush = ui->rheology_contractor->toPlainText().toStdString();
                        break;
                    case 11:
                        topush = ui->rh_loc->text().toStdString();
                        break;
                    case 4:
                        topush = ui->rh_sample->text().toStdString();
                        break;
                    case 5:
                        topush = ui->rh_src->text().toStdString();
                        break;
                    case 6:
                        topush = ui->rh_test->text().toStdString();
                        break;
                    case 7:
                        topush = ui->rh_grade->text().toStdString();
                        break;
                    default:
                        topushf = flash_json[QString::fromStdString("rheology_flash_%1").arg(token-12)].toDouble();
                        flash_html_file << topushf;
                    }

                    flash_html_file << topush;
                }
                else
                {
                    flash_html_file << line[i];
                }
            }
        }
        json_file.close();
        flash_html_file.close();
        qDebug() << "file written to";

        template_file.close();
    }
    else
    {
        qDebug() << "flash output html file not opened";
    }

    std::string pen_html_path = cwd.filePath("html/penetration.html").toStdString();
    std::ofstream pen_html_file(pen_html_path, std::ios::out);

    if (pen_html_file.is_open())
    {
        qDebug() << "penetration html file opened";

        QJsonObject penetration_json = rheology_json["pen"].toObject();

        QFile template_file(":/templates/templates/rheology_penetration.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {

            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush;
                    double topushf;

                    //bool ok;
                    switch (token) {
                    case 100:
                        topush = ui->rheology_name->toPlainText().toStdString();
                        break;
                    case 200:
                        topush = ui->rheology_contractor->toPlainText().toStdString();
                        break;
                    case 1:
                        topush = ui->rh_loc->text().toStdString();
                        break;
                    case 2:
                        topush = ui->rh_sample->text().toStdString();
                        break;
                    case 3:
                        topush = ui->rh_src->text().toStdString();
                        break;
                    case 4:
                        topush = ui->rh_test->text().toStdString();
                        break;
                    case 5:
                        topush = ui->rh_grade->text().toStdString();
                        break;
                    default:
                        topushf = penetration_json[QString::fromStdString("rheology_pen_%1").arg(token-6)].toDouble();
                        pen_html_file << topushf;
                    }

                    pen_html_file << topush;
                }
                else
                {
                    pen_html_file << line[i];
                }
            }
        }
        json_file.close();
        pen_html_file.close();
        qDebug() << "file written to";

        template_file.close();
    }
    else
    {
        qDebug() << "pen output html file not opened";
    }

    std::string soft_html_path = cwd.filePath("html/softening.html").toStdString();
    std::ofstream soft_html_file(soft_html_path, std::ios::out);

    if (soft_html_file.is_open())
    {
        qDebug() << "soft html file opened";

        QJsonObject soft_json = rheology_json["soft"].toObject();

        QFile template_file(":/templates/templates/rheology_softening.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {

            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush;
                    double topushf;

                    //bool ok;
                    switch (token) {
                    case 100:
                        topush = ui->rheology_name->toPlainText().toStdString();
                        break;
                    case 200:
                        topush = ui->rheology_contractor->toPlainText().toStdString();
                        break;
                    case 1:
                        topush = ui->rh_src->text().toStdString();
                        break;
                    case 2:
                        topush = ui->rh_sample->text().toStdString();
                        break;
                    case 3:
                        topush = ui->rh_manufacturer->text().toStdString();
                        break;
                    case 4:
                        topush = ui->rh_test->text().toStdString();
                        break;
                    case 5:
                        topush = ui->rh_grade->text().toStdString();
                        break;
                    default:
                        topushf = soft_json[QString::fromStdString("rheology_soft_%1").arg(token-5)].toDouble();
                        soft_html_file << topushf;
                    }

                    soft_html_file << topush;
                }
                else
                {
                    soft_html_file << line[i];
                }
            }
        }
        json_file.close();
        soft_html_file.close();
        qDebug() << "file written to";

        template_file.close();
    }
    else
    {
        qDebug() << "soft output html file not opened";
    }

    std::string spc_html_path = cwd.filePath("html/rh_spc.html").toStdString();
    std::ofstream spc_html_file(spc_html_path, std::ios::out);

    if (spc_html_file.is_open())
    {
        qDebug() << "soft html file opened";

        QJsonObject spc_json = rheology_json["spc"].toObject();

        QFile template_file(":/templates/templates/rheology_spc.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {

            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush;
                    double topushf;

                    //bool ok;
                    switch (token) {
                    case 1:
                        topush = ui->rheology_name->toPlainText().toStdString();
                        break;
                    case 2:
                        topush = ui->rheology_contractor->toPlainText().toStdString();
                        break;
                    case 3:
                        topush = ui->rh_loc->text().toStdString();
                        break;
                    case 4:
                        topush = ui->rh_sample->text().toStdString();
                        break;
                    case 5:
                        topush = ui->rh_src->text().toStdString();
                        break;
                    case 6:
                        topush = ui->rh_test->text().toStdString();
                        break;
                    case 7:
                        topush = ui->rh_grade->text().toStdString();
                        break;
                    case 8:
                        topush = ui->rh_type->text().toStdString();
                        break;
                    case 21:
                        topushf = spc_json["rheology_spc_1"].toDouble();
                        spc_html_file << topushf;
                        break;
                    case 22:
                        topushf = spc_json["rheology_spc_2"].toDouble();
                        spc_html_file << topushf;
                        break;
                    case 23:
                        topushf = spc_json["rheology_spc_3"].toDouble();
                        spc_html_file << topushf;
                        break;
                    case 24:
                        topushf = spc_json["rheology_spc_mean"].toDouble();
                        spc_html_file << topushf;
                        break;
                    case 9:
                        topushf = spc_json["rheology_spc_1_1"].toDouble();
                        spc_html_file << topushf;
                        break;
                    case 10:
                        topushf = spc_json["rheology_spc_1_2"].toDouble();
                        spc_html_file << topushf;
                        break;
                    case 11:
                        topushf = spc_json["rheology_spc_1_3"].toDouble();
                        spc_html_file << topushf;
                        break;
                    case 12:
                        topushf = spc_json["rheology_spc_2_1"].toDouble();
                        spc_html_file << topushf;
                        break;
                    case 13:
                        topushf = spc_json["rheology_spc_2_2"].toDouble();
                        spc_html_file << topushf;
                        break;
                    case 14:
                        topushf = spc_json["rheology_spc_2_3"].toDouble();
                        spc_html_file << topushf;
                        break;
                    case 15:
                        topushf = spc_json["rheology_spc_3_1"].toDouble();
                        spc_html_file << topushf;
                        break;
                    case 16:
                        topushf = spc_json["rheology_spc_3_2"].toDouble();
                        spc_html_file << topushf;
                        break;
                    case 17:
                        topushf = spc_json["rheology_spc_3_3"].toDouble();
                        spc_html_file << topushf;
                        break;
                    case 18:
                        topushf = spc_json["rheology_spc_4_1"].toDouble();
                        spc_html_file << topushf;
                        break;
                    case 19:
                        topushf = spc_json["rheology_spc_4_2"].toDouble();
                        spc_html_file << topushf;
                        break;
                    case 20:
                        topushf = spc_json["rheology_spc_4_3"].toDouble();
                        spc_html_file << topushf;
                        break;
                    }

                    spc_html_file << topush;
                }
                else
                {
                    spc_html_file << line[i];
                }
            }
        }
        json_file.close();
        spc_html_file.close();
        qDebug() << "file written to";

        template_file.close();
    }
    else
    {
        qDebug() << "spc output html file not opened";
    }

    std::string striping_html_path = cwd.filePath("html/strip.html").toStdString();
    std::ofstream striping_html_file(striping_html_path, std::ios::out);

    if (striping_html_file.is_open())
    {
        qDebug() << "soft html file opened";

        QJsonObject strip_json = rheology_json["strip"].toObject();

        QFile template_file(":/templates/templates/rheology_striping.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {

            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush;
                    double topushf;

                    //bool ok;
                    switch (token) {
                    case 1:
                        topush = ui->rheology_name->toPlainText().toStdString();
                        break;
                    case 2:
                        topush = ui->rheology_contractor->toPlainText().toStdString();
                        break;
                    case 3:
                        topush = ui->rh_size->text().toStdString();
                        break;
                    case 4:
                        topush = ui->rh_sample->text().toStdString();
                        break;
                    case 5:
                        topush = ui->rh_grade->text().toStdString();
                        break;
                    case 6:
                        topush = ui->rh_test->text().toStdString();
                        break;
                    case 7:
                        topush = ui->rh_src->text().toStdString();
                        break;
                    default:
                        int odd = token%2;
                        token = (int) (token-odd)/2;
                        token -= 3;

                        int to_get = token*10;
                        to_get += 1 + odd;

                        topush = strip_json[QString::fromStdString("rheology_strip_%1").arg(to_get)].toString().toStdString();
                        break;
                    }

                    striping_html_file << topush;
                }
                else
                {
                    striping_html_file << line[i];
                }
            }
        }
        json_file.close();
        striping_html_file.close();
        qDebug() << "file written to";

        template_file.close();
    }
    else
    {
        qDebug() << "strip output html file not opened";
    }

    std::string viscosity_html_path = cwd.filePath("html/rh_vis.html").toStdString();
    std::ofstream viscosity_html_file(viscosity_html_path, std::ios::out);

    if (viscosity_html_file.is_open())
    {
        qDebug() << "vis html file opened";

        QJsonObject rh_vis_json = rheology_json["viscosity"].toObject();

        QFile template_file(":/templates/templates/rheology_viscosity.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {

            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush;
                    double topushf;

                    //bool ok;
                    switch (token) {
                    case 1:
                        topush = ui->rheology_name->toPlainText().toStdString();
                        break;
                    case 2:
                        topush = ui->rheology_contractor->toPlainText().toStdString();
                        break;
                    case 4:
                        topush = ui->rh_sample->text().toStdString();
                        break;
                    case 5:
                        topush = ui->rh_src->text().toStdString();
                        break;
                    case 6:
                        topush = ui->rh_test->text().toStdString();
                        break;
                    case 7:
                        topush = ui->rh_grade->text().toStdString();
                        break;
                    default:
                        topushf = rh_vis_json[QString::fromStdString("rheology_viscosity_%1").arg(token-10)].toDouble();
                        viscosity_html_file << topushf;
                        break;
                    }

                    viscosity_html_file << topush;
                }
                else
                {
                    viscosity_html_file << line[i];
                }
            }
        }
        json_file.close();
        viscosity_html_file.close();
        qDebug() << "file written to";

        template_file.close();
    }
    else
    {
        qDebug() << "viscosity output html file not opened";
    }
}
void MainWindow::generate_html_wa() {
    qDebug() << "beginning wa save...";

    QString json_path = cwd.filePath("json/wa.json");
    QFile json_file(json_path);
    if (!json_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "json file not opened";
        return;
    }
    else
    {
        qDebug() << "json file opened";
    }
    QByteArray json_vals_bytearray = json_file.readAll();
    QJsonDocument json_doc = QJsonDocument::fromJson(json_vals_bytearray);
    QJsonObject wa_json = json_doc.object();
    QJsonObject wa_10 = wa_json["10"].toObject();
    QJsonObject waa_10 = wa_json["answer_10"].toObject();
    QJsonObject wa_20 = wa_json["20"].toObject();
    QJsonObject waa_20 = wa_json["answer_20"].toObject();
    QJsonObject wa_dust = wa_json["dust"].toObject();
    QJsonObject waa_dust = wa_json["answer_dust"].toObject();
    QJsonObject wa_cleanliness = wa_json["cleanliness"].toObject();
    QJsonObject waa_cleanliness = wa_json["answer_cleanliness"].toObject();

    std::string wa_path_10 = cwd.filePath("html/wa_10.html").toStdString();
    std::ofstream wa_file_10(wa_path_10, std::ios::out);

    if (wa_file_10.is_open())
    {
        qDebug() << "output html file opened";

        QFile template_file(":/templates/templates/gravity_and_water_absorption.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {
            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush = "";
                    double topushf = -12362701;

                    if ((token >= 1) && (token <= 8)) {
                        int j = (token + 1) / 2;  // Calculate the first index after "wa_"
                        int k = token % 2 == 0 ? 2 : 1;  // Calculate the second index
                        std::string key = "wa_" + std::to_string(j) + "_" + std::to_string(k);
                        topushf = wa_10[QString::fromStdString(key)].toDouble();
                    } else if ((token >= 9) && (token <= 14)) {
                        int j = (token-7) / 2;
                        int k = (token-8) % 2 == 0 ? 2 : 1;
                        std::string key = "answer_" + std::to_string(j) + "_" + std::to_string(k);
                        topushf = waa_10[QString::fromStdString(key)].toDouble();
                    } else if (token <= 17) {
                        int k = token-14;
                        topushf = wa_10[QString::fromStdString("wa10_average_%1").arg(k)].toDouble();
                    }

                    if (topushf != -12362701) {wa_file_10 << topushf;}
                    wa_file_10 << topush;
                }
                else
                {
                    wa_file_10 << line[i];
                }
            }
        }
        json_file.close();
        wa_file_10.close();
        qDebug() << "file written to";

        template_file.close();
    }

    std::string wa_path_20 = cwd.filePath("html/wa_20.html").toStdString();
    std::ofstream wa_file_20(wa_path_20, std::ios::out);

    if (wa_file_20.is_open())
    {
        qDebug() << "output html file opened";

        QFile template_file(":/templates/templates/gravity_and_water_absorption.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {
            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush = "";
                    double topushf = -12362701;

                    if ((token >= 1) && (token <= 8)) {
                        int j = (token + 1) / 2;  // Calculate the first index after "wa_"
                        int k = token % 2 == 0 ? 2 : 1;  // Calculate the second index
                        std::string key = "wa_" + std::to_string(j) + "_" + std::to_string(k);
                        topushf = wa_20[QString::fromStdString(key)].toDouble();
                    } else if ((token >= 9) && (token <= 14)) {
                        int j = (token-7) / 2;
                        int k = (token-8) % 2 == 0 ? 2 : 1;
                        std::string key = "answer_" + std::to_string(j) + "_" + std::to_string(k);
                        topushf = waa_20[QString::fromStdString(key)].toDouble();
                    } else if (token <= 17) {
                        int k = token-14;
                        topushf = wa_20[QString::fromStdString("wa20_average_%1").arg(k)].toDouble();
                    }

                    if (topushf != -12362701) {wa_file_20 << topushf;}
                    wa_file_20 << topush;
                }
                else
                {
                    wa_file_20 << line[i];
                }
            }
        }
        json_file.close();
        wa_file_20.close();
        qDebug() << "file written to";

        template_file.close();
    }

    std::string wa_path_dust = cwd.filePath("html/wa_dust.html").toStdString();
    std::ofstream wa_file_dust(wa_path_dust, std::ios::out);

    if (wa_file_dust.is_open())
    {
        qDebug() << "output html file opened";

        QFile template_file(":/templates/templates/gravity_and_water_absorption_fine.html");
        if (!template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
            return;
        }
        else
        {
            qDebug() << "html file opened";
        }
        QTextStream infile(&template_file);

        while (!infile.atEnd())
        {
            std::string line_str = infile.readLine().toStdString();
            const char *line = line_str.c_str();
            int tilda = 0;
            int token;
            for (int i = 0; i < (int)strlen(line); i++)
            {
                if (line[i] == '~' && tilda == 0)
                {
                    tilda = 1;

                    // Gets the token from HTML file
                    for (int j = i + 1; j < (int)strlen(line); j++)
                    {
                        if (line[j] == '~' && j - i == 2)
                        {
                            token = (int)line[i + 1] - 48;
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 3)
                        {
                            token = ((int)line[i + 2] - 48) + 10 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                        else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush = "";
                    double topushf = -12362701;

                    if ((token >= 1) && (token <= 8)) {
                        int j = (token + 1) / 2;  // Calculate the first index after "wa_"
                        int k = token % 2 == 0 ? 2 : 1;  // Calculate the second index
                        std::string key = "wa_" + std::to_string(j) + "_" + std::to_string(k);
                        topushf = wa_dust[QString::fromStdString(key)].toDouble();
                    } else if ((token >= 9) && (token <= 16)) {
                        int j = (token-7) / 2;
                        int k = (token-8) % 2 == 0 ? 2 : 1;
                        std::string key = "answer_" + std::to_string(j) + "_" + std::to_string(k);
                        topushf = waa_dust[QString::fromStdString(key)].toDouble();
                    } else if (token <= 20) {
                        int k = token-14;
                        topushf = wa_dust[QString::fromStdString("wadust_average_%1").arg(k)].toDouble();
                    }

                    if (topushf != -12362701) {wa_file_dust << topushf;}
                    wa_file_dust << topush;
                }
                else
                {
                    wa_file_dust << line[i];
                }
            }
        }
        json_file.close();
        wa_file_dust.close();
        qDebug() << "file written to";

        template_file.close();
    }
}



// Deals with Scrolling
void MainWindow::wheelEvent(QWheelEvent *event)
{
    // the mouse wheel API gives wheel inputs in delta, for most non gaming mice one notch turn a delta of 120
    // setting the sens in this method does not make any sense now, but in the future we will add a mouse sensitivity option in the View QMenuBar to change this
    this->scroll_sens = 20;
    QPoint delta = -1 * event->angleDelta();
    QPointF mouse_pos = event->position();
    int scroll_pos;

    if (mouse_pos.x() > 40 && mouse_pos.y() > 95 && mouse_pos.x() < 1410 && mouse_pos.y() < 760)
    {

        // By the scrollwheel, we are not directly moving any UI element, we are only calling the signal to change the scrollbar of any area.
        // This area has to be the current tab, which is selected with the switch operator
        switch (ui->wmm_tab_list->currentIndex())
        {
        case 0:
            scroll_pos = ui->spc_data_scroll->value();
            ui->spc_data_scroll->setValue((int)(scroll_pos + delta.y() / scroll_sens));
            break;
        case 2:
            scroll_pos = ui->aiv_data_scroll->value();
            ui->aiv_data_scroll->setValue((int)(scroll_pos + delta.y() / scroll_sens));
            break;
        case 3:
            scroll_pos = ui->ind_data_scroll->value();
            ui->ind_data_scroll->setValue((int)(scroll_pos + delta.y() / scroll_sens));
            break;
        default:
            break;
        }
        switch (ui->dbm_tab_list->currentIndex())
        {
        case 0:
            scroll_pos = ui->grad_data_scroll->value();
            ui->grad_data_scroll->setValue((int)(scroll_pos + delta.y() / scroll_sens));
            break;
        case 1:
            scroll_pos = ui->marshall_scroll->value();
            ui->marshall_scroll->setValue((int)(scroll_pos + delta.y() / scroll_sens));
            break;
        case 3:
            scroll_pos = ui->vol_scroll->value();
            ui->vol_scroll->setValue((int)(scroll_pos + delta.y() / scroll_sens));
            break;
        case 4:
            scroll_pos = ui->gmm_scroll->value();
            ui->gmm_scroll->setValue((int)(scroll_pos + delta.y() / scroll_sens));
            break;
        case 5:
            scroll_pos = ui->rheology_scroll->value();
            ui->rheology_scroll->setValue((int)(scroll_pos + delta.y() / scroll_sens));
            break;

        case 6:
            scroll_pos = ui->wa_scroll->value();
            ui->wa_scroll->setValue((int)(scroll_pos + delta.y() / scroll_sens));
            break;
        }
    }
}
void MainWindow::on_spc_data_scroll_valueChanged(int value)
{
    float target = (ui->spc_frame_outer->height() - ui->spc_frame->height()) * value / 100;
    ui->spc_frame->move(0, target);
}
void MainWindow::on_ind_data_scroll_valueChanged(int value)
{
    float target = (ui->ind_frame_outer->height() - ui->ind_frame->height()) * value / 100;
    ui->ind_frame->move(0, target);
}
void MainWindow::on_aiv_data_scroll_valueChanged(int value)
{
    float target = (ui->aiv_frame_outer->height() - ui->aiv_frame->height()) * value / 100;
    ui->aiv_frame->move(0, target);
}
void MainWindow::on_grad_data_scroll_valueChanged(int value)
{
    float target = (ui->grad_frame_outer->height() - ui->grad_frame->height()) * value / 100;
    ui->grad_frame->move(0, target);
}
void MainWindow::on_marshall_scroll_valueChanged(int value)
{
    float target = (ui->marshall_frame_outer->height() - ui->marshall_frame->height()) * value / 100;
    ui->marshall_frame->move(0, target);
}
QVector<int> vol_non_scrolling_elements = {391, 290, 184, 215, 217, 216, 271, 286, 272, 287};
void MainWindow::on_vol_scroll_valueChanged(int value)
{
    float target = (ui->vol_frame_outer->width() - ui->vol_frame->width()) * value / 100;
    ui->vol_frame->move(target, ui->vol_frame->y());

    for (auto i : vol_non_scrolling_elements)
    {
        QString elem_name = QString::fromStdString("label_" + std::to_string(i));
        QLabel *label = ui->vol_frame->findChild<QLabel *>(elem_name);

        if (label)
        {
            label->raise();
            label->move(-1 * target, label->y());
        }
        else
        {
            qDebug() << "element" << i << "not found";
        }
    }
}
void MainWindow::on_gmm_scroll_valueChanged(int value)
{
    float target = (ui->gmm_frame_outer->height() - ui->gmm_frame->height()) * value / 100;
    ui->gmm_frame->move(0, target);
}
void MainWindow::on_rheology_scroll_valueChanged(int value)
{
    float target = (ui->rheology_frame_outer->height() - ui->rheology_frame->height()) * value / 100;
    ui->rheology_frame->move(0, target);
}
void MainWindow::on_wa_scroll_valueChanged(int value)
{
    float target = (ui->wa_frame_outer->height() - ui->wa_frame->height()) * value / 100;
    ui->wa_frame->move(0, target);
}



// Deal with switchin windows
void MainWindow::on_actionWMM_triggered()
{
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_actionDBM_triggered()
{
    ui->stackedWidget->setCurrentIndex(1);
}



// Deals with autoupdating labels on the AIV tab. Also the worst code I have ever written.
void MainWindow::on_aiv_20_21_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_20_11->text().toFloat();
    float t2 = ui->aiv_20_21->text().toFloat();
    float target = t2 - t1;

    // Set precision to two decimal places
    std::setprecision(2);
    std::string target_string = std::to_string(target);

    ui->aiv_20_31->setText(QString::fromStdString(target_string));
}
void MainWindow::on_aiv_20_11_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_20_11->text().toFloat();
    float t2 = ui->aiv_20_21->text().toFloat();
    float target = t2 - t1;

    // Set precision to two decimal places
    std::setprecision(1);
    std::string target_string = std::to_string(target);
    ui->aiv_20_31->setText(QString::fromStdString(target_string));
}
void MainWindow::on_aiv_20_22_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_20_12->text().toFloat();
    float t2 = ui->aiv_20_22->text().toFloat();
    float target = t2 - t1;

    // Set precision to two decimal places
    std::setprecision(2);
    std::string target_string = std::to_string(target);

    ui->aiv_20_32->setText(QString::fromStdString(target_string));
}
void MainWindow::on_aiv_20_12_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_20_12->text().toFloat();
    float t2 = ui->aiv_20_22->text().toFloat();
    float target = t2 - t1;

    // Set precision to two decimal places
    std::setprecision(1);
    std::string target_string = std::to_string(target);

    ui->aiv_20_32->setText(QString::fromStdString(target_string));
}
void MainWindow::on_aiv_20_23_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_20_13->text().toFloat();
    float t2 = ui->aiv_20_23->text().toFloat();
    float target = t2 - t1;

    // Set precision to two decimal places
    std::setprecision(2);
    std::string target_string = std::to_string(target);

    ui->aiv_20_33->setText(QString::fromStdString(target_string));
}
void MainWindow::on_aiv_20_13_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_20_13->text().toFloat();
    float t2 = ui->aiv_20_23->text().toFloat();
    float target = t2 - t1;

    // Set precision to two decimal places
    std::setprecision(1);
    std::string target_string = std::to_string(target);

    ui->aiv_20_33->setText(QString::fromStdString(target_string));
}
void MainWindow::on_aiv_10_21_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_10_11->text().toFloat();
    float t2 = ui->aiv_10_21->text().toFloat();
    float target = t2 - t1;

    // Set precision to two decimal places
    std::setprecision(2);
    std::string target_string = std::to_string(target);

    ui->aiv_10_31->setText(QString::fromStdString(target_string));
}
void MainWindow::on_aiv_10_11_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_10_11->text().toFloat();
    float t2 = ui->aiv_10_21->text().toFloat();
    float target = t2 - t1;

    // Set precision to one decimal place
    std::setprecision(1);
    std::string target_string = std::to_string(target);

    ui->aiv_10_31->setText(QString::fromStdString(target_string));
}
void MainWindow::on_aiv_10_22_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_10_12->text().toFloat();
    float t2 = ui->aiv_10_22->text().toFloat();
    float target = t2 - t1;

    // Set precision to two decimal places
    std::setprecision(2);
    std::string target_string = std::to_string(target);

    ui->aiv_10_32->setText(QString::fromStdString(target_string));
}
void MainWindow::on_aiv_10_12_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_10_12->text().toFloat();
    float t2 = ui->aiv_10_22->text().toFloat();
    float target = t2 - t1;

    // Set precision to one decimal place
    std::setprecision(1);
    std::string target_string = std::to_string(target);

    ui->aiv_10_32->setText(QString::fromStdString(target_string));
}
void MainWindow::on_aiv_10_23_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_10_13->text().toFloat();
    float t2 = ui->aiv_10_23->text().toFloat();
    float target = t2 - t1;

    // Set precision to two decimal places
    std::setprecision(2);
    std::string target_string = std::to_string(target);

    ui->aiv_10_33->setText(QString::fromStdString(target_string));
}
void MainWindow::on_aiv_10_13_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_10_13->text().toFloat();
    float t2 = ui->aiv_10_23->text().toFloat();
    float target = t2 - t1;

    // Set precision to one decimal place
    std::setprecision(1);
    std::string target_string = std::to_string(target);

    ui->aiv_10_33->setText(QString::fromStdString(target_string));
}
void MainWindow::on_aiv_20_41_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_20_41->text().toFloat();
    float t2 = ui->aiv_20_31->text().toFloat();
    std::string target = std::to_string(100 * t1 / t2);
    ui->aiv_20_51->setText(QString::fromStdString(target));
}
void MainWindow::on_aiv_20_42_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_20_42->text().toFloat();
    float t2 = ui->aiv_20_32->text().toFloat();
    std::string target = std::to_string(100 * t1 / t2);
    ui->aiv_20_52->setText(QString::fromStdString(target));
}
void MainWindow::on_aiv_20_43_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_20_43->text().toFloat();
    float t2 = ui->aiv_20_33->text().toFloat();
    std::string target = std::to_string(100 * t1 / t2);
    ui->aiv_20_53->setText(QString::fromStdString(target));
}
void MainWindow::on_aiv_10_41_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_10_41->text().toFloat();
    float t2 = ui->aiv_10_31->text().toFloat();
    std::string target = std::to_string(100 * t1 / t2);
    ui->aiv_10_51->setText(QString::fromStdString(target));
}
void MainWindow::on_aiv_10_42_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_10_42->text().toFloat();
    float t2 = ui->aiv_10_32->text().toFloat();
    std::string target = std::to_string(100 * t1 / t2);
    ui->aiv_10_52->setText(QString::fromStdString(target));
}
void MainWindow::on_aiv_10_43_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_10_43->text().toFloat();
    float t2 = ui->aiv_10_33->text().toFloat();
    std::string target = std::to_string(100 * t1 / t2);
    ui->aiv_10_53->setText(QString::fromStdString(target));
}
void MainWindow::on_aiv_20_6_clicked()
{
    float t1 = ui->aiv_20_51->text().toFloat(), t2 = ui->aiv_20_52->text().toFloat(), t3 = ui->aiv_20_53->text().toFloat();
    std::string target = std::to_string((t1 + t2 + t3) / 3);
    ui->aiv_20_6->setText(QString::fromStdString(target));
}
void MainWindow::on_aiv_10_6_clicked()
{
    float t1 = ui->aiv_10_51->text().toFloat(), t2 = ui->aiv_10_52->text().toFloat(), t3 = ui->aiv_10_53->text().toFloat();
    std::string target = std::to_string((t1 + t2 + t3) / 3);
    ui->aiv_10_6->setText(QString::fromStdString(target));
}
