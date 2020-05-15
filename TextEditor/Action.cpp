//
// 21/05/2019.
//

#include <iostream>
#include "Action.h"

Action::Action(const QString docId, const unsigned int sender, const unsigned int counter, const Action::MessageType type, const Symbol symbol)
:_sender(sender),_counter(counter),_type(type),_symbol(symbol),_docId(docId){
    //std::cout << "8" << std::endl;
    /*_timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch());*/
    //std::cout << "8.1" << std::endl;
}

Action::Action(const QJsonObject &json):
        _sender(json["sender"].toString().toUInt()),
        _counter(json["counter"].toString().toUInt()),
        _type(typeFromString(json["type"].toString())),
        _symbol(json["symbol"].toObject()),
        _docId(json["doc_id"].toString()){
}

Version Action::getVersion() const {
    return Version(getSender(), getCounter());
}

QString Action::getDocId() const {
    return _docId;
}

unsigned int Action::getSender() const{
    return _sender;
}

unsigned int Action::getCounter() const{
    return _counter;
}

Symbol Action::getSymbol() const {
    return _symbol;
}

/*std::chrono::milliseconds Action::getTimeStamp() const {
    return _timestamp;
}*/

Action::MessageType Action::getType() const {
    return _type;
}

std::string Action::getTypeString() const {
    std::string ret;
    switch(_type){
        case insert_type:{
            ret = "insert";
            break;
        }
        case delete_type:{
            ret = "delete";
            break;
        }
        default:{
            ret = "nop";
            break;
        }
    }
    return ret;
}

Action::MessageType Action::typeFromString(const QString &typeString) {
    if(typeString=="insert"){
        return MessageType::insert_type;
    }
    if(typeString=="delete"){
        return MessageType::delete_type;
    }
    return MessageType::nop;
}

QJsonObject Action::toQJsonObject() const {
    QJsonObject json;
    json.insert("sender", QString::number(_sender));
    json.insert("counter", QString::number(_counter));
    json.insert("type", QString::fromStdString(getTypeString())); // insert, delete, nop
    json.insert("symbol", _symbol.toQJsonObject());
    json.insert("doc_id", _docId);
    return json;
}

Action::~Action() {
    //std::cout << "Action)" << std::endl;
}
