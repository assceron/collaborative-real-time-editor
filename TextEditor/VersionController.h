//
// 22/05/2019.
//

#ifndef LAB3_VERSIONCONTROLLER_H
#define LAB3_VERSIONCONTROLLER_H

#include "Version.h"
#include <vector>
#include <QJsonObject>
#include <memory>

class VersionController {
public:
    explicit VersionController(const unsigned int siteId);
    VersionController(const QJsonObject &json, const unsigned int siteId);
    VersionController() = delete;
    ~VersionController();
    unsigned int increment();
    void update(const Version &incomingVersion);
    bool hasBeenApplied(const Version &incomingVersion);
    std::shared_ptr<Version> getVersionFromVector(const Version & incomingVersion);
    std::shared_ptr<Version> getVersionFromVector(const unsigned int siteId);
    [[nodiscard]] QJsonObject toQJsonObject() const;
private:
    std::shared_ptr<Version> _localVersion;
    std::vector<std::shared_ptr<Version>> _versions;
};


#endif //LAB3_VERSIONCONTROLLER_H
