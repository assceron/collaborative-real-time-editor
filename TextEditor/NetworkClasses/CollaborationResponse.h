//
// 28/09/2019.
//

#ifndef TEXTEDITOR_COLLABORATIONRESPONSE_H
#define TEXTEDITOR_COLLABORATIONRESPONSE_H

#include "GenericMessage.h"

class CollaborationResponse : public GenericMessage{
public:
    inline explicit CollaborationResponse(QJsonObject json): GenericMessage(std::move(json)){};
    inline CollaborationResponse(const QString &docId, const QString &name, const bool addCollaborator, const bool byURL):
            GenericMessage(bodyTypeToQString(BodyType::collaboration_res)){
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

#endif //TEXTEDITOR_COLLABORATIONRESPONSE_H
