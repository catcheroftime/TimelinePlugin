#include "timelineplugin.h"
#include "ui_timelineplugin.h"
#include "timeselector.h"

#include <QPen>
#include <QFont>
#include <QPainter>
#include <QMessageBox>
#include <QDebug>

#define TIMECELL_ACCOUNT 30
#define MIN_DISTANCE 2

TimelinePlugin::TimelinePlugin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TimelinePlugin)
  , m_pTimeSelector(new TimeSelector(parent))
  , m_pCurrentWidget(nullptr)
  , m_pCurrentTimeCell(nullptr)
  , m_direc(StretchDirction::NONE)
  , m_bmove(false)
  , m_createWidgetColor("rgb(255,0,0)")
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    m_pTimeSelector->hide();

    ui->rb_red->setChecked(true);


    ui->widget_timeaxis->setStyleSheet("background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 rgb(120, 120, 120), stop:1 rgb(140, 140, 140));");
    ui->widget_timeaxis->installEventFilter(this);

    connect(m_pTimeSelector, &TimeSelector::sigChangeTime, this, &TimelinePlugin::slotChangeTimeWidgetLength);
}

TimelinePlugin::~TimelinePlugin()
{
    delete m_pTimeSelector;
    deleteTimecell();
    delete ui;
}

QList<TimelinePlugin::TimecellDetail> TimelinePlugin::getTimeCell()
{
    QList<TimelinePlugin::TimecellDetail> result;
    int count = m_listTimeCell.count();
    for (int i=0; i<count; ++i) {
        TimelinePlugin::TimecellDetail info;
        QWidget *timecell = m_listTimeCell.at(i);

        QTime start_time, end_time;
        this->getDateTime(timecell->pos().x(),timecell->width(), start_time,end_time);

        info.start_time = start_time;
        info.end_time = end_time;
        info.color = m_mapWidgetInfos[timecell].color;

        result.append(info);
    }
    return result;
}

void TimelinePlugin::setTimeCell(const TimelinePlugin::TimecellDetail &detail)
{
    QTime starttime = detail.start_time;
    QTime endtime = detail.end_time;

    int width,start_position;
    if(!getWidgetStartpositionAndWidth(starttime, endtime , width,start_position))
        return;

    WidgetInfo range;
    range.color = detail.color;
    bool invalid = getCanMoveRange(NULL, start_position, range);

    if (invalid && range.maxlength >= width + start_position && start_position >= range.minlength) {
        QWidget *timecell = new QWidget(ui->widget_timeaxis);
        timecell->resize(width, this->height());
        timecell->move(start_position, 0);
        timecell->show();
        QString background = QString("background-color:") +detail.color;
        timecell->setStyleSheet(background);
        timecell->installEventFilter(this);
        timecell->setMouseTracking(true);


        m_listTimeCell.append(timecell);
        m_mapWidgetInfos[timecell] = range;
    } else {
        QString title = QString("新添加的 ") + starttime.toString("hh:mm") +QString("-") + endtime.toString("hh:mm") + QString(" 时间段和已有的时间段存在冲突！！");
        QMessageBox::information(this, "错误", title);
        return;
    }
}

void TimelinePlugin::deleteTimecell()
{
    int count = m_listTimeCell.count();
    for (int i=0; i<count ; ++i) {
        QWidget * widget = m_listTimeCell.at(i);
        delete  widget;
        widget = NULL;
    }

    m_listTimeCell.clear();
    m_mapWidgetInfos.clear();
}

