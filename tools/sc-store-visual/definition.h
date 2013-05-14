#ifndef DEFINITION_H
#define DEFINITION_H

#include <glib.h>

extern "C" {
  #include "sc-store/sc_types.h"
  #include "sc-store/sc_store.h"
  #include "sc-store/sc_element.h"
}

struct sc_element_ex {
    sc_element *element;
    sc_addr address;
};

#endif // DEFINITION_H
