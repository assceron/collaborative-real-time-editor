//
// Created by mirko on 03/10/19
//

#ifndef TEXTEDITORSERVER_DELETEDOCUMENTRESPONSE_H
#define TEXTEDITORSERVER_DELETEDOCUMENTRESPONSE_H


#include "GenericMessage.h"

class DeleteDocumentResponse : public GenericMessage{
public:
    inline explicit DeleteDocumentResponse(QJsonObject json): GenericMessage(std::move(json)){};
    inline explicit DeleteDocumentResponse(const QString &docId):
            GenericMessage(bodyTypeToQString(BodyType::delete_document_res)){
        insert("id", docId);
    }
    [[nodiscard]] inline QString getId() const {
        return _json["id"].toString();
    }
};


#endif //TEXTEDITORSERVER_DELETEDOCUMENTRESPONSE_H