// 主要实现2部分， 1是创建新的时间段(timecell)，2是对已有的时间段(timecell)进行移动和拖拽
bool TimelinePlugin::eventFilter(QObject *target, QEvent *event)
{
    // 鼠标停留位置是上方绘制的时间条时， 返回
    QWidget *object = dynamic_cast<QWidget *>(target);
    if (object == ui->widget_graduation)
        return false;

    // 鼠标停留位置是时间轴空白位置， 需要生成新的 timecell
    if (object == ui->widget_timeaxis) {
        // 鼠标点击时， 记录timecell的开始位置， 准备生成新的timecell
        if (event->type() == QMouseEvent::MouseButtonPress) {
            QMouseEvent *mouse_press = static_cast<QMouseEvent *>(event);
            if (mouse_press->button() == Qt::LeftButton) {
                m_pTimeSelector->hide();
                // 时间段存在上限
                if (m_listTimeCell.count() >= TIMECELL_ACCOUNT) {
                    m_pCurrentWidget = nullptr;
                    return true;
                }

                m_widgetStartposition = mouse_press->pos().x();
                WidgetInfo info;
                getCanMoveRange(NULL, m_widgetStartposition, info);
                QWidget *new_timecell = new QWidget(ui->widget_timeaxis);
                new_timecell->resize(0, this->height());
                new_timecell->show();
                QString background = QString("background-color:") +m_createWidgetColor;
                new_timecell->setStyleSheet(background);
                new_timecell->installEventFilter(this);
                new_timecell->setMouseTracking(true);

                info.color = m_createWidgetColor;

                m_pCurrentWidget = new_timecell;
                m_listTimeCell.append(new_timecell);
                m_mapWidgetInfos[new_timecell] = info;
            }
        }
        // 鼠标移动过程中, 需要改变m_pCurrentWidget的大小以及位置
        if (event->type() == QMouseEvent::MouseMove) {
            QMouseEvent * mousemove = static_cast<QMouseEvent *> (event);
            int widget_end_postion = mousemove->pos().x();

            if (m_pCurrentWidget != nullptr) {
                WidgetInfo info = m_mapWidgetInfos.value(m_pCurrentWidget);
                // 鼠标移动过程中的位置， 需要在新生成时间段的有效范围内
                if (info.maxlength >= widget_end_postion && info.minlength <= widget_end_postion) {
                    int width =  widget_end_postion - m_widgetStartposition;
                    // 由于鼠标相对于其实位置， 可能向左或者向右移动， width大小正负都有可能
                    m_pCurrentWidget->resize( qAbs(width), this->height());
                    // timecell 的位置需要重新设置， 计算一下新的起始位置
                    int new_startpostion = width>0?m_widgetStartposition:widget_end_postion;

                    info.currentposition = new_startpostion;
                    m_mapWidgetInfos[m_pCurrentWidget] = info;
                    m_pCurrentWidget->move(new_startpostion, 0);
                    m_pCurrentWidget->show();
                }
            }
        }
        // 鼠标释放时， 代表新的timecell生成完毕
        if (event->type() == QMouseEvent::MouseButtonRelease ) {
            QMouseEvent * mouse_release = static_cast<QMouseEvent *> (event);
            if (mouse_release->button() == Qt::LeftButton) {
                if (m_pCurrentWidget) {
                    WidgetInfo info = m_mapWidgetInfos.value(m_pCurrentWidget);
                    info.currentposition = m_pCurrentWidget->pos().x();
                    info.width = m_pCurrentWidget->width();
                    m_mapWidgetInfos[m_pCurrentWidget] = info;
                }

                // 如果m_pCurrentWidget的宽度过小， 认为生成的timecell无效， 删除new的widget, 清除记录
                if (m_pCurrentWidget && m_pCurrentWidget->width() <= MIN_DISTANCE) {
                    delete  m_pCurrentWidget;
                    m_listTimeCell.removeAll(m_pCurrentWidget);
                    m_mapWidgetInfos.remove(m_pCurrentWidget);
                }
                m_pCurrentWidget = nullptr;
            }
        }
    }

    if (object != NULL && m_listTimeCell.contains(object) && object != m_pCurrentWidget) {

        if (event->type() == QMouseEvent::Enter) {
            this->setCursor(Qt::OpenHandCursor);
        } else if (event->type() == QMouseEvent::Leave) {
            this->setCursor(Qt::ArrowCursor);
        } else if (event->type() == QMouseEvent::MouseButtonPress) {
            QMouseEvent * mouse_press = static_cast<QMouseEvent *> (event);
            // 左击移动或者改变timecell大小 ， 右击删除timecell
            if (mouse_press->button() == Qt::LeftButton) {
                m_pCurrentTimeCell = object;
                // m_timecell_pressmouseX 这个值是相对于timecell
                m_timecell_pressmouseX = mouse_press->pos().x();
                m_timecell_pressmouseGlobalX = mouse_press->globalX();

                // 更新选中的timecell的信息
                WidgetInfo new_info = m_mapWidgetInfos.value(object);
                getCanMoveRange(object, object->pos().x(), new_info);
                new_info.currentposition = object->pos().x();
                new_info.width = object->width();
                m_mapWidgetInfos[object] = new_info;

                // 需要判断是移动timecell。 还是改变timecell宽度
                if (m_timecell_pressmouseX <= MIN_DISTANCE || m_timecell_pressmouseX >= object->width()- MIN_DISTANCE ) {
                    this->setCursor(Qt::SizeHorCursor);
                    if (m_timecell_pressmouseX <= MIN_DISTANCE )
                        m_direc = LEFT;
                    else
                        m_direc = RIGHT;
                    m_bmove = false;
                } else {
                    m_direc = NONE;
                    m_bmove = true;
                    this->setCursor(Qt::ClosedHandCursor);
                }
                showTimeSelector(mouse_press, object);

            } else if (mouse_press->button() == Qt::RightButton) {
                QTime start_time,end_time;
                getDateTime(object->pos().x(), object->width(), start_time, end_time);
                QString text = QString("是否确定删除选中的 ") + start_time.toString("hh:mm") +QString("-") + end_time.toString("hh:mm") + QString(" 时间段？");

                QMessageBox mb(QMessageBox::Question,"删除时间段",text,QMessageBox::Yes | QMessageBox::No,this);
                mb.setDefaultButton(QMessageBox::Yes);
                mb.setButtonText(QMessageBox::Yes,QObject::tr("确定"));
                mb.setButtonText(QMessageBox::No,QObject::tr("取消"));
                if(mb.exec() == QMessageBox::Yes){
                    m_listTimeCell.removeAll(object);
                    m_mapWidgetInfos.remove(object);
                    delete object;
                    object = nullptr;
                    this->setCursor(Qt::ArrowCursor);
                }
            }
        } else if (event->type() == QMouseEvent::MouseMove) {
            QMouseEvent * mousemove = static_cast<QMouseEvent *> (event);
            int mouse_x = mousemove->pos().x();

            int mouseTothis = mouse_x + object->pos().x();
            int mouse_golbalX = mousemove->globalX();

            // 需要判断此时是移动timecell还是改变timecell宽度
            WidgetInfo origin_info = m_mapWidgetInfos[object];
            int origin_position = origin_info.currentposition;

            if (mouse_x <= MIN_DISTANCE || mouse_x >=object->width()-MIN_DISTANCE) {
                if (!m_bmove)
                    this->setCursor(Qt::SizeHorCursor);
            } else {
                if (m_bmove) {
                    this->setCursor(Qt::ClosedHandCursor);
                    int move_length = mouse_golbalX - m_timecell_pressmouseGlobalX + origin_position;
                    if (origin_info.maxlength >= move_length + object->width() && origin_info.minlength <= move_length) {
                        object->move(move_length, 0);
                        showTimeSelector(mousemove, object);
                    }
                } else {
                    this->setCursor(Qt::OpenHandCursor);
                }
            }

            if (m_direc == RIGHT) {

                if (origin_info.maxlength >= mouseTothis && mouseTothis >= origin_position) {
                    int current_widget = mouseTothis - origin_position;
                    object->resize(current_widget, this->height() );
                    object->move(origin_position, 0);
                    showTimeSelector(mousemove, object);
                }
                return true;
            } else if (m_direc == LEFT) {
                int right_position = origin_position + origin_info.width;
                if (origin_info.minlength <= mouseTothis && mouseTothis <= right_position) {
                    int current_widget = right_position - mouseTothis;
                    object->resize(current_widget, this->height() );
                    object->move(mouseTothis, 0);
                    showTimeSelector(mousemove, object);
                }
                return true;
            }

        } else if (event->type() == QMouseEvent::MouseButtonRelease) {
            QMouseEvent * mouse_release = static_cast<QMouseEvent *> (event);
            if(mouse_release->button() == Qt::LeftButton )
            {
                m_bmove = false;
                if(object->width() < 2){
                    m_listTimeCell.removeAll(object);
                    m_mapWidgetInfos.remove(object);
                    delete object;
                    object = NULL;
                }
                m_direc = NONE;
                this->setCursor(Qt::OpenHandCursor);
            }
        }

    }
    return true;
}

