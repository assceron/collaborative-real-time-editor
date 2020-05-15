//
// 30/07/2019.
//

#include <iostream>
#include "MetaDocument.h"

MetaDocument::MetaDocument(const QJsonObject &json):
_cursor(json["cursor"].toObject()){
}

MetaDocument::MetaDocument(const CustomCursor &cursor):_cursor(cursor){
}

QJsonObject MetaDocument::toQJsonObject() const{
    QJsonObject json;
    json.insert("cursor", _cursor.toQJsonObject());
    return json;
}

QByteArray MetaDocument::toBytes() const {
    QJsonObject json = toQJsonObject();
    QJsonDocument doc(json);
    QByteArray bytes = doc.toJson();
    return bytes;
}

CustomCursor MetaDocument::getCursor() const {
    return _cursor;
}

MetaDocument::~MetaDocument() {
    //std::cout << "~MetaDocument()" << std::endl;
}
