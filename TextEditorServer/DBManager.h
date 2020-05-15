//
// Created by mirko on 23/08/19.
//

#ifndef TEXTEDITORSERVER_DBMANAGER_H
#define TEXTEDITORSERVER_DBMANAGER_H

#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <mongocxx/database.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include "NetworkClasses/SignupRequest.h"
#include "NetworkClasses/LoginRequest.h"
#include "NetworkClasses/SignupResponse.h"
#include "NetworkClasses/LoginResponse.h"
#include "NetworkClasses/DocumentsRequest.h"
#include "NetworkClasses/DocumentsResponse.h"
#include "NetworkClasses/NewDocumentResponse.h"
#include "NetworkClasses/DocumentInfo.h"
#include "NetworkClasses/DocumentResponse.h"
#include "NetworkClasses/NewDocumentRequest.h"
#include "NetworkClasses/DocumentRequest.h"
#include "NetworkClasses/DeleteDocumentRequest.h"
#include "NetworkClasses/Actions.h"
#include "NetworkClasses/SubscribeResponse.h"
#include "NetworkClasses/SubscribeRequest.h"
#include "NetworkClasses/CollaborationResponse.h"
#include "NetworkClasses/CollaborationRequest.h"
#include "NetworkClasses/DeleteDocumentResponse.h"

#define DB_NAME "text_editor_1"
#define COLLECTION_USERS_NAME "users"
#define COLLECTION_DOCUMENTS_NAME "documents"
#define COLLECTION_ENVIRONMENT_NAME "environments"
#define COLLECTION_ACTIONS_NAME "actions"


class DBManager {
public:
    DBManager();
    ~DBManager();
    SignupResponse signup(const SignupRequest &json);
    LoginResponse login(const LoginRequest &json);
    Actions getAllActions(const SubscribeRequest &json);
    DocumentsResponse getDocs(unsigned int siteId);
    DocumentInfo getDocInfo(const QString &docId);
    DocumentInfo getDocInfo(unsigned int docId);
    NewDocumentResponse newDoc(unsigned int siteId, const NewDocumentRequest &json);
    CollaborationResponse addCollaborator(const CollaborationRequest &json);
    CollaborationResponse removeCollaborator(const CollaborationRequest &json);
    DeleteDocumentResponse deleteDoc(unsigned int siteId, const DeleteDocumentRequest &json);
    DeleteDocumentResponse deleteDoc(unsigned int siteId, const QString &docId);
    DocumentResponse getDoc(unsigned int siteId, const DocumentRequest &json);
    unsigned int GetSiteId(QString name);
    void applyActions(const Actions &actions);
    std::vector<unsigned int>getCollaboratorIds(unsigned int docId);
    //void logout(const QJsonObject &json);
    inline static QJsonObject toQJson(const std::string &string){
        QByteArray array = QByteArray::fromStdString(string);
        QJsonDocument doc = QJsonDocument::fromJson(array);
        return doc.object();
    };
    inline static std::string toJString(const QJsonObject &object){
        QJsonDocument doc = QJsonDocument(object);
        QByteArray array = doc.toJson();
        return array.toStdString();
    };
private:
    mongocxx::database _db;
    mongocxx::client _client;
    mongocxx::collection _users;
    mongocxx::collection _documents;
    mongocxx::collection _actions;
    mongocxx::collection _environment;
    void addDocumentToUser(unsigned int docId, unsigned int siteId);
    void removeDocumentFromUser(unsigned int docId, unsigned int siteId);
    std::vector<QString>getCollaboratorNames(unsigned int docId);
    bool isDocumentAbandoned(unsigned int docId);
};


#endif //TEXTEDITORSERVER_DBMANAGER_H



/*  // ESEMPIO BUILDER

    auto builder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value doc_value = builder
            << "name" << "MongoDB"
            << "type" << "database"
            << "count" << 1
            << "versions" << bsoncxx::builder::stream::open_array
            << "v3.2" << "v3.0" << "v2.6"
            << close_array
            << "info" << bsoncxx::builder::stream::open_document
            << "x" << 203
            << "y" << 102
            << bsoncxx::builder::stream::close_document
            << bsoncxx::builder::stream::finalize;
    bsoncxx::document::view view = doc_value.view();
 */
