//
// Created by alexander on 23.05.20.
//

#ifndef SC_MACHINE_DUMPER_H
#define SC_MACHINE_DUMPER_H

#define NREL_FORMAT_STR     "nrel_format"
#define DUMP_FOLDER "/home/alexander/Desktop/KnowledgeDump.scs"
#define DUMP_CONTENT_FOLDER "/home/alexander/Desktop/content/"

#include <cstdio>
#include <sc-core/sc-store/sc_types.h>
#include <string>
#include "sc-memory/sc_memory.hpp"
#include "model/DumpElement.h"

using namespace std;

struct BuilderParams
{
    //! Input directory path
    std::string m_inputPath;
    //! Output directory path
    std::string m_outputPath;
    //! Path to memory extensions
    std::string m_extensionsPath;
    //! Path to file with a list of enabled extensions
    std::string m_enabledExtPath;
    //! Flag to clear output
    bool m_clearOutput:1;
    //! Flag to generate format information based on file extensions
    bool m_autoFormatInfo:1;
    //! Path to configuration file
    std::string m_configFile;
};

bool printElement(ScAddr element, string *strBuilder);
bool printNewElement(ScAddr element, string *strBuilder);
void printExistingElement(ScAddr element, string *strBuilder);
void printEdge(ScAddr element, const string& connector, DumpElement* dumpElement, string *strBuilder);
string getElementContent(ScAddr element);
bool isContentHasFormat(ScAddr element, string *format);
sc_char *saveContentAsFile(ScAddr element, string data, const string& format);
bool isAddrExist(ScAddr addr);
//return 0, if element has system ID (not ..)
int getElementIdByAddr(ScAddr addr);
bool isEdge(ScAddr addr);
string processAlias(string line);
string prepareElementToPrint(string* strBuffer);
string resolveConnector(ScType *type);

#endif //SC_MACHINE_DUMPER_H
