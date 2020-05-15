//
// 23/08/2019.
//

#ifndef TEXTEDITOR_ABSTRACTREQUEST_H
#define TEXTEDITOR_ABSTRACTREQUEST_H

#include <QString>
#include <QJsonObject>
#include <QByteArray>
#include <QJsonValue>
#include <QtCore/QJsonDocument>
#include "MessageTypes.h"

class GenericMessage {
public:
    inline explicit GenericMessage(const BodyType type): _json(QJsonObject()){
        _json.insert("type", bodyTypeToQString(type));
    }
    inline explicit GenericMessage(const QString &type): _json(QJsonObject()){
        _json.insert("type", type);
    }
    inline explicit GenericMessage(QJsonObject json): _json(std::move(json)){}
    GenericMessage() = delete;
    [[nodiscard]] inline QByteArray toBinaryData() const {
        QJsonDocument doc{_json};
        return doc.toJson();
    }
    [[nodiscard]] inline QJsonObject toJson() const {
        return _json;
    }
    virtual ~GenericMessage() = default;
    inline void setError(){insert("_error_",true);};
    [[nodiscard]] inline bool isError() const {return _json["_error_"].toBool(false);};
    inline void setRejected(){insert("_rejected_", true);};
    [[nodiscard]] inline bool isRejected() const {return _json["_rejected_"].toBool(false);};
protected:
    inline QJsonObject::iterator insert(const QString &key, const QJsonValue &value){
        return _json.insert(key, value);
    }
    QJsonObject _json;
};


#endif //TEXTEDITOR_ABSTRACTREQUEST_H
