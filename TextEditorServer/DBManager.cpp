//
// Created by mirko on 23/08/19.
//

#include "DBManager.h"
#include "NetworkClasses/NewDocumentRequest.h"
#include "NetworkClasses/SubscribeResponse.h"
#include "NetworkClasses/SubscribeRequest.h"
#include <iostream>
#include <list>
#include <iterator>
#include <string>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <QtDebug>
#include <QtCore/QJsonArray>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;

DBManager::DBManager(){
    mongocxx::instance instance{}; // this should be done only once.
    //mongocxx::uri uri{"mongodb://localhost:27017"};
    //mongocxx::client client{mongocxx::uri{}};
    _client = mongocxx::client{mongocxx::uri{}};
    _db = _client[DB_NAME];
    _users = _db[COLLECTION_USERS_NAME];
    _documents = _db[COLLECTION_DOCUMENTS_NAME];
    _actions = _db[COLLECTION_ACTIONS_NAME];
    _environment = _db[COLLECTION_ENVIRONMENT_NAME];
    bsoncxx::stdx::optional<bsoncxx::document::value> environment = _environment.find_one({});
    if(!environment){ // devo creare l'environment.
        auto builder = bsoncxx::builder::stream::document{};
        bsoncxx::document::value doc_value = builder
                << "site_id_counter" << static_cast<int64_t>(0)
                << "document_id_counter" << static_cast<int64_t>(0)
                << bsoncxx::builder::stream::finalize;
        _environment.insert_one(doc_value.view());
    }
    /*bsoncxx::stdx::optional<bsoncxx::document::value> testDocument = _documents.find_one(document{} <<
            "id" << static_cast<int64_t>(0) <<
            finalize);
    if(!testDocument){ // devo creare il documento 0 per i test.
        auto builder = bsoncxx::builder::stream::document{};
        bsoncxx::document::value doc_value = builder
                << "id" << static_cast<int64_t>(0)
                << "name" << "TEST"
                << "site_id" << static_cast<int64_t>(0)
                << bsoncxx::builder::stream::finalize;
        _documents.insert_one(doc_value.view());
    }*/
}

DBManager::~DBManager() {

}

CollaborationResponse DBManager::addCollaborator(const CollaborationRequest &json) {
    bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
            _users.find_one(document{} <<
                                       "username" << json.getName().toUtf8().toStdString() <<
                                       finalize);
    if(maybe_result){ // username presente
        bsoncxx::document::element element = maybe_result.value().view()["site_id"];
        std::vector<QString> collaborators = getCollaboratorNames(json.getId().toUInt());
        for(auto collaborator : collaborators)
        {
            if(collaborator == json.getName())
            {
                CollaborationResponse response = CollaborationResponse{"","",true,json.getByURL()};
                response.setRejected();
                return response;
            }
        }
        //auto cursor = maybe_result.value().view().find(document{}
        //<< "documents" << static_cast<int64_t>(docId.toUInt()) << finalize);

        auto siteId = element.get_int64().value;
        addDocumentToUser(json.getId().toUInt(), siteId);
        CollaborationResponse response = CollaborationResponse{"","",true,json.getByURL()};
        return response;
    }else{
        CollaborationResponse response = CollaborationResponse{"","",true,json.getByURL()};
        response.setRejected();
        return response;
    }
}

unsigned int DBManager::GetSiteId(QString name) {
    unsigned int siteId = 0;
        bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
                _users.find_one(document{} <<
                                           "username" << name.toStdString() <<
                                           finalize);
        if (maybe_result) { // username presente
            bsoncxx::document::element element = maybe_result.value().view()["site_id"];
            siteId = element.get_int64().value;
        }
    return siteId;
}

CollaborationResponse DBManager::removeCollaborator(const CollaborationRequest &json) {
    bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
            _users.find_one(document{} <<
                                       "username" << json.getName().toUtf8().toStdString() <<
                                       finalize);
    if(maybe_result){ // username presente
        bsoncxx::document::element element = maybe_result.value().view()["site_id"];
        auto siteId = element.get_int64().value;
        removeDocumentFromUser(json.getId().toUInt(), siteId);
        if(isDocumentAbandoned(json.getId().toUInt())){
            deleteDoc(siteId, json.getId());
        }
        CollaborationResponse response = CollaborationResponse{"","",false,json.getByURL()};
        return response;
    }else{
        CollaborationResponse response = CollaborationResponse{"","",false,json.getByURL()};
        response.setRejected();
        return response;
    }
}

