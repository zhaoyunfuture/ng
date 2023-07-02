#pragma once

#include <pthread.h>
#define T_NAME_LEN 16

namespace ZLIB
{

  inline pthread_t getSelf() {
    return pthread_self();
  }

  const char * getSelfName_AsyncSignalSafe(void);

  class UThread
  {
    friend void *UThreadStartFun( void *param );

  public:
    UThread();
    virtual ~UThread();

    //virtual int create( const char name[T_NAME_LEN]);
    virtual int create( const char name[T_NAME_LEN], void*(*create_cb)(void*), void* arg);
    virtual void exit( int exitcode = 0 );
    virtual void join();
    virtual bool isRunning();
    virtual pthread_t getId();
    virtual int getName(char name[T_NAME_LEN]);

  protected:
    virtual void run() {};
    virtual void onInit() {};
    virtual void onExit() {};

  private:
    pthread_t       m_threadId;
    pthread_attr_t  m_attrib;
    bool            m_isThreadRunning;
    char            m_threadName[T_NAME_LEN];
    void*           (*m_create_cb)(void*);
    void*           m_arg;
  };

}
