//
// 14/08/2019.
//

#ifndef TEXTEDITOR_CONNECTIONSETTINGSDIALOG_H
#define TEXTEDITOR_CONNECTIONSETTINGSDIALOG_H

#include <QDialog>
#include <memory>
#include "NetworkManager.h"

namespace Ui {
    class ConnectionSettingsDialog;
}

class ConnectionSettingsDialog : public QDialog
{
Q_OBJECT

public:
    explicit ConnectionSettingsDialog(std::shared_ptr<NetworkManager> server, QWidget *parent = nullptr);
    ~ConnectionSettingsDialog() override;

//signals:
    //void settingsAcceptedSignal();
    //void settingsRejectedSignal();

private slots:
    //void onConnectionSettingsRequest();
    void onAccepted();
    void onRejected();

protected:
    //void showEvent(QShowEvent *event) override;

private:
    Ui::ConnectionSettingsDialog *_ui;
    std::shared_ptr<NetworkManager> _server;

    void refreshView();
};

#endif //TEXTEDITOR_CONNECTIONSETTINGSDIALOG_H
