#include "sc_template_struct_builder.hpp"

#include "sc_memory.hpp"
#include "sc_template_builder_impl.hpp"

namespace
{

class ObjectInfo
{
public:
  ObjectInfo(ScAddr const & inAddr, ScType const & inType, std::string const & inSysIdtf)
    : m_addr(inAddr)
    , m_type(inType)
    , m_sysIdtf(inSysIdtf)
    , m_source(nullptr)
    , m_target(nullptr)
  {
  }

  inline bool IsEdge() const { return m_type.IsEdge(); }

  inline ObjectInfo const * GetSource() const { return m_source; }
  inline ObjectInfo const * GetTarget() const { return m_target; }

  inline std::string GetIdtf() const
  {
    if (!m_sysIdtf.empty())
      return m_sysIdtf;

    if (m_addr)
      return m_addr.ToString();

    return {};
  }
  inline ScAddr const & GetAddr() const { return m_addr; }
  inline ScType const & GetType() const { return m_type; }
  inline ScAddr::HashType GetAddrHash() const { return m_addr.Hash(); }

  inline void SetSource(ObjectInfo * src) { m_source = src; }
  inline void SetTarget(ObjectInfo * trg) { m_target = trg; }

  inline uint32_t CalculateEdgeRank() const
  {
    SC_ASSERT(IsEdge(), ());
    SC_ASSERT(m_source, ());
    SC_ASSERT(m_target, ());
    return (m_source->GetType().IsConst() ? 1 : 0) +
           (m_target->GetType().IsConst() ? 1 : 0);
  }

private:
  ScAddr m_addr;
  ScType m_type;
  std::string m_sysIdtf;

  // edge data
  ObjectInfo * m_source;
  ObjectInfo * m_target;
};

class EdgeLessFunctor
{
  using EdgeDependMap = std::unordered_multimap<ScAddr::HashType, ScAddr::HashType>;
  using ObjectVector = std::vector<ObjectInfo>;

public:
  explicit EdgeLessFunctor(EdgeDependMap const & edgeDependMap, ObjectVector const & objects)
    : m_edgeDependMap(edgeDependMap)
    , m_objects(objects)
  {
  }

  // returns true, when edge srouce/target objects depend on otherEdge construction
  bool IsEdgeDependOnOther(ScAddr::HashType const & edge, ScAddr::HashType const & otherEdge) const
  {
    // TODO: support possible loops
    auto const range = m_edgeDependMap.equal_range(edge);
    for (auto it = range.first; it != range.second; ++it)
    {
      if (it->second || IsEdgeDependOnOther(it->second, otherEdge))
        return true;
    }

    return false;
  }

  bool operator() (size_t const indexA, size_t const indexB) const
  {
    ObjectInfo const & objA = m_objects[indexA];
    ObjectInfo const & objB = m_objects[indexB];

    SC_ASSERT(objA.IsEdge(), ());
    SC_ASSERT(objB.IsEdge(), ());

    if (IsEdgeDependOnOther(objA.GetAddrHash(), objB.GetAddrHash()))
    {
      return false;
    }
    else if (IsEdgeDependOnOther(objB.GetAddrHash(), objA.GetAddrHash()))
    {
      return true;
    }

    uint32_t const rankA = objA.CalculateEdgeRank();
    uint32_t const rankB = objB.CalculateEdgeRank();

    if (rankA != rankB)
      return rankA > rankB;

    return objA.GetAddrHash() < objB.GetAddrHash();
  }

private:
  EdgeDependMap const & m_edgeDependMap;
  ObjectVector const & m_objects;
};

} // namespace


ScTemplateStructBuilder::ScTemplateStructBuilder(ScMemoryContext & ctx)
  : m_ctx(ctx)
{
  m_builder = std::make_unique<ScTemplateBuilderImpl>();
}

ScTemplateStructBuilder::~ScTemplateStructBuilder()
{
  // std::unique_ptr
}

