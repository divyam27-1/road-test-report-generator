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

QJsonObject Specific_Gravity;
QDir cwd = QDir::current();
bool i = cwd.cdUp();

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    cwd.mkdir("json");
    cwd.mkdir("templates");
    cwd.mkdir("output");
    cwd.mkdir("html");
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
    struct basic_info basic_info_ = {ui->spc_bsc_1->toPlainText(),
                                     ui->spc_bsc_2->toPlainText(),
                                     ui->spc_bsc_3->toPlainText(),
                                     ui->spc_bsc_4->toPlainText()};
    struct exp_info exp_info_ = {ui->spc_exp_1->date().toString(),
                                 ui->spc_exp_2->date().toString(),
                                 ui->spc_exp_3->text(),
                                 ui->spc_exp_4->text(),
                                 ui->spc_exp_5->text(),
                                 ui->spc_exp_6->text()};

    QJsonObject basic_info;
    basic_info.insert("name_of_work", basic_info_.name_of_work);
    basic_info.insert("name_of_client", basic_info_.name_of_client);
    basic_info.insert("name_of_consultant", basic_info_.name_of_consultant);
    basic_info.insert("name_of_contractor", basic_info_.name_of_contractor);
    qDebug() << "basic info done";

    QJsonObject exp_info;
    exp_info.insert("date_of_sampling", exp_info_.date_of_sampling);
    exp_info.insert("date_of_testing", exp_info_.date_of_testing);
    exp_info.insert("sample_no", exp_info_.sample_no);
    exp_info.insert("sample_at", exp_info_.sample_at);
    exp_info.insert("source_of_material", exp_info_.source_of_material);
    exp_info.insert("type_of_material", exp_info_.type_of_material);
    qDebug() << "experiment info done";

    QJsonObject info;
    info.insert("basic_info",basic_info);
    info.insert("experiment_info", exp_info);
    qDebug() << "info done";
}

