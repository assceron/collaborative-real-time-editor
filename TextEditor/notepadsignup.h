//
// 08/08/2019.
//

#ifndef TEXTEDITOR_NOTEPADSIGNUP_H
#define TEXTEDITOR_NOTEPADSIGNUP_H

#include <QtGui/QtGui>
#include <QMainWindow>
#include "NetworkManager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class NotepadSignup;
}
QT_END_NAMESPACE

class NotepadSignup : public QMainWindow{
Q_OBJECT
public:
    explicit NotepadSignup(std::shared_ptr<NetworkManager> server, QWidget *parent = nullptr);
    ~NotepadSignup() override;
    void signup(const QString &username, const QString &password, const QString &secondPassword, const bool rememberMe);
private:
    Ui::NotepadSignup *_ui;
    bool _buttons_connected = false;
    std::shared_ptr<NetworkManager> _server;
    bool connectButtons(bool connect);
signals:
    void loginRequest();
    void signupDone();
private slots:
    void onSettingsClicked();

    void onSignupRequest();
    void onLoginClicked();
    void onSignupClicked();
    void onSignupResponseSlot(QJsonObject response);
};


#endif //TEXTEDITOR_NOTEPADSIGNUP_H
