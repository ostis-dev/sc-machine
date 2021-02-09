#include "sc_template_named_struct.hpp"

#include "sc_debug.hpp"

ScTemplateNamedStruct::ScTemplateNamedStruct(NamesMap && names, ScAddrVector && elements)
  : m_names(std::move(names))
  , m_elements(std::move(elements))
{
  m_names.reserve(16);
  m_elements.reserve(32);
}

bool ScTemplateNamedStruct::Has(std::string const & name) const
{
  return (m_names.find(name) != m_names.end());
}

ScAddr const & ScTemplateNamedStruct::operator [] (std::string const & name) const
{
  auto const it = m_names.find(name);
  if (it != m_names.end())
  {
    SC_ASSERT(it->second < m_elements.size(), ());
    return m_elements[it->second];
  }

  return ScAddr::Empty;
}

bool ScTemplateNamedStruct::IsEmpty() const
{
  return m_elements.empty();
}

void ScTemplateNamedStruct::Clear()
{
  m_names.clear();
  m_elements.clear();
}

ScAddrVector const & ScTemplateNamedStruct::GetElements() const
{
  return m_elements;
}

ScTemplateNamedStruct::Builder::Builder(ScTemplateNamedStruct & inStruct)
  : m_struct(inStruct)
{
  m_struct.Clear();
  m_addrs.reserve(32);
}

void ScTemplateNamedStruct::Builder::Add(ScAddr const & addr)
{
  uint64_t const hash = addr.Hash();
  if (m_addrs.find(hash) == m_addrs.end())
  {
    m_addrs.insert({ hash, m_struct.m_elements.size() });
    m_struct.m_elements.push_back(addr);
  }
}

void ScTemplateNamedStruct::Builder::Add(std::string const & name, ScAddr const & addr)
{
  if (m_struct.m_names.find(name) != m_struct.m_names.end())
  {
    SC_THROW_EXCEPTION(utils::ExceptionInvalidState,
                       "Name " << name << " already exists");
  }

  uint64_t const hash = addr.Hash();
  auto const it = m_addrs.find(hash);
  if (it == m_addrs.end())
  {
    m_struct.m_names.insert({ name, m_struct.m_elements.size() });
    m_addrs.insert({ hash, m_struct.m_elements.size() });
    m_struct.m_elements.push_back(addr);
  }
  else
  {
    m_struct.m_names.insert({ name, it->second });
  }
}
