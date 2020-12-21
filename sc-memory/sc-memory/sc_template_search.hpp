#pragma once

#include "sc_template.hpp"
#include "sc_template_named_struct.hpp"
#include "sc_template_params.hpp"

#include <limits>
#include <string>


class ScTemplateSearchImpl;

/*!
 * \brief This class allows to run template based search.
 */
class ScTemplateSearch final
{
public:
 class Iterator
 {
    friend class ScTemplateSearch;

  protected:
    static size_t const kEndIndex = std::numeric_limits<size_t>::max();

    Iterator(ScTemplateSearchImpl & search,
             ScTemplateNamedStruct & namedStruct,
             size_t idx = kEndIndex)
     : m_search(search)
     , m_struct(namedStruct)
     , m_index(idx)
    {
    }

  public:

    bool operator == (Iterator const & other) const;
    bool operator != (Iterator const & other) const;
    Iterator & operator ++ ();
    ScTemplateNamedStruct const & operator * () const;

    ScAddr operator[](std::string const & name) const;
    ScAddr operator[](ScAddr const & addr) const;

  private:
    ScTemplateSearchImpl & m_search;
    ScTemplateNamedStruct & m_struct;

    size_t m_index = std::numeric_limits<size_t>::max();
  };

  ScTemplateSearch(ScMemoryContext & ctx,
                   ScTemplate const & templ,
                   ScTemplateParams const & params = {},
                   ScAddr const & structAddr = ScAddr::Empty);
  ~ScTemplateSearch();

  Iterator begin();
  Iterator end();

protected:
  std::pair<ScTemplateNamedStruct const &, bool> DoStep();

private:
  void ApplyParameters();

private:
  ScMemoryContext & m_ctx;
  ScTemplate const & m_template;
  ScTemplatePtr m_templateWithParams;
  ScAddr m_structAddr;
  ScTemplateParams m_params;

  std::unique_ptr<ScTemplateSearchImpl> m_impl;
  ScTemplateNamedStruct m_result;
};
