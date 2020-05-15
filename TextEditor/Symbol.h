//
// 21/05/2019.
//

#ifndef LAB3_SYMBOL_H
#define LAB3_SYMBOL_H

#include <vector>
#include <QChar>
#include <QtCore/QJsonObject>
#include "Version.h"

class Symbol {
public:
    Symbol(const QChar &value, unsigned int siteId, unsigned int counter, std::vector<unsigned int> position);
    explicit Symbol(const QJsonObject &json);
    ~Symbol();
    [[nodiscard]] Version getVersion() const;
    [[nodiscard]] QChar getValue() const;
    [[nodiscard]] unsigned int getSiteId() const;
    [[nodiscard]] unsigned int getCounter() const;
    [[nodiscard]] std::vector<unsigned int> getPosition() const; // prima ritornava un const ref (in caso poi lo rimetto come prima).
    [[nodiscard]] int comparePositionWith(const Symbol &other) const;
    bool operator<=(const Symbol &right) const;
    bool operator<(const Symbol &right) const;
    bool operator>=(const Symbol &right) const;
    bool operator>(const Symbol &right) const;
    [[nodiscard]] QJsonObject toQJsonObject() const;
private:
    QChar _value;
    unsigned int _siteId;
    unsigned int _counter;
    std::vector<unsigned int> _position;
};


#endif //LAB3_SYMBOL_H
