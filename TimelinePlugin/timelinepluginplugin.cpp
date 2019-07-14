#include "timelineplugin.h"
#include "timelinepluginplugin.h"

#include <QtPlugin>

TimelinePluginPlugin::TimelinePluginPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void TimelinePluginPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool TimelinePluginPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *TimelinePluginPlugin::createWidget(QWidget *parent)
{
    return new TimelinePlugin(parent);
}

QString TimelinePluginPlugin::name() const
{
    return QLatin1String("TimelinePlugin");
}

QString TimelinePluginPlugin::group() const
{
    return QLatin1String("Customise");
}

QIcon TimelinePluginPlugin::icon() const
{
    return QIcon();
}

QString TimelinePluginPlugin::toolTip() const
{
    return QLatin1String("时间轴插件");
}

QString TimelinePluginPlugin::whatsThis() const
{
    return QLatin1String("一些描述");
}

bool TimelinePluginPlugin::isContainer() const
{
    return false;
}

QString TimelinePluginPlugin::domXml() const
{
    return QLatin1String("<widget class=\"TimelinePlugin\" name=\"timelinePlugin\">\n</widget>\n");
}

QString TimelinePluginPlugin::includeFile() const
{
    return QLatin1String("timelineplugin.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(timelinepluginplugin, TimelinePluginPlugin)
#endif // QT_VERSION < 0x050000
