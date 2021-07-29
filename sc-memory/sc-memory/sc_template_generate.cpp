#include "sc_template_generate.hpp"

#include "sc_memory.hpp"

#include <algorithm>

class ScTemplateGenerateImpl
{
public:
  ScTemplateGenerateImpl(ScMemoryContext & ctx, ScTemplate::Data const & templ)
    : m_ctx(ctx)
    , m_templ(templ)
  {
  }

  std::optional<ScTemplateNamedStruct> Do(ScTemplateParams const & params)
  {
    ScTemplateNamedStruct result;
    ScTemplateNamedStruct::Builder resultBuilder(result);

    m_createdElements.reserve(m_templ.TriplesNum() * 3);

    // check if it valid
    for (size_t i = 0; i < m_templ.TriplesNum(); ++i)
    {
      ScTemplate::Data::Triple const & triple = m_templ.GetTriple(i);
      if (triple.edge.IsFixed())
        SC_THROW_EXCEPTION(
            utils::ExceptionInvalidParams, "You can't use fixed value for edge in triple for template generation");
    }

    if (!CheckParams(params))
    {
      m_errorMessage = "Invalid parameters";
      return {};
    }

    auto const AddResult = [&resultBuilder, &result](ScAddr const & addr, std::string const & name)
    {
      if (name.empty())
        return;  // do nothing

      if (!result.Has(name))
        resultBuilder.Add(name, addr);
    };

    ScMemoryContextEventsPendingGuard guard(m_ctx);
    ScAddrVector createdElements;
    bool isError = false;

    for (size_t i = 0; i < m_templ.TriplesNum(); ++i)
    {
      ScTemplate::Data::Triple const triple = m_templ.GetTriple(i);

      // check that second command is to generate edge
      SC_ASSERT(triple.edge.m_kind == ScTemplateArg::Kind::Type && triple.edge.m_typeValue.IsEdge(), ());
      // the second item couldn't be a replacement
      SC_ASSERT(triple.edge.m_kind != ScTemplateArg::Kind::Replace, ());

      ScAddr const addr1 = ResolveAddr(triple.source, params, result);
      SC_ASSERT(addr1.IsValid(), ());
      ScAddr const addr2 = ResolveAddr(triple.target, params, result);
      SC_ASSERT(addr2.IsValid(), ());

      if (!addr1.IsValid() || !addr2.IsValid())
      {
        isError = true;
        break;
      }

      ScAddr const edge = m_ctx.CreateEdge(triple.edge.m_typeValue.UpConstType(), addr1, addr2);
      if (!edge.IsValid())
      {
        isError = true;
        break;
      }

      AddResult(addr1, triple.source.m_replacementName);
      AddResult(edge, triple.edge.m_replacementName);
      AddResult(addr2, triple.target.m_replacementName);
    }

    if (isError)
    {
      CleanupCreated();
      return {};
    }

    return std::make_optional<ScTemplateNamedStruct>(std::move(result));
  }

  ScAddr CreateNodeLink(ScType const & type)
  {
    ScAddr addr;
    if (type.IsNode())
    {
      addr = m_ctx.CreateNode(type);
    }
    else if (type.IsLink())
    {
      addr = m_ctx.CreateLink();
    }

    if (addr.IsValid())
      m_createdElements.push_back(addr);

    return addr;
  }

  ScAddr ResolveAddr(
      ScTemplateArg const & itemValue,
      ScTemplateParams const & params,
      ScTemplateNamedStruct const & result)
  {
    /// TODO: improve speed, because not all time we need to replace by params
    // replace by value from params
    if (!params.IsEmpty() && !itemValue.m_replacementName.empty())
    {
      ScAddr const addr = params[itemValue.m_replacementName];
      if (addr.IsValid())
        return addr;
    }

    switch (itemValue.m_kind)
    {
    case ScTemplateArg::Kind::Addr:
      return itemValue.m_addrValue;
    case ScTemplateArg::Kind::Type:
    {
      if (itemValue.HasReplacementName())
      {
        ScAddr const addr = result[itemValue.m_replacementName];
        if (addr.IsValid())
          return addr;
      }
      return CreateNodeLink(itemValue.m_typeValue.UpConstType());
    }
    case ScTemplateArg::Kind::Replace:
    {
      return result[itemValue.m_replacementName];
    }
    default:
      break;
    }

    return ScAddr();
  }

  void CleanupCreated()
  {
    for (ScAddrVector::iterator it = m_createdElements.begin(); it != m_createdElements.end(); ++it)
      m_ctx.EraseElement(*it);
    m_createdElements.clear();
  }

  bool CheckParams(ScTemplateParams const & params) const
  {
    for (auto const & it : params)
    {
      // Skip aliases of addrs
      if (it.first == it.second.ToString())
        continue;

      auto const repl = m_templ.GetReplacement(it.first);
      if (!repl)
        return false;

      ScType const & itemType = repl->m_typeValue;
      /// TODO: check subtype of objects. Can't replace tuple with no tuple object
      if (itemType.HasConstancyFlag() && (!itemType.IsVar() || itemType.IsEdge()))
        return false;
    }

    return true;
  }

  std::string const & GetErrorMessage() const
  {
    return m_errorMessage;
  }

private:
  ScMemoryContext & m_ctx;
  ScTemplate::Data const & m_templ;

  std::string m_errorMessage;

  ScAddrVector m_createdElements;
};

ScTemplateGenerate::ScTemplateGenerate(ScMemoryContext & ctx, ScTemplate const & templ)
{
  m_impl = std::make_unique<ScTemplateGenerateImpl>(ctx, templ.GetData());
}

ScTemplateGenerate::~ScTemplateGenerate()
{
}

std::optional<ScTemplateNamedStruct> ScTemplateGenerate::Do(ScTemplateParams const & params)
{
  return m_impl->Do(params);
}

std::string const & ScTemplateGenerate::GetErrorMessage() const
{
  return m_impl->GetErrorMessage();
}
