#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "addtimecell.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ptn_show_clicked()
{
    QList<TimelinePlugin::TimecellDetail> result = ui->timelinePlugin->getTimeCell();

    QString show_info;
    for (int i=0; i<result.count(); ++i) {
        TimelinePlugin::TimecellDetail info = result.at(i);
        show_info.append(info.start_time.toString("hh:mm"));
        show_info.append(" - ");
        show_info.append(info.end_time.toString("hh:mm"));
        show_info.append(",<font color=\"");
        show_info.append(getTimeCellColor(info.color));
        show_info.append("\">color:");
        show_info.append(info.color);
        show_info.append("</font>");
        show_info.append(";<br>\n</br>");
    }
    ui->textEdit_show->setText(show_info);
}

QString MainWindow::getTimeCellColor(QString color)
{
    if ( color == "rgb(255,0,0)")
        return "#FF0000";
    else if (color == "rgb(0,255,0)")
        return "#00FF00";
    else if(color == "rgb(0,0,255)")
        return "#0000FF";
    else
        return "#000000";
}

void MainWindow::on_ptn_add_clicked()
{
    AddTimeCell dialog{};
    if (dialog.exec() == QDialog::Accepted) {
        TimelinePlugin::TimecellDetail info;
        info.start_time = dialog.getStartTime();
        info.end_time = dialog.getEndTime();
        info.color = dialog.getColor();

        ui->timelinePlugin->setTimeCell(info);
    }
}
