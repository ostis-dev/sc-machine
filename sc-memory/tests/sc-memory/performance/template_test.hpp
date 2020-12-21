#pragma once

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_templates.hpp>

#include <memory>

class TestTemplate
{
public:
  void Initialize(size_t constrCount)
  {
    sc_memory_params params;
    sc_memory_params_clear(&params);
    params.clear = SC_TRUE;
    params.repo_path = "test_repo";

    ScMemory::LogMute();
    ScMemory::Initialize(params);

    m_ctx = std::make_unique<ScMemoryContext>(sc_access_lvl_make_min, "test");

    Setup(constrCount);
  }

  void Shutdown()
  {
    m_ctx.reset();

    ScMemory::Shutdown(false);
  }

  virtual void Setup(size_t constrCount) = 0;
  bool Run()
  {
    ScTemplateSearch searchResults(*m_ctx, *m_templ);
    ScTemplateSearch::Iterator it = searchResults.begin();
    bool const result = (it != searchResults.end());
    while (it != searchResults.end())
      ++it;

    return result;
  }

protected:
  std::unique_ptr<ScMemoryContext> m_ctx;
  ScTemplatePtr m_templ;
};
