//
// 14/08/2019.
//

#include <iostream>
#include "connectionsettingsdialog.h"
#include "ui_connectionsettingsdialog.h"
#include "Settings.h"

ConnectionSettingsDialog::ConnectionSettingsDialog(std::shared_ptr<NetworkManager> server, QWidget *parent) :
QDialog(parent),
_server(std::move(server)),
_ui(new Ui::ConnectionSettingsDialog)
{
    _ui->setupUi(this);

    auto &settings = Settings::get();

    _ui->serverAddressLineEdit->setPlaceholderText(settings.getServerAddressDefault());
    _ui->serverPortLineEdit->setPlaceholderText(settings.getServerPortDefault());

    connect(this, SIGNAL(accepted()), this, SLOT(onAccepted()));
    connect(this, SIGNAL(rejected()), this, SLOT(onRejected()));

    refreshView();
}

void ConnectionSettingsDialog::refreshView() {
    _ui->serverAddressLineEdit->setText(Settings::get().getServerAddress());
    _ui->serverPortLineEdit->setText(Settings::get().getServerPort());
}

ConnectionSettingsDialog::~ConnectionSettingsDialog()
{
    delete _ui;
}

/*void ConnectionSettingsDialog::onConnectionSettingsRequest() {
    refreshView();
    show();
}*/

/*void ConnectionSettingsDialog::showEvent( QShowEvent* event ) {
    QDialog::showEvent( event );
    refreshView();
}*/

void ConnectionSettingsDialog::onAccepted() {
    auto &settings = Settings::get();
    QString newAddress = _ui->serverAddressLineEdit->text();
    QString newPort = _ui->serverPortLineEdit->text();

    if(newAddress!=settings.getServerAddress() || newPort!=settings.getServerPort()){
        settings.setServerAddress(newAddress);
        settings.setServerPort(newPort);

        _server->connectSocket();
    }else{

    }
    //emit settingsAcceptedSignal();
    //close();
}

void ConnectionSettingsDialog::onRejected() {
    //emit settingsRejectedSignal();
    //close();
}
