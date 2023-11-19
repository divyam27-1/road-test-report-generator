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

QDir cwd = QDir::current();
bool i = cwd.cdUp();
std::vector<std::string> tracked_files;
std::string OS;

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

#ifdef _WIN32
    OS = "win";
#elif __linux__
    OS = "linux";
#elif __APPLE__
    OS = "apple";
#endif

    qDebug() << OS;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    ui->tab_list->removeTab(index);
}

void MainWindow::on_actionSave_Project_triggered()
{
    ui->spc_save->click();
    ui->fei_save->click();
    ui->aiv_save->click();
    ui->ind_save->click();
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
    float Bulk_specific_gravity_40[3];
    float apparent_specific_gravity_40[3];
    float water_absorption_40[3];
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

    float Bulk_specific_gravity_20[3];
    float apparent_specific_gravity_20[3];
    float water_absorption_20[3];
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
    float Bulk_specific_gravity_10[3];
    float apparent_specific_gravity_10[3];
    float water_absorption_10[3];
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

    float Bulk_specific_gravity_s_d[3];
    float apparent_specific_gravity_s_d[3];
    float water_absorption_s_d[3];
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
            ind_cum_pass[i][j] = 100 - (cumsum / total_weight[i - 1]);
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
        cumsum = 1;
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
}
// Deals with exports to PDF
void MainWindow::on_actionExport_to_PDF_triggered()
{
    qDebug() << "EXPORT triggered";
    QString program;
    QStringList args;

    if (OS == "win")
    {
        program = cwd.filePath("executable/wkhtmltopdf.exe");
    }
    else if (OS == "linux")
    {
        program = cwd.filePath("executable/wkhtmltopdf");
    }
    else if (OS == "apple")
    {
        //TO WRITE
    }

    for (auto i = tracked_files.begin(); i != tracked_files.end(); ++i)
    {
        qDebug() << *i;

        if (*i == "spc")
        {
            ui->spc_export->click();
            args << cwd.filePath("html/spc_10mm.html");
            args << cwd.filePath("html/spc_20mm.html");
            args << cwd.filePath("html/spc_40mm.html");
            args << cwd.filePath("html/spc_stone_dust.html");
        }
        else if (*i == "fei")
        {
            ui->fei_export->click();
            args << cwd.filePath("html/fei.html");
        }
        else if (*i == "aiv")
        {
            ui->aiv_export->click();
            args << cwd.filePath("html/aiv_10mm.html");
            args << cwd.filePath("html/aiv_20mm.html");
        }
        else if (*i == "ind")
        {
            ui->ind_export->click();
            args << cwd.filePath("html/ind_40mm.html");
            args << cwd.filePath("html/ind_20mm.html");
            args << cwd.filePath("html/ind_10mm.html");
            args << cwd.filePath("html/ind_0mm.html");
        }
    }

    args << cwd.filePath("output/REPORT.pdf");

    QProcess *converter = new QProcess();
    converter->start(program, args);
}
void MainWindow::on_spc_export_clicked()
{
    ui->spc_save->click();
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
                QFile template_file(template_path);
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
void MainWindow::on_fei_export_clicked()
{
    ui->fei_save->click();
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
        QFile template_file(template_path);
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
void MainWindow::on_aiv_export_clicked()
{
    ui->aiv_save->click();
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
                QFile template_file(template_path);
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
void MainWindow::on_ind_export_clicked()
{
    ui->ind_save->click();
    qDebug() << "beginning ind save...";
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
                QFile template_file(template_path);
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
        QFile bld_template_file(bld_template_path);
        if (!bld_template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
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
                    qDebug() << topushf;
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
        QFile cmb_template_file(cmb_template_path);
        if (!cmb_template_file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            qDebug() << "html not opened";
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



                    default:
                        qDebug() << "smthlikeyou11";
                    }
                    cmb_output_html_file << topush;
                    qDebug() << topushf;
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
        switch (ui->tab_list->currentIndex())
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

void MainWindow::on_idg_d_s21_cursorPositionChanged(int arg1, int arg2)
{

}

