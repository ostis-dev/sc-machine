/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#include "sc_memory.hpp"
#include "sc_utils.hpp"
#include "sc_stream.hpp"
#include "kpm/sc_agent.hpp"

#include "utils/sc_log.hpp"

#include <assert.h>

#include <iostream>
#include <sstream>

extern "C"
{
#include <glib.h>
}

#define SC_BOOL(x) (x) ? SC_TRUE : SC_FALSE

namespace
{

GMutex gContextMutex;
struct ContextMutexLock
{
  ContextMutexLock() { g_mutex_lock(&gContextMutex); }
  ~ContextMutexLock() { g_mutex_unlock(&gContextMutex); }
};

bool gIsLogMuted = false;

void _logPrintHandler(gchar const * log_domain, GLogLevelFlags log_level,
  gchar const * message, gpointer user_data)
{
  if (gIsLogMuted)
    return;

  std::string stype;
  switch (log_level)
  {
  case G_LOG_LEVEL_CRITICAL:
  case G_LOG_LEVEL_ERROR:
    SC_LOG_ERROR(message);
    break;

  case G_LOG_LEVEL_WARNING:
    SC_LOG_WARNING(message);
    break;

  case G_LOG_LEVEL_INFO:
  case G_LOG_LEVEL_MESSAGE:
    SC_LOG_INFO(message);
    break;

  case G_LOG_LEVEL_DEBUG:
    SC_LOG_DEBUG(message);
    break;
  };
}

unsigned int gContextGounter;

} // namespace

// ------------------

sc_memory_context * ScMemory::ms_globalContext = 0;
ScMemory::MemoryContextList ScMemory::ms_contexts;

bool ScMemory::Initialize(sc_memory_params const & params)
{
  gContextGounter = 0;

  g_log_set_default_handler(_logPrintHandler, nullptr);

  ms_globalContext = sc_memory_initialize(&params);
  
  ScAgentInit(true);
  return ms_globalContext != null_ptr;
}

void ScMemory::Shutdown(bool saveState /* = true */)
{
  if (ms_contexts.size() > 0)
  {
    std::stringstream description;
    description << "There are " << ms_contexts.size() << " contexts, wan't destroyed, before Memory::shutdown:";
    for (auto const * ctx : ms_contexts)
      description << "\t\n" << ctx->GetName();

    SC_THROW_EXCEPTION(utils::ExceptionInvalidState, description.str());
  }

  sc_memory_shutdown(SC_BOOL(saveState));
  ms_globalContext = 0;

  g_log_set_default_handler(g_log_default_handler, nullptr);
}

void ScMemory::LogMute()
{
  gIsLogMuted = true;
}

void ScMemory::LogUnmute()
{
  gIsLogMuted = false;
}

void ScMemory::RegisterContext(ScMemoryContext const * ctx)
{
  assert(!HasMemoryContext(ctx));

  ContextMutexLock lock;
  ms_contexts.push_back(ctx);
}

void ScMemory::UnregisterContext(ScMemoryContext const * ctx)
{
  assert(HasMemoryContext(ctx));

  ContextMutexLock lock;
  for (auto it = ms_contexts.begin(); it != ms_contexts.end(); ++it)
  {
    if (*it == ctx)
    {
      ms_contexts.erase(it);
      return;
    }
  }
}

bool ScMemory::HasMemoryContext(ScMemoryContext const * ctx)
{
  ContextMutexLock lock;
  for (auto const it : ms_contexts)
  {
    if (it == ctx)
      return true;
  }
  return false;
}

// ---------------

ScMemoryContext::ScMemoryContext(sc_uint8 accessLevels /* = 0 */, std::string const & name /* = "" */)
  : m_context(0)
{
  m_context = sc_memory_context_new(accessLevels);
  if (name.empty())
  {
    std::stringstream ss;
    ss << "Context_" << gContextGounter;
    m_name = ss.str();
  }
  else
  {
    m_name = name;
  }

  ScMemory::RegisterContext(this);
}

ScMemoryContext::~ScMemoryContext()
{
  Destroy();
}

