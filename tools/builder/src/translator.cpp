/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "translator.hpp"

#include "keynodes.hpp"
#include "sc-memory/sc_link.hpp"
#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_templates.hpp"

Translator::Translator(ScMemoryContext & ctx)
  : m_ctx(ctx)
{
}

bool Translator::Translate(Params const & params)
{
  return TranslateImpl(params);
}

void Translator::GenerateFormatInfo(ScAddr const & addr, std::string const & ext)
{
  std::string const fmtStr = "format_" + ext;

  ScAddr const formatAddr = m_ctx.HelperResolveSystemIdtf(fmtStr, ScType::NodeConstClass);

  ScTemplatePtr templ =
      ScTemplateBuilder()
          .TripleWithRelation(
              addr, ScType::EdgeDCommonVar, formatAddr, ScType::EdgeAccessVarPosPerm, Keynodes::kNrelFormat())
          .Make();

  ScTemplateGenerate generator(m_ctx, *templ);
  ScTemplateGenerate::Result result = generator.Do();
  if (!result)
  {
    SC_THROW_EXCEPTION(
        utils::ExceptionInvalidState, "Error to generate format for sc-link: " << generator.GetErrorMessage());
  }
}

void Translator::GetFileContent(std::string const & fileName, std::string & outContent)
{
  std::ifstream ifs(fileName);
  if (!ifs.is_open())
  {
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Can't open file " << fileName);
  }

  outContent.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
  ifs.close();
}

void Translator::Clean(ScMemoryContext & ctx)
{
  // remove global identifers
  ScAddr const nrelSCsGlobalIdtf = ctx.HelperResolveSystemIdtf("nrel_scs_global_idtf");
  if (!nrelSCsGlobalIdtf.IsValid())
  {
    ScConsole::PrintLine() << ScConsole::Color::Red << "Can't resolve keynode 'nrel_scs_global_idtf'";
    return;
  }

  ScTemplatePtr templ = ScTemplateBuilder()
                            .TripleWithRelation(
                                ScType::Unknown,
                                ScType::EdgeDCommonVar,
                                ScType::Link >> "_link",
                                ScType::EdgeAccessVarPosPerm,
                                nrelSCsGlobalIdtf)
                            .Make();

  ScTemplateSearch search(ctx, *templ);
  for (ScTemplateNamedStruct const & res : search)
    ctx.EraseElement(res["_link"]);
}
