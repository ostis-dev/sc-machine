
#include "sc_template_search.hpp"

#include "sc_memory.hpp"

#include <algorithm>
#include <stack>

class ScTemplateSearchImpl
{
public:
  ScTemplateSearchImpl(
      ScTemplate::Data const & templ,
      ScMemoryContext & context,
      ScAddr const & scStruct,
      ScTemplateNamedStruct & result)
    : m_templ(templ)
    , m_context(context)
    , m_struct(scStruct)
    , m_result(result)
  {
  }

  void Reset()
  {
    UpdateSearchCache();

    size_t const triplesNum = m_templ.TriplesNum();
    m_state.didIter.resize(triplesNum);
    m_state.edges.resize(triplesNum);
    m_state.finishIdx = triplesNum - 1;
    m_state.newIteration = true;
    m_state.isFinished = false;

    m_resultAddrs.resize(triplesNum * 3);
    m_repls.clear();
  }

  void UpdateSearchCache()
  {
    if (!m_templ.IsEmpty())
    {
      // update it
      std::vector<size_t> preCache(m_templ.TriplesNum());
      for (size_t i = 0; i < preCache.size(); ++i)
        preCache[i] = i;

      static const size_t kScoreEdge = 5;
      static const size_t kScoreOther = 1;

      /** First of all we need to calculate scores for all triples
       * (more scores - should be search first).
       * Also need to store all replacements that need to be resolved
       */
      std::vector<uint8_t> tripleScores(m_templ.TriplesNum());
      std::unordered_map<std::string, std::vector<size_t>> replDependMap;
      for (size_t i = 0; i < m_templ.TriplesNum(); ++i)
      {
        ScTemplate::Data::Triple const & triple = m_templ.GetTriple(i);
        auto const CalculateScore = [](ScTemplate::Data::Triple const & constr)
        {
          uint8_t score = 0;
          if (constr.edge.IsAddr() && constr.source.IsAssign() && constr.target.IsAssign())
            score += kScoreEdge;
          else if (constr.source.IsAddr() && constr.edge.IsAssign() && constr.target.IsAddr())
            score += kScoreOther * 2;  // should be a sum of (f_a_a and a_a_f)
          else if (constr.source.IsAddr() || constr.target.IsAddr())
            score += kScoreOther;

          return score;
        };

        tripleScores[i] = CalculateScore(triple);
        // doesn't add edges into depend map
        auto const TryAppendRepl = [&](ScTemplateArg const & value, size_t idx)
        {
          SC_ASSERT(idx < 3, ());
          if (!value.IsAddr() && !value.m_replacementName.empty())
            replDependMap[value.m_replacementName].push_back((i << 2) + idx);
        };

        // do not use loop, to make it faster (not all compilators will make a linear code)
        TryAppendRepl(triple.source, 0);
        TryAppendRepl(triple.edge, 1);
        TryAppendRepl(triple.target, 2);
      }

      // sort by scores
      std::sort(
          preCache.begin(),
          preCache.end(),
          [&](size_t a, size_t b)
          {
            return (tripleScores[a] > tripleScores[b]);
          });

      // now we need to append triples, in order, when previous resolve replacement for a next one
      std::vector<size_t> & cache = m_state.processOrder;
      cache.resize(preCache.size());

      std::vector<bool> isTripleCached(cache.size(), false);
      std::unordered_set<std::string> usedReplacements;

      size_t preCacheIdx = 1;
      size_t orderIdx = 1;
      cache[0] = preCache[0];
      isTripleCached[cache[0]] = true;
      while (orderIdx < cache.size())
      {
        size_t const curTripleIdx = cache[orderIdx - 1];
        auto const & triple = m_templ.GetTriple(curTripleIdx);

        // get resolved replacements by the last triple
        std::vector<std::string> resolvedReplacements;
        resolvedReplacements.reserve(3);
        for (auto const & value : {triple.source, triple.edge, triple.target})
        {
          if (!value.m_replacementName.empty())
            resolvedReplacements.emplace_back(value.m_replacementName);
        }

        // collect triples, that depend on resolved replacements and update their scores
        std::unordered_set<size_t> resolvedTriples;
        for (auto const & name : resolvedReplacements)
        {
          if (usedReplacements.find(name) != usedReplacements.end())
            continue;

          auto const it = replDependMap.find(name);
          if (it != replDependMap.end())
            resolvedTriples.insert(it->second.begin(), it->second.end());

          usedReplacements.insert(name);
        }

        // find next non used triple
        while (isTripleCached[preCache[preCacheIdx]])
          preCacheIdx++;

        size_t bestTripleIdx = preCache[preCacheIdx];
        // now update scores of resolved triples and find best one scores
        for (size_t idx : resolvedTriples)
        {
          // unpack it
          size_t const tripleIdx = idx >> 2;

          if (isTripleCached[tripleIdx])
            continue;

          // for a constants see initial calculation
          if (idx == 1)
            tripleScores[tripleIdx] += kScoreEdge;
          else
            tripleScores[tripleIdx] += kScoreOther;

          // check if it better
          if (tripleScores[bestTripleIdx] < tripleScores[tripleIdx])
            bestTripleIdx = tripleIdx;
        }

        // append new best triple
        cache[orderIdx++] = bestTripleIdx;
        isTripleCached[bestTripleIdx] = true;
      }
    }
  }

