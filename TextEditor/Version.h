//
// 22/05/2019.
//

#ifndef LAB3_VERSION_H
#define LAB3_VERSION_H

#include <vector>
#include <QtCore/QJsonObject>

class Version {
public:
    explicit Version(unsigned int siteId);
    Version(unsigned int siteId, unsigned int counter);
    explicit Version(const QJsonObject &json);
    Version(const Version &other) = default;
    Version() = delete;
    void update(Version incomingVersion);
    ~Version();
    // Declare prefix and postfix increment operators.
    Version& operator++();       // Prefix increment operator.
    const Version operator++(int);     // Postfix increment operator.
    // Declare prefix and postfix decrement operators.
    Version& operator--();       // Prefix decrement operator.
    const Version operator--(int);     // Postfix decrement operator.
    // comparison operators
    bool operator==(const Version &right) const;
    bool operator!=(const Version &right) const;
    bool operator==(int right) const;
    bool operator!=(int right) const;
    bool operator<=(const Version &right) const;
    bool operator<(const Version &right) const;
    bool operator>=(const Version &right) const;
    bool operator>(const Version &right) const;
    // getters
    [[nodiscard]] unsigned int getSiteId() const;
    [[nodiscard]] unsigned int getCounter() const;
    [[nodiscard]] bool containsException(unsigned int exception) const;
    [[nodiscard]] QJsonObject toQJsonObject() const;
private:
    unsigned int _siteId;
    unsigned int _counter;
    std::vector<unsigned int> _exceptions;
};


#endif //LAB3_VERSION_H
