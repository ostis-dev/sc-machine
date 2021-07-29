#include "LanguageTypes/Field.hpp"

#include "Cursor.hpp"
#include "LanguageTypes/Class.hpp"
#include "MetaDataConfig.hpp"

Field::Field(Cursor const & cursor, Namespace const & currentNamespace)
  : LanguageType(cursor, currentNamespace)
  , m_isConst(cursor.GetType().IsConst())
  , m_name(cursor.GetSpelling())
  , m_type(cursor.GetType().GetDisplayName())
{
  auto displayName = m_metaData.GetNativeString(kMetaDisplayName);
  m_displayName = displayName.empty() ? m_name : displayName;

  m_metaData.Check();
}

bool Field::ShouldCompile() const
{
  return isAccessible();
}

bool Field::isAccessible() const
{
  bool const metaFlag = (m_accessModifier == CX_CXXPublic && !m_metaData.GetFlag(kMetaDisable));
  return m_hasExplicitGetter || m_hasExplicitSetter || metaFlag;
}

bool Field::isGetterAccessible() const
{
  return m_hasExplicitGetter || (m_accessModifier == CX_CXXPublic);
}

bool Field::isSetterAccessible(void) const
{
  return m_hasExplicitSetter || (!m_isConst && m_accessModifier == CX_CXXPublic);
}

std::string Field::GetForceType(MetaDataManager const & metaData)
{
  if (metaData.HasProperty(Props::ForceCreate))
  {
    std::string const value = metaData.GetNativeString(Props::ForceCreate);
    return value.empty() ? "ScType::Node" : value;
  }

  return "";
}

void Field::GenarateInitCode(std::stringstream & outCode) const
{
  if (m_metaData.HasProperty(Props::Keynode))
  {
    GenerateResolveKeynodeCode(
        m_metaData.GetNativeString(Props::Keynode), m_displayName, GetForceType(m_metaData), outCode);
  }
}

void Field::GenerateResolveKeynodeCode(
    std::string const & sysIdtf,
    std::string const & displayName,
    std::string const & forceCreation,
    std::stringstream & outCode)
{
  outCode << displayName << " = ctx.HelperResolveSystemIdtf(\"" << sysIdtf << "\"";
  if (!forceCreation.empty())
  {
    outCode << ", " << forceCreation;
  }
  outCode << ");";
  outCode << " result = result && " << displayName << ".IsValid();";
}

std::string const & Field::GetDisplayName() const
{
  return m_displayName;
}
