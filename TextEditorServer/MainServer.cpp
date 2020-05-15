//
// 8/22/19.
//

#include "MainServer.h"
#include "NetworkClasses/LoginRequest.h"
#include "NetworkClasses/LoginResponse.h"
#include "NetworkClasses/SignupRequest.h"
#include "NetworkClasses/SignupResponse.h"
#include "NetworkClasses/NewDocumentRequest.h"
#include "NetworkClasses/Actions.h"
#include "NetworkClasses/DocumentRequest.h"
#include "NetworkClasses/DocumentsRequest.h"
#include "NetworkClasses/SubscribeRequest.h"
#include "NetworkClasses/UnsubscribeRequest.h"
#include "NetworkClasses/NewDocumentResponse.h"
#include "NetworkClasses/DocumentsResponse.h"
#include "NetworkClasses/DocumentResponse.h"
#include "NetworkClasses/DocumentInfoRequest.h"
#include "NetworkClasses/CollaborationRequest.h"
#include "NetworkClasses/CursorPosition.h"
#include "NetworkClasses/SubscribeResponse.h"
#include "NetworkClasses/DeleteDocumentResponse.h"

#include <QtCore>
#include <cstdio>
#include <iostream>

#define PDS_SERVER_DEBUG_BASIC_MESSAGES true
#define PDS_SERVER_DEBUG_ANNOYING_MESSAGES true

#if MOCK_VARIABLES_ENABLED
unsigned int MainServer::MOCK_document_counter = 0;
unsigned int MainServer::MOCK_site_id_counter = 0;
#endif

static QString getIdentifier(QWebSocket *peer){
    return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(),
            QString::number(peer->peerPort()));
}

MainServer::MainServer(quint16 port, bool debug, QObject *parent):
QObject(parent),
_debug(debug),
_webSocketServer(new QWebSocketServer(QStringLiteral("Text Editor Server"),
        QWebSocketServer::NonSecureMode, this)){
    if(_webSocketServer->listen(QHostAddress::Any, port)){
        if(_debug) {
#if PDS_SERVER_DEBUG_BASIC_MESSAGES
            QTextStream(stdout) << "Text Editor Server listening on port " << port << '\n';
#endif
        }
        connect (_webSocketServer, &QWebSocketServer::newConnection,
                this, &MainServer::onNewConnectionSlot);

        _dbManager = std::make_unique<DBManager>();
    }
}

MainServer::~MainServer() {
    _webSocketServer->close();
}

void MainServer::onNewConnectionSlot() {
    auto socket = _webSocketServer->nextPendingConnection();
    if(_debug) {
#if PDS_SERVER_DEBUG_BASIC_MESSAGES
        QTextStream(stdout) << getIdentifier(socket) << " connected.\n";
#endif
    }
    socket->setParent(this);

    connect(socket, &QWebSocket::binaryMessageReceived,
            this, &MainServer::processMessageSlot);
    connect(socket, &QWebSocket::disconnected,
            this, &MainServer::socketDisconnectedSlot);
    connect(socket, &QWebSocket::pong,
            this, &MainServer::onPongSlot);

    _clientsManager.addClient(socket);
}

void MainServer::processMessageSlot(const QByteArray &message) {
    QWebSocket *senderSocket = qobject_cast<QWebSocket *>(sender());
    if(_debug) {
#if PDS_SERVER_DEBUG_ANNOYING_MESSAGES
        QTextStream(stdout) << "Message received from " << getIdentifier(senderSocket) << ":\n" <<
        QString(message) << '\n';
#endif
    }
    processIncomingMessage(senderSocket, message);
}

