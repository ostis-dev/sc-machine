#pragma once

#include "Invokable.hpp"
#include "LanguageType.hpp"

class Method final
  : public LanguageType
  , public Invokable
{
public:
  Method(Cursor const & cursor, Namespace const & currentNamespace);

private:
  std::string m_name;
};
