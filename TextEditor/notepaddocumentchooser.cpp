//
// 12/08/2019.
//

#include <QtWidgets/QMessageBox>
#include "notepaddocumentchooser.h"
#include "ui_notepaddocumentchooser.h"
#include "connectionsettingsdialog.h"
#include "documentinfodialog.h"
#include "NetworkClasses/DocumentsResponse.h"
#include <memory>
#include <iostream>
#include <QtWidgets/QInputDialog>
#include "NetworkClasses/DocumentInfo.h"
#include "NetworkClasses/NewDocumentResponse.h"
#include "NetworkClasses/CollaborationResponse.h"


NotepadDocumentChooser::NotepadDocumentChooser(std::shared_ptr<NetworkManager> server, QWidget *parent):
QMainWindow(parent),
_ui(new Ui::NotepadDocumentChooser),
_server(std::move(server)){
    _ui->setupUi(this);
    //this->setCentralWidget();

    _model = new QStringListModel(this);
    refreshModel();

    connect(_server.get(), SIGNAL(documentsResponseSignal(QJsonObject)), this, SLOT(onDocumentsResponseSlot(QJsonObject)));
    connect(_server.get(), SIGNAL(newDocumentResponseSignal(QJsonObject)), this, SLOT(onNewDocumentResponseSlot(QJsonObject)));
    connect(_server.get(), SIGNAL(deleteDocumentResponseSignal(QJsonObject)), this, SLOT(onDeleteDocumentResponseSlot(QJsonObject)));
    connect(_server.get(), SIGNAL(collaborationResponseSignal(QJsonObject)), this, SLOT(onCollaborationResponseSlot(QJsonObject)));

    connect(_ui->actionAbout, &QAction::triggered, this, &NotepadDocumentChooser::onAboutClicked);
    connect(_ui->actionLogout, &QAction::triggered, this, &NotepadDocumentChooser::onLogoutClicked);
    connect(_ui->actionExit, &QAction::triggered, this, &NotepadDocumentChooser::onExitClicked);
    connect(_ui->actionSettings, &QAction::triggered, this, &NotepadDocumentChooser::onSettingsClicked);

    connectButtons(true);
}

void NotepadDocumentChooser::refreshModel() {
    // Populate our model
    _model->setStringList(_modelList);

    // Glue model and view together
    _ui->listView->setModel(_model);
    _ui->listView->update();
}

void NotepadDocumentChooser::onAboutClicked()
{
    QMessageBox::about(this, tr("About Text Editor"),
                       tr("<b>Text Editor</b> è il progetto del corso "
                          "Programmazione di sistema"));

}

NotepadDocumentChooser::~NotepadDocumentChooser() {
    delete _ui;
}

void NotepadDocumentChooser::onLogoutClicked()
{
    logout();
}

void NotepadDocumentChooser::onExitClicked() {
    QCoreApplication::quit();
}

void NotepadDocumentChooser::logout() {
    auto &settings = Settings::get();
    settings.setUsername("");
    settings.setPassword("");
    settings.setRememberMe(false);

    emit loginRequestSignal();
    close();
}

void NotepadDocumentChooser::onSettingsClicked() {
    auto *connectionSettingsDialog = new ConnectionSettingsDialog(_server, this);
    int retval = connectionSettingsDialog->exec();
    connectionSettingsDialog->deleteLater();
    // TODO: se retval è 1 allora ho salvato le impostazioni quindi devo fare qualcosa adesso...
}

bool NotepadDocumentChooser::connectButtons(bool connect) {
    bool connected = _buttons_connected;
    if(connect xor connected) {
        if(connect) {
            this->connect(_ui->newPushButton, SIGNAL(clicked()), this, SLOT(onNewDocClicked()));
            this->connect(_ui->openPushButton, SIGNAL(clicked()), this, SLOT(onOpenDocClicked()));
            this->connect(_ui->modifyPushButton, SIGNAL(clicked()), this, SLOT(onModifyDocClicked()));
            this->connect(_ui->deletePushButton, SIGNAL(clicked()), this, SLOT(onDeleteDocClicked()));
            this->connect(_ui->importURL, SIGNAL(clicked()), this, SLOT(OnParseURL()));
        }else{
            this->disconnect(_ui->newPushButton, SIGNAL(clicked()), this, SLOT(onNewDocClicked()));
            this->disconnect(_ui->openPushButton, SIGNAL(clicked()), this, SLOT(onOpenDocClicked()));
            this->disconnect(_ui->modifyPushButton, SIGNAL(clicked()), this, SLOT(onModifyDocClicked()));
            this->disconnect(_ui->deletePushButton, SIGNAL(clicked()), this, SLOT(onDeleteDocClicked()));
            this->disconnect(_ui->importURL, SIGNAL(clicked()), this, SLOT(OnParseURL()));
        }
        _buttons_connected = connect;
    }
    return connected;
}

