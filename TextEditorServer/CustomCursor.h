//
// 30/07/2019.
//

#ifndef TEXTEDITOR_CUSTOMCURSOR_H
#define TEXTEDITOR_CUSTOMCURSOR_H

#include <QtGui/QTextCursor>
#include <QtCore/QJsonObject>

class CustomCursor{
public:
    unsigned int position;
    bool hasSelection;
    bool reverseSelection;
    unsigned int selectionStart;
    unsigned int selectionEnd;
    explicit CustomCursor(const QJsonObject &json);
    explicit CustomCursor(const QTextCursor &qTextCursor);
    CustomCursor(const CustomCursor &cursor) = default;
    CustomCursor();
    ~CustomCursor();
    QJsonObject toQJsonObject() const;
    QTextCursor toQTextCursor(const QTextCursor &base) const;
    inline bool operator==(const CustomCursor &rhs){
        return
                this->position==rhs.position
                && this->hasSelection==rhs.hasSelection
                && this->reverseSelection==rhs.reverseSelection
                && this->selectionStart==rhs.selectionStart
                && this->selectionEnd==rhs.selectionEnd
                ;
    }

    inline bool operator!=(const CustomCursor &rhs){
        return !(*this==rhs);
    }
};

#endif //TEXTEDITOR_CUSTOMCURSOR_H
