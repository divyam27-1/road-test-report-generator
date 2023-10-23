#include "dialog.h"
#include "ui_dialog.h"
#include <QPixmap>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    QPixmap pix("/home/nothing007/Downloads/yoo.png");
    ui->label_3->setPixmap(pix);
}

Dialog::~Dialog()
{
    delete ui;
}