  ScAddr const & ResolveAddr(ScTemplateArg const & value) const
  {
    switch (value.m_kind)
    {
    case ScTemplateArg::Kind::Addr:
      return value.m_addrValue;

    case ScTemplateArg::Kind::Replace:
    {
      auto const it = m_repls.find(value.m_replacementName);
      if (it != m_repls.end())
      {
        SC_ASSERT(it->second.addr.IsValid(), ());
        return it->second.addr;
      }
    }

    case ScTemplateArg::Kind::Type:
    {
      if (!value.m_replacementName.empty())
      {
        auto const it = m_repls.find(value.m_replacementName);
        if (it != m_repls.end())
        {
          SC_ASSERT(it->second.addr.IsValid(), ());
          return it->second.addr;
        }
      }
      break;
    }

    default:
      break;
    };

    return ScAddr::Empty;
  }

  ScIterator3Ptr CreateIterator(ScTemplate::Data::Triple const & triple)
  {
    ScAddr const sourceAddr = ResolveAddr(triple.source);
    ScAddr const edgeAddr = ResolveAddr(triple.edge);
    ScAddr const targetAddr = ResolveAddr(triple.target);

    auto const PrepareType = [](ScType const & type)
    {
      if (type.HasConstancyFlag())
        return type.UpConstType();

      return type;
    };

    if (sourceAddr.IsValid())
    {
      if (!edgeAddr.IsValid())
      {
        if (targetAddr.IsValid())  // F_A_F
        {
          return m_context.Iterator3(sourceAddr, PrepareType(triple.edge.m_typeValue), targetAddr);
        }
        else  // F_A_A
        {
          return m_context.Iterator3(
              sourceAddr, PrepareType(triple.edge.m_typeValue), PrepareType(triple.target.m_typeValue));
        }
      }
      else
      {
        if (targetAddr.IsValid())  // F_F_F
        {
          return m_context.Iterator3(sourceAddr, edgeAddr, targetAddr);
        }
        else  // F_F_A
        {
          return m_context.Iterator3(sourceAddr, edgeAddr, PrepareType(triple.target.m_typeValue));
        }
      }
    }
    else if (targetAddr.IsValid())
    {
      if (edgeAddr.IsValid())  // A_F_F
      {
        return m_context.Iterator3(PrepareType(triple.source.m_typeValue), edgeAddr, targetAddr);
      }
      else  // A_A_F
      {
        return m_context.Iterator3(
            PrepareType(triple.source.m_typeValue), PrepareType(triple.edge.m_typeValue), targetAddr);
      }
    }
    else if (edgeAddr.IsValid() && !targetAddr.IsValid())  // A_F_A
    {
      return m_context.Iterator3(
          PrepareType(triple.source.m_typeValue), edgeAddr, PrepareType(triple.target.m_typeValue));
    }

    //// unknown iterator type
    // SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Unknown iterator type");

    return ScIterator3Ptr();
  }

