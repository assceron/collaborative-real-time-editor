//
// 8/27/19.
//

#ifndef TEXTEDITORSERVER_LOGOUTREQUEST_H
#define TEXTEDITORSERVER_LOGOUTREQUEST_H

#include "GenericMessage.h"

class LogoutRequest : public GenericMessage{
public:
    inline explicit LogoutRequest(QJsonObject json): GenericMessage(std::move(json)){};
    inline LogoutRequest(): GenericMessage(bodyTypeToQString(BodyType::logout_req)){};
};

#endif //TEXTEDITORSERVER_LOGOUTREQUEST_H
