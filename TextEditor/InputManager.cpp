//
// 13/07/2019.
//

class Notepad;

#include <iostream>
#include <QtCore/QJsonArray>
#include <QtCore/QStringListIterator>
#include <memory>
#include "InputManager.h"
#include "notepad.h"

#define ACTIONS_MANAGER_DEBUG false

InputManager::InputManager() {
}

void InputManager::attachSharedDocument(std::shared_ptr<SharedDocument> sharedDocument) {
    detachSharedDocument();
    _sharedDocument = std::move(sharedDocument);

    if(_sharedDocument!= nullptr) {
        startDispatcherThread(PDS_COMPUTE_SLEEP_TIME);
    }
}

void InputManager::detachSharedDocument() {
    stopDispatcherThread();
    _sharedDocument.reset();
}

void InputManager::startDispatcherThread(int sleepTime) {
    _stopDispatcher = false;
}

void InputManager::stopDispatcherThread() {
    _stopDispatcher = true;
    if(_sharedDocument!= nullptr) {
        _sharedDocument->stopDispatcher();
    }
    Notepad::updateStatusBar(Notepad::status::ok);
}

InputManager::~InputManager() {
    stopDispatcherThread();
}

void InputManager::addSequenceFromEditor(const EditorActionType type, const QString &sequence, const unsigned int pos_from, const unsigned int pos_to) {
    EditorAction action;
    action.type=type;
    action.sequence = QString(sequence);
    action.pos_from=pos_from;
    action.pos_to=pos_to;
#if ACTIONS_MANAGER_DEBUG
    std::cout << "action: " << action.type << " from " << action.pos_from << " to " << action.pos_to << "." << std::endl;
#endif
    unfoldAction(action);
    Notepad::updateStatusBar(Notepad::status::syncing);
    _sharedDocument->dispatchMessages();
    Notepad::updateStatusBar(Notepad::status::ok);
}

bool InputManager::unfoldAction(const EditorAction &action) {
    switch(action.type) {
        case insertSequence: {
            unsigned int size = action.sequence.size();
            for (int charCounter = size-1; charCounter >=0; charCounter--) {
                Notepad::updateStatusBar(Notepad::status::processing, (int)((size-charCounter)*100/size));
                if(_stopDispatcher){
                    return false;
                }
                QChar currentChar = action.sequence.at(charCounter);
                _sharedDocument->localInsert(action.pos_from, currentChar);
            }
            break;
        }
        case deleteSequence: {
            unsigned int size = action.pos_to - action.pos_from;
            for(unsigned int charCounter = 1;charCounter<=size; charCounter++){
                Notepad::updateStatusBar(Notepad::status::processing, (int)(charCounter*100/size));
                if(_stopDispatcher){
                    return false;
                }
                _sharedDocument->localDelete(action.pos_from);
            }
            break;
        }
        default: {
            break;
        }
    }
    return true;
}

bool InputManager::hasDocument() const {
    return _sharedDocument!= nullptr;
}

bool InputManager::isConnected() const {
    return (_sharedDocument!= nullptr && _sharedDocument->isConnected());
}

QString InputManager::getDocumentName() const {
    return (_sharedDocument!= nullptr?_sharedDocument->getDocumentName():"");
}

QString InputManager::getDocumentId() const {
    return (_sharedDocument!= nullptr?_sharedDocument->getDocumentId():"");
}

SymbolVector InputManager::getDocumentSymbols() const {
    return (_sharedDocument!= nullptr?_sharedDocument->getDocumentSymbols():SymbolVector());
}

CustomCursor InputManager::getMetaCursor() const {
    return _sharedDocument->getMetaCursor();
}

bool InputManager::hasMetaDocument() const {
    return (_sharedDocument!= nullptr && _sharedDocument->hasMetaDocument());
}
