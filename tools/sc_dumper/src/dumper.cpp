//
// Created by alexander Yurkov on 23.05.20.
//

#include "dumper.h"
#include <cstdio>
#include <unistd.h>
#include "model/DumpElement.h"
#include "model/Alias.h"
#include <vector>
#include <sc-core/sc_helper.h>

using namespace std;

std::unique_ptr<ScMemoryContext> m_ctx;
ScAddr concertedKb;

vector<DumpElement> nodeVector;
vector<DumpElement> edgeVector;
vector<Alias> aliasVector;
vector<string> linkVector;
int uniqId;

string getElementContent(ScAddr element)
{
    string content;
    ScStreamPtr stream = m_ctx->GetLinkContent(element);
    string str;
    if (ScStreamConverter::StreamToString(stream, str)) {
        stringstream streamString(str);
        content = streamString.str();
    }
    string format;
    if (isContentHasFormat(element, &format)) {
        content = saveContentAsFile(element, content, format);
    }
    return content;
}

bool isContentHasFormat(ScAddr element, string *format) {
    bool result = false;
    ScAddr nrel_format;
    nrel_format = m_ctx->HelperFindBySystemIdtf(NREL_FORMAT_STR);
    if (!nrel_format.IsValid()) {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Error, Content has unknow format");
    }
    ScIterator5Ptr it = m_ctx->Iterator5(element,
                                         ScType::EdgeDCommonConst,
                                         ScType::NodeConst,
                                         ScType::EdgeAccessConstPosPerm,
                                         nrel_format);
    if (it->Next())
    {
        ScAddr t_node =it->Get(2);
        string temp_string = m_ctx -> HelperGetSystemIdtf(t_node);
        size_t n = temp_string.find_last_of('_');
        if (n != string::npos)
            *format = temp_string.substr(n + 1);
        result=true;
    }
    return result;
}

sc_char *saveContentAsFile(ScAddr element, string data, const string& format) {
    string filename = "content_";
    string answer = "file://content/";
    filename.append(to_string(element.Hash()));
    filename.append(".");
    filename.append(format);
    FILE *f;
    f = fopen((DUMP_CONTENT_FOLDER + filename).c_str(), "w");
    for (int i = 0; i < data.size(); i++) {
        fprintf(f, "%c", data[i]);
    }
    fclose(f);
    answer.append(filename);
    char *cstr = new char[answer.length() + 1];
    strcpy(cstr, answer.c_str());
    return cstr;
}

void run_dump()
{
    FILE *f;
    f = fopen("/home/alexander/Desktop/KnowledgeDump.scs", "w");
    string conceredKbName = "concertedKB_hash_iF95K2";
    concertedKb = m_ctx->HelperFindBySystemIdtf(conceredKbName);

    vector<string> outputLines;
    auto currentPrintBuffer = new std::string("");

    //print all constants
    ScIterator3Ptr it = m_ctx->Iterator3(concertedKb,
                                         ScType::EdgeAccessConstPosPerm,
                                         ScType::Const);
    while (it->Next()) {
        ScAddr printableElement = it->Get(2);
        currentPrintBuffer->clear();
        if (printElement(printableElement, currentPrintBuffer)) {
          string releaseElement = prepareElementToPrint(currentPrintBuffer);
            //no need to print systemId, cause it will be generate automatically during kb build
            size_t found = releaseElement.find("nrel_system_identifier");
            if (found == std::string::npos) {
                outputLines.push_back(releaseElement);
            }
        }
    }
    outputLines.emplace_back("\n");
    //print all vars
    it = m_ctx->Iterator3(concertedKb,
                          ScType::EdgeAccessConstPosPerm,
                          ScType::Var);
    while (it->Next()) {
        ScAddr t_arc =it->Get(2);
        currentPrintBuffer->clear();
        if (printElement(t_arc, currentPrintBuffer)) {
            string x2 = currentPrintBuffer->substr(1, currentPrintBuffer->size() - 2);
            x2.append(";;\n");
            size_t found = x2.find("nrel_system_identifier");
            if (found == std::string::npos) {
                outputLines.push_back(x2);
            }
        }
    }
    //print all link aliases
    for (auto & item : linkVector) {
        fprintf(f, "%s;;\n", item.c_str());
    }
    //print all edge aliases
    for (auto & alias : aliasVector) {
        currentPrintBuffer->clear();
        if (printNewElement(alias.getAddr(), currentPrintBuffer)) {
            fprintf(f, "@edge_alias_%d = %s;;\n", alias.getId(), currentPrintBuffer->c_str());
            alias.setEdge(*currentPrintBuffer);
        }
    }
    fprintf(f, "\n");
    //print all outputLines
    for (auto & line : outputLines) {
        fprintf(f, "%s", processAlias(line).c_str());
    }
    //print all node atributes
    for (auto & node : nodeVector) {
        currentPrintBuffer->clear();
        printExistingElement(node.getAddr(), currentPrintBuffer);
        if (!node.getTypes().empty()) {
            fprintf(f, "%s", currentPrintBuffer->c_str());
        }
        for (int j = 0; j < node.getTypes().size(); j++) {
            fprintf(f, "<-%s;", node.getTypes().at(j).c_str());
        }
        if (!node.getTypes().empty()) {
            fprintf(f, ";\n");
        }
    }
    nodeVector.clear();
    edgeVector.clear();
    aliasVector.clear();
    linkVector.clear();
    outputLines.clear();
    fclose(f);
}

