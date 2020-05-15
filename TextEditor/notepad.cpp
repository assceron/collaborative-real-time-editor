#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printer)
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#endif // QT_CONFIG(printdialog)
#include <QPrinter>
#endif // QT_CONFIG(printer)
#endif // QT_PRINTSUPPORT_LIB
#include <QFont>
#include <QFontDialog>
#include <iostream>
#include <mutex>
#include <chrono>
#include <QTextDocumentFragment>

#include "notepad.h"
#include "ui_notepad.h"
#include "connectionsettingsdialog.h"
#include "documentinfodialog.h"
#include "NetworkClasses/CursorPosition.h"
#include <QShortcut>
#include <QShortcutEvent>
#include <QKeySequence>
#include "NetworkClasses/DeleteDocumentResponse.h"

#define NOTEPAD_DEBUG false

QStatusBar *Notepad::_statusBar;

Notepad::Notepad(std::shared_ptr<NetworkManager> server, std::unique_ptr<InputManager> actionsManager, QWidget *parent) :
        QMainWindow(parent),
        _ui(new Ui::Notepad),
        _server(std::move(server)),
        _inputManager(std::move(actionsManager)),
        _cursorsVector(std::map<unsigned int, CustomCursor>{}),
        _cursorColors(std::map<unsigned int, QColor>{})
{
    //qApp->applicationDirPath();
    //std::cout << qApp->applicationDirPath().toStdString() << std::endl;

    _ui->setupUi(this);
    this->setCentralWidget(_ui->textEdit);

    _ui->textEdit->installEventFilter(this);

    _ui->textEdit->setUndoRedoEnabled(false);
    //_ui->textEdit->setShortcutEnabled(QShortcut(QKeySequence::Bold, this).id(), false);
    //connect(_ui->actionNew, &QAction::triggered, this, &Notepad::newDocument);
    connect(_ui->actionOpen, &QAction::triggered, this, &Notepad::onOpenClicked);
    //connect(_ui->actionSave, &QAction::triggered, this, &Notepad::storeDocument);
    //connect(_ui->actionSave_as, &QAction::triggered, this, &Notepad::saveAs);
    connect(_ui->actionPrint, &QAction::triggered, this, &Notepad::print);
    connect(_ui->actionLogout, &QAction::triggered, this, &Notepad::onLogoutClicked);
    connect(_ui->actionExit, &QAction::triggered, this, &Notepad::exit);
    connect(_ui->actionCopy, &QAction::triggered, this, &Notepad::copy);
    connect(_ui->actionCut, &QAction::triggered, this, &Notepad::cut);
    connect(_ui->actionPaste, &QAction::triggered, this, &Notepad::paste);
    //connect(_ui->actionUndo, &QAction::triggered, this, &Notepad::undo);
    //connect(_ui->actionRedo, &QAction::triggered, this, &Notepad::redo);
    //connect(_ui->actionFont, &QAction::triggered, this, &Notepad::selectFont);
    //connect(_ui->actionBold, &QAction::triggered, this, &Notepad::setFontBold);
    //connect(_ui->actionUnderline, &QAction::triggered, this, &Notepad::setFontUnderline);
    //connect(_ui->actionItalic, &QAction::triggered, this, &Notepad::setFontItalic);
    connect(_ui->actionAbout, &QAction::triggered, this, &Notepad::about);
    connect(_ui->actionSettings, &QAction::triggered, this, &Notepad::onSettingsClicked);
    connect(_ui->actionModify, &QAction::triggered, this, &Notepad::onDocumentInfoClicked);

    connectTextEditor(true);

// Disable menu actions for unavailable features
#if !QT_CONFIG(printer)
    ui->actionPrint->setEnabled(false);
#endif

#if !QT_CONFIG(clipboard)
    ui->actionCut->setEnabled(false);
    ui->actionCopy->setEnabled(false);
    ui->actionPaste->setEnabled(false);
#endif

    _statusBar = _ui->statusBar;
    updateStatusBar(status::ok);
    initText();
}

