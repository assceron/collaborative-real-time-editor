#ifndef NOTEPAD_H
#define NOTEPAD_H

#include <QMainWindow>
#include <mutex>
#include <atomic>
#include <thread>
#include <future>
#include <QtGui/QTextCursor>
#include "InputManager.h"
#include "CustomCursor.h"
#include <QStatusBar>
#include <QTextCursor>
#include <QString>
#include "MetaDocument.h"
#include "SharedDocument.h"
#include "GraphicSymbol.h"
#include "NetworkClasses/CursorPosition.h"

Q_DECLARE_METATYPE(std::shared_ptr<SharedDocument>)

#define ATOMIC_CONNECTION false

QT_BEGIN_NAMESPACE
namespace Ui {
    class Notepad;
}
QT_END_NAMESPACE

class Notepad : public QMainWindow
{
Q_OBJECT

public:
    enum status{
        processing, syncing, ok
    };
    enum CursorType{
        position_changed, selection_changed, text_changed, all_changed
    };

public:
    Notepad(std::shared_ptr<NetworkManager> server, std::unique_ptr<InputManager> actionsManager, QWidget *parent = nullptr);
    ~Notepad() override;
    static void updateStatusBar(Notepad::status status, const int percent = -1);
    [[nodiscard]] QString getDocName() const;
    void attachSharedDocument(std::shared_ptr<SharedDocument> sharedDocument);
    void detachSharedDocument();
    void logout();
    bool eventFilter(QObject *watched, QEvent *event) override ;
private:
    static QStatusBar *_statusBar;
    void initText();
    bool connectTextEditor(const bool connect);
    void moveCursor(const CustomCursor &cursor);
    CustomCursor getCustomCursor();
    void resetText();
    void resetCursors();
    void updateCursors(const CustomCursor &cursor);
    void updateCursors();
    static QString symbolsToQstring(const SymbolVector &symbols);
    void insertRemoteSymbol(const unsigned int position, const Symbol symbol);
    void deleteRemoteSymbol(const unsigned int position);
    // (position 1 must be lower than position2)
    void updateRemoteCursor(CursorPosition cursor);
    void drawRemoteCursors();
    //void save();
signals:
    void loginRequest();
    void documentChoiceRequestSignal();
private slots:
    void onOpenClicked();
    void onSharedDocumentReady(std::shared_ptr<SharedDocument> sharedDocument);
    void onRemoteInsertSlot(GraphicSymbol gSymbol);
    void onRemoteDeleteSlot(GraphicSymbol gSymbol);
    void onDeleteDocumentResponseSlot(QJsonObject json);
    //void newDocument();
    //void saveAs();
    void print();
    void onLogoutClicked();
    void onSettingsClicked();
    void onDocumentInfoClicked();
    void exit();
    void copy();
    void cut();
    void paste();
    //void undo();
    //void redo();
    //void selectFont();
    //void setFontBold(bool bold);
    //void setFontUnderline(bool underline);
    //void setFontItalic(bool italic);
    void about();

    void onTextChanged();
    void onCursorPositionChanged();
    void onSelectionChanged();
    void onCursorReceivedSlot(QJsonObject json);

private:
    Ui::Notepad *_ui;
    std::shared_ptr<NetworkManager> _server;
    CustomCursor _newCursor = CustomCursor();
    CustomCursor _oldCursor = CustomCursor();
    bool _editorConnected = false;
    bool _key_to_handle = false;
    QString _currentText = QString{};
#if ATOMIC_CONNECTION
    std::mutex _connectMutex;
#endif
    //std::atomic<bool> _shuttingDown = false;
    std::unique_ptr<InputManager> _inputManager;
    std::shared_ptr<SharedDocument> _sharedDocument;
    std::map<unsigned int, CustomCursor> _cursorsVector;
    std::map<unsigned int, QColor> _cursorColors;
};

#endif // NOTEPAD_H