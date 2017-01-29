/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_struct.hpp"
#include "sc_memory.hpp"
#include "sc_template.hpp"

ScStruct::ScStruct(ScMemoryContext * ctx, ScAddr const & structAddr)
  : m_addr(structAddr)
  , m_context(ctx)

{
}

bool ScStruct::Append(ScAddr const & elAddr)
{
  SC_ASSERT(m_context, ());
  if (!HasElement(elAddr))
    return m_context->CreateEdge(sc_type_arc_pos_const_perm, m_addr, elAddr).IsValid();

  return false;
}

bool ScStruct::Append(ScAddr const & elAddr, ScAddr const & attrAddr)
{
  SC_ASSERT(m_context, ());
  if (!HasElement(elAddr))
  {
    ScAddr const edge = m_context->CreateEdge(sc_type_arc_pos_const_perm, m_addr, elAddr);
    if (edge.IsValid())
    {
      ScAddr const edge2 = m_context->CreateEdge(sc_type_arc_pos_const_perm, attrAddr, edge);
      if (edge2.IsValid())
        return true;

      // cleanup
      m_context->EraseElement(edge);
    }
  }

  return false;
}

bool ScStruct::Remove(ScAddr const & elAddr)
{
  SC_ASSERT(m_context, ());
  bool found = false;
  ScIterator3Ptr iter = m_context->Iterator3(m_addr, ScType::EdgeAccessConstPosPerm, elAddr);
  while (iter->Next())
  {
    m_context->EraseElement(iter->Get(1));
    found = true;
  }

  return found;
}

bool ScStruct::HasElement(ScAddr const & elAddr) const
{
  SC_ASSERT(m_context, ());
  return m_context->HelperCheckEdge(m_addr, elAddr, ScType::EdgeAccessConstPosPerm);
}

ScStruct & ScStruct::operator << (ScAddr const & elAddr)
{
  Append(elAddr);
  return *this;
}

ScStruct & ScStruct::operator << (ScTemplateGenResult const & res)
{
  size_t const res_num = res.Size();
  for (size_t i = 0; i < res_num; ++i)
    Append(res.m_result[i]);

  return *this;
}

ScStruct & ScStruct::operator >> (ScAddr const & elAddr)
{
  Remove(elAddr);
  return *this;
}

ScAddr const & ScStruct::operator * () const
{
  return m_addr;
}

bool ScStruct::IsEmpty() const
{
  SC_ASSERT(m_context, ());
  ScIterator3Ptr iter = m_context->Iterator3(m_addr, SC_TYPE(sc_type_arc_pos_const_perm), SC_TYPE(0));
  return !iter->Next();
}
