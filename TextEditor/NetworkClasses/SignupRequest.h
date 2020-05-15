//
// 26/08/2019.
//

#ifndef TEXTEDITOR_SIGNUPREQUEST_H
#define TEXTEDITOR_SIGNUPREQUEST_H

#include "GenericMessage.h"

class SignupRequest : public GenericMessage{
public:
    inline explicit SignupRequest(QJsonObject json): GenericMessage(std::move(json)){};
    inline SignupRequest(const QString &username, const QString &password):
            GenericMessage(bodyTypeToQString(BodyType::signup_req)){
        insert("username", username);
        insert("password", password);
    }
    [[nodiscard]] inline QString getUsername() const {
        return _json["username"].toString();
    }
    [[nodiscard]] inline QString getPassword() const {
        return _json["password"].toString();
    }
};

#endif //TEXTEDITOR_SIGNUPREQUEST_H