NewDocumentResponse DBManager::newDoc(unsigned int siteId, const NewDocumentRequest &object) {
    bsoncxx::stdx::optional<bsoncxx::document::value> environment = _environment.find_one({});
    if (environment) {
        bsoncxx::document::element element = environment.value().view()["document_id_counter"];
        auto counter = element.get_int64().value;
        unsigned int newCounter = counter + 1;
        _environment.update_one({}, document{} <<
                                               "$set" << open_document <<
                                               "document_id_counter" << static_cast<int64_t>(newCounter) <<
                                               close_document << finalize);
        auto builder = bsoncxx::builder::stream::document{};
        bsoncxx::document::value newDocument = builder
                << "id" << static_cast<int64_t>(newCounter)
                << "name" << object.getName().toUtf8().toStdString()
                << "site_id" << static_cast<int64_t>(siteId)
                << "actions" << open_array << close_array
                << bsoncxx::builder::stream::finalize;
        _documents.insert_one(newDocument.view());
        addDocumentToUser(newCounter, siteId);
        // TODO: CONVERTIRE IL DOCUMENT NEL DB IN UN DOCUMENT (prendere la classe Document dal client).
        NewDocumentResponse response{siteId, QString::number(newCounter), object.getName(), std::vector<QString>{}};
        return response;
    } else {
        NewDocumentResponse response{siteId, QString::number(0), object.getName(), std::vector<QString>{}};
        response.setError();
        return response;
    }
}

DeleteDocumentResponse DBManager::deleteDoc(unsigned int siteId, const QString &docId) {
    // qui si potrebbe fare un controllo per vedere se l'utente possiede davvero il documento...
    DocumentInfo info = getDocInfo(docId);
    if(!info.isRejected()){
        std::vector<unsigned int> collaborators = getCollaboratorIds(info.getId().toUInt());
        for(unsigned int collaborator : collaborators){
            removeDocumentFromUser(info.getId().toUInt(), collaborator);
        }
        _documents.delete_one(document{} << "id" << static_cast<int64_t>(info.getId().toUInt()) << finalize);
        DeleteDocumentResponse response = DeleteDocumentResponse{docId};
        return response;
    }else{
        DeleteDocumentResponse response = DeleteDocumentResponse{docId};
        response.setRejected();
        return response;
    }
}

DeleteDocumentResponse DBManager::deleteDoc(unsigned int siteId, const DeleteDocumentRequest &json) {
    return deleteDoc(siteId, json.getId());
}

void DBManager::addDocumentToUser(unsigned int docId, unsigned int siteId) {
    _users.update_one(document{} << "site_id" << static_cast<int64_t>(siteId) << finalize,
                      document{} << "$push" << open_document <<
                                 "documents" << static_cast<int64_t>(docId) << close_document << finalize);
}

void DBManager::removeDocumentFromUser(unsigned int docId, unsigned int siteId) {
    _users.update_one(document{} << "site_id" << static_cast<int64_t>(siteId) << finalize,
                      document{} << "$pull" << open_document <<
                                 "documents" << static_cast<int64_t>(docId) << close_document << finalize);
}

DocumentInfo DBManager::getDocInfo(const QString &docId) {
    return getDocInfo(docId.toUInt());
}

DocumentInfo DBManager::getDocInfo(unsigned int docId) {
    //std::cout << "DBManager::getDocs()Z1" << std::endl;

    bsoncxx::stdx::optional<bsoncxx::document::value> doc =
            _documents.find_one(document{} << "id" << static_cast<int64_t>(docId) << finalize);
    //std::cout << "DBManager::getDocs()Z2" << std::endl;

    if(doc) {
        QString docName = QString::fromStdString(doc.value().view()["name"].get_utf8().value.to_string());
        unsigned int docCreatorId = (unsigned int) doc.value().view()["site_id"].get_int64().value;
        std::vector<QString> collaborators = getCollaboratorNames(docId);
        std::vector<unsigned int> collaboratorIds = getCollaboratorIds(docId);
        DocumentInfo document = DocumentInfo{docCreatorId, QString::number(docId), docName, collaborators, collaboratorIds};
        return document;
    }else{
        DocumentInfo document = DocumentInfo{0, QString::number(docId), "", std::vector<QString>{}, std::vector<unsigned int>{}};
        document.setRejected();
        return document;
    }
}

