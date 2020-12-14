//
// Created by alexander on 23.05.20.
//

#ifndef SC_MACHINE_DUMPELEMENT_H
#define SC_MACHINE_DUMPELEMENT_H

#include <sc-core/sc-store/sc_types.h>
#include <vector>
#include <string>
#include <sc-memory/sc_addr.hpp>

class DumpElement {
private:
    ScAddr addr;
    //value 0 means that element has systemId
    int id;
    std::vector<std::string> types;


public:

    DumpElement(ScAddr addr, int id);
    ~DumpElement();

    ScAddr getAddr();
    int getId();
    std::vector<std::string> getTypes();
    void addType(std::string type);

    static const int DEFAULT_ID_IF_ELEMENT_HAS_SYSTEM_ID = 0;
};

#endif //SC_MACHINE_DUMPELEMENT_H