void printEdge(ScAddr element, const string& connector, DumpElement *dumpElement, string *strBuilder) {
    ScAddr sourceEl = m_ctx -> GetEdgeSource(element);
    ScAddr targetEl = m_ctx -> GetEdgeTarget(element);
    strBuilder->append("(");

    if (!printElement(sourceEl, strBuilder)) {
        //if 0, than element have system ID (no ..)
        int x = getElementIdByAddr(sourceEl);
        if (x!=0) {
            if (isEdge(sourceEl)) {
                strBuilder->append("@edge_alias_").append(to_string(x));
                aliasVector.emplace_back(sourceEl, x);
            }
            else {
                strBuilder->append("..").append(to_string(x));
            }
        }
        else {
            printExistingElement(sourceEl, strBuilder);
        }
    }
    strBuilder->append(connector);
    if (!printElement(targetEl, strBuilder)) {
        int x = getElementIdByAddr(targetEl);
        if (x!=0) {
            if (isEdge(targetEl)) {
                strBuilder->append("@edge_alias_").append(to_string(x));
                aliasVector.emplace_back(targetEl, x);
            }
            else {
                strBuilder->append("..").append(to_string(x));
            }
        }
        else {
            printExistingElement(targetEl, strBuilder);
        }
    }
    strBuilder->append(")");
    //todo use method to get uniqID
    dumpElement = new DumpElement(element, uniqId);
    edgeVector.push_back(*dumpElement);
    uniqId++;
}

string prepareElementToPrint(string* strBuffer)
{
  string result = strBuffer->substr(1, strBuffer->size() - 2);
  return result.append(";;\n");
}

string resolveConnector(ScType *type) {
  if (*type == ScType::EdgeDCommonConst) return "=>";
  if (*type == ScType::EdgeDCommonVar) return "_=>";
  if (*type == ScType::EdgeUCommonConst) return "<=>";
  if (*type == ScType::EdgeUCommonVar) return "_<=>";
  if (*type == ScType::EdgeAccess) return "..>";
  if (*type == ScType::EdgeAccessConstPosPerm) return "->";
  if (*type == ScType::EdgeAccessVarPosPerm) return "_->";
  if (*type == ScType::EdgeAccessConstNegPerm) return "-|>";
  if (*type == ScType::EdgeAccessVarNegPerm) return "_-|>";
  if (*type == ScType::EdgeAccessConstFuzPerm) return "-/>";
  if (*type == ScType::EdgeAccessVarFuzPerm) return "_-/>";
  if (*type == ScType::EdgeAccessConstPosTemp) return "~>";
  if (*type == ScType::EdgeAccessVarPosTemp) return "_~>";
  if (*type == ScType::EdgeAccessConstNegTemp) return "~|>";
  if (*type == ScType::EdgeAccessVarNegTemp) return "_~|>";
  if (*type == ScType::EdgeAccessConstFuzTemp) return "~/>";
  if (*type == ScType::EdgeAccessVarFuzTemp) return "_~/>";
  return "";
}

bool printElement(ScAddr element, string* strBuilder)
{
    if (isAddrExist(element))
        return false;
    else
        return printNewElement(element, strBuilder);
}

