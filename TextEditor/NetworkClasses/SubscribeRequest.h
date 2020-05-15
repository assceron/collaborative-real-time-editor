//
// 27/08/2019.
//

#ifndef TEXTEDITORSERVER_SUBSCRIBE_REQUEST_H
#define TEXTEDITORSERVER_SUBSCRIBE_REQUEST_H

#include "GenericMessage.h"

class SubscribeRequest : public GenericMessage{
public:
    inline explicit SubscribeRequest(QJsonObject json): GenericMessage(std::move(json)){};
    inline explicit SubscribeRequest(const QString &docId):
            GenericMessage(bodyTypeToQString(BodyType::subscribe_req)){
        insert("doc_id", docId);
    }
    [[nodiscard]] inline QString getDocId() const {
        return _json["doc_id"].toString();
    }
};

#endif //TEXTEDITORSERVER_SUBSCRIBE_REQUEST_H