QFile current_save(cwd.filePath("json/spc.json"));
void MainWindow::on_save_40mm_clicked()
{
    type_of_material _40mm;

    //achha lagega:)
    _40mm.Weight_of_sample_of_water[4][0] = 0;
    _40mm.Weight_of_SSD_Sample[4][0] = 0;
    _40mm.Weight_of_Oven_dry_sample[4][0] = 0;

    _40mm.Weight_of_sample_of_water[4][1] = ui->spc_40_w1_t1->text();
    _40mm.Weight_of_sample_of_water[4][2] = ui->spc_40_w1_t2->text();
    _40mm.Weight_of_sample_of_water[4][3] = ui->spc_40_w1_t3->text();
    _40mm.Weight_of_SSD_Sample[4][1] = ui->spc_40_w2_t1->text();
    _40mm.Weight_of_SSD_Sample[4][2] = ui->spc_40_w2_t2->text();
    _40mm.Weight_of_SSD_Sample[4][3] = ui->spc_40_w2_t3->text();
    _40mm.Weight_of_Oven_dry_sample[4][1] = ui->spc_40_w3_t1->text();
    _40mm.Weight_of_Oven_dry_sample[4][2] = ui->spc_40_w3_t2->text();
    _40mm.Weight_of_Oven_dry_sample[4][3] = ui->spc_40_w3_t3->text();


    QJsonObject _40_mm;
    //achha lg rha hai??:) i am noobda
    _40_mm.insert("Weight_of_sample_of_water_1",_40mm.Weight_of_sample_of_water[4][1]);
    _40_mm["Weight_of_sample_of_water_2"] = _40mm.Weight_of_sample_of_water[4][2];
    _40_mm["Weight_of_sample_of_water_3"] = _40mm.Weight_of_sample_of_water[4][3];
    _40_mm["Weight_of_SSD_Sample_1"] = _40mm.Weight_of_SSD_Sample[4][1];
    _40_mm["Weight_of_SSD_Sample_2"] = _40mm.Weight_of_SSD_Sample[4][2];
    _40_mm["Weight_of_SSD_Sample_3"] = _40mm.Weight_of_SSD_Sample[4][3];
    _40_mm["Weight_of_Oven_dry_sample_1"] = _40mm.Weight_of_Oven_dry_sample[4][1];
    _40_mm["Weight_of_Oven_dry_sample_2"] = _40mm.Weight_of_Oven_dry_sample[4][2];
    _40_mm["Weight_of_Oven_dry_sample_3"] = _40mm.Weight_of_Oven_dry_sample[4][3];

    Specific_Gravity[ "40mm" ] = _40_mm;
    if (current_save.open(QFile::WriteOnly | QFile::Text)) {
        // json data ko file me likhta hua mai
        QTextStream out(&current_save);
        QJsonDocument jsonDoc_1(Specific_Gravity);

        out << jsonDoc_1.toJson();

        // Close the file
        current_save.close();
        qDebug() << "Combined JSON data saved to file: " << "Jsonfile_1";
    }
    else {
        qDebug() << "Failed to open the file for writing.";
    }
}
void MainWindow::on_pushButton_clicked()
{
    type_of_material _20mm;


    _20mm.Weight_of_sample_of_water[2][0] = 0;
    _20mm.Weight_of_SSD_Sample[2][0] = 0;
    _20mm.Weight_of_Oven_dry_sample[2][0] = 0;

    _20mm.Weight_of_sample_of_water[2][1] = ui->spc_20_w1_t1->text();
    _20mm.Weight_of_sample_of_water[2][2] = ui->spc_20_w1_t2->text();
    _20mm.Weight_of_sample_of_water[2][3] = ui->spc_20_w1_t3->text();
    _20mm.Weight_of_SSD_Sample[2][1] = ui->spc_20_w2_t1->text();
    _20mm.Weight_of_SSD_Sample[2][2] = ui->spc_20_w2_t2->text();
    _20mm.Weight_of_SSD_Sample[2][3] = ui->spc_20_w2_t3->text();
    _20mm.Weight_of_Oven_dry_sample[2][1] = ui->spc_20_w3_t1->text();
    _20mm.Weight_of_Oven_dry_sample[2][2] = ui->spc_20_w3_t2->text();
    _20mm.Weight_of_Oven_dry_sample[2][3] = ui->spc_20_w3_t3->text();

    QJsonObject _20_mm;
    _20_mm["Weight_of_sample_of_water_1"] = _20mm.Weight_of_sample_of_water[2][1];
    _20_mm["Weight_of_sample_of_water_2"] = _20mm.Weight_of_sample_of_water[2][2];
    _20_mm["Weight_of_sample_of_water_3"] = _20mm.Weight_of_sample_of_water[2][3];
    _20_mm["Weight_of_SSD_Sample_1"] = _20mm.Weight_of_SSD_Sample[2][1];
    _20_mm["Weight_of_SSD_Sample_2"] = _20mm.Weight_of_SSD_Sample[2][2];
    _20_mm["Weight_of_SSD_Sample_3"] = _20mm.Weight_of_SSD_Sample[2][3];
    _20_mm["Weight_of_Oven_dry_sample_1"] = _20mm.Weight_of_Oven_dry_sample[2][1];
    _20_mm["Weight_of_Oven_dry_sample_2"] = _20mm.Weight_of_Oven_dry_sample[2][2];
    _20_mm["Weight_of_Oven_dry_sample_3"] = _20mm.Weight_of_Oven_dry_sample[2][3];
    qDebug() << "experiment info done";
    Specific_Gravity[ "20mm" ] = _20_mm;
    qDebug() << "experiment info done";
    if (current_save.open(QFile::WriteOnly | QFile::Text)) {
        // json data ko file me likhta hua mai
        QTextStream out(&current_save);
        QJsonDocument jsonDoc_1(Specific_Gravity);
        out << jsonDoc_1.toJson();

        // Close the file
        current_save.close();
        qDebug() << "Combined JSON data saved to file: " << "Jsonfile_1";
    }
    else {
        qDebug() << "Failed to open the file for writing.";
    }
}
void MainWindow::on_pushButton_2_clicked(){

    type_of_material _10mm;

    _10mm.Weight_of_sample_of_water[1][0] = 0;
    _10mm.Weight_of_SSD_Sample[1][0] = 0;
    _10mm.Weight_of_Oven_dry_sample[1][0] = 0;

    _10mm.Weight_of_sample_of_water[1][1] = ui->spc_10_w1_t1->text();
    _10mm.Weight_of_sample_of_water[1][2] = ui->spc_10_w1_t2->text();
    _10mm.Weight_of_sample_of_water[1][3] = ui->spc_10_w1_t3->text();
    _10mm.Weight_of_SSD_Sample[1][1] = ui->spc_10_w2_t1->text();
    _10mm.Weight_of_SSD_Sample[1][2] = ui->spc_10_w2_t2->text();
    _10mm.Weight_of_SSD_Sample[1][3] = ui->spc_10_w2_t3->text();
    _10mm.Weight_of_Oven_dry_sample[1][1] = ui->spc_10_w3_t1->text();
    _10mm.Weight_of_Oven_dry_sample[1][2] = ui->spc_10_w3_t2->text();
    _10mm.Weight_of_Oven_dry_sample[1][3] = ui->spc_10_w3_t3->text();

    QJsonObject _10_mm;

    _10_mm["Weight_of_sample_of_water_1"] = _10mm.Weight_of_sample_of_water[1][1];
    _10_mm["Weight_of_sample_of_water_2"] = _10mm.Weight_of_sample_of_water[1][2];
    _10_mm["Weight_of_sample_of_water_3"] = _10mm.Weight_of_sample_of_water[1][3];
    _10_mm["Weight_of_SSD_Sample_1"] = _10mm.Weight_of_SSD_Sample[1][1];
    _10_mm["Weight_of_SSD_Sample_2"] = _10mm.Weight_of_SSD_Sample[1][2];
    _10_mm["Weight_of_SSD_Sample_3"] = _10mm.Weight_of_SSD_Sample[1][3];
    _10_mm["Weight_of_Oven_dry_sample_1"] = _10mm.Weight_of_Oven_dry_sample[1][1];
    _10_mm["Weight_of_Oven_dry_sample_2"] = _10mm.Weight_of_Oven_dry_sample[1][2];
    _10_mm["Weight_of_Oven_dry_sample_3"] = _10mm.Weight_of_Oven_dry_sample[1][3];
    Specific_Gravity[ "10mm" ] = _10_mm;
    qDebug() << "experiment info done";
    if (current_save.open(QFile::WriteOnly | QFile::Text)) {
        // json data ko file me likhta hua mai
        QTextStream out(&current_save);
        QJsonDocument jsonDoc_1(Specific_Gravity);
        out << jsonDoc_1.toJson();

        // Close the file
        current_save.close();
        qDebug() << "Combined JSON data saved to file: " << "Jsonfile_1";
    }
    else {
        qDebug() << "Failed to open the file for writing.";
    }
}
void MainWindow::on_pushButton_3_clicked()
{
    type_of_material stone_dust;

    stone_dust.Weight_of_sample_of_water[0][0] = 0;
    stone_dust.Weight_of_SSD_Sample[0][0] = 0;
    stone_dust.Weight_of_Oven_dry_sample[0][0] = 0;

    stone_dust.Weight_of_sample_of_water[0][1] = ui->spc_0_w1_t1->text();
    stone_dust.Weight_of_sample_of_water[0][2] = ui->spc_0_w1_t2->text();
    stone_dust.Weight_of_sample_of_water[0][3] = ui->spc_0_w1_t3->text();
    stone_dust.Weight_of_SSD_Sample[0][1] = ui->spc_0_w2_t1->text();
    stone_dust.Weight_of_SSD_Sample[0][2] = ui->spc_0_w2_t2->text();
    stone_dust.Weight_of_SSD_Sample[0][3] = ui->spc_0_w2_t3->text();
    stone_dust.Weight_of_Oven_dry_sample[0][1] = ui->spc_0_w3_t1->text();
    stone_dust.Weight_of_Oven_dry_sample[0][2] = ui->spc_0_w3_t2->text();
    stone_dust.Weight_of_Oven_dry_sample[0][3] = ui->spc_0_w3_t3->text();

    QJsonObject _stone_dust;

    _stone_dust["Weight_of_sample_of_water_1"] = stone_dust.Weight_of_sample_of_water[0][1];
    _stone_dust["Weight_of_sample_of_water_2"] = stone_dust.Weight_of_sample_of_water[0][2];
    _stone_dust["Weight_of_sample_of_water_3"] = stone_dust.Weight_of_sample_of_water[0][3];
    _stone_dust["Weight_of_SSD_Sample_1"] = stone_dust.Weight_of_SSD_Sample[0][1];
    _stone_dust["Weight_of_SSD_Sample_2"] = stone_dust.Weight_of_SSD_Sample[0][2];
    _stone_dust["Weight_of_SSD_Sample_3"] = stone_dust.Weight_of_SSD_Sample[0][3];
    _stone_dust["Weight_of_Oven_dry_sample_1"] = stone_dust.Weight_of_Oven_dry_sample[0][1];
    _stone_dust["Weight_of_Oven_dry_sample_2"] = stone_dust.Weight_of_Oven_dry_sample[0][2];
    _stone_dust["Weight_of_Oven_dry_sample_3"] = stone_dust.Weight_of_Oven_dry_sample[0][3];
    Specific_Gravity[ "stone_dust" ] = _stone_dust;
    qDebug() << "experiment info done";
    if (current_save.open(QFile::WriteOnly | QFile::Text)) {
        // json data ko file me likhta hua mai
        QTextStream out(&current_save);
        QJsonDocument jsonDoc_1(Specific_Gravity);
        out << jsonDoc_1.toJson();

        // Close the file
        current_save.close();
        qDebug() << "Combined JSON data saved to file: " << "Jsonfile_1";
    }
    else {
        qDebug() << "Failed to open the file for writing.";
    }
}

