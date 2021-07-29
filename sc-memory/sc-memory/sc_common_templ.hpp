#pragma once

#include "sc_link.hpp"
#include "sc_memory.hpp"
#include "sc_template.hpp"
#include "sc_template_builder.hpp"
#include "sc_template_generate.hpp"
#include "sc_template_search.hpp"

namespace sc
{
/* Create construction:
 * elAddr <= relAddr: {};;
 * If construction exist, then returns exist tuple addr; otherwise create new one.
 * Returns ScAddr of tuple
 */
_SC_EXTERN ScAddr ResolveRelationTuple(ScMemoryContext & ctx, ScAddr const & elAddr, ScAddr const & relAddr);

/* Generates construction:
 * elAddr => relAddr: [];;
 * If link with relation exist, then changes it value; otherwise create new one.
 * Returns ScAddr of sc-link
 */
template <typename ValueType>
ScAddr SetRelationValue(ScMemoryContext & ctx, ScAddr const & elAddr, ScAddr const & relAddr, ValueType const & value)
{
  ScTemplatePtr templ =
      ScTemplateBuilder()
          .TripleWithRelation(
              elAddr, ScType::EdgeDCommonVar, ScType::LinkVar >> "_link", ScType::EdgeAccessVarPosPerm, relAddr)
          .Make();

  ScAddr linkAddr;
  ScTemplateSearch search(ctx, *templ);
  auto const it = search.begin();
  if (it != search.end())
    linkAddr = it["_link"];

  if (!linkAddr.IsValid())
  {
    ScTemplateGenerate gen(ctx, *templ);
    auto const result = gen.Do();
    if (result)
      linkAddr = (*result)["_link"];

    if (!linkAddr.IsValid())
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Can't create value relation");
    }
  }

  ScLink link(ctx, linkAddr);
  link.Set(value);

  return linkAddr;
}

}  // namespace sc
