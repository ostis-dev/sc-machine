#include "sc_template_scs_builder.hpp"

#include "sc_memory.hpp"
#include "sc_template_builder_impl.hpp"

#include "scs/scs_parser.hpp"
#include "utils/sc_keynode_cache.hpp"

ScTemplateSCsBuilder::ScTemplateSCsBuilder(ScMemoryContext & ctx)
  : m_ctx(ctx)
{
  m_builder = std::make_unique<ScTemplateBuilderImpl>();
}

ScTemplateSCsBuilder::~ScTemplateSCsBuilder()
{
}

ScTemplatePtr ScTemplateSCsBuilder::Make(std::string const & scsText)
{
  SC_ASSERT(!m_isDone, ("Twice call of Make() method"));
  m_isDone = true;

  scs::Parser parser;
  if (!parser.Parse(scsText))
  {
    m_errorMessage = parser.GetParseError();
    return {};
  }

  utils::ScKeynodeCache keynodes(m_ctx);

  auto const MakeTemplItem = [&keynodes, this](scs::ParsedElement const & el, ScTemplateArg & outValue) -> bool
  {
    std::string const & idtf = el.GetIdtf();
    bool const isUnnamed = scs::TypeResolver::IsUnnamed(idtf);
    bool const isPassed = m_builder->HasReplacement(idtf);

    if (!isUnnamed && isPassed)
    {
      outValue.SetReplacement(idtf.c_str());
    }
    else
    {
      char const * alias = isUnnamed ? nullptr : idtf.c_str();
      ScAddr const addr = keynodes.GetKeynode(idtf);
      if (addr.IsValid())
      {
        outValue.SetAddr(addr, alias);
      }
      else
      {
        if (el.GetType().IsVar())
        {
          outValue.SetType(el.GetType(), alias);
        }
        else
        {
          m_errorMessage = "Can't find element with system identifier " + idtf;
          return false;
        }
      }
    }

    return true;
  };

  for (scs::ParsedTriple const & t : parser.GetParsedTriples())
  {
    scs::ParsedElement const & src = parser.GetParsedElement(t.m_source);
    scs::ParsedElement const & edge = parser.GetParsedElement(t.m_edge);
    scs::ParsedElement const & trg = parser.GetParsedElement(t.m_target);

    ScTemplateArg srcItem, edgeItem, trgItem;

    if (!MakeTemplItem(src, srcItem) || !MakeTemplItem(edge, edgeItem) || !MakeTemplItem(trg, trgItem))
    {
      return {};
    }

    m_builder->Triple(srcItem, edgeItem, trgItem);
  }

  return m_builder->Make();
}

std::string const & ScTemplateSCsBuilder::GetErrorMessage() const
{
  return m_errorMessage;
}
