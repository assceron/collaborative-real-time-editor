//
// 24/08/2019.
//

#ifndef TEXTEDITOR_GRAPHICSYMBOL_H
#define TEXTEDITOR_GRAPHICSYMBOL_H

#include "Symbol.h"

class GraphicSymbol {
public:
    Symbol symbol;
    unsigned int position;
public:
    GraphicSymbol(Symbol s, unsigned int p);
};


#endif //TEXTEDITOR_GRAPHICSYMBOL_H
