#pragma once

#include "sc_template.hpp"

#include <memory>

class ScTemplateBuilder final
{
public:
  ScTemplateBuilder();
  ~ScTemplateBuilder();

  ScTemplateBuilder & Triple(ScTemplateArg const & param1,
                             ScTemplateArg const & param2,
                             ScTemplateArg const & param3);

  ScTemplateBuilder & TripleWithRelation(ScTemplateArg const & param1,
                                         ScTemplateArg const & param2,
                                         ScTemplateArg const & param3,
                                         ScTemplateArg const & param4,
                                         ScTemplateArg const & param5);

  ScTemplatePtr Make();

private:
  std::unique_ptr<class ScTemplateBuilderImpl> m_impl;
};
