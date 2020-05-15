//
// 13/07/2019.
//

#ifndef TEXTEDITOR_INPUTMANAGER_H
#define TEXTEDITOR_INPUTMANAGER_H

#include <queue>
#include <tuple>
#include <QString>
#include <QChar>
#include <string>
#include "SharedDocument.h"
#include <QStatusBar>
#include "CustomCursor.h"

#define PDS_COMPUTE_SLEEP_TIME 500

class InputManager {
public:
    enum EditorActionType{
        insertSequence, deleteSequence
        // changeSequence può essere vista come una sequenza "deleteSequence -> insertSequence".
    };
    class EditorAction{
    public:
        enum EditorActionType type;
        QString sequence = QString();
        unsigned int pos_from;
        unsigned int pos_to;
    };

private:
    std::shared_ptr<SharedDocument> _sharedDocument;
public:
    InputManager();
    void attachSharedDocument(std::shared_ptr<SharedDocument> sharedDocument);
    void detachSharedDocument();
    ~InputManager();
    [[nodiscard]] bool hasMetaDocument() const;
    [[nodiscard]] CustomCursor getMetaCursor() const;
    void addSequenceFromEditor(const EditorActionType type, const QString &sequence, const unsigned int pos_from, const unsigned int pos_to);
    [[nodiscard]] QString getDocumentName() const;
    [[nodiscard]] QString getDocumentId() const;
    [[nodiscard]] SymbolVector getDocumentSymbols() const;
    [[nodiscard]] bool hasDocument() const;
    [[nodiscard]] bool isConnected() const;
private:
    std::condition_variable _dispatcher_cv;
    std::thread _dispatcherThread;
    std::atomic<bool> _stopDispatcher = false;


    void sendActionsAndDispatch(const Action &action);
    void startDispatcherThread(int sleepTime);
    void stopDispatcherThread();

    bool unfoldAction(const EditorAction &action);
};


#endif //TEXTEDITOR_INPUTMANAGER_H
