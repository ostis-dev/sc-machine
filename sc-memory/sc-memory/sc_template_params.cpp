#include "sc_template_params.hpp"

#include "sc_debug.hpp"

ScTemplateParams & ScTemplateParams::Add(std::string const & key, ScAddr const & value)
{
  SC_ASSERT(m_values.find(value.ToString()) == m_values.end(), ("Duplicate addr key"));

  SC_ASSERT(m_values.find(key) == m_values.end(), ("Duplicate key"));
  m_values.insert({key, value});

  return *this;
}

ScTemplateParams & ScTemplateParams::Add(ScAddr const & key, ScAddr const & value)
{
  return Add(key.ToString(), value);
}

bool ScTemplateParams::IsEmpty() const
{
  return m_values.empty();
}

ScAddr ScTemplateParams::TryGet(std::string const & name) const
{
  auto const it = m_values.find(name);
  if (it != m_values.end())
      return it->second;

  return {};
}

ScAddr ScTemplateParams::TryGet(ScAddr const & addr) const
{
  return TryGet(addr.ToString());
}

ScAddr ScTemplateParams::operator [] (std::string const & name) const
{
  return TryGet(name);
}

ScAddr ScTemplateParams::operator [] (ScAddr const & addr) const
{
  return TryGet(addr);
}

bool ScTemplateParams::CanAssignParam(ScType const & varType, ScType const & paramType)
{
  if ((*varType & sc_type_element_mask) != (*paramType & sc_type_element_mask))
    return false;

  if (!varType.IsVar() || paramType.IsVar())
    return false;

  auto const mask = ~(sc_type_element_mask | sc_type_constancy_mask);
  auto const varMask = (*varType & mask);
  if (varMask != 0 && varMask != (*paramType & mask))
    return false;

  return true;
}
