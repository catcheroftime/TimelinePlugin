#include "addtimecell.h"
#include "ui_addtimecell.h"

#include <QMessageBox>

AddTimeCell::AddTimeCell(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTimeCell)
{
    ui->setupUi(this);

    this->setWindowTitle("添加时间段");
    this->setWindowFlags(Qt::WindowCloseButtonHint | Qt::CustomizeWindowHint);
}

AddTimeCell::~AddTimeCell()
{
    delete ui;
}

QTime AddTimeCell::getStartTime()
{
    return ui->timeEdit_starttime->time();
}

QTime AddTimeCell::getEndTime()
{
    return ui->timeEdit_endtime->time();
}

QString AddTimeCell::getColor()
{
    int i = ui->comboBox_color->currentIndex();
    QString result;
    switch (i) {
    case 0:
        result = "rgb(255,0,0)";
        break;
    case 1:
        result = "rgb(0,255,0)";
        break;
    default:
        result = "rgb(0,0,255)";
        break;
    }
    return result;
}

void AddTimeCell::on_ptn_sure_clicked()
{
    if (ui->timeEdit_endtime->time() <= ui->timeEdit_starttime->time()) {
        QMessageBox::information(this, "错误", "错误~填写的开始时间需要小于结束时间！！");
        return;
    }

    accept();
}
