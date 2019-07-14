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
    };

    enum StretchDirction
    {
        NONE,
        RIGHT,
        LEFT
    };

public:
    explicit TimelinePlugin(QWidget *parent = 0);
    ~TimelinePlugin();

    void deleteTimecell();
protected:
    bool eventFilter(QObject *target, QEvent *event);
    void paintEvent(QPaintEvent *);

private slots:
    void slotChangeTimeWidgetLength(QTime start_time, QTime end_time);

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

private:
    void showSettingTimeWidget(QMouseEvent *mousemove, QWidget *timecell);
    void getCanMoveRange(QWidget *widget, int current_position, WidgetInfo &range);
    void getDateTime(int start_position, int width, QTime &start_time, QTime &end_time);
    bool getWidgetStartpositionAndWidth(const QTime start_time, const QTime end_time, int &width, int &start_position);
};

#endif // TIMELINEPLUGIN_H
