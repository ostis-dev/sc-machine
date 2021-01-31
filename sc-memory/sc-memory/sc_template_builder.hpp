#pragma once

#include "sc_template.hpp"

#include <memory>

class _SC_EXTERN ScTemplateBuilder final
{
public:
  ScTemplateBuilder();
  ~ScTemplateBuilder();

  /*! \brief Adds triple construction into template
   *  \param source Source element definition
   *  \param edge Edge element definition
   *  \param target Target element definition
   */
  _SC_EXTERN ScTemplateBuilder & Triple(ScTemplateArg const & source,
                                        ScTemplateArg const & edge,
                                        ScTemplateArg const & target);

  /*! \brief Adds 5-element construction into template.
   *  This is equal to `Triple` function calls:
   *  ```cpp
   *  builder.Triple(source,
   *                 edge >> "repl",
   *                 target);
   *  builder.Triple(attr,
   *                 attrEdge,
   *                 "repl");
   *  ```
   *
   *  \param source Source element definition
   *  \param edge Edge element definition
   *  \param target Target element definition
   *  \param attrEdge Definition of edge that goes from attribute
   *  \param attr Definition of attribute
   */
  _SC_EXTERN ScTemplateBuilder & TripleWithRelation(ScTemplateArg const & source,
                                                    ScTemplateArg const & edge,
                                                    ScTemplateArg const & target,
                                                    ScTemplateArg const & attrEdge,
                                                    ScTemplateArg const & attr);

  /*! \brief Makes template based on appended triples
   *  \return Returns pointer to created template
   */
  _SC_EXTERN ScTemplatePtr Make();

private:
  std::unique_ptr<class ScTemplateBuilderImpl> m_impl;
};
