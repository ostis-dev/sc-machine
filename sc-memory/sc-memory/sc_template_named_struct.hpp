#pragma once

#include "sc_addr.hpp"
#include "sc_defines.hpp"

#include <string>
#include <unordered_map>
#include <vector>

class ScTemplateNamedStruct
{
public:
  class Builder
  {
  public:
    _SC_EXTERN explicit Builder(ScTemplateNamedStruct & inStruct);

    _SC_EXTERN void Add(ScAddr const & addr);
    _SC_EXTERN void Add(std::string const & name, ScAddr const & addr);

  private:
    ScTemplateNamedStruct & m_struct;
    std::unordered_map<uint64_t, size_t> m_addrs;
  };

  using NamesMap = std::unordered_map<std::string, size_t>;

  _SC_EXTERN ScTemplateNamedStruct() = default;
  _SC_EXTERN ScTemplateNamedStruct(NamesMap && names, ScAddrVector && elements);

  _SC_EXTERN bool Has(std::string const & name) const;
  _SC_EXTERN ScAddr const & operator[](std::string const & name) const;

  _SC_EXTERN bool IsEmpty() const;
  _SC_EXTERN void Clear();

  _SC_EXTERN ScAddrVector const & GetElements() const;

private:
  NamesMap m_names;
  ScAddrVector m_elements;
};
