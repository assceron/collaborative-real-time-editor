//
// 08/08/2019.
//

#ifndef TEXTEDITOR_NOTEPADLOGIN_H
#define TEXTEDITOR_NOTEPADLOGIN_H

#include <QtGui/QtGui>
#include <QMainWindow>
#include "NetworkManager.h"
#include "SharedDocument.h"
#include <QString>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui {
    class NotepadLogin;
}
QT_END_NAMESPACE

class NotepadLogin  : public QMainWindow{
Q_OBJECT

public:
    explicit NotepadLogin(std::shared_ptr<NetworkManager> server, QWidget *parent = nullptr);
    ~NotepadLogin() override;
    void login(const QString &username, const QString &password, const bool rememberMe);
    void login();
private:
    Ui::NotepadLogin *_ui;
    bool _buttons_connected = false;
    std::shared_ptr<NetworkManager> _server;
    bool connectButtons(bool connect);
    void refreshFields();
signals:
    void loginDone();
    void signupRequest();
private slots:
    void onSettingsClicked();
    void onLoginRequest();
    void onLoginClicked();
    void startupSlot();
    void onSignupClicked();
    void onLoginResponseSlot(QJsonObject response);
};

#endif //TEXTEDITOR_NOTEPADLOGIN_H
