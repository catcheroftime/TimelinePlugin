#include "timeselector.h"
#include "ui_timeselector.h"

TimeSelector::TimeSelector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimeSelector)
{
    ui->setupUi(this);
}

TimeSelector::~TimeSelector()
{
    delete ui;
}

void TimeSelector::setDateTime(int start_position, int width, int parent_width)
{
    double start_count_second = (double)start_position / (double) parent_width;
    double start_h = start_count_second*24;
    double start_second = start_count_second*24*60 - (int)start_h*60;

    QTime start_time{(int)start_h,(int)start_second};
    ui->timeEdit_starttime->setTime(start_time);

    double end_count_second = ((double)start_position+(double)width )/ (double) parent_width;
    double end_h = end_count_second*24;
    double end_second = end_count_second*24*60 - (int)end_h*60 ;

    if ((int)end_second == 0) {
        QTime end_time{(int)end_h-1,59};
        ui->timeEdit_endtime->setTime(end_time);
    } else {
        QTime end_time{(int)end_h,(int)end_second};
        ui->timeEdit_endtime->setTime(end_time);
    }
}

void TimeSelector::on_ptn_push_clicked()
{
    QTime start_time = ui->timeEdit_starttime->time();
    QTime end_time = ui->timeEdit_endtime->time();

    int duration = end_time.hour()*60 + end_time.minute() - start_time.hour()*60 - start_time.minute();
    if (duration < 5) {
//        QMessage::Warning("时间段最小为5分钟!");
        return;
    }

    emit sigChangeTime(start_time, end_time);
}
