#ifndef MESSAGE_TYPES_H
#define MESSAGE_TYPES_H

#include <QtCore/QString>

enum BodyType{
    cursor_position, delete_document_res, delete_document_req, document_info, document_info_req, collaboration_req, collaboration_res, subscribe_req, unsubscribe_req, actions, summary_req, summary_res, new_document_req, new_document_res, documents_req, documents_res, document_req, document_res, login_req, login_res, signup_req, signup_res, logout_req, logout_res, closedoc_res,nop
};

static BodyType qStringToBodyType(const QString &qString) {
    if (qString == "actions") {
        return BodyType::actions;
    } else if (qString == "cursor_position") {
        return BodyType::cursor_position;
    } else if (qString == "delete_document_req") {
        return BodyType::delete_document_req;
    } else if (qString == "delete_document_res") {
        return BodyType::delete_document_res;
    } else if (qString == "document_info") {
        return BodyType::document_info;
    } else if (qString == "document_info_req") {
        return BodyType::document_info_req;
    } else if (qString == "collaboration_req") {
        return BodyType::collaboration_req;
    } else if (qString == "collaboration_res") {
        return BodyType::collaboration_res;
    } else if (qString == "subscribe_req") {
        return BodyType::subscribe_req;
    } else if (qString == "unsubscribe_req") {
        return BodyType::unsubscribe_req;
    } else if (qString == "summary_req") {
        return BodyType::summary_req;
    } else if (qString == "summary_res") {
        return BodyType::summary_res;
    } else if (qString == "new_document_req") {
        return BodyType::new_document_req;
    } else if (qString == "new_document_res") {
        return BodyType::new_document_res;
    } else if (qString == "document_req") {
        return BodyType::document_req;
    } else if (qString == "document_res") {
        return BodyType::document_res;
    } else if (qString == "documents_req") {
        return BodyType::documents_req;
    } else if (qString == "documents_res") {
        return BodyType::documents_res;
    } else if (qString == "login_req") {
        return BodyType::login_req;
    } else if (qString == "login_res") {
        return BodyType::login_res;
    } else if (qString == "signup_req") {
        return BodyType::signup_req;
    } else if (qString == "signup_res") {
        return BodyType::signup_res;
    } else if (qString == "logout_req") {
        return BodyType::logout_req;
    } else if (qString == "logout_res") {
        return BodyType::logout_res;
    } else if (qString == "nop") {
        return BodyType::nop;
    } else if (qString == "closedoc_res"){
        return BodyType::closedoc_res;
    }else{
        return BodyType::nop;
    }
}

static QString bodyTypeToQString(BodyType type) {
    switch (type){
        case actions: {
            return "actions";
        }case cursor_position:{
            return "cursor_position";
        }case delete_document_req:{
            return "delete_document_req";
        }case delete_document_res:{
            return "delete_document_res";
        }case document_info:{
            return "document_info";
        }case document_info_req:{
            return "document_info_req";
        }case collaboration_req:{
            return "collaboration_req";
        }case collaboration_res:{
            return "collaboration_res";
        }case subscribe_req:{
            return "subscribe_req";
        }case unsubscribe_req:{
            return "unsubscribe_req";
        }case summary_req:{
            return "summary_req";
        }case summary_res:{
            return "summary_res";
        }case new_document_req:{
            return "new_document_req";
        }case new_document_res:{
            return "new_document_res";
        }case document_req:{
            return "document_req";
        }case document_res:{
            return "document_res";
        }case documents_req:{
            return "documents_req";
        }case documents_res:{
            return "documents_res";
        }case login_req:{
            return "login_req";
        }case login_res:{
            return "login_res";
        }case signup_req:{
            return "signup_req";
        }case signup_res:{
            return "signup_res";
        }case logout_req:{
            return"logout_req";
        }case logout_res:{
            return "logout_res";
        }case nop:{
            return "nop";
        }default:{
            return "nop";
        }
    }
}

#endif