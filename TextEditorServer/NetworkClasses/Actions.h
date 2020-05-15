//
// 27/08/2019.
//

#ifndef TEXTEDITOR_ACTIONS_H
#define TEXTEDITOR_ACTIONS_H

#include "../Action.h"
#include <QtCore/QJsonArray>
#include <vector>
#include "GenericMessage.h"

class Actions : public GenericMessage{
public:
    inline explicit Actions(QJsonObject json): GenericMessage(std::move(json)){};
    inline explicit Actions(const QString &docId, const std::vector<Action> &actions):
            GenericMessage(bodyTypeToQString(BodyType::actions)){
        insert("doc_id", docId);
        QJsonArray array = QJsonArray{};
        for(const Action &action : actions){
            array.push_back(action.toQJsonObject());
        }
        insert("actions", array);
    }
    [[nodiscard]] inline std::vector<Action> getActions() const {
        QJsonArray array = _json["actions"].toArray();
        std::vector<Action> actions = std::vector<Action>{};
        for(const auto &elem : array){
            actions.emplace_back(elem.toObject());
        }
        return actions;
    }
    [[nodiscard]] inline QString getDocId() const{
        return _json["doc_id"].toString();
    }
};

#endif //TEXTEDITOR_ACTIONS_H
