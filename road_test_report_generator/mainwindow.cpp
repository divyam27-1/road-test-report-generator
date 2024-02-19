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


QDir cwd = QDir::current();
QDir swd = cwd;
bool i = cwd.cdUp();

std::vector<std::string> tracked_files;
const std::string all_experiments[] = {"spc", "aiv", "fei", "ind", "mdd", "grad"};

std::string OS;
bool saveas_done = false;

void removeDuplicates(std::vector<std::string> &vec)
{
    std::sort(vec.begin(), vec.end());
    auto it = std::unique(vec.begin(), vec.end());
    vec.erase(it, vec.end());
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cwd.mkdir("json");
    cwd.mkdir("templates");
    cwd.mkdir("output");
    cwd.mkdir("html");

    ui->spc_frame->move(0,0);
    ui->aiv_frame->move(0,0);
    ui->ind_frame->move(0,0);

    for (int i = 0; i < 4; i++) {
        ui->ind_graph_1->addGraph();
        ui->ind_graph_2->addGraph();
        ui->grad_graph_1->addGraph();
        ui->grad_graph_2->addGraph();
    }
    ui->mdd_graph->addGraph();
    ui->mdd_graph->addGraph();
    ui->grad_graph_2->addGraph(); ui->grad_graph_2->addGraph();

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


//Deals with saving to JSON
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
    for (int i = 0; i < 5; i++) {
        std::string I = std::to_string(i);
        std::string wsm = "wsm" + I, ws = "ws" + I, wds = "wds" + I, tray = "tray" + I, wt_tray = "wt_tray" + I, wst = "wst" + I, wdst = "wdst" + I, ww = "ww" + I, wdsm = "wdsm" + I, water_contents = "water_content" + I, density = "density" + I;

        float wet_density = (mdd_ds[0][i] - mass)/vol;
        float weight_of_water = mdd_ds[3][i] - mdd_ds[4][i];
        float weight_of_dry_sample = mdd_ds[4][i] - mdd_ds[2][i];
        float water_content = 100*weight_of_water/weight_of_dry_sample;
        float dry_density = 100*(wet_density/(100+water_content));

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
    } else {
        qDebug() << "SUYGETSU AIMS THE RESET ABSOLUTELY INCREDIBLE";
    }

    save_check();
}


//Deals with save as requests
void MainWindow::on_spc_saveas_clicked()
{
    swd = QDir(QFileDialog::getExistingDirectory(this, tr("Open Directory"), cwd.currentPath(), QFileDialog::ShowDirsOnly));
    if (!saveas_done) {
        QMessageBox::information(this, "Outputs Folder", tr("When you click EXPORT in the 'File' tab the output will be stored on that particular directory"));
        saveas_done = true;
    }
    ui->spc_save->click();
}
void MainWindow::on_fei_saveas_clicked()
{
    swd = QDir(QFileDialog::getExistingDirectory(this, tr("Open Directory"), cwd.currentPath(), QFileDialog::ShowDirsOnly));
    if (!saveas_done) {
        QMessageBox::information(this, "Outputs Folder", tr("When you click EXPORT in the 'File' tab the output will be stored on that particular directory"));
        saveas_done = true;
    }
    ui->fei_save->click();
}
void MainWindow::on_aiv_saveas_clicked()
{
    swd = QDir(QFileDialog::getExistingDirectory(this, tr("Open Directory"), cwd.currentPath(), QFileDialog::ShowDirsOnly));
    if (!saveas_done) {
        QMessageBox::information(this, "Outputs Folder", tr("When you click EXPORT in the 'File' tab the output will be stored on that particular directory"));
        saveas_done = true;
    }
    ui->aiv_save->click();
}
void MainWindow::on_ind_saveas_clicked()
{
    swd = QDir(QFileDialog::getExistingDirectory(this, tr("Open Directory"), cwd.currentPath(), QFileDialog::ShowDirsOnly));
    if (!saveas_done) {
        QMessageBox::information(this, "Outputs Folder", tr("When you click EXPORT in the 'File' tab the output will be stored on that particular directory"));
        saveas_done = true;
    }
    ui->ind_save->click();
}
void MainWindow::on_mdd_saveas_clicked()
{
    swd = QDir(QFileDialog::getExistingDirectory(this, tr("Open Directory"), cwd.currentPath(), QFileDialog::ShowDirsOnly));
    if (!saveas_done) {
        QMessageBox::information(this, "Outputs Folder", tr("When you click EXPORT in the 'File' tab the output will be stored on that particular directory"));
        saveas_done = true;
    }
    ui->mdd_save->click();
}
void MainWindow::save_check() {
    for (auto exp: all_experiments) {
        if (std::find(tracked_files.begin(), tracked_files.end(), exp) != tracked_files.end()) {
            QLabel* indicator = ui->stackedWidget->findChild<QLabel*>(QString::fromStdString(exp)+"_saved");
            if (indicator) {
                //qDebug() << "indicator" << exp << "found";
                indicator->setPixmap(QPixmap(":/saved_icons/icons/tab_saved_icon.png"));
                if (indicator->height() < 200) {
                    indicator->setPixmap(QPixmap(":/saved_icons/icons/tab_saved_icon_small.png"));
                } qDebug () << "saved pixmap set";
            } else {
                //qDebug() << "indicator not found";
            }
        } else {
            QLabel* indicator = ui->stackedWidget->findChild<QLabel*>(QString::fromStdString(exp)+"_saved");
            if (indicator) {
                //qDebug() << "indicator found";
                indicator->setPixmap(QPixmap(":/saved_icons/icons/tab_unsaved_icon.png"));
                if (indicator->height() < 200) {
                    indicator->setPixmap(QPixmap(":/saved_icons/icons/tab_unsaved_icon_small.png"));
                }
            } else {
                qDebug() << "indicator not found";
            }//
        }
    }

    for (auto file: tracked_files) {
        if (file == "spc") {
            generate_html_spc();
        } else if (file == "fei") {
            generate_html_fei();
        } else if (file == "aiv") {
            generate_html_aiv();
        } else if (file == "ind") {
            generate_html_ind();
        } else if (file == "mdd") {
            generate_html_mdd();
        } else if (file == "grad") {
            generate_html_grad();
        }
    }
}



