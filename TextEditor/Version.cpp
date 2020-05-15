//
// 22/05/2019.
//

#include "Version.h"
#include <algorithm>
#include <QtCore/QJsonArray>
#include <iostream>

Version::Version(unsigned int siteId):_siteId(siteId),_counter(0){
    _exceptions = std::vector<unsigned int>();
}

Version::Version(unsigned int siteId, unsigned int counter):_siteId(siteId),_counter(counter){
}

Version::Version(const QJsonObject &json):_siteId(json["site_id"].toString().toUInt()), _counter(json["counter"].toString().toUInt()){
        QJsonArray exceptions = json["exceptions"].toArray();
            foreach(const QJsonValue &value, exceptions){
            _exceptions.push_back(value.toString().toUInt());
        }
}

Version::~Version() {
    //std::cout << "~Version()" << std::endl;
    _exceptions.clear();
}

QJsonObject Version::toQJsonObject() const {
    QJsonObject json;
    json.insert("site_id", QString::fromStdString(std::to_string(_siteId)));
    json.insert("counter", QString::fromStdString(std::to_string(_counter)));
    QJsonArray exceptions;
    for(int i=0; i<_exceptions.size(); i++) {
        exceptions.insert(i, QString::fromStdString(std::to_string(_exceptions.at(i))));
    }
    json.insert("exceptions", exceptions);
    return json;
}

unsigned int Version::getSiteId() const{
    return _siteId;
}

unsigned int Version::getCounter() const{
    return _counter;
}

bool Version::containsException(unsigned int exception) const{
    return std::find(_exceptions.begin(), _exceptions.end(), exception)!=_exceptions.end();
}

// Define prefix increment operator.
Version& Version::operator++()
{
    _counter++;
    return *this;
}
// Define postfix increment operator.
const Version Version::operator++(int)
{
    Version temp = *this;
    ++*this;
    return temp;
}
// Define prefix decrement operator.
Version& Version::operator--()
{
    _counter--;
    return *this;
}
// Define postfix decrement operator.
const Version Version::operator--(int)
{
    Version temp = *this;
    --*this;
    return temp;
}
bool Version::operator==(const Version& right) const{
    return (_siteId==right._siteId && _counter==right._counter);
}
bool Version::operator==(const int right) const{
    return _siteId==right;
}
bool Version::operator!=(const Version& right) const{
    return (_siteId!=right._siteId || _counter!=right._counter);
}
bool Version::operator!=(const int right) const{
    return _siteId!=right;
}
bool Version::operator<=(const Version &right) const{
    return (_siteId==right._siteId && _counter<=right._counter);
}
bool Version::operator<(const Version &right) const{
    return (_siteId==right._siteId && _counter<right._counter);
}
bool Version::operator>=(const Version &right) const{
    return (_siteId==right._siteId && _counter>=right._counter);
}
bool Version::operator>(const Version &right) const{
    return (_siteId==right._siteId && _counter>right._counter);
}

void Version::update(Version incomingVersion) {
    unsigned int incomingCounter = incomingVersion._counter;

    if(incomingCounter <= _counter){
        auto exceptionIterator = std::find(_exceptions.begin(), _exceptions.end(), incomingCounter);
        _exceptions.erase(exceptionIterator);
    } else if(incomingCounter == _counter + 1){
        _counter = _counter + 1;
    } else{
        for(unsigned int i = _counter + 1; i < incomingCounter; i++){
            _exceptions.push_back(i);
        }
        _counter = incomingCounter;
    }
}