void MainWindow::specificgravity(QJsonDocument jsonDoc_1, QFile file)
{

}


//Deals with Scrolling
void MainWindow::wheelEvent(QWheelEvent *event)
{
    //the mouse wheel API gives wheel inputs in delta, for most non gaming mice one notch turn a delta of 120
    //setting the sens in this method does not make any sense now, but in the future we will add a mouse sensitivity option in the View QMenuBar to change this
    this->scroll_sens = 20;
    QPoint delta = -1 * event->angleDelta();
    QPointF mouse_pos = event->position();
    int scroll_pos;

    if (mouse_pos.x() > 40 && mouse_pos.y() > 95 && mouse_pos.x() < 1410 && mouse_pos.y() < 760) {

        //By the scrollwheel, we are not directly moving any UI element, we are only calling the signal to change the scrollbar of any area.
        //This area has to be the current tab, which is selected with the switch operator
        switch (ui->tab_list->currentIndex()) {
        case 0:
            scroll_pos = ui->spc_data_scroll->value();
            ui->spc_data_scroll->setValue((int) (scroll_pos + delta.y()/scroll_sens));
            break;
        case 2:
            scroll_pos = ui->aiv_data_scroll->value();
            ui->aiv_data_scroll->setValue((int) (scroll_pos + delta.y()/scroll_sens));
            break;
        case 3:
            scroll_pos = ui->ind_data_scroll->value();
            ui->ind_data_scroll->setValue((int) (scroll_pos + delta.y()/scroll_sens));
            qDebug() << ui->ind_frame->y();
            break;
        default:
            break;
        }
    }
}

