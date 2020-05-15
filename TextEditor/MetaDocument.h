//
// 30/07/2019.
//

#ifndef TEXTEDITOR_METADOCUMENT_H
#define TEXTEDITOR_METADOCUMENT_H


#include "CustomCursor.h"
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QByteArray>

class MetaDocument {
public:
    explicit MetaDocument(const CustomCursor &cursor);
    explicit MetaDocument(const QJsonObject &json);
    ~MetaDocument();
    [[nodiscard]] QJsonObject toQJsonObject() const;
    [[nodiscard]] QByteArray toBytes() const;
    [[nodiscard]] CustomCursor getCursor() const;
private:
    CustomCursor _cursor;
};


#endif //TEXTEDITOR_METADOCUMENT_H
