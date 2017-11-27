#ifndef QKINDLEPLATFORMINPUTCONTEXT_H
#define QKINDLEPLATFORMINPUTCONTEXT_H

#include <qpa/qplatforminputcontext.h>
#include <QString>
#include <openlipc.h>
#include <QInputMethodEvent>

QT_BEGIN_NAMESPACE

class QKindlePlatformInputContext : public QPlatformInputContext
{
    Q_OBJECT

public:
    QKindlePlatformInputContext();
    ~QKindlePlatformInputContext();

    bool isValid() const Q_DECL_OVERRIDE;
    void setFocusObject(QObject *object) Q_DECL_OVERRIDE;

    void showInputPanel() Q_DECL_OVERRIDE;
    void hideInputPanel() Q_DECL_OVERRIDE;
    bool isInputPanelVisible() const Q_DECL_OVERRIDE;

    QString lipcName;

private:

    void handleKeyboardGetSurround();
    void handleKeyboradReplace(int beforeCursor, int afterCursor, const QString &content);
    static LIPCcode LipcEventHandler(LIPC *lipc, const char *property,
                                     void *value, void *data);

    QSharedPointer<QInputMethodQueryEvent> focusObjectInputMethodQueryThreadSafe(Qt::InputMethodQueries queries = Qt::ImQueryAll);
    Q_INVOKABLE QInputMethodQueryEvent *focusObjectInputMethodQueryUnsafe(Qt::InputMethodQueries queries);

    void sendInputMethodEventThreadSafe(QInputMethodEvent *event);
    Q_INVOKABLE void sendInputMethodEventUnsafe(QInputMethodEvent *event);

    /*
    void HandleKeyboardDelete(void* value);
    void HandleKeyboardGetSurround(void* value);
    void HandleKeyboardSetPreeditString(void* value);
    */


    QObject *m_focusObject;
    LIPC *lipcInstance;
    static const QString keyboardLipcName;
};

QT_END_NAMESPACE

#endif // QKINDLEPLATFORMINPUTCONTEXT_H
