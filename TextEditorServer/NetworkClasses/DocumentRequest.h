//
// 26/08/2019.
//

#ifndef TEXTEDITOR_DOCUMENTREQUEST_H
#define TEXTEDITOR_DOCUMENTREQUEST_H

#include "GenericMessage.h"

class DocumentRequest : public GenericMessage{
public:
    inline explicit DocumentRequest(QJsonObject json): GenericMessage(std::move(json)){};
    inline explicit DocumentRequest(const QString &docId):
            GenericMessage(bodyTypeToQString(BodyType::document_req)){
        insert("id", docId);
    }
    [[nodiscard]] inline QString getId() const {
        return _json["id"].toString();
    }
};

#endif //TEXTEDITOR_DOCUMENTREQUEST_H
