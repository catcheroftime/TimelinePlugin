#ifndef TIMELINEPLUGIN_H
#define TIMELINEPLUGIN_H

#include <QtUiPlugin/QDesignerExportWidget>
#include <QWidget>
#include <QEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QTime>

class TimeSelector;

namespace Ui {
class TimelinePlugin;
}

class QDESIGNER_WIDGET_EXPORT  TimelinePlugin : public QWidget
{
    Q_OBJECT
    struct WidgetInfo
    {
        int currentposition;
        int width;
        int minlength;
        int maxlength;
        QString color;
    };

    enum StretchDirction
    {
        NONE,
        RIGHT,
        LEFT
    };

public:
    struct TimecellDetail
    {
        QTime start_time;
        QTime end_time;
        QString color;
    };

public:
    explicit TimelinePlugin(QWidget *parent = 0);
    ~TimelinePlugin();

    QList<TimecellDetail> getTimeCell();
    void setTimeCell(const TimecellDetail &detail);
protected:
    bool eventFilter(QObject *target, QEvent *event);
    void paintEvent(QPaintEvent *);

private slots:
    void slotChangeTimeWidgetLength(QTime start_time, QTime end_time);

    void on_rb_red_clicked();
    void on_rb_green_clicked();
    void on_rb_blue_clicked();

private:
    Ui::TimelinePlugin *ui;
    TimeSelector* m_pTimeSelector;

    QList<QWidget *> m_listTimeCell;
    QMap<QWidget *,WidgetInfo> m_mapWidgetInfos;

    int m_widgetStartposition;
    QWidget * m_pCurrentWidget;         // 创建时判断

    int m_timecell_pressmouseX;
    int m_timecell_pressmouseGlobalX;
    QWidget * m_pCurrentTimeCell;       // 已存在的选中时判断

    StretchDirction m_direc;
    bool m_bmove;

    QString m_createWidgetColor;

private:
    void deleteTimecell();
    void showTimeSelector(QMouseEvent *mousemove, QWidget *timecell);
    bool validityOfStartposition(int current_position);
    void getCanMoveRange(QWidget *widget, int current_position, WidgetInfo &range);
    void getDateTime(int start_position, int width, QTime &start_time, QTime &end_time);
    bool getWidgetStartpositionAndWidth(const QTime start_time, const QTime end_time, int &width, int &start_position);


};

#endif // TIMELINEPLUGIN_H
