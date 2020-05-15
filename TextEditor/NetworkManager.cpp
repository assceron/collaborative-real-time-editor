//
// 21/05/2019.
//

#include "NetworkManager.h"
#include "SharedDocument.h"
#include "NetworkClasses/LoginRequest.h"
#include "NetworkClasses/LoginResponse.h"
#include "NetworkClasses/SignupResponse.h"
#include "NetworkClasses/SignupRequest.h"
#include "NetworkClasses/NewDocumentRequest.h"
#include "NetworkClasses/DocumentRequest.h"
#include "NetworkClasses/DocumentsRequest.h"
#include "NetworkClasses/DocumentsResponse.h"
#include "NetworkClasses/Actions.h"
#include "NetworkClasses/LogoutRequest.h"
#include "NetworkClasses/SubscribeRequest.h"
#include "NetworkClasses/UnsubscribeRequest.h"
#include "NetworkClasses/DocumentInfoRequest.h"
#include "NetworkClasses/DocumentInfo.h"
#include "NetworkClasses/CollaborationRequest.h"
#include "NetworkClasses/DeleteDocumentRequest.h"
#include "NetworkClasses/NewDocumentResponse.h"
#include "NetworkClasses/CursorPosition.h"
#include "NetworkClasses/CollaborationResponse.h"
#include <iostream>
#include <chrono>
#include <QtCore/QStringListIterator>
#include <QtCore/QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <map>

#define NETWORK_MANAGER_DEBUG true

NetworkManager::NetworkManager(bool debug, QObject *parent):
        QObject(parent),
        _socket(new SocketManager(
        Settings::get().getServerAddress(),
        Settings::get().getServerPort(),
        debug, this)),
        _siteId(0), // rimetterlo a 0
        _outgoing_actions(std::queue<Action>()){
    connect(_socket, SIGNAL(connectedSignal()), this, SIGNAL(connectedSignal()));
    connect(_socket, SIGNAL(disconnectedSignal()), this, SIGNAL(disconnectedSignal()));
    connect(this, SIGNAL(messageReadySignal(QJsonObject)), _socket, SLOT(onMessageReadySlot(QJsonObject)));
}

NetworkManager::~NetworkManager() {
    //std::cout << "~NetworkManager()" << std::endl;
    while(!_outgoing_actions.empty()){
        _outgoing_actions.pop();
    }
    _socket->deleteLater();
}

void NetworkManager::connectSocket(){
    disconnectSocket();
    _socket->setAddress(Settings::get().getServerAddress(), Settings::get().getServerPort());
    _socket->connectSocket();
}

void NetworkManager::disconnectSocket() {
    if(_socket->isSocketValid()) {
        _socket->disconnectSocket();
    }
}

void NetworkManager::signup(){
    if(_socket->isSocketValid()){
        SignupRequest req = SignupRequest(
                Settings::get().getUsername(),
                Settings::get().getPassword());
        _socket->sendMessage(req.toBinaryData());
    }
}

void NetworkManager::login(){
    if(_socket->isSocketValid()){
        LoginRequest req = LoginRequest{Settings::get().getUsername(),Settings::get().getPassword()};
        _socket->sendMessage(req.toBinaryData());
    }
}

void NetworkManager::logout() {
    if(_socket->isSocketValid()){
        LogoutRequest req = LogoutRequest{};
        _socket->sendMessage(req.toBinaryData());
    }
}

void NetworkManager::getDocuments() {
    if(_socket->isSocketValid()){
        DocumentsRequest req = DocumentsRequest{};
        _socket->sendMessage(req.toBinaryData());
    }
}

void NetworkManager::subscribe(const QString &docId) {
    if(_socket->isSocketValid()){
        SubscribeRequest req = SubscribeRequest{docId};
        _socket->sendMessage(req.toBinaryData());
    }
}

void NetworkManager::unsubscribe(const QString &docId) {
    if(_socket->isSocketValid()){
        UnsubscribeRequest req = UnsubscribeRequest{docId};
        _socket->sendMessage(req.toBinaryData());
    }
}

void NetworkManager::getNewDocument(const QString &docName) {
    if(_socket->isSocketValid()){
        NewDocumentRequest req = NewDocumentRequest{docName};
        _socket->sendMessage(req.toBinaryData());
    }
}

void NetworkManager::getDocument(const QString &docId) {
    if(_socket->isSocketValid()){
        DocumentRequest req = DocumentRequest{docId};
        _socket->sendMessage(req.toBinaryData());
    }
}

void NetworkManager::getDocumentInfo(const QString &docId) {
    if(_socket->isSocketValid()){
        DocumentInfoRequest req = DocumentInfoRequest{docId};
        _socket->sendMessage(req.toBinaryData());
    }
}

unsigned int NetworkManager::getSiteId() {
    return _siteId;
}

bool NetworkManager::isConnected() {
    return (_socket->isSocketValid() && _socket->isSocketConnected()); // se esplodono cose prova a cambiare st'espressione.
}

