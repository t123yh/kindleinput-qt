#include <QDebug>

#include "qkindleplatforminputcontext.h"

#include "qkindleplatforminputcontextplugin.h"


QKindlePlatformInputContextPlugin::QKindlePlatformInputContextPlugin(QObject *parent) :
    QPlatformInputContextPlugin(parent)
{
}

QKindlePlatformInputContext *QKindlePlatformInputContextPlugin::create(const QString& key, const QStringList& paramList)
{
    Q_UNUSED(paramList);

    if (key == QLatin1String("kindle")) {
        return new QKindlePlatformInputContext;
    }

    return 0;
}
