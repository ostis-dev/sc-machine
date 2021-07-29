#pragma once

#include "sc_addr.hpp"
#include "sc_type.hpp"

#include <string>
#include <unordered_map>

class ScTemplateParams
{
public:
  using ContainerType = std::unordered_map<std::string, ScAddr>;

  _SC_EXTERN ScTemplateParams & Add(std::string const & key, ScAddr const & value);
  _SC_EXTERN ScTemplateParams & Add(ScAddr const & key, ScAddr const & value);

  _SC_EXTERN ContainerType::const_iterator begin() const
  {
    return m_values.begin();
  }
  _SC_EXTERN ContainerType::const_iterator end() const
  {
    return m_values.end();
  }

  //! Check if parameters are empty
  _SC_EXTERN bool IsEmpty() const;

  /*! Safe way to get parameter by name.
   * \param name Name of parameter
   * \return Returns ScAddr associated with name if it exists, otherwise returns empty ScAddr
   */
  _SC_EXTERN ScAddr TryGet(std::string const & name) const;

  /*! Same as TryGet by name, but used ScAddr::ToString as name
   */
  _SC_EXTERN ScAddr TryGet(ScAddr const & addr) const;

  /// Operators that wrap TryGet methods
  _SC_EXTERN ScAddr operator[](std::string const & name) const;
  _SC_EXTERN ScAddr operator[](ScAddr const & addr) const;

  /*! Check if template element with type `varType` can be replaced with
   *  parameter that have type `paramType`
   */
  _SC_EXTERN static bool CanAssignParam(ScType const & varType, ScType const & paramType);

private:
  std::unordered_map<std::string, ScAddr> m_values;
};
