//
// 26/08/2019.
//

#ifndef TEXTEDITOR_DOCUMENTSREQUEST_H
#define TEXTEDITOR_DOCUMENTSREQUEST_H

#include "GenericMessage.h"

class DocumentsRequest : public GenericMessage{
public:
    inline explicit DocumentsRequest(QJsonObject json): GenericMessage(std::move(json)){};
    inline explicit DocumentsRequest():
            GenericMessage(bodyTypeToQString(BodyType::documents_req)){
    }
};

#endif //TEXTEDITOR_DOCUMENTSREQUEST_H
