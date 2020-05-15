//
// 08/08/2019.
//

#include "notepadlogin.h"
#include "ui_notepadlogin.h"
#include "InputManager.h"
#include "connectionsettingsdialog.h"
#include <QString>
#include <iostream>
#include <QtWidgets/QMessageBox>

#define NOTEPAD_LOGIN_DEBUG false

NotepadLogin::NotepadLogin(std::shared_ptr<NetworkManager> server, QWidget *parent):
QMainWindow(parent),
_ui(new Ui::NotepadLogin),
_server(std::move(server)){
    _ui->setupUi(this);
    _ui->passwordLineEdit->setEchoMode(QLineEdit::EchoMode::Password);
    //this->setCentralWidget();

    connect(_server.get(), SIGNAL(loginResponseSignal(QJsonObject)), this, SLOT(onLoginResponseSlot(QJsonObject)));
    connect(_ui->actionSettings, &QAction::triggered, this, &NotepadLogin::onSettingsClicked);

    refreshFields();
    connectButtons(true);
}

void NotepadLogin::onSettingsClicked() {
    auto *connectionSettingsDialog = new ConnectionSettingsDialog(_server, this);
    int retval = connectionSettingsDialog->exec();
    connectionSettingsDialog->deleteLater();
    // TODO: se retval è 1 allora ho salvato le impostazioni quindi devo fare qualcosa adesso...
}

NotepadLogin::~NotepadLogin() {
    delete _ui;
}

void NotepadLogin::onSignupClicked() {
    emit signupRequest();
    close();
}

void NotepadLogin::refreshFields() {
    _ui->usernameLineEdit->setText(Settings::get().getUsername());
    _ui->passwordLineEdit->setText(Settings::get().getPassword());
    _ui->rememberMeCheckBox->setChecked(Settings::get().getRememberMe());
}

void NotepadLogin::login(){
    _server->login();
}

void NotepadLogin::login(const QString &username, const QString &password, const bool rememberMe){
    auto &settings = Settings::get();
    settings.setUsername(username);
    settings.setPassword(password);
    settings.setRememberMe(rememberMe);

    login();
}

void NotepadLogin::startupSlot(){
    disconnect(_server.get(), SIGNAL(connectedSignal()), this, SLOT(startupSlot()));
    disconnect(_server.get(), SIGNAL(disconnectedSignal()), this, SLOT(startupSlot()));
    auto &settings = Settings::get();
    if (_server->isConnected() && settings.getRememberMe()) {
        login();
    }/* else {
        show();
    }*/
}

void NotepadLogin::onLoginClicked() {
    QString username = QString(this->_ui->usernameLineEdit->text());
    QString password = QString(this->_ui->passwordLineEdit->text());
    bool rememberMe = this->_ui->rememberMeCheckBox->isChecked();
#if NOTEPAD_LOGIN_DEBUG && false
    std::cout << username.toStdString() << " - " << password.toStdString() << " - " << (rememberMe?"true":"false") << std::endl;
#endif
    if(username.isEmpty() || password.isEmpty()){
        QMessageBox *dialog = new QMessageBox{this};
        dialog->setWindowTitle("Warning");
        dialog->setIcon(QMessageBox::Icon::Critical);
        dialog->setText("Fill all fields.");
        dialog->exec();
        return;
    }
    //connectButtons(false);
    login(username, password, rememberMe);
}

bool NotepadLogin::connectButtons(const bool connect) {
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

void NotepadLogin::onLoginRequest() {
    _server->logout();
    refreshFields();
    //connectButtons(true);
    show();
}

void NotepadLogin::onLoginResponseSlot(QJsonObject response) {
    LoginResponse res{std::move(response)};
    if(res.getAccepted()) {
        emit loginDone();
        close();
    }else{
        QMessageBox *dialog = new QMessageBox{this};
        dialog->setWindowTitle("Warning");
        dialog->setIcon(QMessageBox::Icon::Critical);
        dialog->setText("Username and/or password invalid.");
        dialog->exec();
        onLoginRequest();
    }
}
