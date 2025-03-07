#include <thread>
#include "infoserver.hpp"
#include "rserver.hpp"

int main()
{
  std::thread infoServerThread([]()
                               {
    InfoServer _infoServer;
    _infoServer.initMethods();
    _infoServer.start(); });

  std::this_thread::sleep_for(std::chrono::seconds(1));

  // 启动 RServer
  RServer::getInstance()->start("0.0.0.0", 6868);

  infoServerThread.join(); // 等待 InfoServer 线程结束
  return 0;
}