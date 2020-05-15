//
// 23/08/2019.
//

#include <QtCore/QJsonDocument>
#include <iostream>
#include "SocketManager.h"
#include "NetworkManager.h"

SocketManager::SocketManager(const QString &ip, const QString &port, bool debug, QObject *parent)
:SocketManager(QUrl(QStringLiteral("ws://%1:%2").arg(ip, port)), debug, parent){
}

SocketManager::SocketManager(const QUrl &url, bool debug, QObject *parent) :
        QObject(parent),
        _url(url),
        _debug(debug)
{
    connect(&_webSocket, &QWebSocket::connected, this, &SocketManager::onConnectedSlot);
    connect(&_webSocket, &QWebSocket::disconnected, this, &SocketManager::onDisconnectedSlot);
    connect(&_webSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onErrorSlot(QAbstractSocket::SocketError)));
    //connect(&_webSocket, &QWebSocket::disconnected, this, &SocketManager::closedSignal);
    //_webSocket.open(QUrl(_url));
}

SocketManager::~SocketManager() {
    if(_webSocket.isValid()){
        disconnectSocket(QWebSocketProtocol::CloseCodeGoingAway);
    }
}

bool SocketManager::isSocketValid() const {
    return _webSocket.isValid();
}

bool SocketManager::isSocketConnected() const {
    return _webSocket.state() == QAbstractSocket::SocketState::ConnectedState;
}

void SocketManager::connectSocket() {
    if (_debug) {
        qDebug() << "Connecting to " << _url << '.';
    }
    _webSocket.open(QUrl(_url));
}

void SocketManager::disconnectSocket(QWebSocketProtocol::CloseCode closeCode) {
    if (_debug) {
        qDebug() << "Disconnecting.";
    }
    _webSocket.close(closeCode);
}

void SocketManager::setAddress(const QUrl &url) {
    _url = QUrl(url);
}

void SocketManager::setAddress(const QString &ip, const QString &port) {
    setAddress(QUrl(QStringLiteral("ws://%1:%2").arg(ip, port)));
}

void SocketManager::onConnectedSlot(){
    if (_debug) {
        qDebug() << "WebSocket connected";
    }
    connect(&_webSocket, &QWebSocket::binaryMessageReceived,
            this, &SocketManager::onMessageReceivedSlot);
    //_webSocket.sendTextMessage(QStringLiteral("Hello, world!"));
    emit connectedSignal();
}

void SocketManager::onDisconnectedSlot() {
    if (_debug) {
        qDebug() << "WebSocket disconnected";
    }
    disconnect(&_webSocket, &QWebSocket::binaryMessageReceived,
               this, &SocketManager::onMessageReceivedSlot);
    emit disconnectedSignal();
}

void SocketManager::onErrorSlot(QAbstractSocket::SocketError error){
    if (_debug) {
        qDebug() << "WebSocket error";
    }
}

void SocketManager::onMessageReceivedSlot(const QByteArray &message){
    if (_debug) {
        qDebug() << "Message received:" << QString(message);
    }
    QJsonDocument doc = QJsonDocument::fromJson(message);
    QJsonObject json = doc.object();
    qobject_cast<NetworkManager *>(parent())->processIncomingMessage(json);
}

void SocketManager::sendMessage(const QByteArray &message) {
    _webSocket.sendBinaryMessage(message);
}

void SocketManager::sendMessage(const QJsonObject &message) {
    QJsonDocument doc(message);
    QByteArray bytes = doc.toJson();
    sendMessage(bytes);
}

void SocketManager::onMessageReadySlot(QJsonObject message) {
    if(isSocketValid() && isSocketConnected()) {
        sendMessage(message);
    }
}
