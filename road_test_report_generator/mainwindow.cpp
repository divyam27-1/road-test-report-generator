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

        default:
            break;
        }
    }
}

