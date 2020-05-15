//
// 31/08/2019.
//

#ifndef TEXTEDITOR_DOCUMENTINFOREQUEST_H
#define TEXTEDITOR_DOCUMENTINFOREQUEST_H

#include "GenericMessage.h"

class DocumentInfoRequest : public GenericMessage{
public:
    inline explicit DocumentInfoRequest(QJsonObject json): GenericMessage(std::move(json)){};
    inline explicit DocumentInfoRequest(const QString &docId):
            GenericMessage(bodyTypeToQString(BodyType::document_info_req)){
        insert("id", docId);
    }
    [[nodiscard]] inline QString getId() const {
        return _json["id"].toString();
    }
};

#endif //TEXTEDITOR_DOCUMENTINFOREQUEST_H
