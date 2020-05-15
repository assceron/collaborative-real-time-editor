//
// 27/08/2019.
//

#include <iostream>
#include "ClientsManager.h"

ClientsManager::~ClientsManager() {
    for(std::shared_ptr<Client> client : _clients){
        client->socket->deleteLater();
    }
    _clients.clear();
}

std::shared_ptr<ClientsManager::Client> ClientsManager::addClient(QWebSocket *socket) {
    auto newClient = std::make_shared<Client>(socket);
    _clients << newClient;
    return newClient;
}

void ClientsManager::removeClient(QWebSocket *socket) {
    if(socket){
        for(auto customClient : _clients){
            QWebSocket *temp = qAsConst(customClient->socket);
            if(socket == temp){
                temp->deleteLater();
                _clients.removeAll(customClient);
                break;
            }
        }
    }
}

QList<std::shared_ptr<ClientsManager::Client>> ClientsManager::getClientObjectsViewingDocument(const QString &docId, QWebSocket *exclude) {
    QList<std::shared_ptr<Client>> list;
    for(const std::shared_ptr<Client> &customClient : _clients){
        if(customClient->getOpenDocId()==docId) {
            if (customClient->socket != exclude) {
                list << customClient;
            }
        }
    }
    return list;
}

QList<QWebSocket *> ClientsManager::getClientsViewingDocument(const QString &docId, QWebSocket *exclude) {
    // flooding ignorante. controllare il db per vedere a chi mandare indietro i messaggi.
    // (non sto facendo il controllo del docId).
    QList<QWebSocket *> list;
    for(const std::shared_ptr<Client> &customClient : _clients){
        if(customClient->getOpenDocId()==docId) {
            QWebSocket *client = qAsConst(customClient->socket);
            if (client != exclude) {
                list << client;
            }
        }
    }
    return list;
}

QWebSocket * ClientsManager::getClientBySite(unsigned int siteId, QWebSocket *exclude) {
    for(std::shared_ptr<Client> customClient : _clients) {
        if (customClient->getSiteId() == siteId) {
            return customClient->socket;
        }
    }
    return nullptr;
}

std::shared_ptr<ClientsManager::Client> ClientsManager::getClient(QWebSocket *socket) {
    for(std::shared_ptr<Client> client : _clients){
        if(client->socket == socket){
            return client;
        }
    }
    return addClient(socket);
}

std::shared_ptr<ClientsManager::Client> ClientsManager::getClientById(unsigned int id) {
    for(std::shared_ptr<Client> client : _clients){
        if(client->getSiteId() == id){
            return client;
        }
    }
    return nullptr;
}