  bool CheckInStruct(ScAddr const & addr)
  {
    return m_context.HelperCheckEdge(m_struct, addr, ScType::EdgeAccessConstPosPerm);
  }

  void MakeResult()
  {
    ScTemplateNamedStruct::Builder builder(m_result);
    for (auto const & r : m_repls)
    {
      SC_ASSERT(r.second.addr.IsValid(), ());
      builder.Add(r.first, r.second.addr);
    }

    // iterate all addrs and append missed into result
    for (ScAddr const & addr : m_resultAddrs)
      builder.Add(addr);
  }

  void UnrefReplacement(ScTemplateArg const & item)
  {
    if (!item.m_replacementName.empty())
    {
      auto it = m_repls.find(item.m_replacementName);
      SC_ASSERT(it != m_repls.end(), ());

      if (it->second.refs > 1)
        --it->second.refs;
      else
        m_repls.erase(it);
    }
  }

  void RefReplacement(ScTemplateArg const & item, ScAddr const & addr)
  {
    if (!item.m_replacementName.empty())
    {
      auto it = m_repls.find(item.m_replacementName);
      if (it == m_repls.end())
      {
        m_repls[item.m_replacementName] = {addr, 1};
      }
      else
      {
        ++it->second.refs;
      }
    }
  }

  bool DoIter()
  {
    if (m_state.isFinished)
      return false;  // do nothing

    bool found = false;
    do
    {
      size_t const orderIndex = m_state.newIteration ? m_state.iterators.size() : m_state.iterators.size() - 1;
      size_t const constrIndex = m_state.processOrder[orderIndex];
      size_t const resultIdx = constrIndex * 3;
      SC_ASSERT(constrIndex < m_templ.TriplesNum(), ());

      ScTemplate::Data::Triple const triple = m_templ.GetTriple(constrIndex);

      ScIterator3Ptr it;
      if (m_state.newIteration)
      {
        it = CreateIterator(triple);
        m_state.iterators.push(it);
      }
      else
      {
        UnrefReplacement(triple.source);
        UnrefReplacement(triple.edge);
        UnrefReplacement(triple.target);

        auto const itEdge = m_usedEdges.find(m_state.edges[orderIndex]);
        if (itEdge != m_usedEdges.end())
          m_usedEdges.erase(itEdge);

        it = m_state.iterators.top();
      }

      // make one iteration
      if (it)
      {
        bool isFinished = true;

        while (it->Next())
        {
          ScAddr const addr1 = it->Get(0);
          ScAddr const addr2 = it->Get(1);
          ScAddr const addr3 = it->Get(2);

          // check if search in structure
          if (m_struct.IsValid() && (!CheckInStruct(addr1) || !CheckInStruct(addr2) || !CheckInStruct(addr3)))
          {
            continue;
          }

          auto const res = m_usedEdges.insert(addr2);
          if (!res.second)  // don't iterate the same edge twicely
            continue;

          m_state.edges[orderIndex] = addr2;

          // do not make cycle for optimization issues (remove comparison expresion)
          m_resultAddrs[resultIdx] = addr1;
          m_resultAddrs[resultIdx + 1] = addr2;
          m_resultAddrs[resultIdx + 2] = addr3;

          RefReplacement(triple.source, addr1);
          RefReplacement(triple.edge, addr2);
          RefReplacement(triple.target, addr3);

          if (orderIndex == m_state.finishIdx)
          {
            MakeResult();
            found = true;
            m_state.newIteration = false;
          }
          else
          {
            m_state.newIteration = true;
          }

          m_state.didIter[orderIndex] = true;
          isFinished = false;
          break;
        }

        if (isFinished)  // finish iterator
        {
          m_state.iterators.pop();
          m_state.newIteration = false;
        }
      }
      else  // special checks and search
      {
        SC_THROW_EXCEPTION(utils::ExceptionInvalidState, "Invalid state during template search");
      }
    }
    while (!found && !m_state.iterators.empty());

    if (m_state.iterators.empty())
      m_state.isFinished = true;

    return found;
  }

private:
  struct InternalState
  {
    std::vector<size_t> processOrder;
    std::stack<ScIterator3Ptr> iterators;
    std::vector<bool> didIter;
    std::vector<ScAddr> edges;
    size_t finishIdx = 0;
    bool newIteration = true;
    bool isFinished = false;
  };

private:
  ScTemplate::Data const & m_templ;
  ScMemoryContext & m_context;
  ScAddr const m_struct;

