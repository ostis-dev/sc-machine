#pragma once

#include "sc_debug.hpp"
#include "sc_template_arg.hpp"

class ScTemplateData
{
public:
  using ReplacementsMap = std::unordered_map<std::string, size_t>;
  struct Triple
  {
    ScTemplateArg const & source;
    ScTemplateArg const & edge;
    ScTemplateArg const & target;
  };

  inline void Reserve(size_t triplesNum)
  {
    m_triples.reserve(triplesNum);
    m_elements.reserve(triplesNum * 3);
  }

  inline size_t TriplesNum() const
  {
    return m_triples.size();
  }

  inline bool IsEmpty() const
  {
    return m_triples.empty();
  }

  inline void Clear()
  {
    m_repl.clear();
    m_triples.clear();
  }

  inline bool HasReplacement(std::string const & name) const
  {
    return (m_repl.find(name) != m_repl.end());
  }

  inline ScTemplateArg const * GetReplacement(std::string const & name) const
  {
    ReplacementsMap::const_iterator const it = m_repl.find(name);
    if (it != m_repl.end())
    {
      SC_ASSERT(it->second < m_elements.size(), ());
      return &(m_elements[it->second]);
    }

    return nullptr;
  }

  inline ScTemplateArg * GetReplacementForWrite(std::string const & name)
  {
    ReplacementsMap::const_iterator const it = m_repl.find(name);
    if (it != m_repl.end())
    {
      SC_ASSERT(it->second < m_elements.size(), ());
      return &(m_elements[it->second]);
    }

    return nullptr;
  }

  inline bool SetupReplAlias(std::string const & replName, std::string const & newAlias)
  {
    SC_ASSERT(m_repl.find(newAlias) == m_repl.end(), ());
    auto const it = m_repl.find(replName);
    if (it != m_repl.end())
    {
      m_repl.insert({newAlias, it->second});
      return true;
    }

    return false;
  }

  inline void AddTriple(ScTemplateArg const & src, ScTemplateArg const & edge, ScTemplateArg const & trg)
  {
    if (!edge.m_replacementName.empty() &&
        (edge.m_replacementName == src.m_replacementName || edge.m_replacementName == trg.m_replacementName))
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "You can't use same replacement for an edge and source/target");
    }

    std::array<size_t, 3> triple;
    std::array<ScTemplateArg const *, 3> elements = {&src, &edge, &trg};
    for (size_t i = 0; i < elements.size(); ++i)
    {
      size_t & tripleIndex = triple[i];
      ScTemplateArg const & value = *(elements[i]);

      if (value.m_kind == ScTemplateArg::Kind::Type && value.m_typeValue.IsConst())
      {
        SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "Using of constant types in template is restricted");
      }

      if (value.m_kind == ScTemplateArg::Kind::Addr && !value.m_addrValue.IsValid())
      {
        SC_THROW_EXCEPTION(utils::ExceptionInvalidParams, "ScAddr is empty");
      }

      if (!value.IsReplacement() && !value.m_replacementName.empty())
      {
        if (m_repl.find(value.m_replacementName) != m_repl.end())
        {
          SC_THROW_EXCEPTION(
              utils::ExceptionInvalidParams, "Replacement " << value.m_replacementName << " already exists");
        }
      }

      if (value.IsAddr())
      {
        std::string const sAddr = value.m_addrValue.ToString();
        auto const it = m_repl.find(sAddr);
        if (it == m_repl.end())
        {
          tripleIndex = m_elements.size();
          m_repl.insert({sAddr, tripleIndex});
          if (value.HasReplacementName())
            m_repl.insert({value.m_replacementName, tripleIndex});

          m_elements.emplace_back(value);
        }
        else
        {
          tripleIndex = it->second;
        }
      }
      else if (value.IsType())
      {
        tripleIndex = m_elements.size();
        if (value.HasReplacementName())
          m_repl.insert({value.m_replacementName, tripleIndex});

        m_elements.emplace_back(value);
      }
      else if (value.IsReplacement())
      {
        auto const it = m_repl.find(value.m_replacementName);
        if (it == m_repl.end())
        {
          SC_THROW_EXCEPTION(
              utils::ExceptionInvalidParams, "There are no parameter with name " << value.m_replacementName);
        }
        else
        {
          tripleIndex = it->second;
        }
      }
    }  // for

    m_triples.emplace_back(std::move(triple));
  }

  inline Triple GetTriple(size_t index) const
  {
    SC_ASSERT(index < m_triples.size(), ());
    auto const & indecies = m_triples[index];

    SC_ASSERT(indecies[0] < m_elements.size(), ());
    SC_ASSERT(indecies[1] < m_elements.size(), ());
    SC_ASSERT(indecies[2] < m_elements.size(), ());

    return {m_elements[indecies[0]], m_elements[indecies[1]], m_elements[indecies[2]]};
  }

  ReplacementsMap const & Replacements() const
  {
    return m_repl;
  }

private:
  // Store mapping of name to index in result vector
  ReplacementsMap m_repl;
  // Store elements
  std::vector<ScTemplateArg> m_elements;
  // Store construction (triples)
  std::vector<std::array<size_t, 3>> m_triples;
};
