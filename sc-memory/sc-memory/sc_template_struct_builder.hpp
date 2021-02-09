#pragma once

#include "sc_template.hpp"

#include <memory>
#include <string>

class ScTemplateStructBuilder
{
public:
  explicit ScTemplateStructBuilder(class ScMemoryContext & ctx);
  ~ScTemplateStructBuilder();

  /*! Builds ScTempalte from ScAddr of struct in ScMemory
   *  \param structAddr ScAddr of struct that represent template in sc-memory
   *  \return Returns pointer to created ScTemplate or nullptr in case of error.
   */
  _SC_EXTERN ScTemplatePtr Make(ScAddr const & structAddr);

  //! Returns error message in case of error during template creation
  _SC_EXTERN std::string const & GetErrorMessage() const;

private:
  class ScMemoryContext & m_ctx;
  std::unique_ptr<class ScTemplateBuilderImpl> m_builder;
  std::string m_errorMessage;
  bool m_isDone = false;
};
