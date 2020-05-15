//
// 27/08/2019.
//

#ifndef TEXTEDITORSERVER_UNSUBSCRIBE_REQUEST_H
#define TEXTEDITORSERVER_UNSUBSCRIBE_REQUEST_H

#include "GenericMessage.h"

class UnsubscribeRequest : public GenericMessage{
public:
    inline explicit UnsubscribeRequest(QJsonObject json): GenericMessage(std::move(json)){};
    inline explicit UnsubscribeRequest(const QString &docId):
            GenericMessage(bodyTypeToQString(BodyType::unsubscribe_req)){
        insert("doc_id", docId);
    }
    [[nodiscard]] inline QString getDocId() const {
        return _json["doc_id"].toString();
    }
};

#endif //TEXTEDITORSERVER_UNSUBSCRIBE_REQUEST_H
