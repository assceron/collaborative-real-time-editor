//
// 29/07/2019.
//

#include "crdt_exception.h"

crdt_exception::crdt_exception(const char *msg):std::invalid_argument(msg){}