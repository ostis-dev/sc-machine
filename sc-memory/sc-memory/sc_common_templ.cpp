#include "sc_common_templ.hpp"

namespace sc
{

ScAddr ResolveRelationTuple(ScMemoryContext & ctx, ScAddr const & elAddr, ScAddr const & relAddr)
{
  ScTemplatePtr templ = ScTemplateBuilder()
    .TripleWithRelation(
      ScType::NodeVarTuple >> "_tuple",
      ScType::EdgeDCommonVar,
      elAddr,
      ScType::EdgeAccessVarPosPerm,
      relAddr)
    .Make();

  ScTemplateSearch search(ctx, *templ);
  ScTemplateSearch::Iterator it = search.begin();
  if (it != search.end())
  {
    ScAddr const result = it["_tuple"];
    SC_ASSERT(++it == search.end(), ("Invalid state of knowledge base"));
    return result;
  }

  ScTemplateGenerate generator(ctx, *templ);
  ScTemplateGenerate::Result const generated = generator.Do();
  if (!generated)
  {
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState,
                       "Can't create tuple");
  }

  return (*generated)["_tuple"];
}

} // namespace sc
