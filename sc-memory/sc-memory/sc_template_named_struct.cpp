#include "sc_template_named_struct.hpp"

#include "sc_debug.hpp"

std::string const kEmptyString = "";

ScTemplateNamedStruct::ScTemplateNamedStruct(NamesMap && names, ScAddrVector && elements)
  : m_names(std::move(names))
  , m_elements(std::move(elements))
{
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

size_t ScTemplateNamedStruct::ElementsNum() const
{
  return m_elements.size();
}

ScTemplateNamedStruct::Builder::Builder(ScTemplateNamedStruct & inStruct)
  : m_struct(inStruct)
{
  m_struct.Clear();
}

void ScTemplateNamedStruct::Builder::Add(ScAddr const & addr)
{
  std::string const name = addr.ToString();
  if (!CheckExists(name, addr))
  {
    m_struct.m_names.insert({ name, m_struct.m_elements.size() });
    m_struct.m_elements.push_back(addr);
  }
}

void ScTemplateNamedStruct::Builder::Add(std::string const & name, ScAddr const & addr)
{
  CheckExists(name, addr);
  Add(addr);

  auto const it = m_struct.m_names.find(addr.ToString());
  SC_ASSERT(it != m_struct.m_names.end(), ());
  if (it != m_struct.m_names.end())
    m_struct.m_names.insert({ name, it->second });
}

bool ScTemplateNamedStruct::Builder::CheckExists(std::string const & name, ScAddr const & addr) const
{
  auto const it = m_struct.m_names.find(name);
  if (it != m_struct.m_names.end())
  {
    SC_ASSERT(it->second < m_struct.m_elements.size(), ());
    ScAddr const & existAddr = m_struct.m_elements[it->second];
    if (existAddr != addr)
    {
      SC_THROW_EXCEPTION(utils::ExceptionInvalidState,
                         "NameStruct already have different element with name " << name << " it is " << existAddr.ToString()
                         << ", but you are trying to replace it with " << addr.ToString());
    }

    return true;
  }

  return false;
}
