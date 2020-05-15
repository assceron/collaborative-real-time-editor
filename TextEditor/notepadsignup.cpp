//
// 08/08/2019.
//

#include "notepadsignup.h"
#include "ui_notepadsignup.h"
#include "InputManager.h"
#include "connectionsettingsdialog.h"
#include "NetworkClasses/SignupResponse.h"
#include <QString>
#include <iostream>
#include <QtWidgets/QMessageBox>

#define NOTEPAD_SIGNUP_DEBUG false

NotepadSignup::NotepadSignup(std::shared_ptr<NetworkManager> server, QWidget *parent):
QMainWindow(parent),
_ui(new Ui::NotepadSignup),
_server(std::move(server)){
    _ui->setupUi(this);
    _ui->passwordLineEdit->setEchoMode(QLineEdit::EchoMode::Password);
    _ui->repeatPasswordLineEdit->setEchoMode(QLineEdit::EchoMode::Password);

    connect(_server.get(), SIGNAL(signupResponseSignal(QJsonObject)), this, SLOT(onSignupResponseSlot(QJsonObject)));
    connect(_ui->actionSettings, &QAction::triggered, this, &NotepadSignup::onSettingsClicked);
    //this->setCentralWidget();
    connectButtons(true);
}

void NotepadSignup::onSettingsClicked() {
    auto *connectionSettingsDialog = new ConnectionSettingsDialog(_server, this);
    int retval = connectionSettingsDialog->exec();
    connectionSettingsDialog->deleteLater();
    // TODO: se retval è 1 allora ho salvato le impostazioni quindi devo fare qualcosa adesso...
}

NotepadSignup::~NotepadSignup() {
    delete _ui;
}

void NotepadSignup::onLoginClicked() {
    emit loginRequest();
    close();
}


void NotepadSignup::signup(const QString &username, const QString &password, const QString &secondPassword, const bool rememberMe) {
    auto &settings = Settings::get();
    settings.setUsername(username);
    settings.setPassword(password);
    settings.setRememberMe(rememberMe);

    _server->signup();
}


void NotepadSignup::onSignupClicked() {
    QString username = QString(this->_ui->usernameLineEdit->text());
    QString password = QString(this->_ui->passwordLineEdit->text());
    QString secondPassword = QString(this->_ui->repeatPasswordLineEdit->text());
    bool rememberMe = this->_ui->rememberMeCheckBox->isChecked();
#if NOTEPAD_LOGIN_DEBUG && false
    std::cout << username.toStdString() << " - " << password.toStdString() << " - " << (rememberMe?"true":"false") << std::endl;
#endif
    if(username.isEmpty() || password.isEmpty() || secondPassword.isEmpty()){
        QMessageBox *dialog = new QMessageBox{this};
        dialog->setWindowTitle("Warning");
        dialog->setIcon(QMessageBox::Icon::Critical);
        dialog->setText("Fill all fields.");
        dialog->exec();
        return;
    }
    if(password!=secondPassword){
        QMessageBox *dialog = new QMessageBox{this};
        dialog->setWindowTitle("Warning");
        dialog->setIcon(QMessageBox::Icon::Critical);
        dialog->setText("Passwords are different.");
        dialog->exec();
        return;
    }
    //connectButtons(false);
    signup(username, password, secondPassword, rememberMe);
}

bool NotepadSignup::connectButtons(const bool connect) {
    bool connected = _buttons_connected;
    if(connect xor connected) {
        if(connect) {
            this->connect(_ui->logInButton, SIGNAL(clicked()), this, SLOT(onLoginClicked()));
            this->connect(_ui->signUpButton, SIGNAL(clicked()), this, SLOT(onSignupClicked()));
        }else{
            this->disconnect(_ui->logInButton, SIGNAL(clicked()), this, SLOT(onLoginClicked()));
            this->disconnect(_ui->signUpButton, SIGNAL(clicked()), this, SLOT(onSignupClicked()));
        }
        _buttons_connected = connect;
    }
    return connected;
}

void NotepadSignup::onSignupRequest() {
    _ui->usernameLineEdit->clear();
    _ui->passwordLineEdit->clear();
    _ui->repeatPasswordLineEdit->clear();
    //connectButtons(true);
    show();
}

void NotepadSignup::onSignupResponseSlot(QJsonObject response) {
    SignupResponse res{std::move(response)};
    if(res.getAccepted()) {
        emit signupDone();
        close();
    }else{
        QMessageBox *dialog = new QMessageBox{this};
        dialog->setWindowTitle("Warning");
        dialog->setIcon(QMessageBox::Icon::Critical);
        dialog->setText("Username already in use.");
        dialog->exec();
        onSignupRequest();
    }
}
