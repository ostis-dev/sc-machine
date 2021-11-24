#include "MetaUtils.hpp"

#include "Cursor.hpp"
#include "CursorType.hpp"
#include "Types.hpp"

#include <boost/algorithm/string/join.hpp>

#include <iostream>

namespace utils
{
void ToString(const CXString & str, std::string & output)
{
  auto cstr = clang_getCString(str);

  output = cstr;

  clang_disposeString(str);
}

std::string GetQualifiedName(const std::string & displayName, const Namespace & currentNamespace)
{
  auto name = boost::algorithm::join(currentNamespace, "::");

  if (!currentNamespace.empty())
    name += "::";

  name += displayName;

  return name;
}

std::string GetQualifiedName(const Cursor & cursor, const Namespace & currentNamespace)
{
  return GetQualifiedName(cursor.GetSpelling(), currentNamespace);
}

void FatalError(const std::string & error)
{
  std::cerr << "Error: " << error << std::endl;

  exit(EXIT_FAILURE);
}

}  // namespace utils