void Notepad::attachSharedDocument(std::shared_ptr<SharedDocument> sharedDocument){
    _sharedDocument=std::move(sharedDocument);
    _inputManager->attachSharedDocument(_sharedDocument);
    setWindowTitle(getDocName());
    initText();
    resetCursors();
    connect(_sharedDocument.get(), SIGNAL(remoteInsertSignal(GraphicSymbol)), this, SLOT(onRemoteInsertSlot(GraphicSymbol)));
    connect(_sharedDocument.get(), SIGNAL(remoteDeleteSignal(GraphicSymbol)), this, SLOT(onRemoteDeleteSlot(GraphicSymbol)));
    connect(_sharedDocument.get(), SIGNAL(handleCursorReceivedSignal(QJsonObject)), this, SLOT(onCursorReceivedSlot(QJsonObject)));
    connect(_server.get(), SIGNAL(deleteDocumentResponseSignal(QJsonObject)), this, SLOT(onDeleteDocumentResponseSlot(QJsonObject)));
    _server->subscribe(_sharedDocument->getDocumentId());
}
void Notepad::detachSharedDocument() {
#if DOCUMENT_STORES_METADATA
    FileManager::getInstance().storeMetaData(_inputManager->getDocumentId(), MetaDocument(getCustomCursor()));
#endif
    _server->unsubscribe(_sharedDocument->getDocumentId());
    disconnect(_sharedDocument.get(), SIGNAL(remoteInsertSignal(GraphicSymbol)), this, SLOT(onRemoteInsertSlot(GraphicSymbol)));
    disconnect(_sharedDocument.get(), SIGNAL(remoteDeleteSignal(GraphicSymbol)), this, SLOT(onRemoteDeleteSlot(GraphicSymbol)));
    disconnect(_sharedDocument.get(), SIGNAL(handleCursorReceivedSignal(QJsonObject)), this, SLOT(onCursorReceivedSlot(QJsonObject)));
    disconnect(_server.get(), SIGNAL(deleteDocumentResponseSignal(QJsonObject)), this, SLOT(onDeleteDocumentResponseSlot(QJsonObject)));
    _inputManager->detachSharedDocument();
    setWindowTitle("");
    initText();
    _sharedDocument.reset();
}

void Notepad::initText() {

    bool previousBool = connectTextEditor(false);

    _ui->textEdit->setText(QString());

    int position = 0;
    SymbolVector symbols = _inputManager->getDocumentSymbols();

    QTextCursor myQTC = QTextCursor(_ui->textEdit->textCursor());
    QTextCursor externQTC = QTextCursor(myQTC);
    unsigned int externSize = symbols.size();
    externQTC.clearSelection();
    externQTC.movePosition(QTextCursor::MoveOperation::Start, QTextCursor::MoveMode::MoveAnchor, 1);
    externQTC.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::MoveAnchor, position);

    _ui->textEdit->setTextCursor(externQTC);
    _ui->textEdit->textCursor().insertText(symbolsToQstring(symbols));
    _ui->textEdit->setTextCursor(myQTC);

#if DOCUMENT_LOADS_METADATA
    if(_inputManager->hasMetaDocument()) {
        moveCursor(_inputManager->getMetaCursor());
    }else{
        moveCursor(CustomCursor());
    }
#else
    moveCursor(CustomCursor());
#endif

    resetCursors();
    resetText();

    connectTextEditor(previousBool);
}

QString Notepad::getDocName() const {
    return _inputManager->getDocumentName();
}

Notepad::~Notepad()
{
    //std::cout << "~Notepad()" << std::endl;
    //_shuttingDown = true;
    if (_inputManager->hasDocument()) {
        detachSharedDocument();
    }
    //delete _inputManager;
    delete _ui;
}

void Notepad::onCursorReceivedSlot(QJsonObject json) {
    CursorPosition cp{std::move(json)};
    updateRemoteCursor(cp);
    drawRemoteCursors();
}

void Notepad::updateRemoteCursor(CursorPosition cursor) {
    unsigned int siteId = cursor.getSiteId();
    if(cursor.getRemove()){
        _cursorsVector.erase(siteId);
        _cursorColors.erase(siteId);
    }else {
        _cursorsVector[siteId] = cursor.getCursor();
        _cursorColors[siteId] = cursor.getSiteColor();
    }
}

