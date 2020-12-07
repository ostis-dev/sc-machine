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

int constexpr kNodeIters = 1000000;

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

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateNode)
  ->Threads(32)
  ->Iterations(kNodeIters / 32)
  ->Unit(benchmark::TimeUnit::kMicrosecond);

int constexpr kLinkIters = 7000;

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

BENCHMARK_TEMPLATE(BM_MemoryThreaded, TestCreateLink)
  ->Threads(32)
  ->Iterations(kLinkIters / 128)
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

BENCHMARK_TEMPLATE(BM_Template, TestTemplateSearchSmoke)
  ->Unit(benchmark::TimeUnit::kMicrosecond)
  ->Arg(5)->Arg(50)->Arg(500);

BENCHMARK_TEMPLATE(BM_Template, TestTemplateSearchComplex)
  ->Unit(benchmark::TimeUnit::kMicrosecond)
  ->Arg(5)->Arg(50)->Arg(500);

// SC-code base vs extended
BENCHMARK_TEMPLATE(BM_Template, TestScCodeBase)
  ->Unit(benchmark::TimeUnit::kMicrosecond)
  ->Arg(1000)->Arg(10000)->Arg(100000);

BENCHMARK_TEMPLATE(BM_Template, TestScCodeExtended)
  ->Unit(benchmark::TimeUnit::kMicrosecond)
  ->Arg(1000)->Arg(10000)->Arg(100000);


BENCHMARK_MAIN();
