//
// 26/08/2019.
//

#ifndef TEXTEDITOR_NEWDOCUMENTREQUEST_H
#define TEXTEDITOR_NEWDOCUMENTREQUEST_H

#include "GenericMessage.h"

class NewDocumentRequest : public GenericMessage{
public:
    inline explicit NewDocumentRequest(QJsonObject json): GenericMessage(std::move(json)){};
    inline explicit NewDocumentRequest(const QString &docName):
            GenericMessage(bodyTypeToQString(BodyType::new_document_req)){
        insert("name", docName);
    }
    [[nodiscard]] inline QString getName() const {
        return _json["name"].toString();
    }
};

#endif //TEXTEDITOR_NEWDOCUMENTREQUEST_H
