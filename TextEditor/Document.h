//
// 22/07/2019.
//

#ifndef TEXTEDITOR_DOCUMENT_H
#define TEXTEDITOR_DOCUMENT_H

class FileManager;

#include <string>
#include <list>
#include <vector>
#include "Symbol.h"
#include "VersionController.h"
#include "ModifiedLSEQ.h"
#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QByteArray>
#include <QChar>
#include "Action.h"
#include "FileManager.h"
#include <QObject>
#include <mutex>

/*
#define DOCUMENT_STORES_VERSIONS false
#define DOCUMENT_STORES_SYMBOLS false
#define DOCUMENT_STORES_ACTIONS true
#define DOCUMENT_STORES_DELETIONS false
#define DOCUMENT_LOADS_VERSIONS false
#define DOCUMENT_LOADS_SYMBOLS false
#define DOCUMENT_LOADS_ACTIONS true
#define DOCUMENT_LOADS_DELETIONS false
#define DOCUMENT_APPLIES_ACTIONS true
#define DOCUMENT_SAVES_IN_MEM_ACTIONS true
#define DOCUMENT_SAVES_IN_MEM_LOCAL_ACTIONS true
 */

#define DOCUMENT_STORES_VERSIONS false
#define DOCUMENT_STORES_SYMBOLS false
#define DOCUMENT_STORES_ACTIONS false
#define DOCUMENT_STORES_DELETIONS false
#define DOCUMENT_LOADS_VERSIONS false
#define DOCUMENT_LOADS_SYMBOLS false
#define DOCUMENT_LOADS_ACTIONS false
#define DOCUMENT_LOADS_DELETIONS false
#define DOCUMENT_APPLIES_ACTIONS false
#define DOCUMENT_SAVES_IN_MEM_ACTIONS false
#define DOCUMENT_SAVES_IN_MEM_LOCAL_ACTIONS false
#define DOCUMENT_STORES_METADATA false
#define DOCUMENT_LOADS_METADATA false
#define DOCUMENT_STORES_DOCUMENT false
#define DOCUMENT_LOADS_DOCUMENT false

#define SYMBOLS_USE_ARRAY false

typedef
#if SYMBOLS_USE_ARRAY
std::vector<Symbol>
#else
std::list<Symbol>
#endif
SymbolVector;

typedef std::vector<Action> ActionVector;
typedef std::list<Action> DeleteVector;

class Document : public QObject{
Q_OBJECT
private:
    QString _id;
    QString _name;
    VersionController _versionController;
    ModifiedLSEQ _lSeq;
    const unsigned int _my_site_id;
    unsigned int nextLocalCounter();
    bool validIndex(unsigned int index);
    void insertSymbol(Symbol symbol, bool draw);
    void deleteSymbol(Symbol symbol, bool draw);
    bool hasInsertionBeenApplied(const Action &message);
    ActionVector _actionVector;
    SymbolVector _symbols;
    DeleteVector _deleteVector;
    SymbolVector::iterator findPositionForSymbol(Symbol &symbol);
    SymbolVector::iterator findSymbolPosition(Symbol &symbol);
    void checkDeleteVector(bool draw);
public:
    void printJson();
    void applyAction(const Action &action, const bool saveAction, const bool draw);
    SymbolVector getSymbols();
    Document(QString id, QString name, const unsigned int siteId, QObject *parent = nullptr);
    Document(const QJsonObject &json, const unsigned int siteId, QObject *parent = nullptr);
    Document() = delete;
    ~Document() override ;
    Action localInsert(unsigned int index, const QChar &value);
    Action localDelete(unsigned int index);
    [[nodiscard]] QString getId() const;
    [[nodiscard]] QString getName() const;
    [[nodiscard]] QJsonArray serializeSymbols() const;
    [[nodiscard]] QJsonArray serializeActions() const;
    [[nodiscard]] QJsonArray serializeDeletions() const;
    [[nodiscard]] QJsonObject toQJsonObject() const;
    [[nodiscard]] QByteArray toBytes() const;
};

#endif //TEXTEDITOR_DOCUMENT_H
