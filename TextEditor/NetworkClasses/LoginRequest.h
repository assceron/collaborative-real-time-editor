//
// 23/08/2019.
//

#ifndef TEXTEDITOR_LOGINREQUEST_H
#define TEXTEDITOR_LOGINREQUEST_H

#include "GenericMessage.h"

class LoginRequest : public GenericMessage{
public:
    inline explicit LoginRequest(QJsonObject json): GenericMessage(std::move(json)){};
    inline LoginRequest(const QString &username, const QString &password):
            GenericMessage(bodyTypeToQString(BodyType::login_req)){
        insert("username", username);
        insert("password", password);
    };
    [[nodiscard]] inline QString getUsername() const {
        return _json["username"].toString();
    };
    [[nodiscard]] inline QString getPassword() const {
        return _json["password"].toString();
    };
};


#endif //TEXTEDITOR_LOGINREQUEST_H
