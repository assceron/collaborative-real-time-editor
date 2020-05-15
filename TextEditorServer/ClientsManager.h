//
// 27/08/2019.
//

#ifndef TEXTEDITORSERVER_CLIENTSMANAGER_H
#define TEXTEDITORSERVER_CLIENTSMANAGER_H


#include <QtWebSockets/QWebSocket>
#include <memory>
#include "NetworkClasses/NewDocumentResponse.h"
#include "CustomCursor.h"
#include <QColor>

typedef NewDocumentResponse Document;

class ClientsManager {
public:
    class Client{
    public:
        explicit Client(QWebSocket *clientSocket):
            socket(clientSocket),_logged(false),_siteId(0),_openDocId(QString()){
            qsrand(time(NULL));
            _color = QColor{qrand()%256,qrand()%256,qrand()%256,128};
            _cursor.
        };
        explicit Client(QWebSocket *clientSocket, bool clientLogged):
                socket(clientSocket),_logged(clientLogged),_siteId(0),_openDocId(QString()){
            qsrand(time(NULL));
            _color = QColor{qrand()%256,qrand()%256,qrand()%256,128};
        };
        explicit Client(QWebSocket *clientSocket, bool clientLogged, unsigned int clientSiteId):
            socket(clientSocket),_logged(clientLogged),_siteId(clientSiteId),_openDocId(QString()){
            qsrand(time(NULL));
            _color = QColor{qrand()%256,qrand()%256,qrand()%256,128};
        };
        bool operator==(const Client &other){
            return socket == other.socket;
        }
        [[nodiscard]] inline CustomCursor getCursor() const {
            return _cursor;
        }
        inline void setCursor(const CustomCursor &cursor){
            _cursor = CustomCursor{cursor};
            _validCursor = true;
        }
        inline void removeCursor(){
            _cursor = CustomCursor{};
            _validCursor = false;
        }
        [[nodiscard]] inline bool isCursorValid() const {return _validCursor;};
        [[nodiscard]] inline bool isLogged() const {return _logged;};
        inline void login(){_logged = true;};
        inline void logout(){_logged=false;};
        inline void subscribe(QString docId){
            _openDocId=std::move(docId);
            removeCursor();
        };
        inline void unsubscribe(QString docId = QString{}){
            _openDocId=QString{};
            removeCursor();
        };
        inline QString getOpenDocId(){return _openDocId;};
        inline unsigned int getSiteId(){return _siteId;};
        inline void setSiteId(unsigned int siteId){_siteId=siteId;};
        inline QColor getColor() const{
            return _color;
        }
    public:
        QWebSocket *socket;
    private:
        bool _validCursor = false;
        CustomCursor _cursor = CustomCursor{};
        bool _logged;
        unsigned int _siteId;
        QString _openDocId;
        QColor _color = QColor::fromRgb(255,255,255,255);
    };
public:
    ClientsManager() = default;
    ~ClientsManager();
    std::shared_ptr<Client> addClient(QWebSocket *socket);
    void removeClient(QWebSocket *socket);
    std::shared_ptr<Client> getClient(QWebSocket *socket);
    QList<QWebSocket *> getClientsViewingDocument(const QString &docId, QWebSocket *exclude = nullptr);
    QList<std::shared_ptr<Client>> getClientObjectsViewingDocument(const QString &docId, QWebSocket *exclude = nullptr);

    QWebSocket * getClientBySite(unsigned int siteId, QWebSocket *exclude = nullptr);

    std::shared_ptr<Client> getClientById(unsigned int id);
    QList<std::shared_ptr<Client>> _clients;
};


#endif //TEXTEDITORSERVER_CLIENTSMANAGER_H
