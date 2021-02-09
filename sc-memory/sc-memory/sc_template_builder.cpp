#include "sc_template_builder.hpp"
#include "sc_template_builder_impl.hpp"

ScTemplateBuilder::ScTemplateBuilder()
{
  m_impl = std::make_unique<ScTemplateBuilderImpl>();
}

ScTemplateBuilder::~ScTemplateBuilder()
{
  // need to use forward declaration for std::unique_ptr
}

ScTemplateBuilder & ScTemplateBuilder::Triple(ScTemplateArg const & source,
                                              ScTemplateArg const & edge,
                                              ScTemplateArg const & target)
{
  m_impl->Triple(source, edge, target);
  return *this;
}

ScTemplateBuilder & ScTemplateBuilder::TripleWithRelation(ScTemplateArg const & source,
                                                          ScTemplateArg const & edge,
                                                          ScTemplateArg const & target,
                                                          ScTemplateArg const & attrEdge,
                                                          ScTemplateArg const & attr)
{
  m_impl->TripleWithRelation(source, edge, target, attrEdge, attr);
  return *this;
}

ScTemplatePtr ScTemplateBuilder::Make()
{
  return m_impl->Make();
}


