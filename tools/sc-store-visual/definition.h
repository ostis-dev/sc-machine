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

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "segmentitem.h"
#include "segmentscene.h"
#include "segmentview.h"
#include "segmentarea.h"


#endif // DEFINITION_H
