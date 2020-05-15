//
// Created by mirko on 07/09/19.
//

#ifndef TEXTEDITORSERVER_SUBSCRIBERESPONSE_H
#define TEXTEDITORSERVER_SUBSCRIBERESPONSE_H

#include "GenericMessage.h"

class SubscribeResponse : public GenericMessage {
public:
    inline explicit SubscribeResponse(QJsonObject json): GenericMessage(std::move(json)){};
    inline SubscribeResponse(const QString &docId, const Actions &actions):
            GenericMessage(bodyTypeToQString(BodyType::subscribe_res)){
        insert("doc_id", docId);
        insert("actions", actions.toJson());
    }
   /* [[nodiscard]] inline bool getAccepted() const {
        return _json["accepted"].toBool();
    }
    [[nodiscard]] inline unsigned int getSiteId() const {
        return _json["site_id"].toString().toUInt();
    }*/

};
#endif //TEXTEDITORSERVER_SUBSCRIBERESPONSE_H