void MainWindow::on_spc_data_scroll_valueChanged(int value)
{
    float target = (ui->spc_frame_outer->height() - ui->spc_frame->height())*value/100;
    ui->spc_frame->move(0, target);
}

void MainWindow::on_ind_data_scroll_valueChanged(int value)
{
    float target = (ui->ind_frame_outer->height() - ui->ind_frame->height())*value/100;
    ui->ind_frame->move(0, target);
}

void MainWindow::on_aiv_data_scroll_valueChanged(int value)
{
    float target = (ui->aiv_frame_outer->height() - ui->aiv_frame->height())*value/100;
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
    bool update_info = false;

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
                                topush = json_lookups_data["Weight_of_sample_of_water_1"].toString().toStdString();
                                qDebug() << "value pushed" << topush;
                                break;
                            case 12:
                                topush = json_lookups_data["Weight_of_sample_of_water_2"].toString().toStdString();
                                break;
                            case 13:
                                topush = json_lookups_data["Weight_of_sample_of_water_3"].toString().toStdString();
                                break;
                            case 14:
                                topush = json_lookups_data["Weight_of_SSD_Sample_1"].toString().toStdString();
                                break;
                            case 15:
                                topush = json_lookups_data["Weight_of_SSD_Sample_2"].toString().toStdString();
                                break;
                            case 16:
                                topush = json_lookups_data["Weight_of_SSD_Sample_3"].toString().toStdString();
                                break;
                            case 17:
                                topush = json_lookups_data["Weight_of_Oven_dry_sample_1"].toString().toStdString();
                                break;
                            case 18:
                                topush = json_lookups_data["Weight_of_Oven_dry_sample_2"].toString().toStdString();
                                break;
                            case 19:
                                topush = json_lookups_data["Weight_of_Oven_dry_sample_3"].toString().toStdString();
                                break;
                            case 20:
                                // Bulk Specific Gravity
                                // Rest will have to be calculated
                                break;
                            }

                            output_html_file << topush;
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

//Unused functions
void specificgravity(QJsonDocument a1,QFile a2)
{
// DO NOT MODIFY
}
