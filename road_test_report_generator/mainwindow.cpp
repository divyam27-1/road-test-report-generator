#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "entry.h"
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    Dialog dialog;
    dialog.setModal(true);
    dialog.exec();
}


void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    ui->tab_list->removeTab(index);
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->tab_list->addTab(new entry(),QString("Tab %0").arg(ui->tab_list->count()+1));
}


void MainWindow::on_spc_save_clicked()
{
    struct basic_info basic_info_ = {ui->spc_bsc_1->toPlainText(),
                                     ui->spc_bsc_2->toPlainText(),
                                     ui->spc_bsc_3->toPlainText(),
                                     ui->spc_bsc_4->toPlainText()};
//    wrong sahi krenge kal ayush* sahu finding prlm in last stage;
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


void MainWindow::on_spc_data_scroll_valueChanged(int value)
{
    float target = (0-590)*value/100;
    ui->spc_frame->move(0, target);
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    //the mouse wheel API gives wheel inputs in delta, for most non gaming mice one notch turn a delta of 120
    //setting the sens in this method does not make any sense now, but in the future we will add a mouse sensitivity option in the View QMenuBar to change this
    this->scroll_sens = 20;
    QPoint delta = -1 * event->angleDelta();
    QPointF mouse_pos = event->position();
    int scroll_pos = ui->spc_data_scroll->value();


    if (mouse_pos.x() > 40 && mouse_pos.y() > 95 && mouse_pos.x() < 1410 && mouse_pos.y() < 760) {

        //By the scrollwheel, we are not directly moving any UI element, we are only calling the signal to change the scrollbar of any area.
        //This area has to be the current tab, which is selected with the switch operator
        switch (ui->tab_list->currentIndex()) {
        case 0:
            ui->spc_data_scroll->setValue((int) (scroll_pos + delta.y()/scroll_sens));
            break;
        case 1:
            ui->ind_scroll_bar_3->setValue((int) (scroll_pos + delta.y()/scroll_sens));
            break;
        default:
            break;
        }
    }
}

void MainWindow::on_save_40mm_clicked()
{
    type_of_material _40mm;

    //achha lagega:)
    _40mm.Weight_of_sample_of_water[4][0] = 0;
    _40mm.Weight_of_SSD_Sample[4][0] = 0;
    _40mm.Weight_of_Oven_dry_sample[4][0] = 0;

    _40mm.Weight_of_sample_of_water[4][1] = ui->spc_40_w1_t1->placeholderText();
    _40mm.Weight_of_sample_of_water[4][2] = ui->spc_40_w1_t2->placeholderText();
    _40mm.Weight_of_sample_of_water[4][3] = ui->spc_40_w1_t3->placeholderText();
    _40mm.Weight_of_SSD_Sample[4][1] = ui->spc_40_w2_t1->placeholderText();
    _40mm.Weight_of_SSD_Sample[4][2] = ui->spc_40_w2_t2->placeholderText();
    _40mm.Weight_of_SSD_Sample[4][3] = ui->spc_40_w2_t3->placeholderText();
    _40mm.Weight_of_Oven_dry_sample[4][1] = ui->spc_40_w3_t1->placeholderText();
    _40mm.Weight_of_Oven_dry_sample[4][2] = ui->spc_40_w3_t2->placeholderText();
    _40mm.Weight_of_Oven_dry_sample[4][3] = ui->spc_40_w3_t3->placeholderText();

    QJsonObject _40_mm;
   //achha lg rha hai??:) i am noobda
    _40_mm["Weight_of_sample_of_water_1"] = _40mm.Weight_of_sample_of_water[4][1];
    _40_mm["Weight_of_sample_of_water_2"] = _40mm.Weight_of_sample_of_water[4][2];
    _40_mm["Weight_of_sample_of_water_3"] = _40mm.Weight_of_sample_of_water[4][3];
    _40_mm["Weight_of_SSD_Sample_1"] = _40mm.Weight_of_SSD_Sample[4][1];
    _40_mm["Weight_of_SSD_Sample_2"] = _40mm.Weight_of_SSD_Sample[4][2];
    _40_mm["Weight_of_SSD_Sample_3"] = _40mm.Weight_of_SSD_Sample[4][3];
    _40_mm["Weight_of_Oven_dry_sample_1"] = _40mm.Weight_of_Oven_dry_sample[4][1];
    _40_mm["Weight_of_Oven_dry_sample_2"] = _40mm.Weight_of_Oven_dry_sample[4][2];
    _40_mm["Weight_of_Oven_dry_sample_3"] = _40mm.Weight_of_Oven_dry_sample[4][3];

    type_of_material _20mm;


    _20mm.Weight_of_sample_of_water[2][0] = 0;
    _20mm.Weight_of_SSD_Sample[2][0] = 0;
    _20mm.Weight_of_Oven_dry_sample[2][0] = 0;

    _20mm.Weight_of_sample_of_water[2][1] = ui->spc_20_w1_t1->placeholderText();
    _20mm.Weight_of_sample_of_water[2][2] = ui->spc_20_w1_t2->placeholderText();
    _20mm.Weight_of_sample_of_water[2][3] = ui->spc_20_w1_t3->placeholderText();
    _20mm.Weight_of_SSD_Sample[2][1] = ui->spc_20_w2_t1->placeholderText();
    _20mm.Weight_of_SSD_Sample[2][2] = ui->spc_20_w2_t2->placeholderText();
    _20mm.Weight_of_SSD_Sample[2][3] = ui->spc_20_w2_t3->placeholderText();
    _20mm.Weight_of_Oven_dry_sample[2][1] = ui->spc_20_w3_t1->placeholderText();
    _20mm.Weight_of_Oven_dry_sample[2][2] = ui->spc_20_w3_t2->placeholderText();
    _20mm.Weight_of_Oven_dry_sample[2][3] = ui->spc_20_w3_t3->placeholderText();

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


}

void MainWindow::on_ind_scroll_bar_3_valueChanged(int value)
{
    float target = (0-590)*value/100;
    ui->spc_frame->move(0, target);
}
