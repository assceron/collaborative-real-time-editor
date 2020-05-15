//
// 21/05/2019.
//

#ifndef LAB3_NETWORKSERVER_H
#define LAB3_NETWORKSERVER_H

class SharedDocument;

#include <map>
#include <queue>
#include "SharedDocument.h"
#include "Action.h"
#include "SocketManager.h"
#include "NetworkClasses/LoginResponse.h"
#include <thread>
#include <mutex>
#include <future>
#include <atomic>
#include <memory>
#include <QJsonObject>
#include <QString>
#include <QByteArray>
#include <QObject>
#include "CustomCursor.h"

class NetworkManager : public QObject{
    Q_OBJECT
public:
    void connectSocket();
    void disconnectSocket();
    void sendMessageToQueue(const Action &action);
    void dispatchMessages();
    explicit NetworkManager(bool debug = false, QObject *parent = nullptr);
    ~NetworkManager() override;
    void stopMessagesDispatcher();
    unsigned int getSiteId();
    bool isConnected();
    void login();
    void signup();
    void logout();
    void getDocuments();
    void subscribe(const QString &docId);
    void unsubscribe(const QString &docId = QString{});
    void getNewDocument(const QString &docName);
    void getDocument(const QString &docId);
    void processIncomingMessage(const QJsonObject &json);
    void getDocumentInfo(const QString &docId);
    void addMeAsCollaborator(const QString &docId);
    void addCollaborator(const QString &docId, const QString &name);
    void removeCollaborator(const QString &docId, const QString &name);
    void deleteDocument(const QString &docId);
    void sendCursor(const QString &docId, const CustomCursor &cursor);
private:
    void sendToServer(const QJsonObject &json);
    void printJSON(const QJsonObject &json);
    QByteArray serialize(const QJsonObject &json);
    QJsonObject deserialize(const QByteArray &bytes);

    unsigned int _siteId;
    std::queue<Action> _outgoing_actions;
    std::atomic<bool> _stopMessagesDispatcher = false;
    SocketManager *_socket;
signals:
    void connectedSignal();
    void disconnectedSignal();
    void messageReadySignal(QJsonObject json);
    void actionsReceivedSignal(std::vector<Action> actions);
    void loginResponseSignal(QJsonObject response);
    void signupResponseSignal(QJsonObject response);
    void documentsResponseSignal(QJsonObject response);
    void documentInfoResponseSignal(QJsonObject response);
    void collaborationResponseSignal(QJsonObject response);
    void newDocumentResponseSignal(QJsonObject response);
    void deleteDocumentResponseSignal(QJsonObject response);
    void cursorReceivedSignal(QJsonObject response);
    void closeDocumentResponseSignal(QJsonObject response);
};


#endif //LAB3_NETWORKSERVER_H
