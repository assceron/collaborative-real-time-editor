//
// 21/05/2019.
//

#ifndef LAB3_MESSAGE_H
#define LAB3_MESSAGE_H

#include "Symbol.h"
#include "Version.h"
#include <chrono>
#include <string>
#include <vector>
#include <list>
#include <QJsonObject>

class Action {
public:
    enum MessageType {
        insert_type, delete_type, nop
    };
    static MessageType typeFromString(const QString &typeString);
    Action(const QString docId, const unsigned int sender, const unsigned int counter, const MessageType type, const Symbol symbol);
    explicit Action(const QJsonObject &json);
    ~Action();
    [[nodiscard]] unsigned int getSender() const;
    [[nodiscard]] Symbol getSymbol() const;
    //std::chrono::milliseconds getTimeStamp() const;
    [[nodiscard]] MessageType getType() const;
    [[nodiscard]] std::string getTypeString() const;
    [[nodiscard]] unsigned int getCounter() const;
    [[nodiscard]] QJsonObject toQJsonObject() const;
    [[nodiscard]] Version getVersion() const;
    [[nodiscard]] QString getDocId() const;
private:
    //std::chrono::milliseconds _timestamp;
    Symbol _symbol;
    unsigned int _sender;
    MessageType _type; //insert/delete
    unsigned int _counter;
    QString _docId;
};


#endif //LAB3_MESSAGE_H
