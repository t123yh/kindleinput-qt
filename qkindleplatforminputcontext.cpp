#include "qkindleplatforminputcontext.h"

#include <openlipc.h>
#include <unistd.h>
#include <QString>
#include <QInputMethodQueryEvent>
#include <QDebug>
#include <QGuiApplication>
#include <QInputMethodEvent>
#include <QThread>

const QString QKindlePlatformInputContext::keyboardLipcName = QString("com.lab126.keyboard");


LIPCcode QKindlePlatformInputContext::LipcEventHandler(LIPC *lipc, const char *property,
                                 void *value, void *data)
{
    Q_UNUSED(lipc);

    QKindlePlatformInputContext *self = (QKindlePlatformInputContext *)data;
    QString prop(property);
    if (prop == "keyboardGetSurround")
    {
        qDebug() << "Keyboard requesting getSurround";
        self->handleKeyboardGetSurround();
    }
    else if (prop == "keyboardCommit")
    {
        const char *str = static_cast<const char *>(value);
        qWarning() << "Keyboard calling commit (not implemented)" << str;
    }
    else if (prop == "keyboardSetPreeditString")
    {
        const char *str = static_cast<const char *>(value);
        qWarning() << "Keyboard calling preedit (not implemented)" << str;
    }
    else if (prop == "keyboardDelete")
    {
        const char *str = static_cast<const char *>(value);
        qWarning() << "Keyboard calling delete (not implemented)" << str;
    }
    else if (prop == "keyboardReplace")
    {
        const char *str = static_cast<const char *>(value);
        qDebug() << "Keyboard requesting replace " << str;
        QStringList list = QString(str).split(':');
        if (list.length() != 3)
        {
            qWarning() << "Wrong number of parameters: should be 3, received " << list.length();
        }
        else
        {
            self->handleKeyboradReplace(list[0].toInt(), list[1].toInt(), list[2]);
        }
    }
    else
    {
        qWarning() << "Unknown operation";
        return LIPC_ERROR_INVALID_ARG;
    }
    return LIPC_OK;
}

QKindlePlatformInputContext::QKindlePlatformInputContext()
{
    this->lipcName = QString("com.t123yh.qtInputMethod%1").arg(getpid());
    qDebug() << "Initializing QKindlePlatformInputContext, name = " << this->lipcName;
    this->lipcInstance = LipcOpen(this->lipcName.toUtf8().constData());
    LipcRegisterStringProperty(this->lipcInstance, "keyboardCommit", NULL, LipcEventHandler, this);
    LipcRegisterStringProperty(this->lipcInstance, "keyboardDelete", NULL, LipcEventHandler, this);
    LipcRegisterStringProperty(this->lipcInstance, "keyboardGetSurround", NULL, LipcEventHandler, this);
    LipcRegisterStringProperty(this->lipcInstance, "keyboardReplace", NULL, LipcEventHandler, this);
    LipcRegisterStringProperty(this->lipcInstance, "keyboardSetPreeditString", NULL, LipcEventHandler, this);
}

QKindlePlatformInputContext::~QKindlePlatformInputContext()
{
    qDebug() << "Destroying kindleplatforminputcontext";
    this->hideInputPanel();
    LipcClose(this->lipcInstance);
}

void QKindlePlatformInputContext::handleKeyboardGetSurround()
{
    auto queryResult = focusObjectInputMethodQueryThreadSafe(Qt::ImCursorPosition | Qt::ImSurroundingText);
    if (queryResult.isNull())
        return;
    QString txt = queryResult->value(Qt::ImSurroundingText).toString();
    int pos = queryResult->value(Qt::ImCursorPosition).toInt();
    LipcSetStringProperty(this->lipcInstance, this->keyboardLipcName.toUtf8().constData(), "setSurround", QString("%1:%2").arg(pos).arg(txt).toUtf8().constData());
}

