//
// 09/08/2019.
//

#ifndef TEXTEDITOR_SETTINGS_H
#define TEXTEDITOR_SETTINGS_H

#include <QtCore/QJsonObject>
#include <QByteArray>
#include <QString>
#include <memory>
#include "FileManager.h"

#define SERVER_ADDRESS_DEFAULT "127.0.0.1"
#define SERVER_PORT_DEFAULT "1234"
#define REMEMBER_ME_DEFAULT false

class Settings{//}; : public std::enable_shared_from_this<Settings> {
public:
    //static std::shared_ptr<Settings> get();
    static Settings &get();
    void load();
    void setFileManager(std::shared_ptr<FileManager> fileManager);

    [[nodiscard]] QString getServerAddressDefault() const;
    [[nodiscard]] QString getServerPortDefault() const;
    [[nodiscard]] bool getRememberMeDefault() const;

    ~Settings();
    [[nodiscard]] QJsonObject toQJsonObject() const;
    void setRememberMe(bool remember);
    [[nodiscard]] bool getRememberMe() const;
    [[nodiscard]] QString getUsername() const;
    [[nodiscard]] QString getPassword() const;
    [[nodiscard]] QString getServerAddress() const;
    [[nodiscard]] QString getServerPort() const;
    void setUsername(const QString &username);
    void setPassword(const QString &password);
    void setServerAddress(const QString &address);
    void setServerPort(const QString &port);
    [[nodiscard]] QByteArray toBytes() const ;

private:
    //explicit Settings(const QJsonObject &json);
    void save();
    Settings() = default;

    //static const QString SERVER_ADDRESS_DEFAULT;
    //static const QString SERVER_PORT_DEFAULT;
    //static const bool REMEMBER_ME_DEFAULT;

    //static std::shared_ptr<Settings> _instance;

    std::shared_ptr<FileManager> _fileManager;
    QString _username = QString();
    QString _password = QString();
    bool _rememberMe = REMEMBER_ME_DEFAULT;
    QString _serverAddress = QString(SERVER_ADDRESS_DEFAULT);
    QString _serverPort = QString(SERVER_PORT_DEFAULT);
    const bool _rememberMeDefault = REMEMBER_ME_DEFAULT;
    const QString _serverAddressDefault = QString(SERVER_ADDRESS_DEFAULT);
    const QString _serverPortDefault = QString(SERVER_PORT_DEFAULT);
};

#endif //TEXTEDITOR_SETTINGS_H