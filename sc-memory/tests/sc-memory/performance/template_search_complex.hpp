#pragma once

#include "template_test.hpp"

class TestTemplateSearchComplex : public TestTemplate
{
public:
  void Setup(size_t constrCount) override
  {
    ScAddr const node = m_ctx->CreateNode(ScType::NodeConstStruct);
    m_templ = ScTemplateBuilder()
        .TripleWithRelation(
            node,
            ScType::EdgeDCommonVar,
            ScType::NodeVarTuple >> "_tuple",
            ScType::EdgeAccessVarPosPerm,
            ScType::NodeVarNoRole)
        .TripleWithRelation(
            ScType::NodeVarClass,
            ScType::EdgeDCommonVar,
            "_tuple",
            ScType::EdgeAccessVarPosPerm,
            ScType::NodeVarNoRole)
        .Triple(
            ScType::NodeVarClass,
            ScType::EdgeAccessVarPosPerm,
            node)
        .Make();

    for (size_t i = 0; i < constrCount; ++i)
    {
      ScTemplateGenerate::Result result = ScTemplateGenerate(*m_ctx, *m_templ).Do();
      assert(result);
    }
  }
};