bool printNewElement(ScAddr element, string* strBuilder)
{
    bool isPrinted = false;
    DumpElement *dumpElement = nullptr;
    ScType type = m_ctx->GetElementType(element);
    if (type.IsNode()) {
        try {
            string systemIdtf = m_ctx -> HelperGetSystemIdtf(element);
            int elementId;
            if (!systemIdtf.empty()) {
                elementId = DumpElement::DEFAULT_ID_IF_ELEMENT_HAS_SYSTEM_ID;
            }
            else
            {
                elementId = uniqId;
                //todo add method for generating id (based on uniqId)
                systemIdtf = ".." + to_string(uniqId);
                uniqId++;
            }
            strBuilder->append(systemIdtf);
            dumpElement = new DumpElement(element, elementId);
            nodeVector.push_back(*dumpElement);
            isPrinted = true;
            if ((type == ScType::NodeConstStruct) | (type == ScType::NodeVarStruct)) nodeVector.at(nodeVector.size()-1).addType("sc_node_struct");
            if ((type == ScType::NodeConstTuple) | (type == ScType::NodeVarTuple)) nodeVector.at(nodeVector.size()-1).addType("sc_node_not_binary_tuple");
            if ((type == ScType::NodeConstRole) | (type == ScType::NodeVarRole)) nodeVector.at(nodeVector.size()-1).addType("sc_node_role_relation");
            if ((type == ScType::NodeConstNoRole) | (type == ScType::NodeVarNoRole)) nodeVector.at(nodeVector.size()-1).addType("sc_node_norole_relation");
            if ((type == ScType::NodeConstClass) | (type == ScType::NodeVarClass)) nodeVector.at(nodeVector.size()-1).addType("sc_node_not_relation");
            if ((type == ScType::NodeConstAbstract) | (type == ScType::NodeVarAbstract)) nodeVector.at(nodeVector.size()-1).addType("sc_node_abstract");
            if ((type == ScType::NodeConstMaterial) | (type == ScType::NodeVarMaterial)) nodeVector.at(nodeVector.size()-1).addType("sc_node_material");
            return isPrinted;
        }
        catch (...) {
            strBuilder->append("failed to process node element");
        }
    }
    if (type.IsLink()) {
        strBuilder->append("@link_alias_").append(to_string(uniqId));
        string newLink;
        newLink.append("@link_alias_").append(to_string(uniqId)).append(" = [").append(getElementContent(element)).append("]");
        linkVector.push_back(newLink);
        dumpElement = new DumpElement(element, uniqId);
        nodeVector.push_back(*dumpElement);
        uniqId++;
        isPrinted = true;
        return isPrinted;
    }
    string connector = resolveConnector(&type);
    if (!connector.empty()) {
        printEdge(element, connector, dumpElement, strBuilder);
        isPrinted = true;
        return isPrinted;
    }
    //THIS BLOCK IS FOR UNKNOWN TYPE
    try {
        string idtf = m_ctx -> HelperGetSystemIdtf(element);
        if (!idtf.empty()) {
            strBuilder->append(idtf);
            dumpElement = new DumpElement(element, 0);
            nodeVector.push_back(*dumpElement);
            isPrinted = true;
        }
        else
        {
            dumpElement = new DumpElement(element, uniqId);
            nodeVector.push_back(*dumpElement);
            strBuilder->append("..").append(to_string(uniqId));
            uniqId++;
            isPrinted = true;
        }
    }
    catch (...) {
        strBuilder->append("fail");
    }
    return isPrinted;
}

void printExistingElement(ScAddr element, string *strBuilder) {
    ScType type = m_ctx->GetElementType(element);
    if (type.IsNode()) {
        try {
            string idtf = m_ctx -> HelperGetSystemIdtf(element);
            if (!idtf.empty()) {
                strBuilder->append(idtf);
            } else {
                strBuilder->append("..").append(to_string(getElementIdByAddr(element)));
            }
        }
        catch (...) {
            strBuilder->append("failed to print existing node");
        }
        return;
    }
    if (type.IsLink()) {
        strBuilder->append("@link_alias_").append(to_string(getElementIdByAddr(element)));
        return;
    }
    if (type.IsEdge()) {
        strBuilder->append("@edge_alias_").append(to_string(getElementIdByAddr(element)));
        aliasVector.emplace_back(element, getElementIdByAddr(element));
        return;
    }
    //THIS BLOCK IS FOR UNKNOWN TYPE
    try {
        string idtf = m_ctx -> HelperGetSystemIdtf(element);
        if (!idtf.empty()) {
            strBuilder->append(idtf);
        } else {
            strBuilder->append("..").append(to_string(getElementIdByAddr(element)));
        }
    }
    catch (...) {
        strBuilder->append("failed to print existing element with unknown type");
    }
}

bool isAddrExist(ScAddr addr) {
    bool isExist = false;
    for (auto & node : nodeVector) {
        if (addr == node.getAddr()) {
            isExist = true;
            return isExist;
        }
    }
    for (auto & node : edgeVector) {
        if (addr == node.getAddr()) {
            isExist = true;
            return isExist;
        }
    }
    return isExist;
}

int getElementIdByAddr(ScAddr addr) {
  for (auto & node : nodeVector) {
    if (addr == node.getAddr()) {
      return node.getId();
    }
  }
  for (auto & edge : edgeVector) {
    if (addr == edge.getAddr()) {
      return edge.getId();
    }
  }
  SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Element was marked as resolved, but not exist.");
}

bool isEdge(ScAddr addr) {
    for (auto & edge : edgeVector) {
        if (addr == edge.getAddr()) {
            return true;
        }
    }
    return false;
}

string processAlias(string line) {
    for (auto & alias : aliasVector) {
        size_t found = 0;
        while ((found = line.find(alias.getEdge())) != string::npos) {
            string x = "@edge_alias_";
            line.replace(found, alias.getEdge().length(), x.append(to_string(alias.getId())));
        }
    }
    return line;
}

int main()
{
  //todo make loading the same as for builder
    sc_memory_params params;
    sc_memory_params_clear(&params);
    params.repo_path = "/home/alexander/work/0.6.0Release/ostis-web-platform/kb.bin";
    params.config_file = "/home/alexander/work/0.6.0Release/ostis-web-platform/config/sc-web.ini";
    params.ext_path = "/home/alexander/work/0.6.0Release/ostis-web-platform/sc-machine/bin/extensions";
    params.clear = SC_FALSE;

    ScMemory::Initialize(params);
    m_ctx.reset(new ScMemoryContext(sc_access_lvl_make_max, "sc_dumper"));

    uniqId = 1;
    run_dump();
    m_ctx.reset();
    ScMemory::Shutdown(true);

    return 0;
}