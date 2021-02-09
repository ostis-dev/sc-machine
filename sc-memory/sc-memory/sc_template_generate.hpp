#pragma once

#include "sc_template.hpp"
#include "sc_template_named_struct.hpp"
#include "sc_template_params.hpp"

#include <optional>
#include <string>

/*!
 * \brief Class to generate sc-construction by template in sc-memory
 */
class ScTemplateGenerate final
{
public:
  using Result = std::optional<ScTemplateNamedStruct>;

  ScTemplateGenerate(class ScMemoryContext & ctx, ScTemplate const & templ);
  ~ScTemplateGenerate();

  /*! Runs generation from template.
   * \param params Parameters for template generation
   *
   * \return Returns optional value. If generation failed, then optional value is empty, else
   * it contains result.
   */
  Result Do(ScTemplateParams const & params = {});

  //! Returns error message if it occured during generation
  std::string const & GetErrorMessage() const;

private:
  std::unique_ptr<class ScTemplateGenerateImpl> m_impl;
};
