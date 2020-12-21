#pragma once

#include "sc_addr.hpp"
#include "sc_type.hpp"

#include <string>

struct ScTemplateArg
{
  enum class Kind : uint8_t
  {
    None,
    Type,
    Addr,
    Replace
  };

  ScTemplateArg()
  {
    m_kind = Kind::Type;
    m_typeValue = ScType::Unknown;
  }

  ScTemplateArg(ScAddr const & addr, char const * replName = 0)
  {
    SetAddr(addr, replName);
  }

  ScTemplateArg(ScType const & type, char const * replName = 0)
  {
    SetType(type, replName);
  }

  ScTemplateArg(char const * name)
  {
    SetReplacement(name);
  }

  ScTemplateArg(std::string const & name)
  {
    SetReplacement(name.c_str());
  }

  inline bool IsAddr() const
  {
    return (m_kind == Kind::Addr);
  }

  inline bool IsReplacement() const
  {
    return (m_kind == Kind::Replace);
  }

  inline bool IsType() const
  {
    return (m_kind == Kind::Type);
  }

  inline bool IsFixed() const
  {
    return (IsAddr() || IsReplacement());
  }

  inline bool IsAssign() const
  {
    return (m_kind == Kind::Type);
  }

  inline bool IsItemType(Kind type) const
  {
    return (m_kind == type);
  }

  inline bool HasReplacementName() const
  {
    return !m_replacementName.empty();
  }

  inline void SetAddr(ScAddr const & addr, std::string const & replName)
  {
    SetAddr(addr, replName.c_str());
  }

  void SetAddr(ScAddr const & addr, char const * replName = 0)
  {
    m_kind = Kind::Addr;
    m_addrValue = addr;
    if (replName)
      m_replacementName = replName;
  }

  inline void SetType(ScType const & type, std::string const & replName)
  {
    SetType(type, replName.c_str());
  }

  void SetType(ScType const & type, char const * replName = 0)
  {
    m_kind = Kind::Type;
    m_typeValue = type;
    if (replName)
      m_replacementName = replName;
  }

  inline void SetReplacement(std::string const & name)
  {
    SetReplacement(name.c_str());
  }

  void SetReplacement(char const * name)
  {
    m_kind = Kind::Replace;
    if (name)
      m_replacementName = name;
  }

  Kind m_kind;
  ScAddr m_addrValue;
  ScType m_typeValue;
  std::string m_replacementName;
};

_SC_EXTERN ScTemplateArg operator >> (ScAddr const & value, char const * replName);
_SC_EXTERN ScTemplateArg operator >> (ScAddr const & value, std::string const & replName);
_SC_EXTERN ScTemplateArg operator >> (ScType const & value, char const * replName);
_SC_EXTERN ScTemplateArg operator >> (ScType const & value, std::string const & replName);