void NotepadDocumentChooser::onNewDocClicked() {
    bool ok;
    QString text = QInputDialog::getText(this, tr("New"),
                                         tr("Document name:"), QLineEdit::Normal, "", &ok);
    if(ok){
        if(!text.isEmpty()){
            _server->getNewDocument(text);
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

void NotepadDocumentChooser::onOpenDocClicked() {
    int row = _ui->listView->currentIndex().row();
    QModelIndex index = _model->index(row);
    int indexRow = index.row();
    if(indexRow < 0 || indexRow >= _docIdList.size()) {
        QMessageBox *dialog = new QMessageBox{this};
        dialog->setWindowTitle("Warning");
        dialog->setIcon(QMessageBox::Icon::Critical);
        dialog->setText("Choose a valid document.");
        dialog->exec();
        dialog->deleteLater();
        return;
    }
    chooseDocument(indexRow);
}

void NotepadDocumentChooser::onModifyDocClicked() {
    int row = _ui->listView->currentIndex().row();
    QModelIndex index = _model->index(row);
    int indexRow = index.row();
    if(indexRow < 0 || indexRow >= _docIdList.size()) {
        QMessageBox *dialog = new QMessageBox{this};
        dialog->setWindowTitle("Warning");
        dialog->setIcon(QMessageBox::Icon::Critical);
        dialog->setText("Choose a valid document.");
        dialog->exec();
        dialog->deleteLater();
        return;
    }
    DocumentInfo docInfo = DocumentInfo{_server->getSiteId(), _docIdList[indexRow], _modelList[indexRow], std::vector<QString>{}, std::vector<unsigned int>{}};
    auto *documentInfoDialog = new DocumentInfoDialog(_server, docInfo, false, this);
    int retval = documentInfoDialog->exec();
    documentInfoDialog->deleteLater();
    if(retval==1){
        _server->getDocuments();
    }
}

void NotepadDocumentChooser::onDeleteDocClicked() {
    int row = _ui->listView->currentIndex().row();
    QModelIndex index = _model->index(row);
    int indexRow = index.row();
    if(indexRow < 0 || indexRow >= _docIdList.size()) {
        QMessageBox *dialog = new QMessageBox{this};
        dialog->setWindowTitle("Warning");
        dialog->setIcon(QMessageBox::Icon::Critical);
        dialog->setText("Choose a valid document.");
        dialog->exec();
        dialog->deleteLater();
        return;
    }
    /*if(_docIdList[indexRow]=="0") {
        QMessageBox *dialog = new QMessageBox{this};
        dialog->setWindowTitle("Warning");
        dialog->setIcon(QMessageBox::Icon::Critical);
        dialog->setText("You cannot delete the TEST document.");
        dialog->exec();
        dialog->deleteLater();
        return;
    }*/
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Delete", "Are you sure?",
                                  QMessageBox::Yes|QMessageBox::No);
    if(reply == QMessageBox::Yes){
        _server->deleteDocument(_docIdList[indexRow]);
    }
}

void NotepadDocumentChooser::onChoiceRequest() {
    refreshModel();
    show();
}

void NotepadDocumentChooser::chooseDocument(const QString &docId, const QString &docName) {
    std::unique_ptr<MetaDocument> metaDoc = std::make_unique<MetaDocument>(CustomCursor());
    std::shared_ptr<SharedDocument> sharedDocument = std::make_shared<SharedDocument>(_server, docId, docName, std::move(metaDoc));
    emit documentSelectedSignal(std::move(sharedDocument));
    close();
}

void NotepadDocumentChooser::chooseDocument(int index) {
    std::unique_ptr<MetaDocument> metaDoc;
#if DOCUMENT_LOADS_METADATA
    try{
        metaDoc = std::make_unique<MetaDocument>(FileManager::getInstance().loadMetaData("1", _server->getSiteId()));
    }
    catch(std::invalid_argument &e) {
        metaDoc = std::make_unique<MetaDocument>(CustomCursor());
    }
#else
    metaDoc = std::make_unique<MetaDocument>(CustomCursor());
#endif
    std::shared_ptr<SharedDocument> sharedDocument = std::make_shared<SharedDocument>(_server, _docIdList[index], _modelList[index], std::move(metaDoc));

    emit documentSelectedSignal(std::move(sharedDocument));
    close();
}

void NotepadDocumentChooser::onDocumentsResponseSlot(QJsonObject response){
    DocumentsResponse res{std::move(response)};
    updateDocumentsLists(res);
    refreshModel();
}

void NotepadDocumentChooser::updateDocumentsLists(const DocumentsResponse &response) {
    _modelList.clear();
    _docIdList.clear();
    for(const DocumentInfo &iter : response.getDocuments()){
        _docIdList.push_back(iter.getId());
        _modelList.push_back(iter.getName());
    }
}

void NotepadDocumentChooser::showEvent(QShowEvent *event) {
    _server->getDocuments();
    QWidget::showEvent(event);
}

void NotepadDocumentChooser::onNewDocumentResponseSlot(QJsonObject response) {
    NewDocumentResponse res{std::move(response)};
    if(res.isError()){
        return;
    }
    if(!res.isRejected()){
        chooseDocument(res.getId(), res.getName());
    }else{
        QMessageBox *dialog = new QMessageBox{this};
        dialog->setWindowTitle("Warning");
        dialog->setIcon(QMessageBox::Icon::Critical);
        dialog->setText("New document creation rejected.");
        dialog->exec();
        dialog->deleteLater();
    }
}

void NotepadDocumentChooser::onDeleteDocumentResponseSlot(QJsonObject response) {
    GenericMessage res{std::move(response)};
    if(res.isError()){
        return;
    }
    if(!res.isRejected()){
        _server->getDocuments();
    }else{
        QMessageBox *dialog = new QMessageBox{this};
        dialog->setWindowTitle("Warning");
        dialog->setIcon(QMessageBox::Icon::Critical);
        dialog->setText("You cannot delete the selected document.");
        dialog->exec();
        dialog->deleteLater();
    }
}

void NotepadDocumentChooser::OnParseURL()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Insert URL"),
                                         tr("Insert a valid link:"), QLineEdit::Normal, "", &ok);
    if(ok){
        if(!text.isEmpty()){
            QStringList stringList = text.split("_");
            if(stringList.size()>1) {
                qulonglong id = stringList[1].toULongLong();
                std::hash<std::string> hasher;
                auto hashed = hasher("URL_DI_INVITO");
                int uid = id - hashed;
                _server->addMeAsCollaborator(QString::number(uid));
            }else{
                QMessageBox *dialog = new QMessageBox{this};
                dialog->setWindowTitle("Error!");
                dialog->setIcon(QMessageBox::Icon::Warning);
                dialog->setText( "Insert a valid URL!");
                dialog->exec();
                dialog->deleteLater();
            }
        }else{
            QMessageBox *dialog = new QMessageBox{this};
            dialog->setWindowTitle("Error!");
            dialog->setIcon(QMessageBox::Icon::Warning);
            dialog->setText( "Insert a URL!");
            dialog->exec();
            dialog->deleteLater();
        }
    }
}

void NotepadDocumentChooser::onCollaborationResponseSlot(QJsonObject response)
{
    CollaborationResponse res{std::move(response)};
    if(!res.getByURL()){
        return;
    }
    if(res.isError()){
        return;
    }
    if(!res.isRejected()){
        _server->getDocuments();
    }else{
        QMessageBox *dialog = new QMessageBox{this};
        dialog->setWindowTitle("Warning");
        dialog->setIcon(QMessageBox::Icon::Critical);
        dialog->setText("Server error! The document is not inserted.");
        dialog->exec();
        dialog->deleteLater();
    }
}