void ScMemoryContext::Destroy()
{
  if (m_context)
  {
    ScMemory::UnregisterContext(this);

    sc_memory_context_free(m_context);
    m_context = 0;
  }
}

bool ScMemoryContext::IsValid() const
{
  return m_context != 0;
}

bool ScMemoryContext::IsElement(ScAddr const & addr) const
{
  SC_ASSERT(IsValid(), ());
  return (sc_memory_is_element(m_context, *addr) == SC_TRUE);
}

bool ScMemoryContext::EraseElement(ScAddr const & addr)
{
  SC_ASSERT(IsValid(), ());
  return sc_memory_element_free(m_context, *addr) == SC_RESULT_OK;
}

ScAddr ScMemoryContext::CreateNode(sc_type type)
{
  SC_ASSERT(IsValid(), ());
  return ScAddr(sc_memory_node_new(m_context, type));
}

ScAddr ScMemoryContext::CreateLink()
{
  SC_ASSERT(IsValid(), ());
  return ScAddr(sc_memory_link_new(m_context));
}

ScAddr ScMemoryContext::CreateArc(sc_type type, ScAddr const & addrBeg, ScAddr const & addrEnd)
{
  return CreateEdge(type, addrBeg, addrEnd);
}

ScAddr ScMemoryContext::CreateEdge(sc_type type, ScAddr const & addrBeg, ScAddr const & addrEnd)
{
  SC_ASSERT(IsValid(), ());
  return ScAddr(sc_memory_arc_new(m_context, type, *addrBeg, *addrEnd));
}

ScType ScMemoryContext::GetElementType(ScAddr const & addr) const
{
  SC_ASSERT(IsValid(), ());
  sc_type type = 0;
  return (sc_memory_get_element_type(m_context, *addr, &type) == SC_RESULT_OK) ? ScType(type) : ScType(0);
}

bool ScMemoryContext::SetElementSubtype(ScAddr const & addr, sc_type subtype)
{
  SC_ASSERT(IsValid(), ());
  return sc_memory_change_element_subtype(m_context, *addr, subtype) == SC_RESULT_OK;
}

ScAddr ScMemoryContext::GetEdgeSource(ScAddr const & edgeAddr) const
{
  SC_ASSERT(IsValid(), ());
  ScAddr addr;
  if (sc_memory_get_arc_begin(m_context, *edgeAddr, &addr.m_realAddr) != SC_RESULT_OK)
    addr.Reset();

  return addr;
}

ScAddr ScMemoryContext::GetEdgeTarget(ScAddr const & edgeAddr) const
{
  SC_ASSERT(IsValid(), ());
  ScAddr addr;
  if (sc_memory_get_arc_end(m_context, *edgeAddr, &addr.m_realAddr) != SC_RESULT_OK)
    addr.Reset();

  return addr;
}

bool ScMemoryContext::GetEdgeInfo(ScAddr const & edgeAddr, ScAddr & outSourceAddr, ScAddr & outTargetAddr) const
{
  SC_ASSERT(IsValid(), ());
  if (sc_memory_get_arc_info(m_context, *edgeAddr, &outSourceAddr.m_realAddr, &outTargetAddr.m_realAddr) != SC_RESULT_OK)
  {
    outSourceAddr.Reset();
    outTargetAddr.Reset();

    return false;
  }

  return true;
}

ScAddr ScMemoryContext::GetArcBegin(ScAddr const & arcAddr) const
{
  return GetEdgeSource(arcAddr);
}

ScAddr ScMemoryContext::GetArcEnd(ScAddr const & arcAddr) const
{
  return GetEdgeTarget(arcAddr);
}

bool ScMemoryContext::SetLinkContent(ScAddr const & addr, ScStream const & stream)
{
  SC_ASSERT(IsValid(), ());
  return sc_memory_set_link_content(m_context, *addr, stream.m_stream) == SC_RESULT_OK;
}

bool ScMemoryContext::GetLinkContent(ScAddr const & addr, ScStream & stream)
{
  SC_ASSERT(IsValid(), ());

  sc_stream * s = 0;
  if (sc_memory_get_link_content(m_context, *addr, &s) != SC_RESULT_OK)
  {
    stream.Reset();
    return false;
  }

  stream.Init(s);

  return stream.IsValid();
}

