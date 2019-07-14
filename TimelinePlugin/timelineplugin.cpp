#include "timelineplugin.h"
#include "ui_timelineplugin.h"

#include <QLabel>

TimelinePlugin::TimelinePlugin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimelinePlugin)
{
//    QLabel *label = new QLabel(this);
//    label->setText("test");

    ui->setupUi(this);
}
