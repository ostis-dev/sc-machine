#include "MetaDataManager.hpp"

#include "ReflectionParser.hpp"

#include <iostream>
#include <regex>

MetaDataManager::MetaDataManager(Cursor const & cursor)
{
  m_lineNumber = cursor.GetLineNumber() - 1;
  for (auto & child : cursor.GetChildren())
  {
    if (child.GetKind() != CXCursor_AnnotateAttr)
      continue;

    auto const props = ExtractProperties(child);
    for (auto & prop : props)
      m_properties[prop.first] = prop.second;
  }
}

void MetaDataManager::Merge(MetaDataManager const & metaData)
{
  for (auto const & prop : metaData.m_properties)
    SetProperty(prop.first, prop.second);
}

std::string MetaDataManager::GetProperty(std::string const & key) const
{
  auto search = m_properties.find(key);

  // use an empty string by default
  return search == m_properties.end() ? "" : search->second;
}

bool MetaDataManager::GetPropertySafe(std::string const & key, std::string & outValue) const
{
  auto search = m_properties.find(key);

  if (search != m_properties.end())
  {
    outValue = search->second;
    return true;
  }

  return false;
}

void MetaDataManager::SetProperty(std::string const & key, std::string const & value)
{
  assert(!HasProperty(key));
  m_properties[key] = value;
}

bool MetaDataManager::HasProperty(std::string const & key) const
{
  return (m_properties.find(key) != m_properties.end());
}

bool MetaDataManager::GetFlag(std::string const & key) const
{
  return m_properties.find(key) == m_properties.end() ? false : true;
}

size_t MetaDataManager::GetLineNumber() const
{
  return m_lineNumber;
}

std::string MetaDataManager::GetNativeString(std::string const & key) const
{
  auto const search = m_properties.find(key);

  // wasn't set
  if (search == m_properties.end())
    return "";

  static const std::regex quotedString(
      // opening quote
      "(?:\\s*\")"
      // actual string contents
      "([^\"]*)"
      // closing quote
      "\"");

  std::smatch match;

  if (std::regex_match(search->second, match, quotedString))
  {
    assert(match.size() >= 2);
    return match[1].str();
  }

  // couldn't find one
  return search->second;
}

std::vector<MetaDataManager::Property> MetaDataManager::ExtractProperties(Cursor const & cursor) const
{
  std::vector<Property> properties;

  static const std::regex propertyList(
      // property name
      "([a-zA-Z\\:]+)"
      // optional whitespace before
      "(?:\\s*)"
      // constructor
      "("
      // opening paren
      "\\("
      // arguments
      "([^\\)]*)"
      // closing paren
      "\\)"
      // end constructor
      ")?"
      // optional comma/whitespace
      "(?:(\\s|,)*)");

  auto const meta = cursor.GetDisplayName();
  auto start = meta.cbegin();

  // collect properties and optional arguments
  std::smatch match;
  while (std::regex_search(start, meta.cend(), match, propertyList))
  {
    auto name = match[1].str();
    auto arguments = match[3].str();

    if (name == Props::Body)
      arguments = std::to_string(cursor.GetLineNumber());

    properties.emplace_back(name, arguments);

    // advance the first capture group
    start += match[0].length();
  }

  return properties;
}

#define EMIT_ERROR_LINE(__descr) EMIT_ERROR(__descr << " at line " << GetLineNumber())

void MetaDataManager::Check() const
{
  bool const hasAgent = HasProperty(Props::Agent);
  bool const hasKeynode = HasProperty(Props::Keynode);
  bool const hasForceCreation = HasProperty(Props::ForceCreate);
  bool const hasCmdClass = HasProperty(Props::AgentCommandClass);

  if (hasForceCreation && !hasKeynode)
  {
    EMIT_ERROR_LINE("You can use " << Props::ForceCreate << " just with " << Props::Keynode);
  }

  if (hasKeynode && GetNativeString(Props::Keynode).empty())
  {
    EMIT_ERROR_LINE("Can't use empty " << Props::Keynode << ". You should specify system identifier of keynode in it");
  }

  std::string const cmdClass = GetNativeString(Props::AgentCommandClass);

  std::string const parentClass = GetProperty(ParserMeta::ParentClass);
  bool const isAgentActionParent = (parentClass == Classes::AgentAction);

  if (hasAgent && isAgentActionParent && !hasCmdClass)
  {
    EMIT_ERROR_LINE("You should specify " << Props::AgentCommandClass);
  }

  if (hasCmdClass)
  {
    if (!isAgentActionParent)
    {
      EMIT_ERROR_LINE("You can use " << Props::AgentCommandClass << " just with " << Classes::AgentAction << " class");
    }
    else if (cmdClass.empty())
    {
      EMIT_ERROR_LINE("Don't use empty " << Props::AgentCommandClass);
    }
  }
}
