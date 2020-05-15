//
// 31/08/2019.
//

#ifndef TEXTEDITOR_COLLABORATIONREQUEST_H
#define TEXTEDITOR_COLLABORATIONREQUEST_H

#include "GenericMessage.h"

class CollaborationRequest : public GenericMessage{
public:
    inline explicit CollaborationRequest(QJsonObject json): GenericMessage(std::move(json)){};
    inline CollaborationRequest(const QString &docId, const QString &name, const bool addCollaborator, const bool byURL):
            GenericMessage(bodyTypeToQString(BodyType::collaboration_req)){
        insert("id", docId);
        insert("name", name);
        insert("add", addCollaborator);
        insert("by_url", byURL);
    };
    [[nodiscard]] inline QString getId() const {
        return _json["id"].toString();
    };
    [[nodiscard]] inline QString getName() const {
        return _json["name"].toString();
    };
    [[nodiscard]] inline bool getAddCollaborator() const {
        return _json["add"].toBool(false);
    };
    [[nodiscard]] inline bool getByURL() const {
        return _json["by_url"].toBool(false);
    };
};

#endif //TEXTEDITOR_COLLABORATIONREQUEST_H