void Notepad::drawRemoteCursors() {
    bool prev = connectTextEditor(false);
    QTextCursor myCursor = QTextCursor(_ui->textEdit->textCursor());
    QTextCursor tempCursor = QTextCursor(myCursor);
    tempCursor.clearSelection();
    tempCursor.movePosition(QTextCursor::MoveOperation::Start, QTextCursor::MoveMode::MoveAnchor, 1);
    tempCursor.movePosition(QTextCursor::MoveOperation::End, QTextCursor::MoveMode::KeepAnchor, 1);
    _ui->textEdit->setTextCursor(tempCursor);
    _ui->textEdit->setTextBackgroundColor(QColor(255,255,255,255));
    _ui->textEdit->setTextCursor(myCursor);
    _ui->textEdit->setTextBackgroundColor(QColor{255,255,255, 255});
    for(std::pair<unsigned int, CustomCursor> cPair : _cursorsVector){
        tempCursor.movePosition(QTextCursor::MoveOperation::Start, QTextCursor::MoveMode::MoveAnchor, 1);
        CustomCursor cCursor = cPair.second;
        if(cCursor.hasSelection){
            tempCursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::MoveAnchor, (int)cCursor.selectionStart);
            tempCursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::KeepAnchor, (int)cCursor.selectionEnd-(int)cCursor.selectionStart);
        }else{
            tempCursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::MoveAnchor, (int)cCursor.position);
            if(cCursor.position==0){
                tempCursor.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::KeepAnchor, 1);
            }else{
                tempCursor.movePosition(QTextCursor::MoveOperation::Left, QTextCursor::MoveMode::KeepAnchor, 1);
            }
        }
        _ui->textEdit->setTextCursor(tempCursor);
        _ui->textEdit->setTextBackgroundColor(_cursorColors[cPair.first]);
        //_ui->textEdit->setTextCursor(myCursor);
        tempCursor.clearSelection();
        tempCursor.movePosition(QTextCursor::MoveOperation::Start, QTextCursor::MoveMode::MoveAnchor, 1);
    }
    _ui->textEdit->setTextCursor(myCursor);
    _ui->textEdit->setTextBackgroundColor(QColor{255,255,255, 255});
    connectTextEditor(prev);
}

CustomCursor Notepad::getCustomCursor() {
    QTextCursor myQTC = QTextCursor(_ui->textEdit->textCursor());
    return CustomCursor(myQTC);
}

QString Notepad::symbolsToQstring(const SymbolVector &symbols) {
    QString text = QString();
    for(const Symbol &symbol : symbols){
        text.push_back((symbol.getValue()));
    }
    return text;
}

void Notepad::updateStatusBar(Notepad::status status, const int percent) {
    //std::cout << "5" << std::endl;
    if(_statusBar == nullptr){
        return;
    }
    //std::cout << "5.1" << std::endl;
    switch(status){
        case processing:{
            _statusBar->showMessage(QString("processing%1")
                                           .arg((percent<0?"":(QString::fromStdString(" ("+std::to_string(percent)+"%)")))));
            break;
        }
        case syncing:{
            _statusBar->showMessage(QString("syncing%1")
            .arg((percent<0?"":(QString::fromStdString(" ("+std::to_string(percent)+"%)")))));
            break;
        }
        case ok:{
            _statusBar->showMessage(QString(""));
            break;
        }
        default:{
            _statusBar->showMessage(QString(""));
            break;
        }
    }
}

