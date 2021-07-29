/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_template.hpp"

#include "sc_memory.hpp"

#include <algorithm>

ScTemplateArg operator>>(ScAddr const & value, char const * replName)
{
  return ScTemplateArg(value, replName);
}

ScTemplateArg operator>>(ScAddr const & value, std::string const & replName)
{
  return ScTemplateArg(value, replName.c_str());
}

ScTemplateArg operator>>(ScType const & value, char const * replName)
{
  return ScTemplateArg(value, replName);
}

ScTemplateArg operator>>(ScType const & value, std::string const & replName)
{
  return ScTemplateArg(value, replName.c_str());
}

// --------------------------------

ScTemplate::ScTemplate(Data && data)
  : m_data(std::move(data))
{
}

bool ScTemplate::IsEmpty() const
{
  return m_data.IsEmpty();
}

bool ScTemplate::HasReplacement(std::string const & repl) const
{
  return m_data.HasReplacement(repl);
}

ScTemplate::Data const & ScTemplate::GetData() const
{
  return m_data;
}
