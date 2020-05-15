//
// 26/08/2019.
//

#ifndef TEXTEDITOR_SIGNUPRESPONSE_H
#define TEXTEDITOR_SIGNUPRESPONSE_H

#include "GenericMessage.h"

class SignupResponse : public GenericMessage{
public:
    inline explicit SignupResponse(QJsonObject json): GenericMessage(std::move(json)){};
    inline SignupResponse(const bool accepted, const unsigned int siteId):
            GenericMessage(bodyTypeToQString(BodyType::signup_res)){
        insert("accepted", accepted);
        insert("site_id", QString::number(siteId));
    }
    [[nodiscard]] inline bool getAccepted() const {
        return _json["accepted"].toBool();
    }
    [[nodiscard]] inline unsigned int getSiteId() const {
        return _json["site_id"].toString().toUInt();
    }
};

#endif //TEXTEDITOR_SIGNUPRESPONSE_H
