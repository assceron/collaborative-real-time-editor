cmake_minimum_required(VERSION 3.14)
project(TextEditor)

#set(CMAKE_PREFIX_PATH C:/Qt/5.13.0/mingw73_64/lib/cmake)
#set(CMAKE_PREFIX_PATH C:/MinGW/lib/cmake)
#set(CMAKE_PREFIX_PATH /usr/lib/x86_64-linux-gnu/cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SOURCES
    notepad.qrc
    notepad.ui
    notepadlogin.ui
    notepadsignup.ui
    notepaddocumentchooser.ui
    connectionsettingsdialog.ui
    documentinfodialog.ui
    notepad.cpp
    notepadlogin.cpp
    notepadsignup.cpp
    notepaddocumentchooser.cpp
    connectionsettingsdialog.cpp
    documentinfodialog.cpp
    InputManager.cpp
    Symbol.cpp
    Action.cpp
    SharedDocument.cpp
    NetworkManager.cpp
    Version.cpp
    VersionController.cpp
    ModifiedLSEQ.cpp
    FileManager.cpp
    Document.cpp
    crdt_exception.cpp
    CustomCursor.cpp
    MetaDocument.cpp
    Settings.cpp
    SocketManager.cpp
    GraphicSymbol.cpp
    main.cpp

        NetworkClasses/GenericMessage.h
        NetworkClasses/LoginRequest.h
        NetworkClasses/LoginResponse.h
        NetworkClasses/SignupRequest.h
        NetworkClasses/SignupResponse.h
        NetworkClasses/NewDocumentRequest.h
        NetworkClasses/NewDocumentResponse.h
        NetworkClasses/DocumentRequest.h
        NetworkClasses/DocumentResponse.h
        NetworkClasses/DocumentsRequest.h
        NetworkClasses/DocumentsResponse.h
        NetworkClasses/Actions.h
        NetworkClasses/SubscribeRequest.h
        NetworkClasses/UnsubscribeRequest.h
        NetworkClasses/LogoutRequest.h
        NetworkClasses/DocumentInfo.h
        NetworkClasses/DocumentInfoRequest.h
        NetworkClasses/CollaborationRequest.h
        NetworkClasses/DeleteDocumentRequest.h
        NetworkClasses/CursorPosition.h
        NetworkClasses/CollaborationResponse.h
        NetworkClasses/DeleteDocumentResponse.h
        )

#find_package(Boost 1.70.0 REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
#find_package(Qt5Charts REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5WebSockets REQUIRED)

#include_directories(${Boost_INCLUDE_DIRS})

list(APPEND LIBRARIES
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    #Qt5::Charts
    Qt5::PrintSupport
    Qt5::WebSockets
)

list(REMOVE_DUPLICATES LIBRARIES)

add_executable(TextEditor ${SOURCES})# ${Boost_INCLUDE_DIRS})

target_link_libraries(TextEditor ${LIBRARIES})# ${Boost_LIBRARIES})
#ws2_32

#find_package(Boost 1.57.0 COMPONENTS system filesystem REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})
#add_executable(BoostTest main.cpp)
#target_link_libraries(BoostTest ${Boost_LIBRARIES})