  using UsedEdges = std::set<ScAddr, ScAddLessFunc>;
  UsedEdges m_usedEdges;

  ScAddrVector m_resultAddrs;
  struct ReplCounter
  {
    ScAddr addr;
    uint32_t refs;
  };
  std::unordered_map<std::string, ReplCounter> m_repls;

  InternalState m_state;
  ScTemplateNamedStruct & m_result;
};

bool ScTemplateSearch::Iterator::operator==(Iterator const & other) const
{
  return (m_index == other.m_index);
}

bool ScTemplateSearch::Iterator::operator!=(Iterator const & other) const
{
  return !(*this == other);
}

ScTemplateSearch::Iterator & ScTemplateSearch::Iterator::operator++()
{
  auto const result = m_search.DoIter();
  if (result)
  {
    m_index = (m_index + 1) % kEndIndex;  // don't use maximum index
  }
  else
  {
    m_struct.Clear();
    m_index = kEndIndex;
  }

  return *this;
}

ScTemplateNamedStruct const & ScTemplateSearch::Iterator::operator*() const
{
  return m_struct;
}

ScAddr ScTemplateSearch::Iterator::operator[](std::string const & name) const
{
  return m_struct[name];
}

ScAddr ScTemplateSearch::Iterator::operator[](ScAddr const & addr) const
{
  return m_struct[addr.ToString()];
}

// --------------------------------------

ScTemplateSearch::ScTemplateSearch(
    ScMemoryContext & ctx,
    ScTemplate const & templ,
    ScTemplateParams const & params /* = {} */,
    ScAddr const & structAddr /* =  ScAddr::Empty */)
  : m_ctx(ctx)
  , m_template(templ)
  , m_structAddr(structAddr)
  , m_params(params)
{
}

ScTemplateSearch::~ScTemplateSearch()
{
}

ScTemplateSearch::Iterator ScTemplateSearch::begin()
{
  SC_ASSERT(!m_impl, ());

  ApplyParameters();

  m_impl = std::make_unique<ScTemplateSearchImpl>(
      m_templateWithParams ? m_templateWithParams->GetData() : m_template.GetData(), m_ctx, m_structAddr, m_result);

  m_impl->Reset();
  if (m_impl->DoIter())
    return Iterator(*m_impl, m_result, 0);

  return end();
}

ScTemplateSearch::Iterator ScTemplateSearch::end()
{
  return Iterator(*m_impl, m_result);
}

std::pair<ScTemplateNamedStruct const &, bool> ScTemplateSearch::DoStep()
{
  return {m_result, m_impl->DoIter()};
}

void ScTemplateSearch::ApplyParameters()
{
  if (m_params.IsEmpty())
    return;  // do nothing

  // Create new template
  ScTemplate::Data newData = m_template.GetData();

  for (auto const & param : m_params)
  {
    ScTemplateArg * item = newData.GetReplacementForWrite(param.first);

    if (item)
    {
      if (!item->IsType())
      {
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams,
            "Please assign values just only to type items of template. Parameter: " << param.first);
      }
      else if (!item->m_typeValue.IsVar())  // this case is impossible because of template builder checks
      {
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams, "Can't assign value to non variable item. Parameter: " << param.first);
      }

      // check types compatibility
      ScType const paramType = m_ctx.GetElementType(param.second);
      if (paramType.IsVar())
      {
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams, "Do not use variable as parameter value. Parameter: " << param.first);
      }

      if (!ScTemplateParams::CanAssignParam(item->m_typeValue, paramType))
      {
        SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Type of " << param.first << " is not compatible");
      }

      item->m_kind = ScTemplateArg::Kind::Addr;
      item->m_typeValue = ScType::Unknown;
      item->m_addrValue = param.second;
    }
    else
    {
      SC_THROW_EXCEPTION(utils::ExceptionItemNotFound, "Can't find replacement for the parameter " << param.first);
    }
  }

  m_templateWithParams = ScTemplatePtrMake(std::move(newData));
}
