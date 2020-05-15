//
// 03/10/2019.
//

#ifndef TEXTEDITOR_DELETEDOCUMENTRESPONSE_H
#define TEXTEDITOR_DELETEDOCUMENTRESPONSE_H

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

#endif //TEXTEDITOR_DELETEDOCUMENTRESPONSE_H
