//
// 29/07/2019.
//

#ifndef TEXTEDITOR_CRDT_EXCEPTION_H
#define TEXTEDITOR_CRDT_EXCEPTION_H

#include <stdexcept>

class crdt_exception : public std::invalid_argument{
public:
    explicit crdt_exception(const char *msg);
};


#endif //TEXTEDITOR_CRDT_EXCEPTION_H
