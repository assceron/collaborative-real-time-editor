cmake_minimum_required(VERSION 3.14)
project(TextEditorServer)

#set(CMAKE_PREFIX_PATH C:/Qt/5.13.0/mingw73_64/lib/cmake)
#set(CMAKE_PREFIX_PATH C:/MinGW/lib/cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SOURCES
        main.cpp
        MainServer.cpp
        DBManager.cpp
        
        ClientsManager.cpp
        CustomCursor.cpp
        Action.cpp
        Symbol.cpp
        Version.cpp

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
        NetworkClasses/SubscribeResponse.h NetworkClasses/CollaborationResponse.h NetworkClasses/DeleteDocumentResponse.h)

#find_package(Boost 1.70.0 REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
#find_package(Qt5Widgets REQUIRED)
#find_package(Qt5Charts REQUIRED)
#find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5WebSockets REQUIRED)

#include_directories(${Boost_INCLUDE_DIRS})

list(APPEND LIBRARIES
        Qt5::Core
        Qt5::Gui
        #Qt5::Widgets
        #Qt5::Charts
        #Qt5::PrintSupport
        Qt5::WebSockets
        )
include_directories(
        /usr/local/include/bsoncxx/v_noabi
        /usr/local/include/mongocxx/v_noabi
)


list(REMOVE_DUPLICATES LIBRARIES)

add_executable(TextEditorServer ${SOURCES} NetworkClasses/DeleteDocumentResponse.h)# ${Boost_INCLUDE_DIRS})

target_link_libraries(TextEditorServer ${LIBRARIES}
        /usr/local/lib/libbsoncxx.so
        /usr/local/lib/libmongocxx.so)# ${Boost_LIBRARIES})
#ws2_32

#find_package(Boost 1.57.0 COMPONENTS system filesystem REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})
#add_executable(BoostTest main.cpp)
#target_link_libraries(BoostTest ${Boost_LIBRARIES})