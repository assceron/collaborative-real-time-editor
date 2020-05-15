//
// 26/08/2019.
//

#ifndef TEXTEDITOR_DOCUMENTSRESPONSE_H
#define TEXTEDITOR_DOCUMENTSRESPONSE_H

#include <QtCore/QJsonArray>
#include <vector>
#include "GenericMessage.h"
#include "DocumentInfo.h"

class DocumentsResponse : public GenericMessage{
public:
    inline explicit DocumentsResponse(QJsonObject json): GenericMessage(std::move(json)){};
    inline explicit DocumentsResponse(const std::vector<DocumentInfo> &documents):
            GenericMessage(bodyTypeToQString(BodyType::documents_res)){
        QJsonArray array = QJsonArray{};
        for(const DocumentInfo &document : documents){
            array.push_back(document.toJson());
        }
        insert("documents", array);
    }
    [[nodiscard]] inline std::vector<DocumentInfo> getDocuments() const {
        QJsonArray array = _json["documents"].toArray();
        std::vector<DocumentInfo> documents = std::vector<DocumentInfo>{};
        for(const auto &elem : array){
            documents.emplace_back(elem.toObject());
        }
        return documents;
    }
};

#endif //TEXTEDITOR_DOCUMENTSRESPONSE_H