bool Notepad::connectTextEditor(const bool connect){
#if ATOMIC_CONNECTION
    std::lock_guard<std::mutex> lg(_connectMutex);
#endif
    bool connected = _editorConnected;
    if(connect xor connected) {
        if(connect) {
            this->connect(_ui->textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
            this->connect(_ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));
            this->connect(_ui->textEdit, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
        }else{
            this->disconnect(_ui->textEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));
            this->disconnect(_ui->textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(onCursorPositionChanged()));
            this->disconnect(_ui->textEdit, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
        }
        _editorConnected = connect;
    }
    return connected;
}

bool Notepad::eventFilter(QObject *watched, QEvent *event) {
    if (watched == _ui->textEdit && event->type() == QEvent::KeyPress){
#if NOTEPAD_DEBUG && false
        std::cout<<"KeyPress"<<std::endl;
#endif
        auto *keyEvent = dynamic_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Delete)
        {
#if NOTEPAD_DEBUG
            std::cout<<"Key_Delete pressed"<<std::endl;
#endif
            _key_to_handle = true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void Notepad::onSelectionChanged() {
#if NOTEPAD_DEBUG
    std::cout << "onSelectionChanged()" << std::endl;
#endif
    CustomCursor cursor = CustomCursor(_ui->textEdit->textCursor());
    if(_newCursor!=cursor){
        updateCursors(cursor);
    }
    if(_key_to_handle){
        _key_to_handle=false;
    }
    _server->sendCursor(_sharedDocument->getDocumentId(), _newCursor);
}

void Notepad::onTextChanged()//never disconnect this slot!
{
#if NOTEPAD_DEBUG
    std::cout << "onTextChanged()" << std::endl;
#endif
        if(_key_to_handle){
            updateCursors();
        }

        auto oldCur = CustomCursor(_oldCursor);
        auto newCur = CustomCursor(_newCursor);

        int64_t diff = 0;
        int from=0, to=0;

        if(oldCur.hasSelection){
#if NOTEPAD_DEBUG
            std::cout << "onTextChanged() -> delete selection" << std::endl;
#endif
            //std::cout << oldCur.selectionStart << " - " << oldCur.selectionEnd << " (" << oldCur.position << ")" << std::endl;
            diff = static_cast<int64_t>(oldCur.selectionEnd) - static_cast<int64_t>(oldCur.selectionStart);
            from = (int)((diff>0)?oldCur.selectionStart:oldCur.selectionEnd);
            to = (int)((diff>0)?oldCur.selectionEnd:oldCur.selectionStart);
            //std::cout << "deleted: " << _currentText.toStdString().substr(from, to-from) << std::endl;
            _inputManager->addSequenceFromEditor(
                    InputManager::EditorActionType::deleteSequence,
                    _currentText.mid(from, to-from),
                    from,
                    to);
            to = (int)newCur.position;
            diff = static_cast<int64_t>(to) - static_cast<int64_t>(from);
        }
        else {
            //std::cout << "(" << oldCur.position << ")" << std::endl;
            diff = static_cast<int64_t>(newCur.position) - static_cast<int64_t>(oldCur.position);
            from = (int)((diff>0)?oldCur.position:newCur.position);
            to = (int)((diff>0)?newCur.position:oldCur.position);
        }
        //std::cout << "diff: " << diff << std::endl;
        if (diff > 0) { //insert
#if NOTEPAD_DEBUG
            std::cout << "onTextChanged() -> insert" << std::endl;
#endif
            QString newText = QString(_ui->textEdit->toPlainText());

            //std::cout << "inserted: " << newText.toStdString().substr(from, diff) << std::endl;
            _inputManager->addSequenceFromEditor(
                    InputManager::EditorActionType::insertSequence,
                    newText.mid(from, diff),
                    from,
                    to);
        } else if (diff < 0) { //delete
#if NOTEPAD_DEBUG
            std::cout << "onTextChanged() -> delete" << std::endl;
#endif
            //std::cout << "deleted: " << _currentText.toStdString().substr(to, (oldCur.hasSelection)?(0 - diff):1) << std::endl;
            //std::cout << "1" << std::endl;
            _inputManager->addSequenceFromEditor(
                    InputManager::EditorActionType::deleteSequence,
                    _currentText.mid(from, -diff), //diff should always be 1 //(oldCur.hasSelection)?(0 - diff):1),
                    from,
                    to); // to should alwayd be from + 1 //(oldCur.hasSelection)?from:to+1);

        } else { // canc
            //caso particolare
#if NOTEPAD_DEBUG
            std::cout << "onTextChanged() -> nop" << std::endl;
#endif
            if(_key_to_handle) {
                _inputManager->addSequenceFromEditor(
                        InputManager::EditorActionType::deleteSequence,
                        _currentText.mid(from, 1),
                        from,
                        from + 1);
            }
        }

        _key_to_handle=false;
        _currentText = QString(_ui->textEdit->toPlainText());
        updateCursors();

    _server->sendCursor(_sharedDocument->getDocumentId(), _newCursor);
}

void Notepad::onCursorPositionChanged()
{
#if NOTEPAD_DEBUG
    std::cout<<"onCursorPositionChanged()"<<std::endl;
#endif
    updateCursors();
    _key_to_handle = false;
    _server->sendCursor(_sharedDocument->getDocumentId(), _newCursor);
}

void Notepad::updateCursors(const CustomCursor &cursor) {
    //std::cout<<"updateCursors()"<<std::endl;
    _oldCursor = CustomCursor(_newCursor);
    _newCursor = CustomCursor(cursor);
}

void Notepad::resetCursors(){
    _oldCursor = CustomCursor(QTextCursor(_ui->textEdit->textCursor()));
    _newCursor = CustomCursor(QTextCursor(_ui->textEdit->textCursor()));
}

void Notepad::updateCursors() {
    CustomCursor cursor = CustomCursor(QTextCursor(_ui->textEdit->textCursor()));
    updateCursors(cursor);
}

void Notepad::onOpenClicked(){
    detachSharedDocument();
    emit documentChoiceRequestSignal();
    close();
}

void Notepad::onSettingsClicked() {
    auto *connectionSettingsDialog = new ConnectionSettingsDialog(_server, this);
    int retval = connectionSettingsDialog->exec();
    connectionSettingsDialog->deleteLater();
    // TODO: se retval è 1 allora ho salvato le impostazioni quindi devo fare qualcosa adesso...
}

void Notepad::onDocumentInfoClicked() {
    if(_sharedDocument!= nullptr) {
        DocumentInfo docInfo = DocumentInfo{_server->getSiteId(), _sharedDocument->getDocumentId(), getDocName(),
                                            std::vector<QString>{}, std::vector<unsigned int>{}};
        auto *documentInfoDialog = new DocumentInfoDialog(_server, docInfo, false, this);
        int retval = documentInfoDialog->exec();
        documentInfoDialog->deleteLater();
    }
}

void Notepad::print()
{
#if QT_CONFIG(printer)
    QPrinter printDev;
#if QT_CONFIG(printdialog)
    QPrintDialog dialog(&printDev, this);
    if (dialog.exec() == QDialog::Rejected)
        return;
#endif // QT_CONFIG(printdialog)
    _ui->textEdit->print(&printDev);
#endif // QT_CONFIG(printer)
}

void Notepad::logout() {
    detachSharedDocument();
    auto &settings = Settings::get();
    settings.setUsername("");
    settings.setPassword("");
    settings.setRememberMe(false);

    emit loginRequest();
    close();
}

void Notepad::onLogoutClicked()
{
    logout();
}

void Notepad::exit()
{
    QCoreApplication::quit();
}

void Notepad::copy()
{
#if QT_CONFIG(clipboard)
    _ui->textEdit->copy();
#endif
}

void Notepad::cut()
{
#if QT_CONFIG(clipboard)
    _ui->textEdit->cut();
#endif
}

void Notepad::paste()
{
#if QT_CONFIG(clipboard)
    _ui->textEdit->paste();
#endif
}

void Notepad::about()
{
    QMessageBox::about(this, tr("About Text Editor"),
                       tr("<b>Text Editor</b> è il progetto del corso "
                          "Programmazione di sistema"));

}

void Notepad::moveCursor(const CustomCursor &cursor) {
    bool previousBool = connectTextEditor(false);
    _ui->textEdit->setTextCursor(cursor.toQTextCursor(_ui->textEdit->textCursor()));
    connectTextEditor(previousBool);
}

void Notepad::onSharedDocumentReady(std::shared_ptr<SharedDocument> sharedDocument) {
    attachSharedDocument(std::move(sharedDocument));
    setWindowTitle(getDocName());
    show();
}

void Notepad::onRemoteInsertSlot(GraphicSymbol gSymbol) {
    insertRemoteSymbol(gSymbol.position, gSymbol.symbol);
}

void Notepad::onRemoteDeleteSlot(GraphicSymbol gSymbol) {
    deleteRemoteSymbol(gSymbol.position);
}

void Notepad::insertRemoteSymbol(const unsigned int position, const Symbol symbol) {
    bool previousBool = connectTextEditor(false);
    QTextCursor myQTC = QTextCursor(_ui->textEdit->textCursor());
    QTextCursor externQTC = QTextCursor(myQTC);
    externQTC.clearSelection();
    externQTC.movePosition(QTextCursor::MoveOperation::Start, QTextCursor::MoveMode::MoveAnchor, 1);
    externQTC.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::MoveAnchor, (int)position);
    _ui->textEdit->setTextCursor(externQTC);
    _ui->textEdit->textCursor().insertText(symbol.getValue());
    _ui->textEdit->setTextCursor(myQTC);
    resetCursors();
    resetText();
    connectTextEditor(previousBool);
}

void Notepad::deleteRemoteSymbol(const unsigned int position) {
    bool previousBool = connectTextEditor(false);
    QTextCursor myQTC = QTextCursor(_ui->textEdit->textCursor());
    QTextCursor externQTC = QTextCursor(myQTC);
    externQTC.clearSelection();
    externQTC.movePosition(QTextCursor::MoveOperation::Start, QTextCursor::MoveMode::MoveAnchor, 1);
    externQTC.movePosition(QTextCursor::MoveOperation::Right, QTextCursor::MoveMode::MoveAnchor, (int)position);
    _ui->textEdit->setTextCursor(externQTC);
    _ui->textEdit->textCursor().deleteChar();
    _ui->textEdit->setTextCursor(myQTC);
    resetCursors();
    resetText();
    connectTextEditor(previousBool);
}

void Notepad::resetText() {
    _currentText = QString(_ui->textEdit->toPlainText());
}

void Notepad::onDeleteDocumentResponseSlot(QJsonObject json)
{
    std::cout << "AAA" << std::endl;
    DeleteDocumentResponse res{json};
    if(res.getId()==_sharedDocument->getDocumentId()) {
        detachSharedDocument();
        emit documentChoiceRequestSignal();
        close();
    }
}