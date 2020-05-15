//
// 26/08/2019.
//

#ifndef TEXTEDITOR_NEWDOCUMENTRESPONSE_H
#define TEXTEDITOR_NEWDOCUMENTRESPONSE_H

#include <vector>
#include <QtCore/QJsonArray>
#include "GenericMessage.h"

class NewDocumentResponse : public GenericMessage{
public:
    inline explicit NewDocumentResponse(QJsonObject json): GenericMessage(std::move(json)){};
    inline NewDocumentResponse(const unsigned int siteId, const QString &docId, const QString &docName, const std::vector<QString> &collaborators):
            GenericMessage(bodyTypeToQString(BodyType::new_document_res)){
        insert("site_id", QString::number(siteId));
        insert("id", docId);
        insert("name", docName);
        QJsonArray array = QJsonArray{};
        for(const QString &collaborator : collaborators){
            array.push_back(collaborator);
        }
        insert("collaborators", array);

    }
    inline void setName(QString docName){
        insert("name", docName);
    }
    [[nodiscard]] inline unsigned int getSiteId() const { // site id of the creator.
        return _json["site_id"].toString().toUInt();
    }
    [[nodiscard]] inline QString getId() const {
        return _json["id"].toString();
    }
    [[nodiscard]]inline QString getName() const {
        return _json["name"].toString();
    }
    [[nodiscard]] inline std::vector<QString> getCollaborators() const {
        QJsonArray array = _json["collaborators"].toArray();
        std::vector<QString> collaborators = std::vector<QString>{};
        for(const auto &elem : array){
            collaborators.emplace_back(elem.toString());
        }
        return collaborators;
    }
};

#endif //TEXTEDITOR_NEWDOCUMENTRESPONSE_H
