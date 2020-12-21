#pragma once

#include "template_test.hpp"

class TestScCodeExtended : public TestTemplate
{
public:

  void Setup(size_t constrCount) override
  {
    ScAddr const node = m_ctx->CreateNode(ScType::NodeConstClass);
    ScAddr const attr = m_ctx->CreateNode(ScType::NodeConstRole);
    for (size_t i = 0; i < constrCount; ++i)
    {
      ScAddr const trg = m_ctx->CreateNode(ScType::NodeConstAbstract);
      ScAddr const edge = m_ctx->CreateEdge(ScType::EdgeDCommonConst, node, trg);
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, attr, edge);
    }

    m_templ = ScTemplateBuilder()
        .TripleWithRelation(
          node,
          ScType::EdgeDCommonVar,
          ScType::NodeVarAbstract,
          ScType::EdgeAccessVarPosPerm,
          attr)
        .Make();
  }
};


class TestScCodeBase : public TestTemplate
{
public:

  void Setup(size_t constrCount) override
  {
    ScAddr const kAbstract = m_ctx->CreateNode(ScType::NodeConst);
    ScAddr const kClass = m_ctx->CreateNode(ScType::NodeConst);
    ScAddr const kRole = m_ctx->CreateNode(ScType::NodeConst);
    ScAddr const kConst = m_ctx->CreateNode(ScType::NodeConst);

    ScAddr const node = m_ctx->CreateNode(ScType::NodeConst);
    m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, kClass, node);
    m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, kConst, node);

    ScAddr const attr = m_ctx->CreateNode(ScType::NodeConst);
    m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, kRole, attr);
    m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, kConst, attr);

    for (size_t i = 0; i < constrCount; ++i)
    {
      ScAddr const trg = m_ctx->CreateNode(ScType::NodeConstAbstract);
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, kAbstract, trg);
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, kConst, trg);

      ScAddr const edge = m_ctx->CreateEdge(ScType::EdgeDCommonConst, node, trg);
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, kConst, edge);

      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, attr, edge);
    }

    m_templ = ScTemplateBuilder()
        .TripleWithRelation(
          node >> "_node",
          ScType::EdgeDCommonVar >> "_edge",
          ScType::NodeVarAbstract >> "_trg",
          ScType::EdgeAccessVarPosPerm,
          attr >> "_attr")
        .Triple(
          kConst,
          ScType::EdgeAccessVarPosPerm,
          "_node")
        .Triple(
          kClass,
          ScType::EdgeAccessVarPosPerm,
          "_node")
        .Triple(
          kConst,
          ScType::EdgeAccessVarPosPerm,
          "_attr")
        .Triple(
          kRole,
          ScType::EdgeAccessVarPosPerm,
          "_attr")
        .Triple(
          kAbstract,
          ScType::EdgeAccessVarPosPerm,
          "_trg")
        .Triple(
          kConst,
          ScType::EdgeAccessVarPosPerm,
          "_trg")
        .Triple(
          kConst,
          ScType::EdgeAccessVarPosPerm,
          "_edge")
        .Make();
  }
};