void QKindlePlatformInputContext::handleKeyboradReplace(int beforeCursor, int afterCursor, const QString &content)
{
    auto queryResult = focusObjectInputMethodQueryThreadSafe(Qt::ImCursorPosition | Qt::ImSurroundingText);
    if (queryResult.isNull())
        return;
    int pos = queryResult->value(Qt::ImCursorPosition).toInt();
    if (pos < beforeCursor)
        beforeCursor = pos;
    QInputMethodEvent *event = new QInputMethodEvent;
    event->setCommitString(content, -beforeCursor, afterCursor + beforeCursor);
    sendInputMethodEventThreadSafe(event);
}

bool QKindlePlatformInputContext::isValid() const
{
    int value;
    return (LipcGetIntProperty(
                this->lipcInstance,
                QKindlePlatformInputContext::keyboardLipcName.toUtf8().constData(),
                "id", &value) == LIPC_OK);
}

void QKindlePlatformInputContext::setFocusObject(QObject *object)
{
    m_focusObject = object;
}

void QKindlePlatformInputContext::showInputPanel()
{
    LipcSetStringProperty(this->lipcInstance,
                          QKindlePlatformInputContext::keyboardLipcName.toUtf8().constData(),
                          "open",
                          QString("%1:abc:1").arg(this->lipcName).toUtf8().constData());
}

void QKindlePlatformInputContext::hideInputPanel()
{
    qDebug() << "Calling hideInputPanel";
    LipcSetStringProperty(this->lipcInstance,
                          QKindlePlatformInputContext::keyboardLipcName.toUtf8().constData(),
                          "close",
                          this->lipcName.toUtf8().constData());
}

bool QKindlePlatformInputContext::isInputPanelVisible() const
{
    qDebug() << "Checking isInputPanelVisible";
    int value;
    if (LipcGetIntProperty(
                this->lipcInstance,
                QKindlePlatformInputContext::keyboardLipcName.toUtf8().constData(),
                "show", &value) == LIPC_OK)
    {
        return value == 1;
    }
    return false;
}


QSharedPointer<QInputMethodQueryEvent> QKindlePlatformInputContext::focusObjectInputMethodQueryThreadSafe(Qt::InputMethodQueries queries) {
    QSharedPointer<QInputMethodQueryEvent> retval;
    if (!qGuiApp)
        return QSharedPointer<QInputMethodQueryEvent>();
    const bool inMainThread = qGuiApp->thread() == QThread::currentThread();

    QInputMethodQueryEvent *queryEvent = 0;
    QMetaObject::invokeMethod(this, "focusObjectInputMethodQueryUnsafe",
                              inMainThread ? Qt::DirectConnection : Qt::BlockingQueuedConnection,
                              Q_RETURN_ARG(QInputMethodQueryEvent*, queryEvent),
                              Q_ARG(Qt::InputMethodQueries, queries));

    return QSharedPointer<QInputMethodQueryEvent>(queryEvent);
}

QInputMethodQueryEvent *QKindlePlatformInputContext::focusObjectInputMethodQueryUnsafe(Qt::InputMethodQueries queries)
{
    QObject *focusObject = qGuiApp->focusObject();
    if (!focusObject)
        return 0;

    QInputMethodQueryEvent *ret = new QInputMethodQueryEvent(queries);
    QCoreApplication::sendEvent(focusObject, ret);
    return ret;
}


void QKindlePlatformInputContext::sendInputMethodEventUnsafe(QInputMethodEvent *event)
{
    qDebug() << "Beginning to send unsafe event";
    QObject *focusObject = qGuiApp->focusObject();
    if (!focusObject)
        return;

    qDebug() << "Sending unsafe event, focus = " << focusObject << ", event = " << event;

    QCoreApplication::sendEvent(focusObject, event);

    qDebug() << "Sent event";
}

void QKindlePlatformInputContext::sendInputMethodEventThreadSafe(QInputMethodEvent *event)
{
    if (!qGuiApp)
        return;
    const bool inMainThread = qGuiApp->thread() == QThread::currentThread();
    QMetaObject::invokeMethod(this, "sendInputMethodEventUnsafe",
                              inMainThread ? Qt::DirectConnection : Qt::BlockingQueuedConnection,
                              Q_ARG(QInputMethodEvent*, event));
    qDebug() << "returning";
}
