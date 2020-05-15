//
// 22/07/2019.
//

#include <QtCore/QJsonDocument>
#include <QByteArray>
#include "Document.h"
#include <string>
#include <iostream>
#include "crdt_exception.h"
#include "FileManager.h"
#include "SharedDocument.h"

#define DOCUMENT_DEBUG false

Document::Document(QString id, QString name, const unsigned int siteId, QObject *parent):
QObject(parent),
        _my_site_id(siteId),
        _id(std::move(id)),
        _name(std::move(name)),
        _lSeq(),
        _actionVector(ActionVector()),
        _symbols(SymbolVector()),
        _deleteVector(DeleteVector()),
        _versionController(siteId){
#if DOCUMENT_DEBUG
    printJson();
#endif
}

Document::Document(const QJsonObject &json, const unsigned int siteId, QObject *parent):
QObject(parent),
        _my_site_id(siteId),
        _id(json["id"].toString()),
        _name(json["name"].toString()),
        _lSeq(),
        _actionVector(ActionVector()),
        _symbols(SymbolVector()),
        _deleteVector(DeleteVector()),
#if DOCUMENT_LOADS_VERSIONS
_versionController(json["version_controller"].toObject(), siteId)
#else
        _versionController(siteId)
#endif
{
#if DOCUMENT_LOADS_ACTIONS
    QJsonArray messages = json["actions"].toArray();
    foreach(const QJsonValue &value, messages){
        _actionVector.emplace_back(value.toObject());
    }
#endif
#if DOCUMENT_LOADS_DELETIONS
    QJsonArray deletions = json["deletions"].toArray();
            foreach(const QJsonValue &value, deletions){
            _deleteVector.emplace_back(value.toObject());
        }
#endif
#if DOCUMENT_APPLIES_ACTIONS
    for(const Action &message : _actionVector){
        applyAction(message, false, false);
    }
#endif
#if DOCUMENT_LOADS_SYMBOLS
    QJsonArray symbols = json["symbols"].toArray();
            foreach(const QJsonValue &value, symbols){
            _symbols.emplace_back(value.toObject());
        }
#endif
#if DOCUMENT_DEBUG
    printJson();
#endif
}

void Document::applyAction(const Action &action, const bool saveAction, const bool draw) {
    if(_versionController.hasBeenApplied(action.getVersion())){
        return;
    }
#if DOCUMENT_SAVES_IN_MEM_ACTIONS
    if(saveAction){
        _actionVector.push_back(action);
    }
#endif
    _versionController.update(action.getVersion());
    switch(action.getType()){
        case Action::MessageType::insert_type:
            insertSymbol(action.getSymbol(), draw);
            break;
        case Action::MessageType::delete_type:
            _deleteVector.push_back(action);
            break;
        default: // nop
            break;
    }
    checkDeleteVector(draw);
}

void Document::checkDeleteVector(bool draw) {
    auto iter = _deleteVector.begin();
    while (iter != _deleteVector.end())
    {
        if(hasInsertionBeenApplied(*iter)){
            deleteSymbol(iter->getSymbol(), draw);
            iter = _deleteVector.erase(iter);
        }else{
            iter++;
        }
    }
}

bool Document::hasInsertionBeenApplied(const Action &message) {
    Version symbolVersion = message.getSymbol().getVersion();
    return _versionController.hasBeenApplied(symbolVersion);
}

void Document::printJson() {
    QJsonDocument doc(toQJsonObject());
    QByteArray bytes = doc.toJson();
    std::cout << bytes.toStdString() << std::endl;
}

Document::~Document() {
#if DOCUMENT_DEBUG
    printJson();
#endif
#if DOCUMENT_STORES_DOCUMENT
    FileManager::getInstance().storeDocument(*this);
#endif
    _actionVector.clear();
    _deleteVector.clear();
    _symbols.clear();
}

SymbolVector::iterator Document::findPositionForSymbol(Symbol &symbol) {
    return std::lower_bound(_symbols.begin(), _symbols.end(), symbol); // or lower_bound?
}

SymbolVector::iterator Document::findSymbolPosition(Symbol &symbol) {
    return std::lower_bound(_symbols.begin(), _symbols.end(), symbol);
}