bool ScMemoryContext::FindLinksByContent(ScStream const & stream, ScAddrList & found)
{
  SC_ASSERT(IsValid(), ());

  sc_addr * result = 0;
  sc_uint32 resultCount = 0;

  found.clear();
  if (sc_memory_find_links_with_content(m_context, stream.m_stream, &result, &resultCount) != SC_RESULT_OK)
    return false;

  for (sc_uint32 i = 0; i < resultCount; ++i)
    found.push_back(ScAddr(result[i]));

  if (result)
    sc_memory_free_buff(result);

  return found.size() > 0;
}

bool ScMemoryContext::Save()
{
  SC_ASSERT(IsValid(), ());
  return (sc_memory_save(m_context) == SC_RESULT_OK);
}

bool ScMemoryContext::HelperResolveSystemIdtf(std::string const & sysIdtf, ScAddr & outAddr, bool bForceCreation /*= false*/)
{
  SC_ASSERT(IsValid(), ());
  outAddr.Reset();
  bool result = HelperFindBySystemIdtf(sysIdtf, outAddr);
  if (!result && bForceCreation)
  {
    outAddr = CreateNode(sc_type_const);
    if (outAddr.IsValid())
      result = HelperSetSystemIdtf(sysIdtf, outAddr);
  }
  return result;
}

bool ScMemoryContext::HelperSetSystemIdtf(std::string const & sysIdtf, ScAddr const & addr)
{
  SC_ASSERT(IsValid(), ());
  return (sc_helper_set_system_identifier(m_context, *addr, sysIdtf.c_str(), (sc_uint32)sysIdtf.size()) == SC_RESULT_OK);
}

std::string ScMemoryContext::HelperGetSystemIdtf(ScAddr const & addr)
{
  SC_ASSERT(IsValid(), ());
  ScAddr idtfLink;
  if (sc_helper_get_system_identifier_link(m_context, *addr, &idtfLink.m_realAddr) == SC_RESULT_OK)
  {
    if (idtfLink.IsValid())
    {
      ScStream stream;
      if (GetLinkContent(idtfLink, stream))
      {
        std::string result;
        if (ScStreamConverter::StreamToString(stream, result))
          return result;
      }
    }
  }

  return std::string("");
}

bool ScMemoryContext::HelperCheckArc(ScAddr const & begin, ScAddr end, sc_type arcType)
{
  return HelperCheckEdge(begin, end, ScType(arcType));
}

bool ScMemoryContext::HelperCheckEdge(ScAddr const & begin, ScAddr end, ScType const & edgeType)
{
  SC_ASSERT(IsValid(), ());
  return (sc_helper_check_arc(m_context, *begin, *end, *edgeType) == SC_RESULT_OK);
}

bool ScMemoryContext::HelperFindBySystemIdtf(std::string const & sysIdtf, ScAddr & outAddr)
{
  SC_ASSERT(IsValid(), ());
  return (sc_helper_find_element_by_system_identifier(m_context, sysIdtf.c_str(), (sc_uint32)sysIdtf.size(), &outAddr.m_realAddr) == SC_RESULT_OK);
}

bool ScMemoryContext::HelperGenTemplate(ScTemplate const & templ, ScTemplateGenResult & result, ScTemplateGenParams const & params, ScTemplateResultCode * resultCode)
{
  return templ.Generate(*this, result, params, resultCode);
}

bool ScMemoryContext::HelperSearchTemplate(ScTemplate const & templ, ScTemplateSearchResult & result)
{
  return templ.Search(*this, result);
}

bool ScMemoryContext::HelperSearchTemplateInStruct(ScTemplate const & templ, ScAddr const & scStruct, ScTemplateSearchResult & result)
{
  return templ.SearchInStruct(*this, scStruct, result);
}

bool ScMemoryContext::HelperBuildTemplate(ScTemplate & templ, ScAddr const & templAddr)
{
  return templ.FromScTemplate(*this, templAddr);
}
