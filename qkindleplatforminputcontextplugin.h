#ifndef QKINDLEPLATFORMINPUTCONTEXTPLUGIN_H
#define QKINDLEPLATFORMINPUTCONTEXTPLUGIN_H

#include "qkindleplatforminputcontext.h"

#include <qpa/qplatforminputcontextplugin_p.h>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

class QKindlePlatformInputContextPlugin : public QPlatformInputContextPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QPlatformInputContextFactoryInterface_iid FILE "kindleinput.json")

public:
    QKindlePlatformInputContextPlugin(QObject *parent = 0);
    QKindlePlatformInputContext *create(const QString&, const QStringList&) Q_DECL_OVERRIDE;
};

QT_END_NAMESPACE

#endif // QKINDLEPLATFORMINPUTCONTEXTPLUGIN_H
