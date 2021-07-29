#include "LanguageTypes/Function.hpp"

#include "Cursor.hpp"
#include "LanguageTypes/Class.hpp"

#include <boost/algorithm/string/join.hpp>
#include <boost/format.hpp>

Function::Function(Cursor const & cursor, Namespace const & currentNamespace)
  : LanguageType(cursor, currentNamespace)
  , Invokable(cursor)
  , m_name(cursor.GetSpelling())
{
  m_metaData.Check();
}