void MainServer::processIncomingMessage(QWebSocket *senderSocket, const QByteArray &message) {
    QJsonDocument doc = QJsonDocument::fromJson(message);
    QJsonObject json = doc.object();
    std::shared_ptr<ClientsManager::Client> customClient = _clientsManager.getClient(senderSocket);
    switch(qStringToBodyType(json["type"].toString())){
        case actions: {
            if(customClient->isLogged()) {
                Actions request{json};
                //DB PERSISTENCE
                _dbManager->applyActions(request);
                for (QWebSocket *client : _clientsManager.getClientsViewingDocument(request.getDocId(), senderSocket)) {
                    client->sendBinaryMessage(message);
                }

            }
            break;
        }
        case cursor_position:{
            if(customClient->isLogged()) {
                CursorPosition request{json};
                customClient->setCursor(request.getCursor());

                request.setSiteId(customClient->getSiteId());
                request.setSiteColor(customClient->getColor());
                // TODO: inserire il nome del sender
                //request.setName();
                for (QWebSocket *client : _clientsManager.getClientsViewingDocument(request.getId(), senderSocket)) {
                    client->sendBinaryMessage(request.toBinaryData());
                }
            }
            break;
        }
        case login_req: {
            LoginRequest request{json};
            LoginResponse response = _dbManager->login(request);
            if(response.getAccepted()){
                customClient->setSiteId(response.getSiteId());
                customClient->login();
            }else{
                customClient->setSiteId(0);
                customClient->logout();
            }
            senderSocket->sendBinaryMessage(response.toBinaryData());
            break;
        }
        case signup_req: {
            SignupRequest request{json};
            SignupResponse response = _dbManager->signup(request);
            if(response.getAccepted()){
                customClient->setSiteId(response.getSiteId());
                customClient->login();
            }else{
                customClient->setSiteId(0);
                customClient->logout();
            }
            senderSocket->sendBinaryMessage(response.toBinaryData());
            break;
        }
        case subscribe_req:{
            if(customClient->isLogged()) {
                SubscribeRequest request{json};
                customClient->subscribe(request.getDocId());
                //std::cout << "MAIN SERVER";
                //DB_MAN
                Actions response = _dbManager -> getAllActions(request);
                if(response.getDocId().toInt() != -1) {
                    senderSocket->sendBinaryMessage(response.toBinaryData());
                }
                for(auto client : _clientsManager.getClientObjectsViewingDocument(response.getDocId(), senderSocket)){
                    CursorPosition cursor = CursorPosition{request.getDocId(), client->getCursor(), customClient->getSiteId(), ""};
                    if(!client->isCursorValid()) {
                        cursor.setRemove();
                    }
                    senderSocket->sendBinaryMessage(cursor.toBinaryData());
                }
            }
            break;
        }
        case unsubscribe_req:{
            if(customClient->isLogged()) {
                UnsubscribeRequest request{json};
                customClient->unsubscribe(request.getDocId());
                CursorPosition cursor = CursorPosition{request.getDocId(), CustomCursor{}, customClient->getSiteId(), ""};
                cursor.setRemove();
                // TODO: inserire il nome del sender
                //request.setName();
                for (QWebSocket *client : _clientsManager.getClientsViewingDocument(request.getDocId(), senderSocket)) {
                    client->sendBinaryMessage(cursor.toBinaryData());
                }
            }
            break;
        }
        case new_document_req:{
            if(customClient->isLogged()) {
                NewDocumentRequest request{json};
                NewDocumentResponse response = _dbManager->newDoc(customClient->getSiteId(), request);
                senderSocket->sendBinaryMessage(response.toBinaryData());
            }
            break;
        }
        case documents_req: {
            if(customClient->isLogged()) {
                DocumentsRequest request{json};
                DocumentsResponse response = _dbManager->getDocs(customClient->getSiteId());
                senderSocket->sendBinaryMessage(response.toBinaryData());
            }
            break;
        }
        case document_req: {
            if(customClient->isLogged()) {
                DocumentRequest request{json};
                // TODO: qua si prende il documento da qualche parte e gli si passa invece del json vuoto.
                DocumentResponse response{customClient->getSiteId(), QJsonObject{}};
                senderSocket->sendBinaryMessage(response.toBinaryData());
            }
            break;
        }
        case summary_req: {
            break;
        }
        case logout_req: {
            if(customClient->isLogged()) {
                customClient->setSiteId(0);
                customClient->logout();
                GenericMessage response{bodyTypeToQString(BodyType::logout_res)};
                senderSocket->sendBinaryMessage(response.toBinaryData());
            }
            break;
        }
        case document_info_req:{
            if(customClient->isLogged()) {
                DocumentInfoRequest request{json};
                DocumentInfo response = _dbManager->getDocInfo(request.getId());
                std::vector<QColor> colors = std::vector<QColor>{};
                for(unsigned int collaboratorId : response.getCollaboratorIds()) {
                    //std::cout << collaboratorId << std::endl;
                    if (_clientsManager.getClientById(collaboratorId) == nullptr) {
                        colors.push_back(QColor::fromRgb(255,255,255,255));
                    } else {
                        colors.push_back(_clientsManager.getClientById(collaboratorId)->getColor());
                    }
                }
                response.setColors(colors);
                senderSocket->sendBinaryMessage(response.toBinaryData());
            }
            break;
        }
        case delete_document_req:{
            if(customClient->isLogged()){
                DeleteDocumentRequest request{json};

                /*for (QWebSocket *client : _clientsManager.getClientsViewingDocument(request.getId(), senderSocket)) {
                    if(client != nullptr)
                    {
                        client->sendBinaryMessage(response.toBinaryData());
                    }
                }*/
                std::vector<unsigned int> vectorCollaborators = _dbManager->getCollaboratorIds(request.getId().toUInt());

                DeleteDocumentResponse response = _dbManager->deleteDoc(customClient->getSiteId(), request);

                for (unsigned int v:vectorCollaborators) {
                    QWebSocket *client = _clientsManager.getClientBySite(v);
                    if (client != nullptr) {
                        client->sendBinaryMessage(response.toBinaryData());
                    }
                }
                    //senderSocket->sendBinaryMessage(response.toBinaryData());
            }
            break;
        }
        case collaboration_req:{
            if(customClient->isLogged()){
                CollaborationRequest request{json};
                if(request.getAddCollaborator()) {
                    GenericMessage response = _dbManager->addCollaborator(request);
                    senderSocket->sendBinaryMessage(response.toBinaryData());
                    unsigned int siteid = _dbManager->GetSiteId(request.getName());
                    QWebSocket *client = _clientsManager.getClientBySite(siteid, senderSocket);
                    if(client != nullptr)
                    {
                        //riesco a trovare il socket, devo mandare un messaggio
                        DocumentsResponse mess = DocumentsResponse{_dbManager->getDocs(siteid)};
                        client->sendBinaryMessage(mess.toBinaryData());
                    }

                }else {
                    GenericMessage response = _dbManager->removeCollaborator(request);
                    senderSocket->sendBinaryMessage(response.toBinaryData());
                    unsigned int siteid = _dbManager->GetSiteId(request.getName());
                    QWebSocket *client = _clientsManager.getClientBySite(siteid,senderSocket);
                    if(client != nullptr)
                    {
                        DocumentsResponse mess = DocumentsResponse{_dbManager->getDocs(siteid)};
                        client->sendBinaryMessage(mess.toBinaryData());
                        DeleteDocumentResponse close = DeleteDocumentResponse(request.getId());
                        client->sendBinaryMessage(close.toBinaryData());
                    }
                }
            }
            break;
        }
        default: {
            break;
        }
    }
}


void MainServer::onPongSlot(quint64 elapsedTime, const QByteArray &payload) {
    QWebSocket *senderSocket = qobject_cast<QWebSocket *>(sender());
    if(_debug) {
#if PDS_SERVER_DEBUG_ANNOYING_MESSAGES
        QTextStream(stdout) << "Pong to " << getIdentifier(senderSocket) << ".\n";
#endif
    }
}

void MainServer::socketDisconnectedSlot() {
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if(_debug) {
#if PDS_SERVER_DEBUG_BASIC_MESSAGES
        QTextStream(stdout) << getIdentifier(client) << " disconnected with code " << client->closeCode() << ".\n";
#endif
    }
    _clientsManager.removeClient(client);
}