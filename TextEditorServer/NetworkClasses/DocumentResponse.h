//
// 26/08/2019.
//

#ifndef TEXTEDITOR_DOCUMENTRESPONSE_H
#define TEXTEDITOR_DOCUMENTRESPONSE_H

#include "GenericMessage.h"

class DocumentResponse : public GenericMessage{
public:
    inline explicit DocumentResponse(QJsonObject json): GenericMessage(std::move(json)){};
    inline DocumentResponse(const unsigned int siteId, const QJsonObject &document):
            GenericMessage(bodyTypeToQString(BodyType::document_res)){
        insert("site_id", QString::number(siteId));
        insert("document", document);
    }
    [[nodiscard]] inline unsigned int getSiteId() const {
        return _json["site_id"].toString().toUInt();
    }
    [[nodiscard]] inline QJsonObject getDocument() const {
        return _json["document"].toObject();
    }
};

#endif //TEXTEDITOR_DOCUMENTRESPONSE_H
