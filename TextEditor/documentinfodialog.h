//
// 16/08/2019.
//

#ifndef TEXTEDITOR_DOCUMENTINFODIALOG_H
#define TEXTEDITOR_DOCUMENTINFODIALOG_H

#include <QDialog>
#include "Document.h"
#include <memory>
#include <QtCore/QStringListModel>
#include "NetworkClasses/DocumentInfo.h"
#include "NetworkManager.h"


namespace Ui {
    class DocumentInfoDialog;
}

class DocumentInfoDialog : public QDialog {
Q_OBJECT
public:
    class CustomListModel : public QStringListModel{
    public:
        CustomListModel(QObject* parent = nullptr)
        :    QStringListModel(parent)
                {}

        QVariant data(const QModelIndex & index, int role) const override
        {
            if (role == Qt::ForegroundRole)
            {
                auto itr = m_rowColors.find(index.row());
                if (itr != m_rowColors.end());
                return itr->second;
            }

            return QStringListModel::data(index, role);
        }

        bool setData(const QModelIndex & index, const QVariant & value, int role) override
        {
            if (role == Qt::ForegroundRole)
            {
                m_rowColors[index.row()] = value.value<QColor>();
                return true;
            }

            return QStringListModel::setData(index, value, role);
        }
    private:
        std::map<int, QColor> m_rowColors;
    };

    DocumentInfoDialog(std::shared_ptr<NetworkManager> server, DocumentInfo &documentInfo, const bool newDoc, QWidget *parent = nullptr);
    ~DocumentInfoDialog() override;
//signals:
    //void settingsAcceptedSignal();
    //void settingsRejectedSignal();
private slots:
    void onAccepted();
    void onRejected();
    void onAddCollaboratorClickedSlot();
    void onRemoveCollaboratorClickedSlot();
    void onDocumentInfoResponseSlot(QJsonObject response);
    void onCollaborationResponseSlot(QJsonObject response);
    void onGenerateLink();
private:
    Ui::DocumentInfoDialog *_ui;
    void refreshView();
    bool _docNameEditable;
    bool connectButtons(const bool connect);
    bool _buttonsConnected = false;
    CustomListModel *_model;
    QStringList _modelList = QStringList{};
    DocumentInfo &_documentInfo;
    std::shared_ptr<NetworkManager> _server;
protected:
    virtual void showEvent(QShowEvent *event) override ;
};

#endif //TEXTEDITOR_DOCUMENTINFODIALOG_H