ScTemplatePtr ScTemplateStructBuilder::Make(ScAddr const & structAddr)
{
  SC_ASSERT(!m_isDone, ("Twice call of Make() method"));
  m_isDone = true;

  // TODO: add struct blocking
  // TODO: provide error codes
  std::unordered_map<ScAddr::HashType, size_t> addrToObjectIndex;
  std::unordered_multimap<ScAddr::HashType, ScAddr::HashType> edgeDependMap;
  std::vector<size_t> edgeIndices;
  edgeIndices.reserve(100);

  std::vector<ObjectInfo> objects;
  objects.reserve(128);

  size_t index = 0;
  ScIterator3Ptr iter = m_ctx.Iterator3(
        structAddr,
        ScType::EdgeAccessConstPosPerm,
        ScType::Unknown);

  while (iter->Next())
  {
    ScAddr const objAddr = iter->Get(2);
    ScAddr::HashType const objHash = objAddr.Hash();

    auto const it = addrToObjectIndex.find(objHash);
    if (it != addrToObjectIndex.end())
      continue; // object already exist

    addrToObjectIndex[objHash] = objects.size();

    ScType const objType = m_ctx.GetElementType(objAddr);
    if (objType.IsUnknown())
    {
      m_errorMessage = "Can't determine type of ScElement " + std::to_string(objAddr.Hash());
      return {};
    }

    std::string objIdtf = m_ctx.HelperGetSystemIdtf(objAddr);
    if (objIdtf.empty())
      objIdtf = "..obj_" + std::to_string(index++);

    if (objType.IsEdge())
      edgeIndices.emplace_back(objects.size());

    objects.emplace_back(objAddr, objType, objIdtf);
  }

  // iterate all edges and determine source/target objects
  for (auto const i : edgeIndices)
  {
    ObjectInfo & obj = objects[i];

    // determine source and target objects
    ScAddr src, trg;
    if (!m_ctx.GetEdgeInfo(obj.GetAddr(), src, trg))
    {
      m_errorMessage = "Can't get src/trg objects of edge " + std::to_string(obj.GetAddrHash());
      return {};
    }

    auto const itSrc = addrToObjectIndex.find(src.Hash());
    if (itSrc == addrToObjectIndex.end())
    {
      m_errorMessage = "Source element " + std::to_string(src.Hash()) +
          " of edge " + std::to_string(obj.GetAddrHash()) +
          " is not a part of the same structure " + std::to_string(structAddr.Hash());

      return {};
    }

    auto const itTrg = addrToObjectIndex.find(trg.Hash());
    if (itTrg == addrToObjectIndex.end())
    {
      m_errorMessage = "Target element " + std::to_string(trg.Hash()) +
          " of edge " + std::to_string(obj.GetAddrHash()) +
          " is not a part of the same structure " + std::to_string(structAddr.Hash());

      return {};
    }

    ObjectInfo * srcObj = &(objects[itSrc->second]);
    ObjectInfo * trgObj = &(objects[itTrg->second]);

    if (srcObj->IsEdge())
      edgeDependMap.insert({ obj.GetAddr().Hash(), srcObj->GetAddr().Hash() });
    if (trgObj->IsEdge())
      edgeDependMap.insert({ obj.GetAddr().Hash(), srcObj->GetAddr().Hash() });

    obj.SetSource(srcObj);
    obj.SetTarget(trgObj);
  }

  // now need to sort edges for suitable search order
  std::sort(edgeIndices.begin(), edgeIndices.end(), EdgeLessFunctor(edgeDependMap, objects));

  // build template
  {
    auto const WrapIdtf = [this](ObjectInfo const & obj) -> std::string
    {
      std::string const idtf = obj.GetIdtf();
      if (m_builder->HasReplacement(idtf))
        return {};

      return idtf;
    };

    for (auto const i : edgeIndices)
    {
      ObjectInfo const & edge = objects[i];
      SC_ASSERT(edge.GetType().IsVar(), ());

      ObjectInfo const * src = edge.GetSource();
      ObjectInfo const * trg = edge.GetTarget();

      ScType const srcType = src->GetType();
      ScType const trgType = trg->GetType();

      if (srcType.IsConst())
      {
        if (trgType.IsConst())  // F_A_F
        {
          m_builder->Triple(
                src->GetAddr() >> WrapIdtf(*src),
                edge.GetType() >> WrapIdtf(edge),
                trg->GetAddr() >> WrapIdtf(*trg));
        }
        else
        {
          if (m_builder->HasReplacement(trg->GetIdtf())) // F_A_F
          {
            m_builder->Triple(
                  src->GetAddr() >> WrapIdtf(*src),
                  edge.GetType() >> WrapIdtf(edge),
                  trg->GetIdtf());
          }
          else // F_A_A
          {
             m_builder->Triple(
                   src->GetAddr() >> WrapIdtf(*src),
                   edge.GetType() >> WrapIdtf(edge),
                   trgType >> WrapIdtf(*trg));
          }
        }
      }
      else if (trgType.IsConst())
      {
        if (m_builder->HasReplacement(src->GetIdtf())) // F_A_F
        {
          m_builder->Triple(
                src->GetIdtf(),
                edge.GetType() >> WrapIdtf(edge),
                trg->GetAddr() >> WrapIdtf(*trg));
        }
        else // A_A_F
        {
          m_builder->Triple(
                srcType >> WrapIdtf(*src),
                edge.GetType() >> WrapIdtf(edge),
                trg->GetAddr() >> WrapIdtf(*trg));
        }
      }
      else
      {
        bool const srcRepl = m_builder->HasReplacement(src->GetIdtf());
        bool const trgRepl = m_builder->HasReplacement(trg->GetIdtf());

        if (srcRepl && trgRepl) // F_A_F
        {
          m_builder->Triple(
                src->GetIdtf(),
                edge.GetType() >> WrapIdtf(edge),
                trg->GetIdtf());
        }
        else if (!srcRepl && trgRepl) // A_A_F
        {
          m_builder->Triple(
                srcType >> WrapIdtf(*src),
                edge.GetType() >> WrapIdtf(edge),
                trg->GetIdtf());
        }
        else if (srcRepl && !trgRepl) // F_A_A
        {
          m_builder->Triple(
                src->GetIdtf(),
                edge.GetType() >> WrapIdtf(edge),
                trgType >> WrapIdtf(*trg));
        }
        else
        {
          m_builder->Triple(
                srcType >> WrapIdtf(*src),
                edge.GetType() >> WrapIdtf(edge),
                trgType >> WrapIdtf(*trg));
        }
      }

      // Add addr aliases
      /// TODO: Implement this in better way
      std::array<ObjectInfo const *, 3> elements = { src, &edge, trg };
      for (ObjectInfo const * e : elements)
      {
        SC_ASSERT(e->GetAddr(), ());

        std::string const sAddr = e->GetAddr().ToString();
        if (!m_builder->HasReplacement(sAddr))
        {
          std::string const idtf = e->GetIdtf();
          m_builder->SetupReplAlias(e->GetIdtf(), e->GetAddr().ToString());
        }
      }
    }
  }

  return m_builder->Make();
}

std::string const & ScTemplateStructBuilder::GetErrorMessage() const
{
  return m_errorMessage;
}
