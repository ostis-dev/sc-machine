#pragma once

#include <gtest/gtest.h>

#include "sc_test.hpp"

#include "sc-memory/sc_memory.hpp"
#include "sc-memory/sc_scs_helper.hpp"
#include "sc-memory/sc_templates.hpp"

#include <algorithm>

using ScTemplateTest = ScMemoryTest;

namespace
{
struct TestTemplParams
{
  explicit TestTemplParams(ScMemoryContext & ctx)
    : m_ctx(ctx)
  {
  }

  bool operator()(ScTemplateArg param1, ScTemplateArg param2, ScTemplateArg param3)
  {
    bool catched = false;
    try
    {
      ScTemplatePtr testTempl = ScTemplateBuilder().Triple(param1, param2, param3).Make();

      ScTemplateGenerate generator(m_ctx, *testTempl);
      EXPECT_TRUE(generator.Do());
    }
    catch (utils::ExceptionInvalidParams & e)
    {
      (void)e;
      catched = true;
    }

    return !catched;
  }

private:
  ScMemoryContext & m_ctx;
};

inline bool HasAddr(ScAddrVector const & v, ScAddr const & addr)
{
  return std::find(v.begin(), v.end(), addr) != v.end();
}

inline ScAddr ResolveKeynode(ScMemoryContext & ctx, std::string const & idtf)
{
  ScAddr const addr = ctx.HelperResolveSystemIdtf(idtf, ScType::NodeConst);
  EXPECT_TRUE(addr.IsValid());
  return addr;
}

class DummyFileInterface : public SCsFileInterface
{
protected:
  ScStreamPtr GetFileContent(std::string const & fileURL) override
  {
    return {};
  }
};

}  // namespace
