#pragma once

#include "sc_template_data.hpp"
#include "sc_utils.hpp"

#include <memory>
#include <unordered_map>



class ScTemplate final
{
  friend class ScTemplateBuilderImpl;

public:
  using Triple = std::array<ScTemplateArg &, 3>;
  using Data = ScTemplateData;

public:
  explicit ScTemplate(Data && data);

  ScTemplate(ScTemplate const & other) = delete;
  ScTemplate & operator = (ScTemplate const & other) = delete;

  /*  If forceOrder flag is true, then search will be run in the same order,
   * that was used for a triples append
   */
  _SC_EXTERN ScTemplate() = default;
  _SC_EXTERN bool IsEmpty() const;
  _SC_EXTERN bool HasReplacement(std::string const & repl) const;

  Data const & GetData() const;

protected:
  Data m_data;
};

using ScTemplatePtr = std::unique_ptr<ScTemplate>;

template<class... _Args>
ScTemplatePtr ScTemplatePtrMake(_Args&&... args)
{
  return std::make_unique<ScTemplate>(std::forward<_Args>(args)...);
}
