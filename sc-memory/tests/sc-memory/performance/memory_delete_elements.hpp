#pragma once

#include "memory_test.hpp"

class TestDeleteElements : public TestMemory
{
public:
  void Run()
  {
    m_ctx->EraseElement(m_addr);
  }

  void Setup(size_t objectsNum) override
  {
    m_addr = m_ctx->CreateNode(ScType::NodeConst);

    ScAddr const trgAttr = m_ctx->CreateNode(ScType::NodeConstRole);
    ScAddr const srcAttr = m_ctx->CreateNode(ScType::NodeConstRole);

    for (size_t i = 0; i < objectsNum; ++i)
    {
      ScAddr const trg = m_ctx->CreateNode(ScType::NodeClass);
      ScAddr const edgeTrg = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, m_addr, trg);
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, trgAttr, edgeTrg);

      ScAddr const src = m_ctx->CreateNode(ScType::NodeClass);
      ScAddr const edgeSrc = m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, m_addr, src);
      m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm, srcAttr, edgeSrc);
    }
  }

private:
  ScAddr m_addr;
};
