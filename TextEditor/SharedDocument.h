//
// 21/05/2019.
//

#ifndef LAB3_SHAREDEDITOR_H
#define LAB3_SHAREDEDITOR_H

class NetworkManager;

#include <memory>
#include "NetworkManager.h"
#include "VersionController.h"
#include "Document.h"
#include <QChar>
#include <list>
#include <vector>
#include <QString>
#include "MetaDocument.h"
#include "CustomCursor.h"
#include "GraphicSymbol.h"
#include "Action.h"

Q_DECLARE_METATYPE(std::vector<Action>)

class SharedDocument : public QObject{
    Q_OBJECT
public:
    void localInsert(unsigned int index, const QChar &value);
    void localDelete(unsigned int index);
    //void process(const Action &m);
    SharedDocument(std::shared_ptr<NetworkManager> networkServer, QString docId, QString docName, std::unique_ptr<MetaDocument> metaDocument, QObject *parent = nullptr);
    SharedDocument() = delete;
    ~SharedDocument();
    void dispatchMessages();
    void stopDispatcher();
    [[nodiscard]] bool hasMetaDocument() const;
    [[nodiscard]] CustomCursor getMetaCursor() const;
    [[nodiscard]] QString getDocumentName() const;
    [[nodiscard]] QString getDocumentId() const;
    [[nodiscard]] SymbolVector getDocumentSymbols() const;
    [[nodiscard]] bool isConnected() const;
signals:
    void remoteInsertSignal(GraphicSymbol gS);
    void remoteDeleteSignal(GraphicSymbol gS);
    void handleCursorReceivedSignal(QJsonObject json);
private slots:
    void onRemoteActionsReceivedSlot(std::vector<Action> actions);
    void onCursorReceivedSlot(QJsonObject json);
private:
    std::shared_ptr<NetworkManager> _server;
    Document *_document;
    std::unique_ptr<MetaDocument> _metaDocument;
};

#endif //LAB3_SHAREDEDITOR_H
