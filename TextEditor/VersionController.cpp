//
// 22/05/2019.
//

#include "VersionController.h"
#include <algorithm>
#include <QtCore/QJsonArray>
#include <iostream>

VersionController::VersionController(const unsigned int siteId){
    _versions = std::vector<std::shared_ptr<Version>>();
    _versions.emplace_back(std::make_shared<Version>(siteId));
    _localVersion = *_versions.begin();
}

VersionController::VersionController(const QJsonObject &json, const unsigned int siteId){
    //_localVersion = std::make_shared<Version>(json["local_version"].toObject());
    //_siteId = json["local_version"].toString().toUInt();
    _versions = std::vector<std::shared_ptr<Version>>();
    QJsonArray versions = json["versions"].toArray();
        foreach(const QJsonValue &value, versions){
        _versions.emplace_back(std::make_shared<Version>(value.toObject()));
    }
    /*for(auto & _version : _versions){
        if(_version.get()->getSiteId()==siteId){
            _localVersion = _version;
            return;
        }
    }*/
    _localVersion = getVersionFromVector(siteId);
    if(_localVersion == nullptr){
        // version for my id not found
        //_versions = std::vector<std::shared_ptr<Version>>();
        _versions.emplace_back(std::make_shared<Version>(siteId));
        _localVersion = *(_versions.end()-1);
    }
}

VersionController::~VersionController() {
    //std::cout << "~VersionController()" << std::endl;
    _versions.clear();
}

QJsonObject VersionController::toQJsonObject() const {
    QJsonObject json;
    QJsonArray versions;
    for(int i=0; i<_versions.size(); i++) { // la dimensione di _versions non deve superare max int.
        versions.insert(i, _versions.at(i)->toQJsonObject());
    }
    json.insert("versions", versions);
    return json;
}

unsigned int VersionController::increment(){
    (*_localVersion)++;
    return _localVersion->getCounter();
}

std::shared_ptr<Version> VersionController::getVersionFromVector(const unsigned int siteId) {
    auto iter = std::find_if(_versions.begin(), _versions.end(),
                             [siteId](std::shared_ptr<Version> const& i){ return i.get()->getSiteId() == siteId; });
    if(iter!=_versions.end()){
        return *iter;
    }
    return nullptr;
}

std::shared_ptr<Version> VersionController::getVersionFromVector(const Version &incomingVersion){
    auto iter = std::find_if(_versions.begin(), _versions.end(),
                 [incomingVersion](std::shared_ptr<Version> const& i){ return i.get()->getSiteId() == incomingVersion.getSiteId(); });
    if(iter!=_versions.end()){
        return *iter;
    }
    return nullptr;
}

void VersionController::update(const Version &incomingVersion) {
    auto localIncomingVersion = getVersionFromVector(incomingVersion);
    if(localIncomingVersion == nullptr){
        auto newVersion = std::make_shared<Version>(incomingVersion.getSiteId());
        newVersion->update(incomingVersion);
        _versions.emplace_back(newVersion);
    }else{
        localIncomingVersion->update(incomingVersion);
    }
}

bool VersionController::hasBeenApplied(const Version &incomingVersion) {
    auto localIncomingVersion = getVersionFromVector(incomingVersion);
    if(localIncomingVersion == nullptr) {
        return false;
    }
    const bool isIncomingLower = incomingVersion.getCounter() <= localIncomingVersion->getCounter();
    const bool isInException = localIncomingVersion->containsException(incomingVersion.getCounter());

    return isIncomingLower && !isInException;
}
