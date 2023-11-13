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

QDir cwd = QDir::current();
bool i = cwd.cdUp();
std::vector<std::string> tracked_files;
std::string OS;

void removeDuplicates(std::vector<std::string>& vec) {
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
#endif

    qDebug() << OS;
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::get_token(std::string line, int pos, int *output)
{
    //
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    ui->tab_list->removeTab(index);
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
    D[10] = D[1] + D[2] + D[3] + D[4] + D[5] + D[6] + D[7] + D[8] + D[9];   //total weight
    D[11] = 100*C[10]/A[10]; //flekiness index
    D[12] = 100*D[10]/D[10]; //elongation index
    D[13] = D[11] + D[12];  //combined index
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

QFile aiv_(cwd.filePath("json/aiv_.json"));
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
    weight_of_sample[2][1] =  weight_of_cylinder_sample[2][1] - weight_of_cylinder[2][1];
    weight_of_sample[2][2] =  weight_of_cylinder_sample[2][2] - weight_of_cylinder[2][2];
    weight_of_sample[2][3] =  weight_of_cylinder_sample[2][3] - weight_of_cylinder[2][3];
    weight_crushed_material[2][1] = ui->aiv_20_41->text().toFloat();
    weight_crushed_material[2][2] = ui->aiv_20_42->text().toFloat();
    weight_crushed_material[2][3] = ui->aiv_20_43->text().toFloat();
    aggregate_impact_value[2][1] = 100*weight_crushed_material[2][1]/( weight_of_sample[2][1] );
    aggregate_impact_value[2][2] = 100*weight_crushed_material[2][2]/( weight_of_sample[2][2] );
    aggregate_impact_value[2][3] = 100*weight_crushed_material[2][3]/(  weight_of_sample[2][3]);
    avg_aggregate_impact_value_10 = (aggregate_impact_value[2][1] + aggregate_impact_value[2][1] + aggregate_impact_value[2][1])/3;
    aiv_json_20mm["weight_of_cylinder_1"] = weight_of_cylinder[2][1];
    aiv_json_20mm["weight_of_cylinder_2"] = weight_of_cylinder[2][2];
    aiv_json_20mm["weight_of_cylinder_3"] = weight_of_cylinder[2][3];
    aiv_json_20mm["weight_of_cylinder_sample_1"] = weight_of_cylinder_sample[2][1];
    aiv_json_20mm["weight_of_cylinder_sample_2"] = weight_of_cylinder_sample[2][2];
    aiv_json_20mm["weight_of_cylinder_sample_3"] = weight_of_cylinder_sample[2][3];
    aiv_json_20mm["weight_sample_1"] = weight_of_cylinder_sample[2][1];
    aiv_json_20mm["weight_sample_2"] = weight_of_cylinder_sample[2][2];
    aiv_json_20mm["weight_sample_3"] = weight_of_cylinder_sample[2][3];
    aiv_json_20mm["weight_crushed_material_1"] =  weight_crushed_material[2][1];
    aiv_json_20mm["weight_crushed_material_2"] =  weight_crushed_material[2][2];
    aiv_json_20mm["weight_crushed_material_3"] =  weight_crushed_material[2][3];
    aiv_json_20mm["aggeregate_impact_value_1"] = aggregate_impact_value[2][1];
    aiv_json_20mm["aggeregate_impact_value_2"] = aggregate_impact_value[2][2];
    aiv_json_20mm["aggeregate_impact_value_3"] = aggregate_impact_value[2][3];
    aiv_json_20mm["avg_aggregate_impact_value_20"] = avg_aggregate_impact_value_10;

    aiv_json["20mm"] = aiv_json_20mm;
    if (aiv_.open(QFile::WriteOnly | QFile::Text))
    {

        QTextStream out(&aiv_);
        QJsonDocument jsonDoc_2(aiv_json);
        out << jsonDoc_2.toJson();

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
    weight_of_sample[1][1] =  weight_of_cylinder_sample[1][1] - weight_of_cylinder[1][1];
    weight_of_sample[1][2] =  weight_of_cylinder_sample[1][2] - weight_of_cylinder[1][2];
    weight_of_sample[1][3] =  weight_of_cylinder_sample[1][3] - weight_of_cylinder[1][3];
    weight_crushed_material[1][1] = ui->aiv_10_41->text().toFloat();
    weight_crushed_material[1][2] = ui->aiv_10_42->text().toFloat();
    weight_crushed_material[1][3] = ui->aiv_10_43->text().toFloat();
    aggregate_impact_value[1][1] = 100*weight_crushed_material[1][1]/( weight_of_sample[1][1] );
    aggregate_impact_value[1][2] = 100*weight_crushed_material[1][2]/( weight_of_sample[1][2] );
    aggregate_impact_value[1][3] = 100*weight_crushed_material[1][3]/(  weight_of_sample[1][3]);
    avg_aggregate_impact_value_10 = (aggregate_impact_value[1][1] + aggregate_impact_value[1][1] + aggregate_impact_value[1][1])/3;
    aiv_json_10mm["weight_of_cylinder_1"] = weight_of_cylinder[1][1];
    aiv_json_10mm["weight_of_cylinder_2"] = weight_of_cylinder[1][2];
    aiv_json_10mm["weight_of_cylinder_3"] = weight_of_cylinder[1][3];
    aiv_json_10mm["weight_of_cylinder_sample_1"] = weight_of_cylinder_sample[1][1];
    aiv_json_10mm["weight_of_cylinder_sample_2"] = weight_of_cylinder_sample[1][2];
    aiv_json_10mm["weight_of_cylinder_sample_3"] = weight_of_cylinder_sample[1][3];
    aiv_json_10mm["weight_sample_1"] = weight_of_cylinder_sample[1][1];
    aiv_json_10mm["weight_sample_2"] = weight_of_cylinder_sample[1][2];
    aiv_json_10mm["weight_sample_3"] = weight_of_cylinder_sample[1][3];
    aiv_json_10mm["weight_crushed_material_1"] =  weight_crushed_material[1][1];
    aiv_json_10mm["weight_crushed_material_2"] =  weight_crushed_material[1][2];
    aiv_json_10mm["weight_crushed_material_3"] =  weight_crushed_material[1][3];
    aiv_json_10mm["aggeregate_impact_value_1"] = aggregate_impact_value[1][1];
    aiv_json_10mm["aggeregate_impact_value_2"] = aggregate_impact_value[1][2];
    aiv_json_10mm["aggeregate_impact_value_3"] = aggregate_impact_value[1][3];
    aiv_json_10mm["avg_aggregate_impact_value_10"] = avg_aggregate_impact_value_10;
    aiv_json["10mm"] = aiv_json_10mm;
    if (aiv_.open(QFile::WriteOnly | QFile::Text))
    {
       QTextStream out(&aiv_);
       QJsonDocument jsonDoc_2(aiv_json);
       out << jsonDoc_2.toJson();
       aiv_.close();
    }
}





// Deals with Scrolling
void MainWindow::wheelEvent(QWheelEvent *event)
{
    // the mouse wheel API gives wheel inputs in delta, for most non gaming mice one notch turn a delta of 120
    // setting the sens in this method does not make any sense now, but in the future we will add a mouse sensitivity option in the View QMenuBar to change this
    qDebug() << tracked_files.size();
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
            qDebug() << ui->ind_frame->y();
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

void MainWindow::on_spc_export_clicked()
{
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
                            qDebug() << "opening tilda located";
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

                            qDebug() << "token found: " << token;

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
                                topush = ui->spc_exp_5->text().toStdString();
                                break;
                            case 10:
                                topush = ui->spc_exp_6->text().toStdString();
                                break;
                            case 11:
                                qDebug() << json_lookups_data["Weight_of_sample_of_water_1"].toString();
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
                            qDebug() << topushf;
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
                    qDebug() << "opening tilda located";
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

                    qDebug() << "token found: " << token;

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
                        break;
                    case 66:
                        break;
                    case 67:
                        break;
                    case 68:
                        break;
                    case 69:
                        break;
                    case 70:
                        break;
                    case 71:
                        break;
                    default:
                        qDebug() << "something failed....";
                    }

                    output_html_file << topush;
                    qDebug() << topushf;
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
    std::string target = std::to_string(100*t1/t2);
    ui->aiv_20_51->setText(QString::fromStdString(target));
}
void MainWindow::on_aiv_20_42_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_20_42->text().toFloat();
    float t2 = ui->aiv_20_32->text().toFloat();
    std::string target = std::to_string(100*t1/t2);
    ui->aiv_20_52->setText(QString::fromStdString(target));
}
void MainWindow::on_aiv_20_43_textChanged(const QString &arg1)
{
    float t1 = ui->aiv_20_43->text().toFloat();
    float t2 = ui->aiv_20_33->text().toFloat();
    std::string target = std::to_string(100*t1/t2);
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
    std::string target = std::to_string((t1+t2+t3)/3);
    ui->aiv_20_6->setText(QString::fromStdString(target));
}
void MainWindow::on_aiv_10_6_clicked()
{
    float t1 = ui->aiv_10_51->text().toFloat(), t2 = ui->aiv_10_52->text().toFloat(), t3 = ui->aiv_10_53->text().toFloat();
    std::string target = std::to_string((t1+t2+t3)/3);
    ui->aiv_10_6->setText(QString::fromStdString(target));
}



