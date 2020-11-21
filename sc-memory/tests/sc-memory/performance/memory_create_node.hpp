#pragma once

#include "memory_test.hpp"

class TestCreateNode : public TestMemory
{
public:
  void Run()
  {
    m_ctx->CreateNode(ScType::NodeConstAbstract);
  }
};
