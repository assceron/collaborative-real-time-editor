//
// 09/08/2019.
//

#include <QtCore/QJsonDocument>
#include <iostream>
#include "Settings.h"
#include "FileManager.h"

//std::shared_ptr<Settings> Settings::_instance;

QString Settings::getServerAddressDefault() const {
    return _serverAddressDefault;
}

QString Settings::getServerPortDefault() const {
    return _serverPortDefault;
}

bool Settings::getRememberMeDefault() const {
    return _rememberMeDefault;
}

Settings &Settings::get() {
    //if(_instance== nullptr) {
    //    _instance = FileManager::getInstance().loadSettings();
    //}
    static Settings instance = Settings{};
    return instance;
}

Settings::~Settings() {
    save();
}

void Settings::save() {
    if(!getRememberMe()){
    setUsername("");
    setPassword("");
    }
    //FileManager::getInstance().storeSettings(temp);
    if(_fileManager!= nullptr) {
        _fileManager->storeSettings(*this);
    }
}

/*Settings::Settings():
_rememberMe(getRememberMeDefault()),
_serverAddress(getServerAddressDefault()),
_serverPort(getServerPortDefault()){
}*/

/*Settings::Settings(const QJsonObject &json):
        _username(json["username"].toString()),
        _password(json["password"].toString()),
        _rememberMe(json["remember_me"].toBool(getRememberMeDefault())),
        _serverAddress(json["server_address"].toString()),
        _serverPort(json["server_port"].toString()){
    if(_serverAddress.isEmpty()){
        _serverAddress = QString(_serverAddressDefault);
    }
    if(_serverPort.isEmpty()){
        _serverPort = QString(_serverPortDefault);
    }
}*/

QJsonObject Settings::toQJsonObject() const {
    QJsonObject json;
    json.insert("username", _username);
    json.insert("password", _password);
    json.insert("remember_me", _rememberMe);
    json.insert("server_address", _serverAddress);
    json.insert("server_port", _serverPort);
    return json;
}

void Settings::setRememberMe(bool remember) {
    _rememberMe = remember;
}

bool Settings::getRememberMe() const {
    return _rememberMe;
}

QByteArray Settings::toBytes() const {
    QJsonObject json = toQJsonObject();
    QJsonDocument doc = QJsonDocument(json);
    QByteArray bytes = doc.toJson();
    return bytes;
}

QString Settings::getUsername() const {
    return _username;
}

QString Settings::getPassword() const {
    return _password;
}

void Settings::setUsername(const QString &username) {
    _username = QString(username);
}

void Settings::setPassword(const QString &password) {
    _password = QString(password);
}

QString Settings::getServerAddress() const {
    return _serverAddress;
}

QString Settings::getServerPort() const {
    return _serverPort;
}

void Settings::setServerAddress(const QString &address) {
    _serverAddress = QString(address);
    if(_serverAddress.isEmpty()){
        _serverAddress = QString(getServerAddressDefault());
    }
}

void Settings::setServerPort(const QString &port) {
    _serverPort = QString(port);
    if(_serverPort.isEmpty()){
        _serverPort = QString(getServerPortDefault());
    }
}

void Settings::setFileManager(std::shared_ptr<FileManager> fileManager) {
    _fileManager = std::move(fileManager);
}

void Settings::load() {
    if(_fileManager!= nullptr) {
        QJsonObject json = _fileManager->loadSettings();
        _username = json["username"].toString();
        _password = json["password"].toString();
        _rememberMe = json["remember_me"].toBool(getRememberMeDefault());
        _serverAddress = json["server_address"].toString();
        _serverPort = json["server_port"].toString();
        if (_serverAddress.isEmpty()) {
            _serverAddress = QString(_serverAddressDefault);
        }
        if (_serverPort.isEmpty()) {
            _serverPort = QString(_serverPortDefault);
        }
    }
}
