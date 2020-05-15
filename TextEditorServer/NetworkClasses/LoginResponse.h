//
// 26/08/2019.
//

#ifndef TEXTEDITORSERVER_LOGINRESPONSE_H
#define TEXTEDITORSERVER_LOGINRESPONSE_H


#include "GenericMessage.h"

class LoginResponse : public GenericMessage{
public:
    inline explicit LoginResponse(QJsonObject json): GenericMessage(std::move(json)){};
    inline LoginResponse(const bool accepted, const unsigned int siteId):
            GenericMessage(bodyTypeToQString(BodyType::login_res)){
        insert("accepted", accepted);
        insert("site_id", QString::number(siteId));
    }
    [[nodiscard]]inline bool getAccepted() const {
        return _json["accepted"].toBool();
    }
    [[nodiscard]] inline unsigned int getSiteId() const {
        return _json["site_id"].toString().toUInt();
    }
};


#endif //TEXTEDITORSERVER_LOGINRESPONSE_H