void NetworkManager::sendMessageToQueue(const Action &m) {
    _outgoing_actions.push(m);
}

void NetworkManager::stopMessagesDispatcher(){
    _stopMessagesDispatcher=true;
}

void NetworkManager::dispatchMessages() {
    std::multimap<QString, Action> actionsMap = std::multimap<QString, Action>{};
    while(!_outgoing_actions.empty()) {
        /*if(_stopMessagesDispatcher){
#if NETWORK_MANAGER_DEBUG
            std::cout << "NetworkManager::dispatchMessages() stopped by another thread." << std::endl;
#endif
            return;
        }*/
        const Action action = _outgoing_actions.front();
        //sendToServer(message);
        actionsMap.insert(std::pair<QString, Action>(action.getDocId(), action));
        _outgoing_actions.pop();
    }
    while(!actionsMap.empty()){
        std::vector<Action> vector = std::vector<Action>{};
        auto iter = actionsMap.begin();
        QString docId = iter->first;
        while (iter != actionsMap.end())
        {
            if(iter->first==docId){
                vector.push_back(iter->second);
                iter = actionsMap.erase(iter);
            }else{
                iter++;
            }
        }
        Actions actions = Actions{docId, vector};
        sendToServer(actions.toJson());
    }
#if NETWORK_MANAGER_DEBUG && false
    std::cout << "All messages in queue dispatched." << std::endl;
#endif
}

void NetworkManager::sendToServer (const QJsonObject &json) {
#if NETWORK_MANAGER_DEBUG && false
    std::cout << "sendToServer(QJsonObject)" << std::endl;
#endif
#if NETWORK_MANAGER_DEBUG && false
    printJSON(json);
#endif
    emit messageReadySignal(json);
}

void NetworkManager::printJSON(const QJsonObject &json) {
    //qDebug() << toQJsonObject(json);
    std::cout << serialize(json).toStdString() << std::endl;
}

QByteArray NetworkManager::serialize(const QJsonObject &json) {
    QJsonDocument doc(json);
    QByteArray bytes = doc.toJson();
    return bytes;
}

QJsonObject NetworkManager::deserialize(const QByteArray &bytes) {
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    QJsonObject json = doc.object();
    return json;
}

void NetworkManager::processIncomingMessage(const QJsonObject &json) {
    switch(qStringToBodyType(json["type"].toString())){
        case actions: {
            Actions response{json};
            emit actionsReceivedSignal(response.getActions());
            break;
        }
        case cursor_position:{
            //CursorPosition response{json};
            emit cursorReceivedSignal(json);
            break;
        }
        case login_res:{
            LoginResponse response{json};
            if(response.getAccepted()){
                unsigned int siteId = response.getSiteId();
                _siteId = siteId;
            }
            emit loginResponseSignal(json);
            break;
        }
        case signup_res:{
            SignupResponse response{json};
            if(response.getAccepted()){
                unsigned int siteId = response.getSiteId();
                _siteId = siteId;
            }
            emit signupResponseSignal(json);
            break;
        }
        case logout_res:{
            break;
        }
        case documents_res:{
            //DocumentsResponse response{json};
            emit documentsResponseSignal(json);
            break;
        }
        case document_info:{
            //DocumentInfo response{json};
            emit documentInfoResponseSignal(json);
            break;
        }
        case new_document_res:{
            //NewDocumentResponse response{json};
            emit newDocumentResponseSignal(json);
            break;
        }
        case delete_document_res:{
            //GenericMessage response{json};
            emit deleteDocumentResponseSignal(json);
            break;
        }
        case collaboration_res:{
            //CollaborationResponse response{json};
            emit collaborationResponseSignal(json);
            break;
        }
        default: {
            break;
        }
    }
}

void NetworkManager::addMeAsCollaborator(const QString &docId){
    if(_socket->isSocketValid()){
        auto &settings = Settings::get();
        QString Quser = settings.getUsername();
        CollaborationRequest req = CollaborationRequest{docId, Quser, true, true};
        _socket->sendMessage(req.toBinaryData());
    }
}

void NetworkManager::addCollaborator(const QString &docId, const QString &name) {
    if(_socket->isSocketValid()){
        CollaborationRequest req = CollaborationRequest{docId, name, true, false};
        _socket->sendMessage(req.toBinaryData());
    }
}

void NetworkManager::removeCollaborator(const QString &docId, const QString &name) {
    if(_socket->isSocketValid()){
        CollaborationRequest req = CollaborationRequest{docId, name, false, false};
        _socket->sendMessage(req.toBinaryData());
    }
}

void NetworkManager::deleteDocument(const QString &docId) {
    if(_socket->isSocketValid()) {
        DeleteDocumentRequest req = DeleteDocumentRequest{docId};
        _socket->sendMessage(req.toBinaryData());
    }
}

void NetworkManager::sendCursor(const QString &docId, const CustomCursor &cursor) {
    if(_socket->isSocketValid()){
        CursorPosition req = CursorPosition{docId, cursor};
        _socket->sendMessage(req.toBinaryData());
    }
}
