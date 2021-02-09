#pragma once

#include "sc_template.hpp"

class ScTemplateBuilderImpl final
{
public:
  ScTemplateBuilderImpl();

  /** Adds construction:
   *          param2
   * param1 ----------> param3
   */
  ScTemplateBuilderImpl & Triple(ScTemplateArg const & param1,
                                 ScTemplateArg const & param2,
                                 ScTemplateArg const & param3);

  /** Adds construction:
   *           param2
   *	param1 ---------> param3
   *             ^
   *             |
   *             | param4
   *             |
   *           param5
   * Equal to two calls of triple, so this template add 6 items to result (NOT 5), to minimize
   * possible abuse, use result name mapping, and get result by names
   */
  ScTemplateBuilderImpl & TripleWithRelation(ScTemplateArg const & param1,
                                             ScTemplateArg const & param2,
                                             ScTemplateArg const & param3,
                                             ScTemplateArg const & param4,
                                             ScTemplateArg const & param5);

  /*! Returns pointer to created template
  * \remarks Do not use builder after this method call.
  */
  ScTemplatePtr Make();

  //! Check if replacement with specified idtf exists
  bool HasReplacement(std::string const & idtf) const
  {
    return m_templ.HasReplacement(idtf);
  }

  //! Setup alias for replacement
  bool SetupReplAlias(std::string const & replName, std::string const & newAlias)
  {
    return m_templ.SetupReplAlias(replName, newAlias);
  }

private:
  ScTemplate::Data m_templ;
  bool m_isDone = false;
};
