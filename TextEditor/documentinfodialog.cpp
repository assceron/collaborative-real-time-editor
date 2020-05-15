//
// 16/08/2019.
//

#include <iostream>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMessageBox>
#include "documentinfodialog.h"
#include "ui_documentinfodialog.h"

// per inserire un nuovo collaborator usare il QInputDialog...

DocumentInfoDialog::DocumentInfoDialog(std::shared_ptr<NetworkManager> server, DocumentInfo &documentInfo, const bool newDoc, QWidget *parent) :
QDialog(parent),
_ui(new Ui::DocumentInfoDialog),
_server(server),
_documentInfo(documentInfo),
_docNameEditable(newDoc){
    _ui->setupUi(this);

    connect(this, SIGNAL(accepted()), this, SLOT(onAccepted()));
    connect(this, SIGNAL(rejected()), this, SLOT(onRejected()));

    connect(_server.get(), SIGNAL(documentInfoResponseSignal(QJsonObject)), this, SLOT(onDocumentInfoResponseSlot(QJsonObject)));
    connect(_server.get(), SIGNAL(collaborationResponseSignal(QJsonObject)), this, SLOT(onCollaborationResponseSlot(QJsonObject)));

    _model = new CustomListModel(this);
    refreshView();

    connectButtons(true);
}

bool DocumentInfoDialog::connectButtons(const bool connect) {
    bool connected = _buttonsConnected;
    if(connect xor connected) {
        if(connect) {
            this->connect(_ui->addCPushButton, SIGNAL(clicked()), this, SLOT(onAddCollaboratorClickedSlot()));
            this->connect(_ui->removeCPushButton, SIGNAL(clicked()), this, SLOT(onRemoveCollaboratorClickedSlot()));
            this->connect(_ui->generateURL, SIGNAL(clicked()), this, SLOT(onGenerateLink()));
        }else{
            this->disconnect(_ui->addCPushButton, SIGNAL(clicked()), this, SLOT(onAddCollaboratorClickedSlot()));
            this->disconnect(_ui->removeCPushButton, SIGNAL(clicked()), this, SLOT(onRemoveCollaboratorClickedSlot()));
            this->disconnect(_ui->generateURL, SIGNAL(clicked()), this, SLOT(onGenerateLink()));

        }
        _buttonsConnected = connect;
    }
    return connected;
}

DocumentInfoDialog::~DocumentInfoDialog()
{
    delete _ui;
}


void DocumentInfoDialog::onAccepted() {
    //_documentInfo.setName(_ui->docNameLineEdit->text());
    //emit infoAcceptedSignal();
    //close();
}

void DocumentInfoDialog::onRejected() {
    //_documentInfo.setName(QString{});
    //emit infoRejectedSignal();
    //close();
}

void DocumentInfoDialog::refreshView() {
    _ui->docNameLineEdit->setText(_documentInfo.getName());
    _ui->docNameLineEdit->setEnabled(_docNameEditable);

    _modelList.clear();
    for(const QString &collaborator : _documentInfo.getCollaborators()){
        _modelList << collaborator;
    }
    _model->setStringList(_modelList);
    _documentInfo.getColors();
    for(int i=0; i< _documentInfo.getColors().size(); i++){
        if(_documentInfo.getColors()[i] == QColor::fromRgb(255,255,255,255)){
            _model->setData(_model->index(i), QBrush(QColor::fromRgb(0,0,0,255)), Qt::ForegroundRole);
        }else {
            _model->setData(_model->index(i), QBrush(_documentInfo.getColors()[i]), Qt::ForegroundRole);
        }
    }
    _ui->listView->setModel(_model);
    _ui->listView->update();
}

void DocumentInfoDialog::onAddCollaboratorClickedSlot() {
    bool ok;
    QString text = QInputDialog::getText(this, tr("Choose a collaborator"),
                                         tr("User name:"), QLineEdit::Normal, "", &ok);
    if(ok){
        if(!text.isEmpty()){
            _server->addCollaborator(_documentInfo.getId(), text);

        }else{
            QMessageBox *dialog = new QMessageBox{this};
            dialog->setWindowTitle("Warning");
            dialog->setIcon(QMessageBox::Icon::Critical);
            dialog->setText("Not a valid name.");
            dialog->exec();
            dialog->deleteLater();
        }
    }
}

void DocumentInfoDialog::onGenerateLink()
{
    QString sLink ="The generated invite URL is: ";
    QMessageBox *dialog = new QMessageBox{this};
    dialog->setWindowTitle("generate URL");
    dialog->setIcon(QMessageBox::Icon::Information);
    QString sURL = "invite_";
    std::hash<std::string> hasher;
    auto hashed = hasher("URL_DI_INVITO");
    hashed += _documentInfo.getId().toInt();
    sURL.append(QString::number(hashed));
    sLink.append(sURL);
    dialog->setText( sLink);
    dialog->exec();
    dialog->deleteLater();
    return;
}

void DocumentInfoDialog::onRemoveCollaboratorClickedSlot() {
    int row = _ui->listView->currentIndex().row();
    QModelIndex index = _model->index(row);
    int indexRow = index.row();
    if(indexRow < 0 || indexRow >= _modelList.size()) {
        QMessageBox *dialog = new QMessageBox{this};
        dialog->setWindowTitle("Warning");
        dialog->setIcon(QMessageBox::Icon::Critical);
        dialog->setText("Choose a valid collaborator.");
        dialog->exec();
        dialog->deleteLater();
        return;
    }
    _server->removeCollaborator(_documentInfo.getId(), _modelList[indexRow]);
}

void DocumentInfoDialog::showEvent(QShowEvent *event) {
    _server->getDocumentInfo(_documentInfo.getId());
    QDialog::showEvent(event);
}

void DocumentInfoDialog::onDocumentInfoResponseSlot(QJsonObject response) {
    DocumentInfo res{std::move(response)};
    if(!res.isError()) {
        _documentInfo = DocumentInfo{res};
        refreshView();
    }
}

void DocumentInfoDialog::onCollaborationResponseSlot(QJsonObject response) {
    GenericMessage res{std::move(response)};
    if(!res.isError()){
        if(!res.isRejected()) {
            _server->getDocumentInfo(_documentInfo.getId());
        }else{
            QMessageBox *dialog = new QMessageBox{this};
            dialog->setWindowTitle("Warning");
            dialog->setIcon(QMessageBox::Icon::Critical);
            dialog->setText("Choose a valid collaborator.");
            dialog->exec();
            dialog->deleteLater();
        }
    }
}