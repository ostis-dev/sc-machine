/*
 * This source file is part of an OSTIS project. For the latest info, see http://ostis.net
 * Distributed under the MIT License
 * (See accompanying file COPYING.MIT or copy at http://opensource.org/licenses/MIT)
 */

#pragma once

#include "sc_event.hpp"
#include "sc_timer.hpp"

#include <chrono>
#include <condition_variable>
#include <mutex>

/* Class implements common wait logic.
 */
class ScWait
{
  class Impl
  {
  public:
    Impl() = default;
    virtual ~Impl() = default;

    void Resolve()
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_isResolved = true;
      m_cond.notify_one();
    }

    bool Wait(uint32_t timeout_ms)
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      while (!m_isResolved)
      {
        if (m_cond.wait_for(lock, std::chrono::milliseconds(timeout_ms)) == std::cv_status::timeout)
          return false;
      }

      return true;
    }

  private:
    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool m_isResolved = false;
  };

public:
  using DelegateFunc = std::function<void(void)>;

  virtual ~ScWait()
  {
  }

  void Resolve()
  {
    m_impl.Resolve();
  }

  void SetOnWaitStartDelegate(DelegateFunc const & startDelegate)
  {
    m_waitStartDelegate = startDelegate;
  }

  bool Wait(uint32_t timeout_ms = 5000)
  {
    if (m_waitStartDelegate)
      m_waitStartDelegate();

    SC_ASSERT(timeout_ms < 60000, ("Too big timeout (it should be less then a minute)"));

    return m_impl.Wait(timeout_ms);
  }

private:
  Impl m_impl;
  DelegateFunc m_waitStartDelegate;
};

/* Class implements event wait logic.
 * Should be alive, while Memory context is alive.
 */
template <typename EventClassT>
class ScWaitEvent : public ScWait
{
public:
  ScWaitEvent(const ScMemoryContext & ctx, const ScAddr & addr)
    : m_event(
          ctx,
          addr,
          std::bind(
              &ScWaitEvent<EventClassT>::OnEvent,
              this,
              std::placeholders::_1,
              std::placeholders::_2,
              std::placeholders::_3))
  {
  }

  virtual ~ScWaitEvent()
  {
  }

protected:
  bool OnEvent(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr)
  {
    if (OnEventImpl(listenAddr, edgeAddr, otherAddr))
    {
      ScWait::Resolve();
      return true;
    }
    return false;
  }

  virtual bool OnEventImpl(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr)
  {
    return true;
  }

private:
  EventClassT m_event;
};

template <typename EventClassT>
class ScWaitCondition final : public ScWaitEvent<EventClassT>
{
public:
  using DelegateCheckFunc = std::function<bool(ScAddr const &, ScAddr const &, ScAddr const &)>;

  ScWaitCondition(const ScMemoryContext & ctx, const ScAddr & addr, DelegateCheckFunc func)
    : ScWaitEvent<EventClassT>(ctx, addr)
    , m_checkFunc(func)
  {
  }

private:
  virtual bool OnEventImpl(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override
  {
    return m_checkFunc(listenAddr, edgeAddr, otherAddr);
  }

private:
  DelegateCheckFunc m_checkFunc;
};

/* Implements waiting for action finish
 */
class ScWaitActionFinished final : public ScWaitEvent<ScEventAddInputEdge>
{
public:
  _SC_EXTERN ScWaitActionFinished(ScMemoryContext const & ctx, ScAddr const & actionAddr);

private:
  virtual bool OnEventImpl(ScAddr const & listenAddr, ScAddr const & edgeAddr, ScAddr const & otherAddr) override;
};

#define SC_WAIT_CHECK(_func) std::bind(_func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
#define SC_WAIT_CHECK_MEMBER(_class, _func) \
  std::bind(_class, _func, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
