#include <PCH.h>
#include <Foundation/Communication/IpcChannel.h>
#include <Foundation/Communication/Implementation/MessageLoop.h>
#include <Foundation/Communication/RemoteMessage.h>
#include <Foundation/Configuration/Startup.h>

EZ_IMPLEMENT_SINGLETON(ezMessageLoop);

#if EZ_ENABLED(EZ_PLATFORM_WINDOWS)
  #include <Foundation/Communication/Implementation/Win/MessageLoop_win.h>
#endif

EZ_BEGIN_SUBSYSTEM_DECLARATION(Foundation, MessageLoop)

BEGIN_SUBSYSTEM_DEPENDENCIES
"TaskSystem",
"ThreadUtils"
END_SUBSYSTEM_DEPENDENCIES

ON_CORE_STARTUP
{
#if EZ_ENABLED(EZ_PLATFORM_WINDOWS)
  EZ_DEFAULT_NEW(ezMessageLoop_win);
#endif
}

ON_CORE_SHUTDOWN
{
  ezMessageLoop* pDummy = ezMessageLoop::GetSingleton();
  EZ_DEFAULT_DELETE(pDummy);
}

EZ_END_SUBSYSTEM_DECLARATION

class ezLoopThread : public ezThread
{
public:
  ezLoopThread() : ezThread("ezMessageLoopThread") {}
  ezMessageLoop* m_pRemoteInterface = nullptr;
  virtual ezUInt32 Run() override
  {
    m_pRemoteInterface->RunLoop();
    return 0;
  }
};

ezMessageLoop::ezMessageLoop()
  : m_SingletonRegistrar(this)
{
}

void ezMessageLoop::StartUpdateThread()
{
  EZ_LOCK(m_Mutex);
  if (m_pUpdateThread == nullptr)
  {
    m_pUpdateThread = EZ_DEFAULT_NEW(ezLoopThread);
    m_pUpdateThread->m_pRemoteInterface = this;
    m_pUpdateThread->Start();
  }
}

void ezMessageLoop::StopUpdateThread()
{
  EZ_LOCK(m_Mutex);
  if (m_pUpdateThread != nullptr)
  {
    m_bShouldQuit = true;
    WakeUp();
    m_pUpdateThread->Join();

    EZ_DEFAULT_DELETE(m_pUpdateThread);
  }
}

void ezMessageLoop::RunLoop()
{
#if EZ_ENABLED(EZ_COMPILE_FOR_DEBUG)
  m_threadId = ezThreadUtils::GetCurrentThreadID();
#endif

  while (true)
  {
    // process all available data until all is processed and we wait for new messages.
    bool didwork = ProcessTasks();
    if (m_bShouldQuit)
      break;

    if (didwork)
      continue;

    didwork |= WaitForMessages(0, nullptr);
    if (m_bShouldQuit)
      break;

    if (didwork)
      continue;

    //wait until we have work again
    WaitForMessages(-1, nullptr);
  }
}

bool ezMessageLoop::ProcessTasks()
{
  EZ_LOCK(m_TasksMutex);
  for (ezIpcChannel* pChannel : m_ConnectQueue)
  {
    pChannel->InternalConnect();
  }
  for (ezIpcChannel* pChannel : m_SendQueue)
  {
    pChannel->InternalSend();
  }
  for (ezIpcChannel* pChannel : m_DisconnectQueue)
  {
    pChannel->InternalDisconnect();
  }

  bool bDidWork = !m_ConnectQueue.IsEmpty() || !m_SendQueue.IsEmpty() || !m_DisconnectQueue.IsEmpty();
  m_ConnectQueue.Clear();
  m_SendQueue.Clear();
  m_DisconnectQueue.Clear();
  return bDidWork;
}

void ezMessageLoop::Quit()
{
  m_bShouldQuit = true;
}

void ezMessageLoop::AddChannel(ezIpcChannel* pChannel)
{
  StartUpdateThread();
  pChannel->m_pOwner = this;
  InternalAddChannel(pChannel);
}

EZ_STATICLINK_FILE(Foundation, Foundation_Communication_Implementation_MessageLoop);