std::vector<unsigned int> DBManager::getCollaboratorIds(unsigned int docId) {
    std::vector<unsigned int> collaborators = std::vector<unsigned int>{};
    mongocxx::cursor collaboratorsCursor =
            _users.find(document{} << "documents" << static_cast<int64_t>(docId) << finalize);
    mongocxx::cursor::iterator collIter = collaboratorsCursor.begin();
    while(collIter != collaboratorsCursor.end()){
        collaborators.push_back(static_cast<unsigned int>((*collIter)["site_id"].get_int64().value));
        collIter++;
    }
    return collaborators;
}



bool DBManager::isDocumentAbandoned(unsigned int docId) {
    auto maybe_result =
            _users.find_one(document{} << "documents" << static_cast<int64_t>(docId) << finalize);
    return !maybe_result;
}

std::vector<QString> DBManager::getCollaboratorNames(unsigned int docId) {
    std::vector<QString> collaborators = std::vector<QString>{};
    mongocxx::cursor collaboratorsCursor =
            _users.find(document{} << "documents" << static_cast<int64_t>(docId) << finalize);
    mongocxx::cursor::iterator collIter = collaboratorsCursor.begin();
    while(collIter != collaboratorsCursor.end()){
        collaborators.push_back(QString::fromStdString((*collIter)["username"].get_utf8().value.to_string()));
        collIter++;
    }
    return collaborators;
}

DocumentsResponse DBManager::getDocs(unsigned int siteId) {
    //std::cout << "DBManager::getDocs()" << std::endl;
    bsoncxx::stdx::optional<bsoncxx::document::value> user =
            _users.find_one(document{} << "site_id" << static_cast<int64_t>(siteId) << finalize);
    //std::cout << "DBManager::getDocs()1" << std::endl;
    if(user){ // user  presente
        std::vector<DocumentInfo> documents = std::vector<DocumentInfo>{};
        //std::cout << "DBManager::getDocs()2" << std::endl;

        bsoncxx::document::element element = user.value().view()["documents"];
        //std::cout << "DBManager::getDocs()3" << std::endl;

        auto iter = element.get_array().value.begin();
        while(iter!=element.get_array().value.end()){
            //std::cout << "DBManager::getDocs()N" << std::endl;

            unsigned int docId = (unsigned int)iter->get_int64().value;
            //std::cout << "DBManager::getDocs()N1" << std::endl;

            DocumentInfo document = getDocInfo(docId);
            //std::cout << "DBManager::getDocs()N2" << std::endl;

            if(!document.isRejected()){
                documents.push_back(document);
            }
            ++iter;
        }
        //std::cout << "DBManager::getDocs()4" << std::endl;
        DocumentsResponse response{documents};

        return response;
    }else{
        DocumentsResponse response{std::vector<DocumentInfo>{}};
        response.setError();
        return response;
    }
}

Actions DBManager::getAllActions(const SubscribeRequest &object){
    unsigned int uid = object.getDocId().toUInt();
    std::vector<Action> actions;
    bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
            _documents.find_one(document{} << "id" << static_cast<int64_t>(uid) << finalize);
    if(maybe_result)
    {

        auto vAction = maybe_result.value().view()["actions"].get_array().value;
        for(const bsoncxx::array::element& msg : vAction)
        {
            bsoncxx::document::view subdoc = msg.get_document().value;
            bsoncxx::document::element eCounter = subdoc["counter"];
            bsoncxx::document::element eType = subdoc["type"];
            bsoncxx::document::element eSender = subdoc["sender"];
            bsoncxx::document::element eDocid = subdoc["doc_id"];

            auto vSymbol = msg.get_document().view().find("symbol");
            auto subdoc_symbol = vSymbol->get_document().value;
            bsoncxx::document::element eCounter_s = subdoc_symbol["counter"];
            bsoncxx::document::element eSiteid = subdoc_symbol["site_id"];
            bsoncxx::document::element eValue = subdoc_symbol["value"];
            std::vector<uint32_t> sPos;

           auto vPos = subdoc_symbol["position"].get_array().value;
            for(const bsoncxx::array::element& pmsg : vPos) {
                auto ePos = pmsg.get_value();
                sPos.push_back(stoi(ePos.get_utf8().value.to_string()));
            }

            std::string sValue = eValue.get_utf8().value.to_string();
            std::string sSiteid = eSiteid.get_utf8().value.to_string();
            std::string sCounter = eCounter_s.get_utf8().value.to_string();
            std::string doc = eDocid.get_utf8().value.to_string();
            std::string sender = eSender.get_utf8().value.to_string();
            std::string counter = eCounter.get_utf8().value.to_string();
            Action::MessageType message = Action::typeFromString(QString::fromStdString(eType.get_utf8().value.to_string()));


            Symbol symb(QString::fromStdString(sValue)[0],stoi(sSiteid),stoi(sCounter),sPos);
            Action action(QString::fromStdString(doc),stoi(sender),stoi(counter),message,symb);
            actions.push_back(action);
        }
        //SEND OK
        return Actions(QString::fromStdString(object.getDocId().toStdString()),actions);
    }
    else
    {
        return Actions(QString::fromStdString("-1"),actions);
    }
}