void TimelinePlugin::paintEvent(QPaintEvent *)
{
    ui->widget_timeaxis->setStyleSheet("background-color:qlineargradient(x1:0, y1:0, x2:0, y2:1,stop:0 rgb(120, 120, 120), stop:1 rgb(140, 140, 140));");

    int start_position = ui->widget_graduation->pos().x();
    int height = ui->widget_graduation->pos().y() + ui->widget_graduation->height()-1;
    int width = ui->widget_timeaxis->width();

    QPainter painter(this);
    QPen hor_pen(Qt::black, 2);
    painter.setPen(hor_pen);
    painter.drawLine(start_position,height,width+start_position,height);

    int scale = ui->widget_graduation->width() / 24;

    QPen ver_pen(Qt::black, 1);
    painter.setPen(ver_pen);

    QFont font("宋体", 8);
    painter.setFont(font);

    for (int i = 0; i<=24 ; ++i ) {
        int x = i * scale;
        painter.drawLine(x+start_position, height - 5, x+start_position , height);

        int remainder = i%2 ;

        if (remainder == 0)
            painter.drawText(x+start_position-3,height - 8, QString::number(i));

    }
}

void TimelinePlugin::slotChangeTimeWidgetLength(QTime start_time, QTime end_time)
{
    int width,start_position;

    if(!getWidgetStartpositionAndWidth(start_time, end_time , width,start_position))
        return;

    WidgetInfo origin_range = m_mapWidgetInfos.value(m_pCurrentTimeCell);
    int originposition = origin_range.currentposition;
    WidgetInfo new_range;
    getCanMoveRange(m_pCurrentTimeCell, originposition, new_range);

    if (new_range.maxlength >= start_position+ width && start_position >= new_range.minlength) {
        m_pCurrentTimeCell->move(start_position  ,0);
        m_pCurrentTimeCell->resize(width, this->height());
    } else {
        QMessageBox::warning(this,"错误","和已有时间段有冲突!");
    }
}