Action Document::localInsert(unsigned int index, const QChar &value) {
    auto iterBegin = _symbols.begin();
    std::vector<unsigned int> pos1, pos2;
    if(index!=0 && validIndex(index-1)){
        auto iterPos = std::next(iterBegin, index-1);
        pos1 = iterPos->getPosition();
    }else{
        pos1 = std::vector<unsigned int>();
    }
    //controllo che esista un carattere alla destra
    if(validIndex(index)){
        auto iterPos = std::next(iterBegin, index);
        pos2 = iterPos->getPosition();
    }else{
        pos2 = std::vector<unsigned int>();
    }
    //ad ogni nuova azione, incremento ID univoco azioni di questo documento di questo client
    unsigned int newCounter = nextLocalCounter();
    Symbol symbol = Symbol(value, _my_site_id, newCounter, _lSeq.alloc(pos1, pos2, _my_site_id));
    Action message = Action(_id, _my_site_id, newCounter, Action::MessageType::insert_type, symbol);
#if DOCUMENT_SAVES_IN_MEM_LOCAL_ACTIONS
    _actionVector.push_back(message);
#endif
    insertSymbol(symbol, false);
    return message;
}

Action Document::localDelete(unsigned int index){
    if(validIndex(index)) {
        auto iter = _symbols.begin();
        std::advance(iter, index);
        Symbol symbol = Symbol(*iter);
        Action message = Action(_id, _my_site_id, nextLocalCounter(), Action::MessageType::delete_type, symbol);
#if DOCUMENT_SAVES_IN_MEM_LOCAL_ACTIONS
        _actionVector.push_back(message);
#endif
        //_symbols.erase(iter);
        deleteSymbol(symbol, false);
        return message;
    }
    throw crdt_exception("invalid index in local delete");
}

void Document::insertSymbol(Symbol symbol, bool draw) {
    auto pos = findPositionForSymbol(symbol);
    unsigned int dist = std::distance(_symbols.begin(), pos);
    _symbols.insert(pos, symbol);
    if(draw){
        emit qobject_cast<SharedDocument *>(parent())->remoteInsertSignal(GraphicSymbol{symbol, dist});
    }
}

void Document::deleteSymbol(Symbol symbol, bool draw) {
    auto pos = findSymbolPosition(symbol);
    unsigned int dist = std::distance(_symbols.begin(), pos);
    _symbols.erase(pos);
    if(draw){
        emit qobject_cast<SharedDocument *>(parent())->remoteDeleteSignal(GraphicSymbol{symbol, dist});
    }
}

bool Document::validIndex(unsigned int index) {
    return _symbols.size()>index;
}

unsigned int Document::nextLocalCounter(){
    unsigned int value =  _versionController.increment();
    return value;
}

QString Document::getId() const{
    return _id;
}

QString Document::getName() const{
    return _name;
}

SymbolVector Document::getSymbols() {
    return _symbols;
}

QJsonObject Document::toQJsonObject() const {
    QJsonObject json;
    json.insert("id", QString(_id));
    json.insert("name", QString(_name));
#if DOCUMENT_STORES_ACTIONS
    json.insert("actions", serializeActions());
#endif
#if DOCUMENT_STORES_DELETIONS
    json.insert("deletions", serializeDeletions());
#endif
#if DOCUMENT_STORES_SYMBOLS
    json.insert("symbols", serializeSymbols());
#endif
#if DOCUMENT_STORES_VERSIONS
    json.insert("version_controller", _versionController.toQJsonObject());
#endif
    return json;
}

QJsonArray Document::serializeSymbols() const {
    QJsonArray symbols;
    int i=0;
    for(const Symbol &symbol : _symbols){
        symbols.insert(i++, symbol.toQJsonObject());
    }
    return symbols;
}

QJsonArray Document::serializeActions() const {
    QJsonArray messages;
    int i=0;
    for(const Action &message : _actionVector){
        messages.insert(i++, message.toQJsonObject());
    }
    return messages;
}

QJsonArray Document::serializeDeletions() const {
    QJsonArray deletions;
    int i=0;
    for(const Action &deletion : _deleteVector){
        deletions.insert(i++, deletion.toQJsonObject());
    }
    return deletions;
}

QByteArray Document::toBytes() const {
    QJsonObject json = toQJsonObject();
    QJsonDocument doc(json);
    QByteArray bytes = doc.toJson();
    return bytes;
}