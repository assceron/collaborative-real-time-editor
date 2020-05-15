//
// 8/22/19.
//

#ifndef TEXTEDITORSERVER_MAINSERVER_H
#define TEXTEDITORSERVER_MAINSERVER_H

#include <QObject>
#include <QList>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QByteArray>
#include "DBManager.h"
#include <QJsonObject>
#include "NetworkClasses/MessageTypes.h"
#include "ClientsManager.h"

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QByteArray)

#define MOCK_VARIABLES_ENABLED false

class MainServer: public QObject{
Q_OBJECT
public:
    MainServer(quint16 port, bool debug, QObject *parent = nullptr);
    ~MainServer() override;

private slots:
    void onNewConnectionSlot();
    void processMessageSlot(const QByteArray &message);
    void socketDisconnectedSlot();
    void onPongSlot(quint64 elapsedTime, const QByteArray &payload);
    void processIncomingMessage(QWebSocket *senderSocket, const QByteArray &json);
private:
    QWebSocketServer *_webSocketServer;
    const bool _debug;
    std::unique_ptr<DBManager> _dbManager;

    ClientsManager _clientsManager = ClientsManager{};
#if MOCK_VARIABLES_ENABLED
public:
    static unsigned int MOCK_document_counter;
    static unsigned int MOCK_site_id_counter;
#endif
};


#endif //TEXTEDITORSERVER_MAINSERVER_H
