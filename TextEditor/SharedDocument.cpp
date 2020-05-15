//
// 21/05/2019.
//

#include <iostream>
#include "SharedDocument.h"
#include "NetworkClasses/CursorPosition.h"
#include <algorithm>

#define PDS_LAB_3_DEBUG_ENABLED false

void SharedDocument::localInsert(unsigned int index, const QChar &value) {
#if PDS_LAB_3_DEBUG_ENABLED
    std::cout << "local insert" << std::endl;
#endif
    Action message = _document->localInsert(index, value);
    _server->sendMessageToQueue(message);
}

void SharedDocument::localDelete(unsigned int index) {
#if PDS_LAB_3_DEBUG_ENABLED
    std::cout << "local delete" << std::endl;
#endif
    Action message = _document->localDelete(index);
    _server->sendMessageToQueue(message);
}

void SharedDocument::stopDispatcher() {
    _server->stopMessagesDispatcher();
}

void SharedDocument::dispatchMessages() {
    _server->dispatchMessages();
}

SharedDocument::SharedDocument(std::shared_ptr<NetworkManager> networkServer, QString docId, QString docName, std::unique_ptr<MetaDocument> metaDocument, QObject *parent):
QObject(parent),
_server(std::move(networkServer)),
_metaDocument(std::move(metaDocument)){
#if DOCUMENT_LOADS_DOCUMENT
    try{
        _document = FileManager::getInstance().loadDocument(docId, _server->getSiteId(), this);
    }
    catch(std::invalid_argument &e) {
        _document = new Document(docId, "new document", _server->getSiteId(), this);
    }
#else
    _document = new Document(std::move(docId), std::move(docName), _server->getSiteId(), this);
#endif
    connect(_server.get(), SIGNAL(actionsReceivedSignal(std::vector<Action>)), this, SLOT(onRemoteActionsReceivedSlot(std::vector<Action>)));
    connect(_server.get(), SIGNAL(cursorReceivedSignal(QJsonObject)), this, SLOT(onCursorReceivedSlot(QJsonObject)));
}

bool SharedDocument::isConnected() const {
    return _server->isConnected();
}

CustomCursor SharedDocument::getMetaCursor() const {
    return _metaDocument->getCursor();
}

QString SharedDocument::getDocumentName() const {
    return _document->getName();
}

QString SharedDocument::getDocumentId() const {
    return _document->getId();
}

SymbolVector SharedDocument::getDocumentSymbols() const{
    return _document->getSymbols();
}

bool SharedDocument::hasMetaDocument() const {
    return _metaDocument!= nullptr;
}

void SharedDocument::onRemoteActionsReceivedSlot(std::vector<Action> actions) {
    for(const Action &action : actions) {
        _document->applyAction(action, true, true);
    }
}

void SharedDocument::onCursorReceivedSlot(QJsonObject json){
    CursorPosition cp{json};
    if(cp.getId()==getDocumentId()){
        //std::cout << "received cursor from siteId " << cp.getSiteId() << std::endl;
        emit handleCursorReceivedSignal(json);
    }
}

SharedDocument::~SharedDocument() {
    _document->deleteLater();
}
