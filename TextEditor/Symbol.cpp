//
// 21/05/2019.
//

#include "Symbol.h"
#include <algorithm>
#include <QtCore/QJsonArray>
#include <QJsonValue>
#include <iostream>
#include <QtCore/QDataStream>
#include <QtCore/QJsonDocument>
#include <QString>
#include <QVariant>
#include <QIODevice>

Symbol::Symbol(const QChar &value, unsigned int siteId, unsigned int counter, std::vector<unsigned int> position):
    _value(value), _siteId(siteId), _counter(counter), _position(std::move(position)){}

Symbol::Symbol(const QJsonObject &json):
    _value(json["value"].toString().at(0)),
    _siteId(json["site_id"].toString().toUInt()),
    _counter(json["counter"].toString().toUInt())
        {
    QJsonArray positions = json["position"].toArray();
    foreach(const QJsonValue &value, positions){
        _position.push_back(value.toString().toUInt());
    }
}

Symbol::~Symbol() {
    //std::cout << "~Symbol()" << std::endl;
    _position.clear();
}

int Symbol::comparePositionWith(const Symbol &other) const{
    int comp=0, temp1=0, temp2=0;
    std::vector<unsigned int> pos1 = _position, pos2 = other._position;
    for(int i=0; i<std::min(pos1.size(), pos2.size()); i++){
        temp1 = pos1[i];
        temp2 = pos2[i];
        comp = temp1 - temp2;
        if(comp!=0){
            return comp;
        }
    }
    if(pos1.size()<pos2.size()){
        return -1;
    }else if(pos1.size()>pos2.size()){
        return 1;
    }else{
        return 0;
    }
}

bool Symbol::operator<=(const Symbol &right) const{
    return (this->comparePositionWith(right)<=0);
}
bool Symbol::operator<(const Symbol &right) const{
    return (this->comparePositionWith(right)<0);
}
bool Symbol::operator>=(const Symbol &right) const{
    return (this->comparePositionWith(right)>=0);
}
bool Symbol::operator>(const Symbol &right) const{
    return (this->comparePositionWith(right)>0);
}

Version Symbol::getVersion() const {
    return Version(getSiteId(), getCounter());
}

QChar Symbol::getValue() const {
    return _value;
}

unsigned int Symbol::getSiteId() const {
    return _siteId;
}

unsigned int Symbol::getCounter() const {
    return _counter;
}

std::vector<unsigned int> Symbol::getPosition() const {
    return _position;
}

QJsonObject Symbol::toQJsonObject() const {
    QJsonObject json;
    json.insert("value", QString(_value));
    json.insert("site_id", QString::number(_siteId));
    json.insert("counter", QString::number(_counter));

    QJsonArray position;
    for(int i=0; i<_position.size(); i++) {
        position.insert(i, QString::number(_position.at(i)));
    }
    json.insert("position", position);
    return json;
}