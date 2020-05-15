//
// 23/08/2019.
//

#ifndef TEXTEDITOR_SOCKETMANAGER_H
#define TEXTEDITOR_SOCKETMANAGER_H

#include <QtCore/QObject>
#include <QtWebSockets/QWebSocket>
#include <QByteArray>
#include <QJsonObject>
#include <mutex>
#include <condition_variable>
#include "Action.h"
#include <QAbstractSocket>

Q_DECLARE_METATYPE(QJsonObject)

class SocketManager : public QObject {
Q_OBJECT
public:
    SocketManager(const QString &ip, const QString &port, bool debug = false, QObject *parent = nullptr);
    explicit SocketManager(const QUrl &url, bool debug = false, QObject *parent = nullptr);
    ~SocketManager() override;
    void sendMessage(const QJsonObject &message);
    void sendMessage(const QByteArray &message);
    [[nodiscard]] bool isSocketConnected() const;
    [[nodiscard]] bool isSocketValid() const;
    void connectSocket();
    void disconnectSocket(QWebSocketProtocol::CloseCode closeCode = QWebSocketProtocol::CloseCodeNormal);
    void setAddress(const QUrl &url);
    void setAddress(const QString &ip, const QString &port);
signals:
    void connectedSignal();
    void disconnectedSignal();
private slots:
    void onConnectedSlot();
    void onDisconnectedSlot();
    void onErrorSlot(QAbstractSocket::SocketError error);
    void onMessageReceivedSlot(const QByteArray &message);
    void onMessageReadySlot(QJsonObject message);
private:
    QWebSocket _webSocket;
    QUrl _url = QUrl();
    bool _debug = false;
};


#endif //TEXTEDITOR_SOCKETMANAGER_H
