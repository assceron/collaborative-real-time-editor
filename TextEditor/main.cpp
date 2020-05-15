//#include <winsock2.h> // se non compila aggiungi sta cosa prima di includere le classi di boost.
//#include <boost/beast/websocket.hpp> // prova per vedere se compila.

#include "notepad.h"
#include "notepadlogin.h"
#include "notepadsignup.h"
#include <QApplication>
#include "FileManager.h"
#include "notepaddocumentchooser.h"
#include "SocketManager.h"
#include <QObject>
#include <iostream>
//#include <QtWebSockets/QtWebSockets>

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    app.setApplicationName("PDSTextEditor");
    std::shared_ptr<FileManager> fileManager = std::make_shared<FileManager>(app.applicationDirPath());
    //FileManager::getInstance().setAppPath(app.applicationDirPath());
    auto &settings = Settings::get();
    settings.setFileManager(fileManager);
    fileManager.reset();
    settings.load();
    // le cose qua sopra prima di tutto, in questo ordine, sennò esplode tutto.

    QCommandLineParser parser;
    parser.setApplicationDescription("Text Editor Client");
    parser.addHelpOption();

    QCommandLineOption dbgOption(QStringList() << "d" << "debug",
                                 QCoreApplication::translate("main", "Debug output [default: off]."));
    parser.addOption(dbgOption);
    std::string addressText = QString("Server address [default: %1].").arg(settings.getServerAddressDefault()).toStdString();
    char addressChars[addressText.size() + 1];
    std::copy(addressText.begin(), addressText.end(), addressChars);
    addressChars[addressText.size()] = '\0';
    QCommandLineOption addressOption(QStringList() << "a" << "address",
                                  QCoreApplication::translate("main", addressChars),
                                  QCoreApplication::translate("main", "address"), QLatin1Literal(""));
    parser.addOption(addressOption);
    std::string portText = QString("Server port [default: %1].").arg(settings.getServerPortDefault()).toStdString();
    char portChars[portText.size() + 1];
    std::copy(portText.begin(), portText.end(), portChars);
    portChars[portText.size()] = '\0';
    QCommandLineOption portOption(QStringList() << "p" << "port",
                                  QCoreApplication::translate("main", portChars),
                                  QCoreApplication::translate("main", "port"), QLatin1Literal(""));
    parser.addOption(portOption);
    parser.process(app);

    bool debug = parser.isSet(dbgOption);
    QString serverAddress = parser.value(addressOption);
    QString serverPort = parser.value(portOption);

    if(!serverAddress.isEmpty()) {
        settings.setServerAddress(serverAddress);
    }
    if(!serverPort.isEmpty()) {
        settings.setServerPort(serverPort);
    }

    std::shared_ptr<NetworkManager> server = std::make_shared<NetworkManager>(debug);

    NotepadLogin login = NotepadLogin{server};

    NotepadSignup signup = NotepadSignup{server};

    NotepadDocumentChooser docChooser = NotepadDocumentChooser{server};

    std::unique_ptr<InputManager> actionsManager = std::make_unique<InputManager>();

    Notepad editor(server, std::move(actionsManager));

    QObject::connect(&login, SIGNAL(loginDone()), &docChooser, SLOT(onChoiceRequest()));
    QObject::connect(&signup, SIGNAL(signupDone()), &docChooser, SLOT(onChoiceRequest()));

    QObject::connect(&docChooser, SIGNAL(documentSelectedSignal(std::shared_ptr<SharedDocument>)), &editor, SLOT(onSharedDocumentReady(std::shared_ptr<SharedDocument>)));
    QObject::connect(&editor, SIGNAL(documentChoiceRequestSignal()), &docChooser, SLOT(onChoiceRequest()));

    QObject::connect(&editor, SIGNAL(loginRequest()), &login, SLOT(onLoginRequest()));
    QObject::connect(&docChooser, SIGNAL(loginRequestSignal()), &login, SLOT(onLoginRequest()));
    QObject::connect(&login, SIGNAL(signupRequest()), &signup, SLOT(onSignupRequest()));
    QObject::connect(&signup, SIGNAL(loginRequest()), &login, SLOT(onLoginRequest()));

    QObject::connect(server.get(), SIGNAL(connectedSignal()), &login, SLOT(startupSlot()));
    QObject::connect(server.get(), SIGNAL(disconnectedSignal()), &login, SLOT(startupSlot()));

    login.show();
    server->connectSocket();

    /*if(settings.getRememberMe()){
        //login.login(settings.getUsername(), settings.getPassword(), settings.getRememberMe(), false);
    }else {
        login.show();
    }*/

    //editor.show();

    return app.exec();
}

// dall'esterno: insert eseguite subito. buffer per le delete. controllo sul buffer dopo ogni operazione dall'esterno.

// https://conclave-team.github.io/conclave-site/
// https://github.com/conclave-team/conclave/tree/master/lib
// https://github.com/conclave-team/conclave/blob/master/lib/editor.js
// https://github.com/conclave-team/conclave/blob/master/lib/controller.js
// https://github.com/conclave-team/conclave/blob/master/lib/broadcast.js

//https://thispointer.com/c-how-to-read-or-write-objects-in-file-serializing-deserializing-objects/
//https://www.bogotobogo.com/Qt/Qt5_QListView_QStringListModel_ModelView_MVC.php

//[...]
//https://code.qt.io/cgit/qt/qtbase.git/tree/examples/widgets/tutorials/notepad?h=5.13
//https://doc.qt.io/qt-5/qtwidgets-tools-completer-example.html
//https://doc.qt.io/qt-5/qtwidgets-tools-customcompleter-example.html
//https://doc.qt.io/qt-5/qtwidgets-widgets-codeeditor-example.html#


/*
    NetworkServer network{};
    SharedDocument ed1{network};
    SharedDocument ed2{network};

    ed1.connect();
    ed2.connect();

    ed1.localInsert(0, 'c');
    ed1.localInsert(1, 'a');
    ed1.localInsert(2, 't');

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    ed1.localInsert(1, 'h');
    ed2.localErase(1);
    ed2.localInsert(2, 'u');
    ed1.localInsert(1, 'C');
    ed2.localInsert(3, 'b');
    ed2.localInsert(4, 's');
    ed1.localInsert(2, 'f');
    ed1.localErase(0);

    network.dispatchMessages();
    std::cout<<"ed1: "<<ed1.to_string()<<std::endl;
    std::cout<<"ed2: "<<ed2.to_string()<<std::endl;

    ed1.disconnect();
    ed2.disconnect();
 */