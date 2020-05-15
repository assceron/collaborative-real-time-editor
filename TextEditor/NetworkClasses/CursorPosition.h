//
// 14/09/2019.
//

#ifndef TEXTEDITOR_CURSORPOSITION_H
#define TEXTEDITOR_CURSORPOSITION_H

#include "GenericMessage.h"
#include "../CustomCursor.h"

class CursorPosition : public GenericMessage{
public:
    inline explicit CursorPosition(QJsonObject json): GenericMessage(std::move(json)){};
    inline CursorPosition(const QString &docId, CustomCursor cursor, const unsigned int siteId, const QString &name):
            GenericMessage(bodyTypeToQString(BodyType::cursor_position)){
        insert("id", docId);
        insert("cursor", cursor.toQJsonObject());
        insert("site_id", QString::number(siteId));
        insert("name", name);
    }
    inline CursorPosition(const QString &docId, CustomCursor cursor):
            GenericMessage(bodyTypeToQString(BodyType::cursor_position)){
        insert("id", docId);
        insert("cursor", cursor.toQJsonObject());
    }
    inline void setSiteId(unsigned int siteId){
        insert("site_id", QString::number(siteId));
    }
    inline void setName(const QString &name){
        insert("name", name);
    }
    inline void setSiteColor(const QColor &color){
        int r, g, b, a;
        color.getRgb(&r,&g,&b,&a);
        insert("site_color_r", r);
        insert("site_color_g", g);
        insert("site_color_b", b);
        insert("site_color_a", a);
    }
    inline void setRemove(){
        insert("remove", true);
    }

    [[nodiscard]] inline QColor getSiteColor() const {
        int r,g,b,a;
        r = _json["site_color_r"].toInt(255);
        g = _json["site_color_g"].toInt(255);
        b = _json["site_color_b"].toInt(255);
        a = _json["site_color_a"].toInt(255);
        return QColor::fromRgb(r,g,b,a);
    }
    [[nodiscard]] inline QString getId() const {
        return _json["id"].toString();
    }
    [[nodiscard]] inline unsigned int getSiteId() const {
        return _json["site_id"].toString().toUInt();
    }
    [[nodiscard]] inline QString getName() const {
        return _json["name"].toString();
    }
    [[nodiscard]] inline CustomCursor getCursor() const {
        return CustomCursor{_json["cursor"].toObject()};
    }
    [[nodiscard]] inline bool getRemove() const {
        return _json["remove"].toBool(false);
    }
};

#endif //TEXTEDITOR_CURSORPOSITION_H
