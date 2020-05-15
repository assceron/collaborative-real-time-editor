//
// 31/08/2019.
//

#ifndef TEXTEDITOR_DELETEDOCUMENTREQUEST_H
#define TEXTEDITOR_DELETEDOCUMENTREQUEST_H

#include "GenericMessage.h"

class DeleteDocumentRequest : public GenericMessage{
public:
    inline explicit DeleteDocumentRequest(QJsonObject json): GenericMessage(std::move(json)){};
    inline explicit DeleteDocumentRequest(const QString &docId):
            GenericMessage(bodyTypeToQString(BodyType::delete_document_req)){
        insert("id", docId);
    }
    [[nodiscard]] inline QString getId() const {
        return _json["id"].toString();
    }
};

#endif //TEXTEDITOR_DELETEDOCUMENTREQUEST_H
