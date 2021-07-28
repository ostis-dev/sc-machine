#include <benchmark/benchmark.h>

#include "memory_create_edge.hpp"
#include "memory_create_node.hpp"
#include "memory_create_link.hpp"
#include "memory_delete_elements.hpp"

#include "sc_code_base_vs_extend.hpp"

#include "template_search_complex.hpp"
#include "template_search_smoke.hpp"

#include <atomic>

template <class BMType>
void BM_MemoryThreaded(benchmark::State & state)
{
  static std::atomic_int gCtxNum = { 0 };
  BMType test;
  if (state.thread_index == 0)
    test.Initialize();

  uint32_t iterations = 0;
  for (auto _ : state)
  {
    state.PauseTiming();
    if (!test.HasContext())
    {
      test.InitContext();
      gCtxNum.fetch_add(1);
    }
    state.ResumeTiming();

    test.Run();
    ++iterations;
  }
  state.counters["rate"] = benchmark::Counter(iterations, benchmark::Counter::kIsRate);
  if (state.thread_index == 0)
  {
    while (gCtxNum.load() != 0)
    {
    }

    test.Shutdown();
  }
  else
  {
    test.DestroyContext();
    gCtxNum.fetch_add(-1);
  }
}

int constexpr kNodeIters = 10000000;

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateNode)
    ->Threads(1)
    ->Iterations(kNodeIters)
    ->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateNode)
  ->Threads(2)
  ->Iterations(kNodeIters / 2)
  ->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateNode)
  ->Threads(4)
  ->Iterations(kNodeIters / 4)
  ->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateNode)
  ->Threads(8)
  ->Iterations(kNodeIters / 8)
  ->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateNode)
  ->Threads(16)
  ->Iterations(kNodeIters / 16)
  ->Unit(benchmark::TimeUnit::kMicrosecond);

int constexpr kLinkIters = 2500;

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateLink)
    ->Threads(1)
    ->Iterations(kLinkIters)
    ->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateLink)
  ->Threads(2)
  ->Iterations(kLinkIters / 2)
  ->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateLink)
  ->Threads(4)
  ->Iterations(kLinkIters / 4)
  ->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateLink)
  ->Threads(8)
  ->Iterations(kLinkIters / 8)
  ->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateLink)
  ->Threads(16)
  ->Iterations(kLinkIters / 64)
  ->Unit(benchmark::TimeUnit::kMicrosecond);

// ------------------------------------
template <class BMType>
void BM_Memory(benchmark::State & state)
{
  BMType test;
  test.Initialize();
  uint32_t iterations = 0;
  for (auto _ : state)
  {
    test.Run();
    ++iterations;
  }
  state.counters["rate"] = benchmark::Counter(iterations, benchmark::Counter::kIsRate);
  test.Shutdown();
}

BENCHMARK_TEMPLATE(BM_Memory, TestCreateNode)
  ->Unit(benchmark::TimeUnit::kMicrosecond);

BENCHMARK_TEMPLATE(BM_Memory, TestCreateLink)
  ->Unit(benchmark::TimeUnit::kMicrosecond);

template <class BMType>
void BM_MemoryRanged(benchmark::State & state)
{
  BMType test;
  test.Initialize(state.range(0));
  uint32_t iterations = 0;
  for (auto _ : state)
  {
    test.Run();
    ++iterations;
  }
  state.counters["rate"] = benchmark::Counter(iterations, benchmark::Counter::kIsRate);
  test.Shutdown();
}

BENCHMARK_TEMPLATE(BM_MemoryRanged, TestCreateEdge)
  ->Unit(benchmark::TimeUnit::kMicrosecond)
  ->Arg(1000)
  ->Iterations(5000000);

BENCHMARK_TEMPLATE(BM_MemoryRanged, TestDeleteElements)
  ->Unit(benchmark::TimeUnit::kMicrosecond)
  ->Arg(10)->Arg(100)->Arg(1000)
  ->Iterations(5000);

// ------------------------------------
template <class BMType>
void BM_Template(benchmark::State & state)
{
  BMType test;
  test.Initialize(state.range(0));
  uint32_t iterations = 0;
  for (auto _ : state)
  {
    if (!test.Run())
      state.SkipWithError("Empty result");

     ++iterations;
  }
  state.counters["rate"] = benchmark::Counter(iterations, benchmark::Counter::kIsRate);
  test.Shutdown();
}

// Template search
BENCHMARK_TEMPLATE(BM_Template, TestTemplateSearchSmoke)
  ->Unit(benchmark::TimeUnit::kMicrosecond)
  ->Arg(5)->Iterations(500000);

BENCHMARK_TEMPLATE(BM_Template, TestTemplateSearchSmoke)
  ->Unit(benchmark::TimeUnit::kMicrosecond)
  ->Arg(50)->Iterations(50000);

BENCHMARK_TEMPLATE(BM_Template, TestTemplateSearchSmoke)
  ->Unit(benchmark::TimeUnit::kMicrosecond)
  ->Arg(500)->Iterations(5000);

BENCHMARK_TEMPLATE(BM_Template, TestTemplateSearchComplex)
  ->Unit(benchmark::TimeUnit::kMicrosecond)
  ->Arg(5)->Iterations(1000);

BENCHMARK_TEMPLATE(BM_Template, TestTemplateSearchComplex)
  ->Unit(benchmark::TimeUnit::kMicrosecond)
  ->Arg(50)->Iterations(100);

BENCHMARK_TEMPLATE(BM_Template, TestTemplateSearchComplex)
  ->Unit(benchmark::TimeUnit::kMicrosecond)
  ->Arg(500)
  ->Iterations(10);

// SC-code base vs extended
BENCHMARK_TEMPLATE(BM_Template, TestScCodeBase)
  ->Unit(benchmark::TimeUnit::kMicrosecond)
  ->Arg(1000)
  ->Iterations(500);

BENCHMARK_TEMPLATE(BM_Template, TestScCodeBase)
  ->Unit(benchmark::TimeUnit::kMicrosecond)
  ->Arg(10000)->Iterations(50);

BENCHMARK_TEMPLATE(BM_Template, TestScCodeBase)
  ->Unit(benchmark::TimeUnit::kMicrosecond)
  ->Arg(100000)->Iterations(5);

BENCHMARK_TEMPLATE(BM_Template, TestScCodeExtended)
  ->Unit(benchmark::TimeUnit::kMicrosecond)
  ->Arg(1000)->Iterations(1000);

BENCHMARK_TEMPLATE(BM_Template, TestScCodeExtended)
  ->Unit(benchmark::TimeUnit::kMicrosecond)
  ->Arg(10000)->Iterations(100);

BENCHMARK_TEMPLATE(BM_Template, TestScCodeExtended)
  ->Unit(benchmark::TimeUnit::kMicrosecond)
  ->Arg(100000)->Iterations(10);


BENCHMARK_MAIN();