//Deals with saving to JSON (subfunctions)
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
            //qDebug() << cumsum << " " << ind_cumulative_percent[i][j] << " " << ind_cum_pass[i][j];

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
    for (int i = 1; i <= 3; i++) {
        QJsonObject grad_json_mm;

        //this block and the if else stmt get the proportion of a specific experiment in the dbm mixture
        QString prop_spinbox_name = QString("grad_prop_%1").arg(i);
        QDoubleSpinBox* spinbox = ui->grad_frame_outer->findChild<QDoubleSpinBox*>(prop_spinbox_name);
        double proportion = spinbox->value()/100;

        if (spinbox) {
            grad_json_mm["proportion"] = proportion*100;
        } else {
            qDebug() << "Could not find a child with name" << prop_spinbox_name;
        }

        for (int j = 1; j <= 8; j++) {

            //This next part simply takes the average of the 5 samples and saves the sample data along with the average to JSON
            double sum = 0;
            for (int k = 1; k <= 5; k++) {

                QString object_name = QString("grad_p%1%2_%3").arg(i).arg(j).arg(k);

                QLineEdit* tedit = ui->dbm_tab_list->findChild<QLineEdit*>(object_name);

                if (tedit) {
                    double num = tedit->text().toDouble();
                    grad_json_mm[object_name] = num;
                    sum += num;
                } else {
                    qDebug() << "Could not find a child with name " << object_name;
                }
            }

            QString avg_key = QString("avg_%1").arg(j);
            grad_json_mm[avg_key] = sum/5;

            //This part takes the proportion of the average we will take for the final mix
            double avg_prop = proportion * sum/5;
            grad_json_mm["prop_"+avg_key] = avg_prop;
        }

        QString top_level_key;

        switch (i) {
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
    double *proportinal_passing_averages = new double[8];
    for (int i = 0; i < 8; i++) {proportinal_passing_averages[i] = 0;}

    for (QString key: grad_json.keys()) {

        QJsonObject grad_json_mm = grad_json[key].toObject();

        for (int i = 0; i < 8; i++) {
            QString prop_avg_key = QString("prop_avg_%1").arg(i);
            proportinal_passing_averages[i] += grad_json_mm[prop_avg_key].toDouble();
        }
    }

    //create the blending json object, which will added to the grad json file as its own top level json class
    QJsonObject grad_bld_json;
    for (int i = 0; i < 8; i++) {
        grad_bld_json[QString("bld_%1").arg(i+1)] = proportinal_passing_averages[i];
    }

    delete[] proportinal_passing_averages;

    grad_json["blending"] = grad_bld_json;

    QJsonObject seive_sizes;
    //Since the sieve sizes are the same we just save the seives from the first one
    for (int i = 1; i <= 8; i++) {
        QLineEdit* tedit = ui->grad_frame_outer->findChild<QLineEdit*>(QString("grad_s1_%1").arg(i));
        if (tedit) {
            seive_sizes[QString("is_seive_%1").arg(i)] = tedit->text().toDouble();
        } else {
            qDebug() << "seive tedit error";
        }
    }
    grad_json["seive_sizes"] = seive_sizes;

    //Boilerplate write to file code
    QFile grad_file(cwd.filePath("json/grad.json"));

    if (!grad_file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::information(this, "Tits", "Error opening grad json file");
    }

    QTextStream out(&grad_file);
    QJsonDocument json_doc(grad_json);
    out << json_doc.toJson();
    grad_file.close();

    save_check();
}



//Deals with Graphing
std::vector<double> quadfit(std::vector<double>& x, std::vector<double>& y) {

    double x1, x2, x3, y1, y2, y3; // The coordinates of the three points
    double a, b, c; // The coefficients of the quadratic equation
    std::vector<double> result; // The vector to store the coefficients

    int max_index = std::max_element(y.begin(), y.end()) - y.begin();
    int min_index = std::min_element(y.begin(), y.end()) - y.begin();
    int med_index = -1;
    for (int i = 0; i < y.size(); i++) {
        if (i != max_index && i != min_index) {
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
std::vector<double> quadval(std::vector<double>& x, std::vector<double>& coeff) {

    double a, b, c;
    int m = x.size();
    std::vector<double> y;

    a = coeff[0];
    b = coeff[1];
    c = coeff[2];

    for (int i = 0; i < m; i++) {
        y.push_back(a * x[i] * x[i] + b * x[i] + c); // Store the y value
    }

    return y;
}

void MainWindow::on_ind_graph_update_clicked()
{
    MainWindow::updateGraph_idg();
}
void MainWindow::updateGraph_idg() {
    //Graph 1 is the combined gradation graph
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

    QJsonObject cmb_json = ind_json_lookups["cg"].toObject();       // QJsonObject 10mm = cmb_json["10mm"].toObject();
    double mid_[] = {100.00, 97.50, 70.00, 50.00, 32.50, 22.50, 15.00, 2.50};
    double low_[] = {100.0, 95.0, 60.0, 40.0, 25.0, 15.0, 8.0, 0.0};
    double high_[] = {100.0, 100.0, 80.0, 60.0, 40.0, 30.0, 22.0, 5.0};

    QVector<double> is_sieve, passing, high, mid, low;
    for (int i = 1; i <= 8; i++) {
        std::string sieve_iterator = "is_sieve_s", pass_iterator = "pass_";
        char I = i + 48;
        sieve_iterator += I; pass_iterator += I;
        QString qsieve_iterator = QString::fromStdString(sieve_iterator), qpass_iterator = QString::fromStdString(pass_iterator);
        is_sieve << cmb_json[qsieve_iterator].toDouble();
        passing << cmb_json[qpass_iterator].toDouble();
        high << high_[i-1]; low << low_[i-1]; mid << mid_[i-1];
    }

    ui->ind_graph_1->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    ui->ind_graph_1->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    ui->ind_graph_1->legend->setFont(legendFont);
    ui->ind_graph_1->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    ui->ind_graph_1->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

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
    } else {
        ui->ind_graph_1->savePng(file_savepath);
    }

    //Now we update the Blending Graph
    //First step is getting the values from the individual gradation json file and blending the values together in proportion
    QJsonObject idg40 = ind_json_lookups["40mm"].toObject(), idg20 = ind_json_lookups["20mm"].toObject(), idg10 = ind_json_lookups["10mm"].toObject(), idg0 = ind_json_lookups["d"].toObject();
    qDebug() << idg40;
    QVector<double> blend_val;
    for (int i = 1; i <= 8; i++) {
        double components[4];

        //char I = i+48;
        std::string it1 = "pass_1" + std::to_string(i), it2 = "pass_2" + std::to_string(i), it3 ="pass_3" + std::to_string(i);
        qDebug() << it1 << it2 << it3;
        QString qit1 = QString::fromStdString(it1), qit2 = QString::fromStdString(it2), qit3 = QString::fromStdString(it3);
        components[0] = (idg40[qit1].toDouble() + idg40[qit2].toDouble() + idg40[qit3].toDouble()) * 0.33;
        components[1] = (idg20[qit1].toDouble() + idg20[qit2].toDouble() + idg20[qit3].toDouble()) * 0.33;
        components[2] = (idg10[qit1].toDouble() + idg10[qit2].toDouble() + idg10[qit3].toDouble()) * 0.33;
        components[3] = (idg0[qit1].toDouble() + idg0[qit2].toDouble() + idg0[qit3].toDouble()) * 0.33;

        qDebug() << components[0] << components[1] << components[2] << components[3];

        components[0] *= idg40["proportion"].toDouble()/100;
        components[1] *= idg20["proportion"].toDouble()/100;
        components[2] *= idg10["proportion"].toDouble()/100;
        components[3] *= idg0["proportion"].toDouble()/100;

        double sum = 0;
        for (int j = 0; j < 4; j++) {
            sum += components[j];
        }

        blend_val << sum;
    }

    ui->ind_graph_2->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    ui->ind_graph_2->legend->setVisible(true);
    ui->ind_graph_2->legend->setFont(legendFont);
    ui->ind_graph_2->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    ui->ind_graph_2->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

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
    } else {
        ui->ind_graph_2->savePng(file_savepath);
    }
}
void MainWindow::updateGraph_mdd() {

    QString json_path = cwd.filePath("json/mdd.json");
    QFile mdd_file(json_path);
    QString img_savepath = cwd.filePath("html/mdd_graph.png");

    if (!mdd_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "mdd json not opened";
        return;
    }

    QByteArray mdd_bytes = mdd_file.readAll();
    QJsonDocument mdd_json_doc = QJsonDocument::fromJson(mdd_bytes);
    QJsonObject mdd = mdd_json_doc.object();

    std::vector<double> moisture, density;

    for (int i = 0; i < 5; i++) {
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
    for (int i = 0; i < 100; i++) {
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
    } else {
        qDebug() << "SUYGETSU AIMS THE RESET ABSOLUTELY INCREDIBLE";
    }

    QVector<double> x, y, graph_x_fit, graph_y_fit;
    for (int i = 0; i < moisture.size(); i++) {
        x << moisture[i];
        y << density[i];
    } for (int i = 0; i < x_fit.size(); i++) {
        graph_x_fit << x_fit[i];
        graph_y_fit << y_fit[i];
    }

    ui->mdd_graph->graph(0)->setData(x, y);
    ui->mdd_graph->graph(1)->setData(graph_x_fit, graph_y_fit);

    ui->mdd_graph->xAxis->setRange(min_x-0.5, max_x+0.5);
    ui->mdd_graph->xAxis->setLabel("Moisture Content %");
    ui->mdd_graph->yAxis->setRange(density[y_min_index] - 0.1*density[y_min_index], density[y_max_index] + 0.1*density[y_max_index]);
    ui->mdd_graph->yAxis->setLabel("Dry Density gm/cc");

    ui->mdd_graph->graph(0)->setScatterStyle(QCPScatterStyle::ssCross);
    ui->mdd_graph->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->mdd_graph->graph(0)->setPen(QPen(Qt::red));
    ui->mdd_graph->graph(1)->setPen(QPen(Qt::black));

    ui->mdd_graph->replot();

    QFile mdd_graph(img_savepath);
    if(!mdd_graph.open(QIODevice::WriteOnly)) {
        qDebug() << mdd_graph.errorString();
    } else {
        ui->mdd_graph->savePng(img_savepath);
    }

}
void MainWindow::updateGraph_grad() {

    QString json_path = cwd.filePath("json/grad.json");
    QFile grad_file(json_path);
    QString grad_bld_savepath = cwd.filePath("html/grad_graph.png");
    QString grad_jmf_savepath = cwd.filePath("html/jmf_graph.png");

    if (!grad_file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "grad json not opened";
        return;
    }

    QByteArray grad_bytes = grad_file.readAll();
    QJsonDocument grad_json_doc = QJsonDocument::fromJson(grad_bytes);
    QJsonObject grad = grad_json_doc.object();
    QJsonObject grad_bld = grad["blending"].toObject();
    QJsonObject grad_seive = grad["seive_sizes"].toObject();

    QVector<double> max, min, mid, dbm_pass, seives, jmf_max, jmf_min;

    max = {100,100,95,80,54,42,21,8};
    min = {100,90,71,56,38,28,7,2};
    mid = {100,95,83,68,46,35,14,5};
    jmf_max = {100,100,95,79,48,36,18,6};
    jmf_min = {100,90,82,65,36,26,10,2};

    for (auto key: grad_seive.keys()) {
        seives << grad_seive[key].toDouble();
    }
    for (auto key: grad_bld.keys()) {
        dbm_pass << grad_bld[key].toDouble();

    }

    ui->grad_graph_1->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    ui->grad_graph_1->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    ui->grad_graph_1->legend->setFont(legendFont);
    ui->grad_graph_1->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    ui->grad_graph_1->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

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

    ui->grad_graph_1->xAxis->setLabel("IS SIEVE IN MM");
    ui->grad_graph_1->xAxis->setRange(0.05, 100);
    ui->grad_graph_1->yAxis->setLabel("PASSING OF %");
    ui->grad_graph_1->yAxis->setRange(0, 100);
    ui->grad_graph_1->replot();

    ui->grad_bld_graph_label->show();

    ui->grad_graph_2->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    ui->grad_graph_2->legend->setVisible(true);
    ui->grad_graph_2->legend->setFont(legendFont);
    ui->grad_graph_2->legend->setBrush(QBrush(QColor(255,255,255,230)));
    ui->grad_graph_2->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

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
    ui->grad_graph_2->graph(1)->setPen(QPen(QColor(0,0,0)));
    ui->grad_graph_2->graph(3)->setPen(QPen(QColor(0,0,0)));
    ui->grad_graph_2->graph(4)->setPen(QPen(QColor(188,66,245)));
    ui->grad_graph_2->graph(5)->setPen(QPen(QColor(240, 125, 40)));

    ui->grad_graph_2->xAxis->setScaleType(QCPAxis::stLogarithmic);
    ui->grad_graph_2->xAxis->setTicker(logTicker);

    ui->grad_graph_2->xAxis->setLabel("IS SIEVE IN MM");
    ui->grad_graph_2->xAxis->setRange(0.05, 100);
    ui->grad_graph_2->yAxis->setLabel("PASSING OF %");
    ui->grad_graph_2->yAxis->setRange(0, 100);
    ui->grad_graph_2->replot();

    ui->jmf_graph_label->show();

    QFile grad_graph(grad_bld_savepath);
    if(!grad_graph.open(QIODevice::WriteOnly)) {
        qDebug() << grad_graph.errorString();
    } else {
        ui->grad_graph_1->savePng(grad_bld_savepath);
        grad_graph.close();
    }

    QFile grad_jmf_graph(grad_jmf_savepath);
    if (!grad_jmf_graph.open(QIODevice::WriteOnly)) {
        qDebug() << grad_jmf_graph.errorString();
    } else {
        ui->grad_graph_2->savePng(grad_jmf_savepath);
        grad_jmf_graph.close();
    }

}



// Deals with exports to PDF
void MainWindow::on_actionExport_to_PDF_triggered() {
    qDebug() << "EXPORT triggered";
    QString program;
    QStringList args;

    std::string command;
    std::vector<std::string> comm_vec;

    if (OS == "win")
    {
        program = QString("..\\executable\\wkhtmltopdf.exe");
    }
    else if (OS == "linux")
    {
        program = cwd.filePath("executable/wkhtmltopdf");
    }
    else if (OS == "apple")
    {
        //TO WRITE
        program = QString("wkhtmltopdf");
    }
    command = command + program.toStdString();
    QString fname = "";

    for (auto i = tracked_files.begin(); i != tracked_files.end(); ++i) {
        qDebug() << *i;

        if (*i == "spc")
        {
            //ui->spc_export->click();
            args << cwd.filePath("html/spc_10mm.html");
            args << cwd.filePath("html/spc_20mm.html");
            args << cwd.filePath("html/spc_40mm.html");
            args << cwd.filePath("html/spc_stone_dust.html");
        }
        else if (*i == "fei")
        {
            //ui->fei_export->click();
            args << cwd.filePath("html/fei.html");
        }
        else if (*i == "aiv")
        {
            //ui->aiv_export->click();
            args << cwd.filePath("html/aiv_10mm.html");
            args << cwd.filePath("html/aiv_20mm.html");
        }
        else if (*i == "ind")
        {
            //ui->ind_export->click();
            args << cwd.filePath("html/ind_40mm.html");
            args << cwd.filePath("html/ind_20mm.html");
            args << cwd.filePath("html/ind_10mm.html");
            args << cwd.filePath("html/ind_d.html");
            args << cwd.filePath("html/bld.html");
            args << cwd.filePath("html/cmb.html");
            args << cwd.filePath("html/pass.html");
        } else if (*i == "mdd")
        {
            //ui->mdd_export->click();
            args << cwd.filePath("html/mdd.html");
        } else if (*i == "grad") {
            args << cwd.filePath("html/grad.html");
            args << cwd.filePath("html/grad_bld.html");
            args << cwd.filePath("jmf.html");
        }

        fname += "_" + QString::fromStdString(*i);
    }

    fname = "REPORT_" + QDateTime::currentDateTime().toString(Qt::ISODateWithMs) + fname + ".pdf";
    fname.replace(QRegularExpression("[^\\w\\.]"), "_");
    args << swd.filePath(fname);

    QList<QString>::iterator i;
    for (i = args.begin(); i != args.end(); ++i)
        command = command + " " + i->toStdString();
    qDebug() << command;

    if (OS != "apple") {
        QProcess *converter = new QProcess();
        converter->start(program, args);
    } else {
        QMessageBox::information(this, "Copy-Paste this command in your terminal to get your PDF!", QString(command.c_str()), QMessageBox::Ok);
        std::string output_txt_path = cwd.filePath("output/command.txt").toStdString();
        std::ofstream output_txt_file(output_txt_path, std::ios::out);
        if(output_txt_file.is_open()) {
            output_txt_file << command;
        } output_txt_file.close();
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



//Deals with HTML generation
void MainWindow::generate_html_spc()
{
    //ui->spc_save->click();
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
    //ui->fei_save->click();
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
    //ui->ind_save->click();
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
                    double lower_limit_[8] = {100,95,60,40,25,15,8,0};
                    double upper_limit_[8] = {100, 100, 80, 60, 40, 30, 22, 15};
                    ss[0] = json_lookups_data_40["is_sieve_s11"].toDouble();
                    ss[1] = json_lookups_data_40["is_sieve_s12"].toDouble();
                    ss[2] = json_lookups_data_40["is_sieve_s13"].toDouble();
                    ss[3] = json_lookups_data_40["is_sieve_s14"].toDouble();
                    ss[4] = json_lookups_data_40["is_sieve_s15"].toDouble();
                    ss[5] = json_lookups_data_40["is_sieve_s16"].toDouble();
                    ss[6] = json_lookups_data_40["is_sieve_s17"].toDouble();
                    ss[7] = json_lookups_data_40["is_sieve_s18"].toDouble();
                    for(int i = 0; i < 8; ++i) {
                        lower_limit[i] = lower_limit_[i];
                        upper_limit[i] = upper_limit_[i];
                        passing[i] = combined_passing[i+1];
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
                        topushf = (json_lookups_data_40["pass_11"].toDouble() + json_lookups_data_40["pass_21"].toDouble() + json_lookups_data_40["pass_31"].toDouble())/3;
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
                        topushf = (json_lookups_data_40["pass_12"].toDouble() + json_lookups_data_40["pass_22"].toDouble() + json_lookups_data_40["pass_32"].toDouble())/3;
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
                        topushf = (json_lookups_data_40["pass_13"].toDouble() + json_lookups_data_40["pass_23"].toDouble() + json_lookups_data_40["pass_33"].toDouble())/3;
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
                        topushf = (json_lookups_data_40["pass_14"].toDouble() + json_lookups_data_40["pass_24"].toDouble() + json_lookups_data_40["pass_34"].toDouble())/3;
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
                        topushf = (json_lookups_data_40["pass_15"].toDouble() + json_lookups_data_40["pass_25"].toDouble() + json_lookups_data_40["pass_35"].toDouble())/3;
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
                        topushf = (json_lookups_data_40["pass_16"].toDouble() + json_lookups_data_40["pass_26"].toDouble() + json_lookups_data_40["pass_36"].toDouble())/3;
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
                        topushf = (json_lookups_data_40["pass_17"].toDouble() + json_lookups_data_40["pass_27"].toDouble() + json_lookups_data_40["pass_37"].toDouble())/3;
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
                        topushf = (json_lookups_data_40["pass_18"].toDouble() + json_lookups_data_40["pass_28"].toDouble() + json_lookups_data_40["pass_38"].toDouble())/3;
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
                        topushf = (json_lookups_data_20["pass_11"].toDouble() + json_lookups_data_20["pass_21"].toDouble() + json_lookups_data_20["pass_31"].toDouble())/3;
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
                        topushf = (json_lookups_data_20["pass_12"].toDouble() + json_lookups_data_20["pass_22"].toDouble() + json_lookups_data_20["pass_32"].toDouble())/3;
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
                        topushf = (json_lookups_data_20["pass_13"].toDouble() + json_lookups_data_20["pass_23"].toDouble() + json_lookups_data_20["pass_33"].toDouble())/3;
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
                        topushf = (json_lookups_data_20["pass_14"].toDouble() + json_lookups_data_20["pass_24"].toDouble() + json_lookups_data_20["pass_34"].toDouble())/3;
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
                        topushf = (json_lookups_data_20["pass_15"].toDouble() + json_lookups_data_20["pass_25"].toDouble() + json_lookups_data_20["pass_35"].toDouble())/3;
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
                        topushf = (json_lookups_data_20["pass_16"].toDouble() + json_lookups_data_20["pass_26"].toDouble() + json_lookups_data_20["pass_36"].toDouble())/3;
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
                        topushf = (json_lookups_data_20["pass_17"].toDouble() + json_lookups_data_20["pass_27"].toDouble() + json_lookups_data_20["pass_37"].toDouble())/3;
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
                        topushf = (json_lookups_data_20["pass_18"].toDouble() + json_lookups_data_20["pass_28"].toDouble() + json_lookups_data_20["pass_38"].toDouble())/3;
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
                        topushf = (json_lookups_data_10["pass_11"].toDouble() + json_lookups_data_10["pass_21"].toDouble() + json_lookups_data_10["pass_31"].toDouble())/3;
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
                        topushf = (json_lookups_data_10["pass_12"].toDouble() + json_lookups_data_10["pass_22"].toDouble() + json_lookups_data_10["pass_32"].toDouble())/3;
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
                        topushf = (json_lookups_data_10["pass_13"].toDouble() + json_lookups_data_10["pass_23"].toDouble() + json_lookups_data_10["pass_33"].toDouble())/3;
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
                        topushf = (json_lookups_data_10["Pass_14"].toDouble() + json_lookups_data_10["Pass_24"].toDouble() + json_lookups_data_10["Pass_34"].toDouble())/3;
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
                        topushf = (json_lookups_data_10["Pass_15"].toDouble() + json_lookups_data_10["Pass_25"].toDouble() + json_lookups_data_10["Pass_35"].toDouble())/3;
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
                        topushf = (json_lookups_data_10["Pass_16"].toDouble() + json_lookups_data_10["Pass_26"].toDouble() + json_lookups_data_10["Pass_36"].toDouble())/3;
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
                        topushf = (json_lookups_data_10["pass_17"].toDouble() + json_lookups_data_10["pass_27"].toDouble() + json_lookups_data_10["pass_37"].toDouble())/3;
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
                        topushf = (json_lookups_data_10["pass_18"].toDouble() + json_lookups_data_10["pass_28"].toDouble() + json_lookups_data_10["pass_38"].toDouble())/3;
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
                        topushf = (json_lookups_data_d["pass_11"].toDouble() + json_lookups_data_d["pass_21"].toDouble() + json_lookups_data_d["pass_31"].toDouble())/3;
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
                        topushf = (json_lookups_data_d["pass_12"].toDouble() + json_lookups_data_d["pass_22"].toDouble() + json_lookups_data_d["pass_32"].toDouble())/3;
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
                        topushf = (json_lookups_data_d["pass_13"].toDouble() + json_lookups_data_d["pass_23"].toDouble() + json_lookups_data_d["pass_33"].toDouble())/3;
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
                        topushf = (json_lookups_data_10["pass_14"].toDouble() + json_lookups_data_10["pass_24"].toDouble() + json_lookups_data_10["pass_34"].toDouble())/3;
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
                        topushf = (json_lookups_data_10["pass_15"].toDouble() + json_lookups_data_10["pass_25"].toDouble() + json_lookups_data_10["pass_35"].toDouble())/3;
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
                        topushf = (json_lookups_data_10["pass_16"].toDouble() + json_lookups_data_10["pass_26"].toDouble() + json_lookups_data_10["pass_36"].toDouble())/3;
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
                        topushf = (json_lookups_data_10["pass_17"].toDouble() + json_lookups_data_10["pass_27"].toDouble() + json_lookups_data_10["pass_37"].toDouble())/3;
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
                        topushf = (json_lookups_data_10["pass_18"].toDouble() + json_lookups_data_10["pass_28"].toDouble() + json_lookups_data_10["pass_38"].toDouble())/3;
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
    //ui->mdd_save->click();
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
                    int j = (token - 14)%6;

                    if (token == 11) {
                        QString it = QString::fromStdString("mass");
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    } else if (token == 12) {
                        topushf = json_obj["vol"].toDouble();
                        output_html_file << topushf;
                    } else if (token == 13) {
                        topushf = json_obj["MDD"].toDouble();
                    } else if (token == 80) {
                        topushf = json_obj["OMC"].toDouble();
                    } else if ((token >= 14) && (token < 20)) {
                        QString it = QString::fromStdString("wsm" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    } else if ((token >= 20) && (token < 26)) {
                        QString it = QString::fromStdString("ws" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    } else if ((token >= 26) && (token < 32)) {
                        QString it = QString::fromStdString("wds" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    } else if ((token >= 32) && (token < 38)) {
                        QString it = QString::fromStdString("tray" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    } else if ((token >= 38) && (token < 44)) {
                        QString it = QString::fromStdString("wt_tray" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    } else if ((token >= 44) && (token < 50)) {
                        QString it = QString::fromStdString("wst" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    } else if ((token >= 50) && (token < 56)) {
                        QString it = QString::fromStdString("wdst" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    } else if ((token >= 56) && (token < 62)) {
                        QString it = QString::fromStdString("ww" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    } else if ((token >= 62) && (token < 68)) {
                        QString it = QString::fromStdString("wdsm" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    } else if ((token >= 68) && (token < 74)) {
                        QString it = QString::fromStdString("water_content" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    } else if ((token >= 74) && (token < 80)) {
                        QString it = QString::fromStdString("density" + std::to_string(j));
                        topushf = json_obj[it].toDouble();
                        output_html_file << topushf;
                    }

                    switch (token) {
                    case 1:
                        topush = ui->mdd_bsc_1->toPlainText().toStdString(); break;
                    case 2:
                        topush = ui->mdd_bsc_2->toPlainText().toStdString(); break;
                    case 3:
                        topush = ui->mdd_bsc_3->toPlainText().toStdString(); break;
                    case 4:
                        topush = ui->mdd_bsc_4->toPlainText().toStdString(); break;
                    case 5:
                        topush = ui->mdd_exp_1->text().toStdString(); break;
                    case 6:
                        topush = ui->mdd_exp_2->text().toStdString(); break;
                    case 7:
                        topush = ui->mdd_exp_3->text().toStdString(); break;
                    case 8:
                        topush = ui->mdd_exp_4->text().toStdString(); break;
                    case 81:
                        topush = ui->mdd_exp_5->text().toStdString(); break;
                    case 9:
                        topush = ui->mdd_exp_6->text().toStdString(); break;
                    case 82:
                        topush = ui->mdd_exp_7->text().toStdString(); break;
                    case 10:
                        topush = ui->mdd_exp_8->text().toStdString(); break;
                    }

                    output_html_file << topush;
                    topush = "";

                } else {
                    output_html_file << line[i];
                }
            }

        }
        json_file.close();
        output_html_file.close();
        qDebug() << "file written to";

        template_file.close();
    } else {
        qDebug() << "mdd output html file not opened";
    }
}
void MainWindow::generate_html_grad() {
    //ui->mdd_save->click();
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
                        } else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush;
                    double topushf;

                    if ((token >= 4) && (token <= 59)) {
                        token -= 4;
                        QString arg;

                        switch (token%7) {
                        case 0:
                            arg = QString("is_seive_%1").arg(1 + token/7);
                            topushf = seives[arg].toDouble();
                            output_html_file << topushf;
                            break;
                        case 6:
                            arg = QString("avg_%1").arg(1 + token/7);
                            topushf = json1[arg].toDouble();
                            output_html_file << topushf;
                            break;
                        default:
                            arg = QString("grad_p1%1_%2").arg(1 + token/7).arg(token%7);
                            topushf = json1[arg].toDouble();
                            output_html_file << topushf;
                            break;
                        }
                    } else if ((token >= 60) && (token <= 115)) {
                        token -= 60;
                        QString arg;

                        switch (token%7) {
                        case 0:
                            arg = QString("is_seive_%1").arg(1 + token/7);
                            topushf = seives[arg].toDouble();
                            output_html_file << topushf;
                            break;
                        case 6:
                            arg = QString("avg_%1").arg(1 + token/7);
                            topushf = json2[arg].toDouble();
                            output_html_file << topushf;
                            break;
                        default:
                            arg = QString("grad_p2%1_%2").arg(1 + token/7).arg(token%7);
                            topushf = json2[arg].toDouble();
                            output_html_file << topushf;
                            break;
                        }
                    } else if ((token >= 116) && (token <= 171)) {
                        token -= 116;
                        QString arg;

                        switch (token%7) {
                        case 0:
                            arg = QString("is_seive_%1").arg(1 + token/7);
                            topushf = seives[arg].toDouble();
                            output_html_file << topushf;
                            break;
                        case 6:
                            arg = QString("avg_%1").arg(1 + token/7);
                            topushf = json3[arg].toDouble();
                            output_html_file << topushf;
                            break;
                        default:
                            arg = QString("grad_p3%1_%2").arg(1 + token/7).arg(token%7);
                            topushf = json3[arg].toDouble();
                            output_html_file << topushf;
                            break;
                        }
                    } else {
                        switch (token) {
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
                        default:
                            qDebug() << "ilya something petrov";
                        }
                    }

                    output_html_file << topush;
                    topush = "";

                } else {
                    output_html_file << line[i];
                }
            }

        }
        json_file.close();
        output_html_file.close();
        qDebug() << "file written to";

        template_file.close();
    } else {
        qDebug() << "mdd output html file not opened";
    }

    output_html_path = cwd.filePath("html/grad_bld.html").toStdString();
    std::ofstream output_grad_bld_file(output_html_path, std::ios::out);

    if (output_grad_bld_file.is_open())
    {
        qDebug() << "output html file opened";

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
                        } else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush;
                    double topushf;

                    switch (token) {
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
                    default:
                        qDebug() << "FlashBack is like the suygetsu of apac";
                        break;
                    }
                    if ((token >= 4) && (token <= 11)) {
                        token -= 4;
                        QString arg = QString("avg_%1").arg(1 + token%10);
                        output_grad_bld_file << json1[arg].toDouble();
                    } else if ((token >= 20) && (token <= 27)) {
                        token -= 20;
                        output_grad_bld_file << json2[QString("avg_%1").arg(1 + token%10)].toDouble();
                    } else if ((token >= 36) && (token <= 43)) {
                        token -= 36;
                        output_grad_bld_file << json3[QString("avg_%1").arg(1 + token%10)].toDouble();
                    } else if ((token >= 12) && (token <= 19)) {
                        token -= 12;
                        output_grad_bld_file << json1[QString("prop_avg_%1").arg(1 + token%10)].toDouble();
                    } else if ((token >= 28) && (token <= 35)) {
                        token -= 28;
                        output_grad_bld_file << json2[QString("prop_avg_%1").arg(1 + token%10)].toDouble();
                    } else if ((token >= 44) && (token <= 51)) {
                        token -= 44;
                        output_grad_bld_file << json3[QString("prop_avg_%1").arg(1 + token%10)].toDouble();
                    } else if ((token >= 53) && (token <= 60)) {
                        token -= 53;
                        output_grad_bld_file << bld[QString("bld_%1").arg(1 + token%10)].toDouble();
                    } else if (token == 52) {
                        output_grad_bld_file << "100";
                    }

                    output_grad_bld_file << topush;
                    topush = "";

                } else {
                    output_grad_bld_file << line[i];
                }
            }

        }
        json_file.close();
        output_grad_bld_file.close();
        qDebug() << "file written to";

        template_file.close();
    } else {
        qDebug() << "grad bld output html file not opened";
    }

    output_html_path = cwd.filePath("html/grad_jmf.html");
    std::ofstream output_grad_jmf_file(output_html_path, std::ios::out);


    if (output_grad_jmf_file.is_open())
    {
        qDebug() << "output html file opened";

        QString template_path = cwd.filePath("templates/mdd.html");
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
                        } else if (line[j] == '~' && j - i == 4)
                        {
                            token = ((int)line[i + 3] - 48) + 10 * ((int)line[i + 2] - 48) + 100 * ((int)line[i + 1] - 48);
                            i = j;
                            break;
                        }
                    }

                    std::string topush;
                    double topushf;

                    if (token == 1) {
                        topush = ui->grad_bsc_1->toPlainText().toStdString();
                    } else if (token == 2) {
                        topush = ui->grad_bsc_2->toHtml().toStdString();
                    } else if (token == 3) {
                        topush = "IS SEIVE";
                    } else if ((token >= 4) && (token <= 11)) {
                        token -= 4;
                        output_grad_jmf_file << seives[QString("is_seive_%1").arg(1 + token%10)].toDouble();
                    } else if ((token >= 13) && (token <= 20)) {
                        token -= 13;
                        output_grad_jmf_file << bld[QString("bld_%1").arg(1 + token%10)].toDouble();
                    }

                    output_grad_jmf_file << topush;
                    topush = "";

                } else {
                    output_grad_jmf_file << line[i];
                }
            }

        }
        json_file.close();
        output_grad_jmf_file.close();
        qDebug() << "file written to";

        template_file.close();
    } else {
        qDebug() << "grad bld output html file not opened";
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
            ui->grad_data_scroll->setValue((int)(scroll_pos + delta.y()/ scroll_sens));
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



//Deal with switchin windows
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
