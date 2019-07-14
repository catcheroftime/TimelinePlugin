#ifndef TIMESELECTOR_H
#define TIMESELECTOR_H

#include <QWidget>
#include <QTime>

namespace Ui {
class TimeSelector;
}

class TimeSelector : public QWidget
{
    Q_OBJECT

public:
    explicit TimeSelector(QWidget *parent = 0);
    ~TimeSelector();

    void setDateTime(int start_positon, int width, int parent_widget);

signals:
    void sigChangeTime(QTime, QTime);

private slots:
    void on_ptn_push_clicked();

private:
    Ui::TimeSelector *ui;
};

#endif // TIMESELECTOR_H
