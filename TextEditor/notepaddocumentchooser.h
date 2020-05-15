//
// 12/08/2019.
//

#ifndef TEXTEDITOR_NOTEPADDOCUMENTCHOOSER_H
#define TEXTEDITOR_NOTEPADDOCUMENTCHOOSER_H


#include <QtCore/QArgument>
#include <QMainWindow>
#include <QtCore/QStringListModel>
#include "NetworkManager.h"
#include "NetworkClasses/DocumentsResponse.h"
#include "NetworkClasses/NewDocumentResponse.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class NotepadDocumentChooser;
}
QT_END_NAMESPACE

class NotepadDocumentChooser: public QMainWindow{
Q_OBJECT
public:
    explicit NotepadDocumentChooser(std::shared_ptr<NetworkManager> server, QWidget *parent = nullptr);
    ~NotepadDocumentChooser() override;
private:
    Ui::NotepadDocumentChooser *_ui;
    std::shared_ptr<NetworkManager> _server;
    bool _buttons_connected = false;
    QStringListModel *_model;
    QStringList _modelList = QStringList{};
    QStringList _docIdList = QStringList{};
    void updateDocumentsLists(const DocumentsResponse &response);
    bool connectButtons(bool connect);
    void chooseDocument(int index);
    void chooseDocument(const QString &docId, const QString &docName);
    void logout();
    void refreshModel();
signals:
    void documentSelectedSignal(std::shared_ptr<SharedDocument> sharedDocument);
    void loginRequestSignal();
private slots:
    void onSettingsClicked();
    void onChoiceRequest();
    void onNewDocClicked();
    void onOpenDocClicked();
    void onModifyDocClicked();
    void onDeleteDocClicked();
    void OnParseURL();
    void onAboutClicked();
    void onLogoutClicked();
    void onExitClicked();
    void onDocumentsResponseSlot(QJsonObject response);
    void onNewDocumentResponseSlot(QJsonObject response);
    void onDeleteDocumentResponseSlot(QJsonObject response);
    void onCollaborationResponseSlot(QJsonObject response);
protected:
    virtual void showEvent(QShowEvent *event) override ;
};


#endif //TEXTEDITOR_NOTEPADDOCUMENTCHOOSER_H
