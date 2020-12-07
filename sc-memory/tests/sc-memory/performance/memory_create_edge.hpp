#pragma once

#include "memory_test.hpp"

class TestCreateEdge : public TestMemory
{
public:
  void Run()
  {
    m_ctx->CreateEdge(ScType::EdgeAccessConstPosPerm,
                      m_nodes[rand() % m_nodes.size()],
                      m_nodes[rand() % m_nodes.size()]);
  }

  void Setup(size_t elementsNum) override
  {
    m_nodes.reserve(elementsNum);
    for (size_t i = 0; i < elementsNum; ++i)
      m_nodes.push_back(m_ctx->CreateNode(ScType::NodeConst));
  }

private:
  std::vector<ScAddr> m_nodes;
};
