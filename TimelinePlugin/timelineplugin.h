#ifndef TIMELINEPLUGIN_H
#define TIMELINEPLUGIN_H

#include <QWidget>
#include <QtUiPlugin/QDesignerExportWidget>
namespace Ui {
class TimelinePlugin;
}

class QDESIGNER_WIDGET_EXPORT  TimelinePlugin : public QWidget
{
    Q_OBJECT

public:
    explicit TimelinePlugin(QWidget *parent = 0);
    ~TimelinePlugin();

private:
    Ui::TimelinePlugin *ui;
};

#endif // TIMELINEPLUGIN_H
