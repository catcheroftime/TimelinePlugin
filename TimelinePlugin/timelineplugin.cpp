#include "timelineplugin.h"
#include "ui_timelineplugin.h"

TimelinePlugin::TimelinePlugin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimelinePlugin)
{
    ui->setupUi(this);
}

TimelinePlugin::~TimelinePlugin()
{
    delete ui;
}
