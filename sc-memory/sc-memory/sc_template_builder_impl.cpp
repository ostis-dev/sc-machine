#include "sc_template_builder_impl.hpp"

ScTemplateBuilderImpl::ScTemplateBuilderImpl()
{
  m_templ.Reserve(4);
}

ScTemplateBuilderImpl & ScTemplateBuilderImpl::Triple(ScTemplateArg const & param1,
                                                      ScTemplateArg const & param2,
                                                      ScTemplateArg const & param3)
{
  SC_ASSERT(!m_isDone, ("Call method after Done() is not allowed"));
  m_templ.AddTriple(param1, param2, param3);
  return *this;
}

ScTemplateBuilderImpl & ScTemplateBuilderImpl::TripleWithRelation(ScTemplateArg const & param1,
                                                                  ScTemplateArg const & param2,
                                                                  ScTemplateArg const & param3,
                                                                  ScTemplateArg const & param4,
                                                                  ScTemplateArg const & param5)
{
  SC_ASSERT(!m_isDone, ("Call method after Done() is not allowed"));

  ScTemplateArg edgeCommonItem = param2;

  // check if relation edge has replacement
  if (edgeCommonItem.m_replacementName.empty())
    edgeCommonItem.m_replacementName = "_generated_repl_" + std::to_string(m_templ.TriplesNum());

  Triple(param1, edgeCommonItem, param3);
  Triple(param5, param4, edgeCommonItem.m_replacementName.c_str());

  return *this;
}

ScTemplatePtr ScTemplateBuilderImpl::Make()
{
  SC_ASSERT(!m_isDone, ("Twice call of Make() method"));
  m_isDone = true;
  return ScTemplatePtrMake(std::move(m_templ));
}
