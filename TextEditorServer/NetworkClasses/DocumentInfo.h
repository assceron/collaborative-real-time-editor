//
// 31/08/2019.
//

#ifndef TEXTEDITOR_DOCUMENTINFO_H
#define TEXTEDITOR_DOCUMENTINFO_H

#include <vector>
#include <QtCore/QJsonArray>
#include <QtGui/QColor>
#include "GenericMessage.h"

class DocumentInfo : public GenericMessage{
public:
    inline explicit DocumentInfo(QJsonObject json): GenericMessage(std::move(json)){};
    inline DocumentInfo(const unsigned int siteId, const QString &docId, const QString &docName, const std::vector<QString> &collaborators, std::vector<unsigned int> collaboratorIds):
            GenericMessage(bodyTypeToQString(BodyType::document_info)){
        insert("site_id", QString::number(siteId));
        insert("id", docId);
        insert("name", docName);
        QJsonArray array = QJsonArray{};
        for(const QString &collaborator : collaborators){
            array.push_back(collaborator);
        }
        insert("collaborators", array);
        QJsonArray array2 = QJsonArray{};
        for(unsigned int collaboratorId : collaboratorIds){
            array2.push_back(QString::number(collaboratorId));
        }
        insert("collaborator_ids", array2);

    }
    inline void setColors(std::vector<QColor> colors){
        QJsonArray array_r = QJsonArray{};
        QJsonArray array_g = QJsonArray{};
        QJsonArray array_b = QJsonArray{};
        QJsonArray array_a = QJsonArray{};
        for(auto color : colors){
            int r,g,b,a;
            color.getRgb(&r,&g,&b,&a);
            array_r.push_back(r);
            array_g.push_back(g);
            array_b.push_back(b);
            array_a.push_back(a);
        }
        insert("colors_r", array_r);
        insert("colors_g", array_g);
        insert("colors_b", array_b);
        insert("colors_a", array_a);
    }
    inline std::vector<QColor> getColors() const {
        QJsonArray array_r = _json["colors_r"].toArray();
        QJsonArray array_g = _json["colors_g"].toArray();
        QJsonArray array_b = _json["colors_b"].toArray();
        QJsonArray array_a = _json["colors_a"].toArray();
        std::vector<QColor> colors = std::vector<QColor>{};
        for(int i= 0; i< array_r.size(); i++){
            colors.push_back(QColor::fromRgb(array_r[i].toInt(),array_g[i].toInt(),array_b[i].toInt(),array_a[i].toInt()));
        }
        return colors;
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
    [[nodiscard]] inline std::vector<unsigned int> getCollaboratorIds() const {
        QJsonArray array = _json["collaborator_ids"].toArray();
        std::vector<unsigned int> collaborators = std::vector<unsigned int>{};
        for (const auto &elem : array) {
            collaborators.push_back(elem.toString().toUInt());
        }
        return collaborators;
    }
};

#endif //TEXTEDITOR_DOCUMENTINFO_H
