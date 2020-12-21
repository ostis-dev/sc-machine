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

ScTemplateBuilder & ScTemplateBuilder::Triple(ScTemplateArg const & param1,
                                              ScTemplateArg const & param2,
                                              ScTemplateArg const & param3)
{
  m_impl->Triple(param1, param2, param3);
  return *this;
}

ScTemplateBuilder & ScTemplateBuilder::TripleWithRelation(ScTemplateArg const & param1,
                                                          ScTemplateArg const & param2,
                                                          ScTemplateArg const & param3,
                                                          ScTemplateArg const & param4,
                                                          ScTemplateArg const & param5)
{
  m_impl->TripleWithRelation(param1, param2, param3, param4, param5);
  return *this;
}

ScTemplatePtr ScTemplateBuilder::Make()
{
  return m_impl->Make();
}


