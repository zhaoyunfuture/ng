
#include "src/lib/uthread.h"

#include <signal.h>
#include <string.h>


namespace ZLIB
{
  static __thread char g_selfName[T_NAME_LEN];
  static volatile __thread sig_atomic_t g_selfName_flag = 0;

  const char * getSelfName_AsyncSignalSafe(void)
  {
    if (g_selfName_flag)
      return g_selfName;
    else
      return "main";
  }

  void *UThreadStartFun( void *param ) {
    UThread *t = (UThread *)param;

    pthread_setname_np(t->m_threadId, t->m_threadName);
    strncpy(g_selfName, t->m_threadName, sizeof(g_selfName));
    g_selfName[sizeof(g_selfName)-1] = '\0';
    g_selfName_flag = 1; // g_selfName can now be safely accessed

    t->m_isThreadRunning = true;

    if(t->m_create_cb) {
        t->m_create_cb(t->m_arg);
    } else {
        t->onInit();
        t->run();
        t->onExit();
    }

    return 0;
  }
}

namespace
{
  extern "C" void *UThreadStart( void *param )
  {
    return ZLIB::UThreadStartFun( param );
  }
}

using namespace ZLIB;

UThread::UThread():m_threadId(0), m_isThreadRunning(false), m_create_cb(NULL), m_arg(NULL)
{
  memset( &m_attrib, 0, sizeof( m_attrib ) );
  memset( &m_threadName, 0, sizeof(m_threadName) );
}

UThread::~UThread()
{
  if ( m_threadId )
  {
    if ( getSelf() != m_threadId )
    {
      pthread_join( m_threadId, NULL );
    }
  }
  pthread_attr_destroy( &m_attrib );
}

int UThread::create(const char name[T_NAME_LEN], void*(*create_cb)(void*), void* arg)
{
  int policy_integer = 0;
  int result = pthread_attr_init( &m_attrib );
  if ( 0 != result )
    return result;

  result = pthread_attr_setstacksize( &m_attrib, 128*1024 );
  if ( 0 != result )
    return result;

  strncpy(m_threadName, name, T_NAME_LEN-1);
  m_threadName[T_NAME_LEN-1] = '\0';
  m_create_cb = create_cb;
  policy_integer = SCHED_RR; //for default
  m_arg = arg;

  result = pthread_create( &m_threadId, &m_attrib, UThreadStart, this );

  if ( 0 != result )
      m_isThreadRunning = false;

  return result;
}


void UThread::exit( int exitcode )
{
  if ( m_threadId )
  {
    if ( getSelf() == m_threadId )
      /* TODO exitcode cannot be on the stack,
       * but if no pthread_join is waiting for it, its not needed at all
       */
      pthread_exit( NULL );
    else
      pthread_cancel( m_threadId );

    //Thread exits, set flag to false
    m_isThreadRunning = false;
  }
}

void UThread::join()
{
  if ( m_threadId )
  {
    if ( getSelf() != m_threadId )
    {
      pthread_join( m_threadId, NULL );
      m_threadId = 0;
    }
  }
}

bool UThread::isRunning()
{
  return m_isThreadRunning;
}

pthread_t UThread::getId()
{
  return m_threadId;
}

int UThread::getName(char name[T_NAME_LEN])
{
  strncpy(name, m_threadName, T_NAME_LEN-1);
  m_threadName[T_NAME_LEN-1] = '\0';
  return 0;
}