SignupResponse DBManager::signup(const SignupRequest &object) {
    //TODO SANITIZE
    bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
            _users.find_one(document{} << "username" << object.getUsername().toUtf8().toStdString() << finalize);
    if(maybe_result){ // username già presente
        SignupResponse response{false, 0};
        return response;
    }
    else{
        bsoncxx::stdx::optional<bsoncxx::document::value> environment = _environment.find_one({});
        if(environment) {
            bsoncxx::document::element element = environment.value().view()["site_id_counter"];
            auto counter = element.get_int64().value;
            unsigned int newCounter = counter + 1;
            _environment.update_one({}, document{} <<
            "$set" << open_document <<
            "site_id_counter" << static_cast<int64_t>(newCounter) <<
            close_document << finalize);
            auto builder = bsoncxx::builder::stream::document{};
            bsoncxx::document::value newUser = builder
                    << "username" << object.getUsername().toUtf8().toStdString()
                    << "password" << object.getPassword().toUtf8().toStdString()
                    << "site_id" << static_cast<int64_t>(newCounter)
                    << "documents" << bsoncxx::builder::stream::open_array
                    //<< static_cast<int64_t>(0)
                    << close_array
                    << bsoncxx::builder::stream::finalize;
            _users.insert_one(newUser.view());
            SignupResponse response{true, newCounter};
            return response;
        }else{
            SignupResponse response{false, 0};
            response.setError();
            return response;
        }
    }
}

LoginResponse DBManager::login(const LoginRequest &object) {
    bsoncxx::stdx::optional<bsoncxx::document::value> maybe_result =
            _users.find_one(document{} <<
            "username" << object.getUsername().toUtf8().toStdString() <<
            "password" << object.getPassword().toUtf8().toStdString() <<
            finalize);
    if(!maybe_result){ // username non presente
        LoginResponse response{false, 0};
        return response;
    }
    else{
        bsoncxx::document::element element = maybe_result.value().view()["site_id"];
        auto siteId = element.get_int64().value;
        LoginResponse response{true, static_cast<unsigned int>(siteId)};
        //std::cout << "DB_MANAGER::login()->success" << std::endl;
        return response;
    }
}

void DBManager::applyActions(const Actions &actions) {
    //std::cout << "ApplyActions" << std::endl;
    QTextStream(stdout)<<QString(actions.toBinaryData())<<'\n';

    std::vector<Action> azioni = actions.getActions();
    for (auto it = azioni.begin(); it != azioni.end(); ++it){
        QJsonObject json = it->toQJsonObject();
        bsoncxx::document::value val = bsoncxx::from_json(toJString(json));
        _documents.update_one(document{} << "id" << static_cast<int64_t>(actions.getDocId().toUInt()) << finalize, document{} << "$push" << open_document <<
                     "actions" << val << close_document << finalize);
    }


    /*   "actions": [
       {
           "counter": "1",
                   "doc_id": "0",
                   "sender": "1",
                   "symbol": {
               "counter": "1",
                       "position": [
               "2",
                       "1"
               ],
               "site_id": "1",
                       "value": "a"
           },
           "type": "insert"
       }
       ],
       "doc_id": "0",
               "type": "actions"*/


}