void TimelinePlugin::showTimeSelector(QMouseEvent *mousemove, QWidget *timecell)
{
    int width = mousemove->pos().x() + timecell->pos().x() + this->pos().x();
    int height =  this->pos().y()+  ui->widget_timeaxis->pos().y() + ui->widget_timeaxis->height();

    m_pTimeSelector->move(width, height);
    m_pTimeSelector->setDateTime(timecell->pos().x(), timecell->width(),ui->widget_timeaxis->width());
    m_pTimeSelector->show();
}

bool TimelinePlugin::getCanMoveRange(QWidget *widget, int current_position, WidgetInfo &range)
{
    range.minlength = 0;
    range.maxlength = ui->widget_timeaxis->width();

    int count = m_listTimeCell .count();   

    for (int i=0; i< count; ++i) {
        QWidget *judge_widget = m_listTimeCell.at(i);
        if (judge_widget == widget) {
            continue;
        } else {
            int judge_start_point = judge_widget->pos().x();
            int judge_end_point = judge_start_point + judge_widget->width();

            if( current_position >= judge_start_point &&  current_position<= judge_end_point)
                return false;

            if (current_position >= judge_end_point && judge_end_point >= range.minlength) {
                range.minlength = judge_end_point;
                continue;
            }

            if(current_position <= judge_start_point && judge_start_point <= range.maxlength) {
                range.maxlength = judge_start_point;
                continue;
            }
        }
    }
    return true;
}

void TimelinePlugin::getDateTime(int start_position, int width, QTime &start_time, QTime &end_time)
{
    int parent_width = this-> ui->widget_timeaxis->width();
    double start_count_second = (double)start_position / (double) parent_width;
    double start_h = start_count_second*24;
    double start_second = start_count_second*24*60 - (int)start_h*60;

    QTime time{(int)start_h,(int)start_second};
    start_time = time;

    double end_count_second = ((double)start_position+(double)width )/ (double) parent_width;
    double end_h = end_count_second*24;
    double end_second = end_count_second*24*60 - (int)end_h*60 ;

    if ((int)end_second == 0) {
        QTime etime{(int)end_h-1,59};
        end_time = etime;
    } else {
        QTime etime{(int)end_h,(int)end_second};
        end_time = etime;
    }
}

bool TimelinePlugin::getWidgetStartpositionAndWidth(const QTime start_time, const QTime end_time, int &width, int &start_position)
{
    int start_h = start_time.hour();
    int start_m = start_time.minute();
    int count_start_m = start_h *60 +start_m;

    int end_h = end_time.hour();
    int end_m = end_time.minute();
    int count_end_m = end_h *60 +end_m;

    if (count_end_m < count_start_m) {
        QMessageBox::information(this, "错误","设定时间不合法!");
        return false;
    }
    width = (count_end_m - count_start_m  + 1) * ui->widget_timeaxis->width() / 1440;
    start_position = count_start_m * ui->widget_timeaxis->width() /1440;

    return true;
}

void TimelinePlugin::on_rb_red_clicked()
{
    m_createWidgetColor = "rgb(255,0,0)";
}

void TimelinePlugin::on_rb_green_clicked()
{
    m_createWidgetColor = "rgb(0,255,0)";
}

void TimelinePlugin::on_rb_blue_clicked()
{
    m_createWidgetColor = "rgb(0,0,255)";
}
