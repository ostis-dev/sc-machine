#pragma once

#include "memory_test.hpp"

#include "sc-memory/sc_link.hpp"

class TestCreateLink : public TestMemory
{
public:
  void Run()
  {
    ScAddr const addr = m_ctx->CreateLink();

    ScLink link(*m_ctx, addr);
    link.Set(addr.Hash());
  }
};
